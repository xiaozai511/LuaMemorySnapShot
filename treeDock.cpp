#include "treeDock.h"
#include <QDirModel>
#include <QVBoxLayout>
#include "common.h"
#include <QFile>
#include <QTextStream>
#include <QHeaderView>
#include <QFileIconProvider>
#include <QApplication>
#include <QClipboard>
#include <QLineEdit>
#include <QMessageBox>
#include <QProcess>
#include <QThread>
#include <QStringListModel>
//#include "replacethread.h"
//#include "queryresource.h"

class MyFileIconProvider : public QFileIconProvider
{
public:
	virtual QIcon icon(const QFileInfo &info) const
	{
		QString suffix = info.suffix();
		if(suffix == "dt1") {
			return QFileIconProvider::icon(QFileIconProvider::IconType::Drive);
		}else if(suffix == "dt2") {
			return QFileIconProvider::icon(QFileIconProvider::IconType::Network);
		}else if(suffix == "png") {
			return QFileIconProvider::icon(QFileIconProvider::IconType::Desktop);
		}else if(suffix == "jpg") {
			return QFileIconProvider::icon(QFileIconProvider::IconType::Computer);
		}else if(suffix == "arm") {
			return QFileIconProvider::icon(QFileIconProvider::IconType::Trashcan);
		}else{
			return QFileIconProvider::icon(info);
		}
	}
};

TreeDock::TreeDock(QWidget *parent, const QString& rootPath, const QString& caption, const QString& expand):
		QDockWidget(parent)
		, bCtrlPressed(false)
		, mTreeView(new MyTreeView(this))
		, _preFilterIndex(-1)
//		, mReplaceThread(nullptr)
//		, bReplaceEnable(bReplaceEnable)
{
	setObjectName(QLatin1String("treeDock"));

    this->setFloating(false);
    this->setFeatures(QDockWidget::NoDockWidgetFeatures);

    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);
	layout->setMargin(5);

	mFileSuffixList.clear();
	mFileSuffixList << "txt";

	setRootPath(rootPath, expand);

	layout->addWidget(mTreeView);

    setWidget(widget);

//    this->setMaximumWidth(400);

	mFindStrEdit = new QLineEdit();
	mFindStrEdit->setGeometry(QRect(0, 0, this->width(), 25));
	layout->addWidget(mFindStrEdit);
	mFindStrEdit->setVisible(false);

	connect(mFindStrEdit, SIGNAL(returnPressed()), SLOT(onFindString()));
	connect(mModel, SIGNAL(layoutChanged(QList<QPersistentModelIndex>,QAbstractItemModel::LayoutChangeHint)), SLOT(layoutChanged()));
//	connect(mModel, SIGNAL(directoryLoaded(QString)), SLOT(directoryLoaded(QString)));
	connect(mModel, SIGNAL(fileRenamed(QString,QString,QString)), SLOT(fileRenamed(QString,QString,QString)));
	connect(mTreeView, SIGNAL(expanded(QModelIndex)), SLOT(expanded(QModelIndex)));

	connect(mTreeView, SIGNAL(onKeyPressEvent(QKeyEvent*)), SLOT(onKeyPressEvent(QKeyEvent*)));
	connect(mTreeView, SIGNAL(onKeyReleaseEvent(QKeyEvent*)), SLOT(onKeyReleaseEvent(QKeyEvent*)));
	connect(mTreeView, SIGNAL(onMousePressEvent(QMouseEvent*)), SLOT(onMousePressEvent(QMouseEvent*)));
	connect(mTreeView, SIGNAL(onMouseReleaseEvent(QMouseEvent*)), SLOT(onMouseReleaseEvent(QMouseEvent*)));

	connect(mTreeView, SIGNAL(doubleClicked(QModelIndex)), SLOT(onFileClicked(QModelIndex)));

	setWindowTitle(caption);

	this->setFeatures(DockWidgetFeature::DockWidgetMovable|DockWidgetFeature::DockWidgetFloatable);

	mContextMenu = new QMenu;

	q_action_copyPath		= new QAction("复制路径（CTRL+C）", this);
	q_action_findItem		= new QAction("查找文件（CTRL+F）", this);
	q_action_removeItem		= new QAction("删除（CTRL+D, Delete）", this);
	q_action_renameItem		= new QAction("重命名（F2）", this);
	q_action_showExplorer	= new QAction("打开文件所在位置（CTRL+O）", this);
	q_action_addCompare		= new QAction("加入对比（CTRL+P）", this);

	mContextMenu->addAction(q_action_copyPath);
	mContextMenu->addAction(q_action_findItem);
	mContextMenu->addAction(q_action_removeItem);
	mContextMenu->addAction(q_action_renameItem);
	mContextMenu->addAction(q_action_showExplorer);
//	mContextMenu->addAction(q_action_findRefrence);
	mContextMenu->addAction(q_action_addCompare);

	connect(q_action_copyPath, SIGNAL(triggered(bool)), this, SLOT(action_copyPath()));
	connect(q_action_findItem, SIGNAL(triggered(bool)), this, SLOT(action_findItem()));
	connect(q_action_removeItem, SIGNAL(triggered(bool)), this, SLOT(action_removeItem()));
	connect(q_action_renameItem, SIGNAL(triggered(bool)), this, SLOT(action_renameItem()));
	connect(q_action_showExplorer, SIGNAL(triggered(bool)), this, SLOT(action_showExplorer()));
//	connect(q_action_findRefrence, SIGNAL(triggered(bool)), this, SLOT(action_findRefrence()));
	connect(q_action_addCompare, SIGNAL(triggered(bool)), this, SLOT(action_addCompare()));

//	q_action_renameItem->setEnabled(bReplaceEnable);

//	if(bReplaceEnable) {
//		mReplaceThread = new ReplaceThread;
//		connect(mReplaceThread, SIGNAL(replaceStart(QString,QString,QString)), this, SLOT(onReplaceStart(QString,QString,QString)));
//		connect(mReplaceThread, SIGNAL(replaceDone(QString,int)), this, SLOT(onReplaceDone(QString,int)));
//		connect(mReplaceThread, SIGNAL(replaceRenamed(QString)), this, SLOT(onReplaceRenamed(QString)));
//		connect(mReplaceThread, SIGNAL(replaceOver(ReplaceThread::ReplaceResult*)), this, SLOT(onReplaceOver(ReplaceThread::ReplaceResult*)));
//	}

	this->setMouseTracking(true);

	mModel->setReadOnly(false);
	mTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);

//	mListViewDialog = new StringsListDialog(this);
}

TreeDock::~TreeDock()
{
	SAFE_DELETE(mModel);
	SAFE_DELETE(mTreeView);
	SAFE_DELETE(mFindStrEdit);
//	SAFE_DELETE(mReplaceThread);
//	SAFE_DELETE(mListViewDialog);

	SAFE_DELETE(q_action_copyPath);
	SAFE_DELETE(q_action_findItem);
	SAFE_DELETE(q_action_removeItem);
	SAFE_DELETE(q_action_renameItem);
	SAFE_DELETE(q_action_showExplorer);
//	SAFE_DELETE(q_action_findRefrence);
	SAFE_DELETE(q_action_addCompare);
	SAFE_DELETE(mContextMenu);
}

void TreeDock::setRootPath(const QString& rootPath, const QString& expand)
{
	mInitResPath = rootPath;

	if(mModel) {
		mModel = new QFileSystemModel;
		mTreeView->setModel(mModel);
	}
	mModel->setRootPath(mInitResPath);
//	mModel->setNameFilterDisables(false);
//	mModel->setNameFilters(fileFilter);
	mModel->setFilter(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::Files);
	mModel->setIconProvider(new MyFileIconProvider);

//	//先获取所有文件
//	_treeLock.lock();
//	GetAllFileByFilter(mFilesList, mFileSuffixList, mInitResPath.toStdString(), true);
//	_treeLock.unlock();

	setTreeNameFilter();

	QModelIndex index = mModel->index(mInitResPath);
	mTreeView->setRootIndex(index);

	mTreeView->setColumnWidth(0, 200);
	mTreeView->hideColumn(1); //type
	mTreeView->hideColumn(2); //size
//	mTreeView->hideColumn(3); //date

	if(expand.size() > 0) {
		QString expandPath = mInitResPath + "/" + expand;
		QModelIndex index = mModel->index(expandPath);
		mTreeView->expand(index);
	}

//	mTreeView->setHeaderHidden(true);
}

void TreeDock::refreshTree()
{
//    if(mModel)
//    {
//        mModel->refresh();
//    }
}

void TreeDock::onKeyPressEvent(QKeyEvent *event)
{
	if(event->key() == Qt::Key_Control)
	{
		bCtrlPressed = true;
		qDebug() << "bCtrlPressed = " << bCtrlPressed;
	}
}

void TreeDock::onKeyReleaseEvent(QKeyEvent *event)
{
	qDebug() << "keyReleaseEvent -- " << event->key();
	if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
		QModelIndex _index = mTreeView->currentIndex();
		if(mTreeView->model()->hasChildren(_index)) {
			if(mTreeView->isExpanded(_index)) {
				mTreeView->collapse(_index);
			} else {
				mTreeView->expand(_index);
			}
		} else {
			onFileClicked(_index);
		}
	} else if (event->key() == Qt::Key_Control) {
		bCtrlPressed = false;
		qDebug() << "bCtrlPressed = false";
	} else if (event->key() == Qt::Key_Escape) {
		if(mFindStrEdit->isVisible()) {
			onQuitFindString();
			this->setFocus();
		}
	} else if (bCtrlPressed) {
		if(event->key() == Qt::Key_C) {
			action_copyPath();
		} else if (event->key() == Qt::Key_F) {
			action_findItem();
		} else if (event->key() == Qt::Key_O) {
			action_showExplorer();
		} else if (event->key() == Qt::Key_D) {
			action_removeItem();
		} else if (event->key() == Qt::Key_R) {
//			action_findRefrence();
		} else if (event->key() == Qt::Key_P) {
			action_addCompare();
		}
	} else if (event->key() == Qt::Key_F2) {
		action_renameItem();
	} else if (event->key() == Qt::Key_Delete) {
		action_removeItem();
	}
}

void TreeDock::onMousePressEvent(QMouseEvent* event)
{

}

void TreeDock::onMouseReleaseEvent(QMouseEvent* event)
{
	if(event->button() == Qt::RightButton) {
		showContextMenu(QCursor::pos());
	}
}

void TreeDock::onFindString()
{
	QString str = mFindStrEdit->text();
	setTreeNameFilter(str);
}

void TreeDock::onQuitFindString()
{
	mFindStrEdit->setText("");
	mFindStrEdit->setVisible(false);
	_preFilterName.clear();
	_preFilterNameList.clear();
	_preFilterIndex = -1;
	setTreeNameFilter();
}

void TreeDock::setTreeNameFilter(const QString& sFilter)
{
	QModelIndex _preModelIndex = mTreeView->currentIndex();

	bool bExtraFilter = sFilter.length() > 0;
	QStringList fileFilter;
	for(auto& suffix : mFileSuffixList) {
		if(bExtraFilter) {
			fileFilter << QString::asprintf("*%s*.%s", sFilter.toStdString().c_str(), suffix.toStdString().c_str());
		} else {
			fileFilter << QString::asprintf("*.%s", suffix.toStdString().c_str());
		}
			}

	mModel->setNameFilterDisables(false);
	mModel->setNameFilters(fileFilter);

	_layoutChangedListener << [=]() {
		if(!bExtraFilter) {
			if(_preModelIndex.isValid()) {
				mTreeView->collapseAll();
				mTreeView->expand(_preModelIndex);
			}
		} else {
			expandToFirstFile(sFilter);
		}
	};
}

const std::list<std::string>& TreeDock::getFilesList()
{
	std::unique_lock<std::mutex> lk(_treeLock);
	Q_UNUSED(lk);
	return mFilesList;
}

void TreeDock::layoutChanged()
{
	qDebug() << "treeDock::layoutChanged!!!";
	for(auto listener : _layoutChangedListener) {
		listener();
	}

	_layoutChangedListener.clear();
}

//void treeDock::directoryLoaded(const QString& path)
//{
//	qDebug() << "treeDock::directoryLoaded >>> " << path;
//	std::unique_lock<std::mutex> lk(_treeLock);
//	Q_UNUSED(lk);

//	GetAllFileByFilter(mFilesList, mFileSuffixList, path.toStdString(), false);

//	//去重
//	mFilesList.sort();
//	mFilesList.unique();
//}

void TreeDock::expanded(const QModelIndex &index)
{
	if(index.isValid()) {
		qDebug() << "treeDock::expanded >>> " << mModel->filePath(index);
		mTreeView->scrollTo(index);
	}
}

void TreeDock::fileRenamed(const QString &path, const QString &oldName, const QString &newName)
{
	qDebug() << "fileRenamed >>> " << path << ", " << oldName << ", " << newName;
}

void TreeDock::showContextMenu(QPoint pos)
{
	mContextMenu->exec(pos);
	qDebug() << "treeDock::showContextMenu >>> ";
}

void TreeDock::expandToFirstFile(const QString& filename)
{
	auto& filesList = getFilesList();
	if(filename.length() == 0 || filesList.empty())
		return;

	if(_preFilterName != filename) {
		_preFilterNameList.clear();
		_preFilterName = filename;

		for(auto& file : filesList) {
			QString _path = QString::fromStdString(file);
			QString _name = _path.section('/', -1);
			if(_name.indexOf(filename) >= 0) {
				_preFilterNameList << _path;
			}
		}

		expandToFirstFile(filename);
	} else {
		QModelIndex index;
		if(_preFilterIndex < 0) {
			_preFilterIndex = 0;
		}

		if(_preFilterIndex < _preFilterNameList.size()) {
			index = mModel->index(_preFilterNameList.at(_preFilterIndex++));
		}

		if(index.isValid()) {
			mTreeView->setCurrentIndex(index);
			mTreeView->expand(index);
		} else {
			_preFilterIndex = -1;
			QMessageBox::information(this,QString(tr("")),QString(tr("找不到更多文件！")),QMessageBox::Ok);
		}
	}
}

void TreeDock::action_copyPath()
{
	QModelIndex _index = mTreeView->currentIndex();
	qDebug() << "treeDock::action_copyPath --- " << mModel->filePath(_index);

	if(_index.isValid()) {
		QClipboard* clipBoard = QApplication::clipboard();
		clipBoard->setText(mModel->filePath(_index));
	}
}

void TreeDock::action_findItem()
{
	qDebug() << "treeDock::action_findItem!";
	mFindStrEdit->setVisible(true);
	mFindStrEdit->setFocus();
}

void TreeDock::action_showExplorer()
{
	QModelIndex _index = mTreeView->currentIndex();
	qDebug() << "treeDock::action_showExplorer --- " << mModel->filePath(_index);

	if(_index.isValid()) {
		QString path = mModel->filePath(_index);
		path.replace("/", "\\");
		QProcess::startDetached("explorer /select,"+path);
	}
}

void TreeDock::action_removeItem()
{
	QModelIndex _index = mTreeView->currentIndex();
	qDebug() << "treeDock::action_removeItem --- " << mModel->filePath(_index);

	if(_index.isValid()) {
		QString ques = tr("确定要删除文件(%1)？\n\n（该操作将无法撤销！）");
		QString name = mModel->fileName(_index);
		auto button = QMessageBox::question(this,
											tr("删除"),
											ques.arg(name),
											QMessageBox::Yes|QMessageBox::No);
		if(button == QMessageBox::Yes)
		{
			mModel->remove(_index);
		}
	}
}

void TreeDock::action_renameItem()
{
	qDebug() << "treeDock::action_renameItem!";

	QModelIndex _index = mTreeView->currentIndex();
	if(_index.isValid()) {
		mTreeView->edit(_index);
	}
}

void TreeDock::action_addCompare()
{
	qDebug() << "treeDock::action_addCompare!";

	QModelIndex _index = mTreeView->currentIndex();
	if(_index.isValid()) {
		QFileInfo fileInfo(mModel->fileInfo(_index));
		if(fileInfo.isFile()) {
			emit addCompare(fileInfo.absoluteFilePath());
		}
	}
}

void TreeDock::onFileClicked(const QModelIndex &index)
{
	if(index.isValid()) {
		QFileInfo fileInfo(mModel->fileInfo(index));
		if(fileInfo.isFile()) {
			emit fileTriggerd(fileInfo.absoluteFilePath());
			qDebug() << "fileTriggerd" << fileInfo.absoluteFilePath();
		}
	}
}

void TreeDock::resizeEvent(QResizeEvent *event)
{
//	qDebug() << "treeDock::resizeEvent : " << this->width() << ", " << this->height();
}
