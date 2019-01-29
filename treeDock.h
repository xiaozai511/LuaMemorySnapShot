#ifndef TREEDOCK_H
#define TREEDOCK_H

#include <QDockWidget>
//#include <QTreeView>
#include <QDirModel>
#include <QFileSystemModel>
#include <functional>
#include <QList>
#include <list>
#include <mutex>
#include <string>
#include <QMenu>
#include <QListView>
#include "mytreeview.h"
//#include "stringslistdialog.h"
//#include "replacethread.h"

class TreeDock : public QDockWidget{
    Q_OBJECT
public:
	//parent:
	//rootPath:根目录
	//caption:标题
	//expand:默认展开目录
	TreeDock(QWidget *parent, const QString& rootPath, const QString& caption = "", const QString& expand = "");
	virtual ~TreeDock();

	MyTreeView *treeView() {
		return mTreeView;
	}

	QFileSystemModel *model() {
		return mModel;
	}

//    void retranslateUi();
    void refreshTree();

	void setRootPath(const QString& rootPath, const QString& expand = "");
	void setTreeNameFilter(const QString& sFilter = "");

	const std::list<std::string>& getFilesList();

signals:
	void addCompare(const QString& path);
	void fileTriggerd(const QString& path);

public slots:
	void layoutChanged();
//	void directoryLoaded(const QString& path);
	void expanded(const QModelIndex &index);
	void fileRenamed(const QString &path, const QString &oldName, const QString &newName);

protected slots:
	void onFindString();

	void action_copyPath();
	void action_findItem();
	void action_showExplorer();
	void action_removeItem();
	void action_renameItem();
//	void action_findRefrence();
	void action_addCompare();

	void onKeyPressEvent(QKeyEvent *event);
	void onKeyReleaseEvent(QKeyEvent *event);
	void onMousePressEvent(QMouseEvent *event);
	void onMouseReleaseEvent(QMouseEvent *event);

	void onFileClicked(const QModelIndex &index);
protected:
	void onQuitFindString();
	void expandToFirstFile(const QString& filename);

	void showContextMenu(QPoint pos);

	virtual void resizeEvent(QResizeEvent *event);

private:
	MyTreeView *mTreeView;
	QFileSystemModel *mModel;

	std::mutex _treeLock;

	bool bCtrlPressed;

	QLineEdit* mFindStrEdit;
	QList<std::function<void()> > _layoutChangedListener;
	QString mInitResPath;
	QStringList mFileSuffixList;

	std::list<std::string> mFilesList;

	int _preFilterIndex;
	QString _preFilterName;
	QStringList _preFilterNameList;

	QAction* q_action_copyPath;
	QAction* q_action_findItem;
	QAction* q_action_removeItem;
	QAction* q_action_renameItem;
	QAction* q_action_showExplorer;
//	QAction* q_action_findRefrence;
	QAction* q_action_addCompare;

	QMenu* mContextMenu;

//	ReplaceThread* mReplaceThread;
//	bool bReplaceEnable;

//	StringsListDialog* mListViewDialog;
};

#endif // TREEDOCK_H
