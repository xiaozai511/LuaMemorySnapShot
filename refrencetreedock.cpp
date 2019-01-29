#include "common.h"
#include "refrencetreedock.h"
#include "ui_refrencetreedock.h"
#include <QResizeEvent>
#include <QTreeView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QList>
#include <QTime>
#include <QMessageBox>
#include <QClipboard>
#include <QFileInfo>
#include "memoryrefrenceinfo.h"
#include "parsefiledialog.h"

RefrenceTreeDock::RefrenceTreeDock(QWidget *parent) :
	QDockWidget(parent)
	, ui(new Ui::RefrenceTreeDock)
	, m_bShowName(true)
	, m_bShowType(true)
	, m_bShowRefCount(true)
	, m_bShowValue(true)
{
	ui->setupUi(this);
	this->setFeatures(DockWidgetFeature::DockWidgetMovable|DockWidgetFeature::DockWidgetFloatable);
	ui->treeView->setModel(new QStandardItemModel);
	ui->treeView->setExpandsOnDoubleClick(true);
	ui->treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_memoryRefrenceInfoMgr = new MemoryRefrenceInfoMgr;

	this->setWindowTitle("RefrenceTree");
}

RefrenceTreeDock::~RefrenceTreeDock()
{
	delete ui;
	SAFE_DELETE(m_memoryRefrenceInfoMgr);
}

void RefrenceTreeDock::fileReceived(const QString& filepath)
{
	qDebug() << "RefrenceTreeDock::fileReceived" << filepath;
//	m_memoryRefrenceInfoMgr->parsFile(filepath);

//	//更新列表
//	QString title("RefrenceTree-File: %1");
//	updateTreeView(title.arg(QFileInfo(filepath).fileName()));

	ParseFileDialog dialog(filepath, m_memoryRefrenceInfoMgr, this);
	dialog.exec();
}

void RefrenceTreeDock::compareFileReceived(const QString& before, const QString& after)
{
	qDebug() << "RefrenceTreeDock::compareFileReceived" << before << after;
//	MemoryRefrenceInfoMgr* infoMgrBase = new MemoryRefrenceInfoMgr;
//	infoMgrBase->parsFile(before);
//	m_memoryRefrenceInfoMgr->parsFile(after, infoMgrBase);
//	SAFE_DELETE(infoMgrBase);

//	//更新列表
//	QString title("RefrenceTree-Compared: %1 - %2");
//	updateTreeView(title.arg(QFileInfo(before).fileName(), QFileInfo(after).fileName()));

	ParseFileDialog dialog(before, after, m_memoryRefrenceInfoMgr, this);
	dialog.exec();
}

void RefrenceTreeDock::collapseAll()
{
	ui->treeView->collapseAll();
}

void RefrenceTreeDock::expandAll()
{
	ui->treeView->expandAll();
}

void RefrenceTreeDock::resizeEvent(QResizeEvent* event)
{
	Q_UNUSED(event);
	ui->treeView->resize(width() - 10,
						 height() - 30);


	ui->treeView->setColumnWidth(3, 150);
	ui->treeView->setColumnWidth(2, 80);
	ui->treeView->setColumnWidth(1, 80);
	ui->treeView->setColumnWidth(0, ui->treeView->width() - 80 - 80 - 150);
}

QList<QStandardItem *> RefrenceTreeDock::prepareRows(MemoryRefrenceInfo* root)
{
	static auto _prepareRow = [](MemoryRefrenceInfo* info)-> QList<QStandardItem *> {
		QList<QStandardItem *> rowItems;
		rowItems << new QStandardItem(info->getName());
		rowItems << new QStandardItem(info->getType());
		int count = info->getSumRefCount();
//		int count = info->getRefCount();
		rowItems << new QStandardItem(count > 0 ? QString::number(count) : "");
		rowItems << new QStandardItem(info->getJoinValue());
		return rowItems;
	};

	QList<QStandardItem *> rootItem = _prepareRow(root);
	for(auto child : root->getChildren()) {
		rootItem.first()->appendRow(prepareRows(child));
	}

	return rootItem;
}

void RefrenceTreeDock::updateTreeView(const QString& title)
{
	QStandardItemModel* model = dynamic_cast<QStandardItemModel*>(ui->treeView->model());

	model->clear();
	model->setColumnCount(4);
	model->setHeaderData(0, Qt::Horizontal, QVariant("Name"));
	model->setHeaderData(1, Qt::Horizontal, QVariant("Type"));
	model->setHeaderData(2, Qt::Horizontal, QVariant("Count"));
	model->setHeaderData(3, Qt::Horizontal, QVariant("Address/Value"));
	resizeEvent(nullptr);

	QStandardItem *item = model->invisibleRootItem();

	QList<MemoryRefrenceInfo*> rootInfos = m_memoryRefrenceInfoMgr->getAllRootInfos();
	m_memoryRefrenceInfoMgr->sortAll(rootInfos, false);
	for(auto _root : rootInfos) {
		item->appendRow(prepareRows(_root));
	}
//	ui->treeView->expandToDepth(2);
	updateColumnShown();

	this->setWindowTitle(title);
}

void RefrenceTreeDock::showColName(bool show)
{
	m_bShowName = show;
	updateColumnShown();
}

void RefrenceTreeDock::showColType(bool show)
{
	m_bShowType = show;
	updateColumnShown();
}

void RefrenceTreeDock::showColRefCount(bool show)
{
	m_bShowRefCount = show;
	updateColumnShown();
}

void RefrenceTreeDock::showColValue(bool show)
{
	m_bShowValue = show;
	updateColumnShown();
}

void RefrenceTreeDock::copyData(const QModelIndex& index)
{
	if(index.isValid()) {
		auto data = ui->treeView->model()->data(index).toString();
		if(data.length() > 0) {
			QClipboard* clipBoard = QApplication::clipboard();
			clipBoard->setText(data);
			QMessageBox::about(NULL, "", QString(tr("复制成功!\n(%1)").arg(data)));
		}
	}
}

void RefrenceTreeDock::copyName()
{
	auto index = ui->treeView->currentIndex();
	copyData(index.sibling(index.row(), 0));
}

void RefrenceTreeDock::copyValue()
{
	auto index = ui->treeView->currentIndex();
	copyData(index.sibling(index.row(), 3));
}

void RefrenceTreeDock::updateColumnShown()
{
	auto _updateShow = [=](int index, bool bShow) {
		if(bShow) {
			ui->treeView->showColumn(index);
		} else {
			ui->treeView->hideColumn(index);
		}

	};

	_updateShow(0, m_bShowName);
	_updateShow(1, m_bShowType);
	_updateShow(2, m_bShowRefCount);
	_updateShow(3, m_bShowValue);
}
