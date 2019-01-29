#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "common.h"
#include "treeDock.h"
#include "comparedock.h"
#include "refrencetreedock.h"

#include <QDir>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	//treedoc
	QString initPath = globalSetting()->value(INIT_PATH_STR_KEY, "memory").toString();
	m_treeDock = new TreeDock(this, initPath, "Memory");
	m_compareDock = new CompareDock(this);
	m_refrenceDock = new RefrenceTreeDock(this);

	addDockWidget(Qt::LeftDockWidgetArea, m_treeDock);
	addDockWidget(Qt::LeftDockWidgetArea, m_compareDock);
//	addDockWidget(Qt::RightDockWidgetArea, m_refrenceDock);

	this->setCentralWidget(m_refrenceDock);

	//connect
	connect(ui->actionRootDir, SIGNAL(triggered(bool)), SLOT(onSelectPath()));
	connect(m_treeDock, SIGNAL(addCompare(QString)), m_compareDock, SLOT(addCompareFile(QString)));
	connect(m_treeDock, SIGNAL(fileTriggerd(QString)), m_refrenceDock, SLOT(fileReceived(QString)));
	connect(m_compareDock, SIGNAL(compareTriggerd(QString,QString)), m_refrenceDock, SLOT(compareFileReceived(QString,QString)));

	ui->actionCompareView->setChecked(true);
	ui->actionDirView->setChecked(true);
	ui->actionTreeView->setChecked(true);

	connect(ui->actionCompareView, SIGNAL(toggled(bool)), SLOT(onShowDocks()));
	connect(ui->actionDirView, SIGNAL(toggled(bool)), SLOT(onShowDocks()));
//	connect(ui->actionTreeView, SIGNAL(toggled(bool)), SLOT(onShowDocks()));

	connect(ui->actionExpandAll, SIGNAL(triggered(bool)), m_refrenceDock, SLOT(expandAll()));
	connect(ui->actionCollapseAll, SIGNAL(triggered(bool)), m_refrenceDock, SLOT(collapseAll()));

	ui->actionColName->setChecked(true);
	ui->actionColType->setChecked(true);
	ui->actionColValue->setChecked(true);
	ui->actionColRefCount->setChecked(true);

	connect(ui->actionColName, SIGNAL(triggered(bool)), m_refrenceDock, SLOT(showColName(bool)));
	connect(ui->actionColType, SIGNAL(triggered(bool)), m_refrenceDock, SLOT(showColType(bool)));
	connect(ui->actionColRefCount, SIGNAL(triggered(bool)), m_refrenceDock, SLOT(showColRefCount(bool)));
	connect(ui->actionColValue, SIGNAL(triggered(bool)), m_refrenceDock, SLOT(showColValue(bool)));

	connect(ui->actionCopyName, SIGNAL(triggered(bool)), m_refrenceDock, SLOT(copyName()));
	connect(ui->actionCopyValue, SIGNAL(triggered(bool)), m_refrenceDock, SLOT(copyValue()));
}

MainWindow::~MainWindow()
{
	delete ui;

	SAFE_DELETE(m_treeDock);
	SAFE_DELETE(m_compareDock);
	SAFE_DELETE(m_refrenceDock);
}

void MainWindow::onSelectPath()
{
	QString dir = QFileDialog::getExistingDirectory(this, "select path", QDir::currentPath());
	if(!dir.isEmpty()) {
		globalSetting()->setValue(INIT_PATH_STR_KEY, QVariant(dir));
		m_treeDock->setRootPath(dir);
	}
}

void MainWindow::onShowDocks()
{
	m_treeDock->setVisible(ui->actionDirView->isChecked());
	m_compareDock->setVisible(ui->actionCompareView->isChecked());
//	m_refrenceDock->setVisible(ui->actionTreeView->isChecked());
}
