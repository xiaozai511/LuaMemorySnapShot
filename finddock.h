#ifndef FINDDOCK_H
#define FINDDOCK_H

#include <QDockWidget>

namespace Ui {
class FindDock;
}

class FindDock : public QDockWidget
{
	Q_OBJECT

public:
	explicit FindDock(QWidget *parent = 0);
	~FindDock();

private:
	Ui::FindDock *ui;
};

#endif // FINDDOCK_H
