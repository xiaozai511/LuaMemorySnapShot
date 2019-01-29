#include "finddock.h"
#include "ui_finddock.h"

FindDock::FindDock(QWidget *parent) :
	QDockWidget(parent),
	ui(new Ui::FindDock)
{
	ui->setupUi(this);
}

FindDock::~FindDock()
{
	delete ui;
}
