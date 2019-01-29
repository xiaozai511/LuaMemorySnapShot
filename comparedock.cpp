#include "comparedock.h"
#include "ui_comparedock.h"
#include <QFileInfo>
#include <QFileDialog>
#include <QResizeEvent>
#include <QMessageBox>

CompareDock::CompareDock(QWidget *parent) :
	QDockWidget(parent)
	, ui(new Ui::UICompareDock)
{
	ui->setupUi(this);

//	ui->edit_infoFileA->setEnabled(false);
//	ui->edit_infoFileB->setEnabled(false);

	connect(ui->tb_infoFileA, SIGNAL(clicked(bool)), SLOT(selectFileA()));
	connect(ui->tb_infoFileB, SIGNAL(clicked(bool)), SLOT(selectFileB()));
	connect(ui->doCompareButton, SIGNAL(clicked(bool)), SLOT(doCompare()));
	connect(ui->doClearButton, SIGNAL(clicked(bool)), SLOT(doClear()));
	connect(ui->doChangeButton, SIGNAL(clicked(bool)), SLOT(doChange()));

	this->setFeatures(DockWidgetFeature::DockWidgetMovable|DockWidgetFeature::DockWidgetFloatable);
	this->setMinimumHeight(170);
	this->setMaximumHeight(170);

	this->setWindowTitle("Compare");
}

void CompareDock::_setWidgetText(QLineEdit* edit, const QString& path, QString& filepath)
{
	QFileInfo info(path);
	if(info.isFile()) {
		filepath = path;
		edit->setText(info.completeBaseName());
	}
}

void CompareDock::getOpenFileName(QLineEdit* edit, QString& filepath)
{
	QString initPath = globalSetting()->value(INIT_PATH_STR_KEY, "memory").toString();
	QString path = QFileDialog::getOpenFileName(this, "select file", initPath);

	_setWidgetText(edit, path, filepath);
}

void CompareDock::selectFileA()
{
	getOpenFileName(ui->edit_infoFileA, mFileA);
}

void CompareDock::selectFileB()
{
	getOpenFileName(ui->edit_infoFileB, mFileB);
}

void CompareDock::doCompare()
{
	if(mFileA.isEmpty() || mFileB.isEmpty()) {
		return;
	}

//	if(mFileA == mFileB) {
//		QMessageBox::critical(this, "Error", "Can not compare same files!", QMessageBox::Ok);
//		return;
//	}

	qDebug() << "docompare" << mFileA << mFileB;
	emit compareTriggerd(mFileA, mFileB);
}

void CompareDock::doClear()
{
	mFileA.clear();
	mFileB.clear();

	ui->edit_infoFileA->setText(mFileA);
	ui->edit_infoFileB->setText(mFileB);
}

void CompareDock::doChange()
{
	QString _file_a = mFileA;
	QString _file_b = mFileB;

	_setWidgetText(ui->edit_infoFileA, _file_b, mFileA);
	_setWidgetText(ui->edit_infoFileB, _file_a, mFileB);
}

void CompareDock::addCompareFile(const QString& filePath)
{
	QFileInfo info(filePath);
	if(!info.isFile()) {
		return;
	}

	if(mFileA.isEmpty()) {
		mFileA = filePath;
		ui->edit_infoFileA->setText(info.completeBaseName());
	} else {
		mFileB = filePath;
		ui->edit_infoFileB->setText(info.completeBaseName());
	}
}

void CompareDock::resizeEvent(QResizeEvent* event)
{
	const QSize& cs = event->size();

	ui->edit_infoFileA->resize(cs.width() - 60, ui->edit_infoFileA->height());
	ui->edit_infoFileB->resize(cs.width() - 60, ui->edit_infoFileB->height());

	ui->tb_infoFileA->move(ui->edit_infoFileA->geometry().right() + 10, ui->tb_infoFileA->pos().y());
	ui->tb_infoFileB->move(ui->edit_infoFileB->geometry().right() + 10, ui->tb_infoFileB->pos().y());

	ui->doCompareButton->move((cs.width() - ui->doCompareButton->width()) / 2,
							  ui->doCompareButton->pos().y());
	ui->doClearButton->move((cs.width() - ui->doClearButton->width()) / 2,
							  ui->doClearButton->pos().y());
}
