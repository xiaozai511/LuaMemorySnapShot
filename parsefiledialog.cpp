#include "parsefiledialog.h"
#include "ui_parsefiledialog.h"
#include "memoryrefrenceinfo.h"
#include "common.h"
#include "refrencetreedock.h"
#include <QMessageBox>
#include <QTimer>
#include <QFileInfo>

//ParseFileDialog::ParseFileDialog(QWidget *parent) :
//	QDialog(parent),
//	ui(new Ui::ParseFileDialog),
//	bCompare(false),
//	m_infoMgr(nullptr)
//{
//	ui->setupUi(this);
//}

ParseFileDialog::ParseFileDialog(const QString& filePath, MemoryRefrenceInfoMgr* infoMgr, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::ParseFileDialog)
{
	ui->setupUi(this);

	Q_ASSERT(dynamic_cast<RefrenceTreeDock*>(parent));

	bCompare = false;
	m_infoMgr = infoMgr;
	m_filePath = filePath;
	m_infoMgrBase = nullptr;
	bAllOver = false;

	//register
	connect(m_infoMgr, SIGNAL(parseRunning(QString,int,int)), SLOT(onParseRunning(QString,int,int)));
	connect(m_infoMgr, SIGNAL(parseFinished(QString)), SLOT(onParseFinished()));

	onBaseParseFinished();
}

ParseFileDialog::ParseFileDialog(const QString& before, const QString& after, MemoryRefrenceInfoMgr* infoMgr, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::ParseFileDialog)
{
	ui->setupUi(this);

	bCompare = true;
	m_infoMgr = infoMgr;
	m_filePath = after;
	m_filePathBase = before;
	bAllOver = false;
	m_infoMgrBase = new MemoryRefrenceInfoMgr;

	//register
	connect(m_infoMgr, SIGNAL(parseRunning(QString,int,int)), SLOT(onParseRunning(QString,int,int)));
	connect(m_infoMgr, SIGNAL(parseFinished(QString)), SLOT(onParseFinished()));

	connect(m_infoMgrBase, SIGNAL(parseRunning(QString,int,int)), SLOT(onParseRunning(QString,int,int)));
	connect(m_infoMgrBase, SIGNAL(parseFinished(QString)), SLOT(onBaseParseFinished()));

	ui->noticeText->setText(tr("解析文件：") + QFileInfo(m_filePathBase).fileName());
	m_infoMgrBase->parsFile(m_filePathBase);
}

ParseFileDialog::~ParseFileDialog()
{
	delete ui;
	stopParsing();
	SAFE_DELETE(m_infoMgrBase);
}

void ParseFileDialog::onParseRunning(const QString& filepath, int usetime, int progress)
{
	//	qDebug() << "onParseRunning" << filepath << usetime << progress;
	Q_UNUSED(filepath);
	static QString strUseTime(tr("已用时间："));
	ui->noticeTime->setText(strUseTime + QString::asprintf("%0.2f(s)", (float)usetime / 1000));
	ui->progressBar->setValue(progress);
}

void ParseFileDialog::onBaseParseFinished()
{
	if(!m_infoMgr || m_filePath.length() == 0) {
		onParseFinished();
		return;
	}
	ui->noticeText->setText(tr("解析文件：") + QFileInfo(m_filePath).fileName());
	m_infoMgr->parsFile(m_filePath, m_infoMgrBase);
}

void ParseFileDialog::onParseFinished()
{
//	ui->progressBar->setValue(100);
//	QTimer::singleShot(100, this, "close");

	ui->noticeText->setText(tr("文件解析完成，等待更新列表！"));
	ui->noticeText->repaint();

	RefrenceTreeDock* parentDock = (RefrenceTreeDock*)parent();
	QString title("RefrenceTree");
	if(bCompare) {
		title.append(QString("-Compared: %1 - %2").arg(QFileInfo(m_filePathBase).fileName(),
													   QFileInfo(m_filePath).fileName()));
	} else {
		title.append(QString("-File: %1").arg(QFileInfo(m_filePath).fileName()));
	}
	parentDock->updateTreeView(title);

	bAllOver = true;
	this->close();
}

void ParseFileDialog::closeEvent(QCloseEvent* event)
{
	if(bAllOver) {
		event->accept();
	} else {
		auto ret = QMessageBox::question(this, "Notice",
										 tr("文件解析中，是否需要取消？"),
										 QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
		if(ret == QMessageBox::Yes) {
			stopParsing();
			event->accept();
		} else {
			event->ignore();
		}
	}
}

//停止正在执行的线程
void ParseFileDialog::stopParsing()
{
	if(m_infoMgr && m_infoMgr->isRunning()) {
		m_infoMgr->setRunning(false);
		m_infoMgr->wait();
	}

	if(m_infoMgrBase && m_infoMgrBase->isRunning()) {
		m_infoMgrBase->setRunning(false);
		m_infoMgrBase->wait();
	}
}
