#ifndef COMPAREDOCK_H
#define COMPAREDOCK_H

#include <QWidget>
#include <QDockWidget>
#include "common.h"

namespace Ui {
class UICompareDock;
}
class QLineEdit;

class CompareDock : public QDockWidget
{
	Q_OBJECT
public:
	CompareDock(QWidget *parent = nullptr);

signals:
	void compareTriggerd(const QString& befor, const QString& after);

protected slots:
	void selectFileA();
	void selectFileB();

	void doCompare();
	void doClear();
	void doChange();

	void addCompareFile(const QString& filePath);

protected:
	void _setWidgetText(QLineEdit* edit, const QString& path, QString& filepath);
	void getOpenFileName(QLineEdit* edit, QString& filepath);

	void resizeEvent(QResizeEvent* event);

protected:
	Ui::UICompareDock *ui;

	QString mFileA;
	QString mFileB;
};

#endif // COMPAREDOCK_H
