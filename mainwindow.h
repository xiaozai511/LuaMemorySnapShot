#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class TreeDock;
class CompareDock;
class RefrenceTreeDock;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

protected slots:
	void onSelectPath();
	void onShowDocks();

private:
	Ui::MainWindow *ui;

	TreeDock* m_treeDock;
	CompareDock* m_compareDock;
	RefrenceTreeDock* m_refrenceDock;
};

#endif // MAINWINDOW_H
