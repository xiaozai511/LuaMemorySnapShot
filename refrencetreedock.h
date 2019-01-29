#ifndef REFRENCETREEDOCK_H
#define REFRENCETREEDOCK_H

#include <QDockWidget>
#include <QString>
#include <QModelIndex>

namespace Ui {
class RefrenceTreeDock;
}

class QStandardItemModel;
class QStandardItem;
class MemoryRefrenceInfo;
class MemoryRefrenceInfoMgr;

class RefrenceTreeDock : public QDockWidget
{
	Q_OBJECT

public:
	explicit RefrenceTreeDock(QWidget *parent = 0);
	~RefrenceTreeDock();

public slots:
	void fileReceived(const QString& filepath);
	void compareFileReceived(const QString& before, const QString& after);
	void collapseAll();
	void expandAll();

	void showColName(bool show);
	void showColType(bool show);
	void showColRefCount(bool show);
	void showColValue(bool show);

	void copyName();
	void copyValue();

	void updateTreeView(const QString& title);	//更新列表

protected:
	void resizeEvent(QResizeEvent* event);
	QList<QStandardItem *> prepareRows(MemoryRefrenceInfo* info);
	void updateColumnShown();
	void copyData(const QModelIndex& index);

private:
	Ui::RefrenceTreeDock *ui;

	bool m_bShowName;
	bool m_bShowType;
	bool m_bShowRefCount;
	bool m_bShowValue;

	MemoryRefrenceInfoMgr* m_memoryRefrenceInfoMgr;
};

#endif // REFRENCETREEDOCK_H
