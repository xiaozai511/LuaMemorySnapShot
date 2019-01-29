#include "mytreeview.h"
#include <qdebug.h>
#include <QStyledItemDelegate>
#include <QMessageBox>
#include <QFileInfo>
#include <QLineEdit>

class MyTextEditorDelegate : public QStyledItemDelegate
{
public:
	explicit MyTextEditorDelegate(QObject *parent = Q_NULLPTR) :
		mStartSuffix(new QString()),
		QStyledItemDelegate(parent)
	{
	}

	~MyTextEditorDelegate()
	{
		delete (mStartSuffix);
	}

	virtual void setEditorData(QWidget *editor, const QModelIndex &index) const
	{
		QStyledItemDelegate::setEditorData(editor, index);
		QString text = index.model()->data(index).toString();

		//获取后缀名
		QFileInfo tmp(text);
		*mStartSuffix = tmp.suffix();

		MyTreeView* _par = dynamic_cast<MyTreeView*>(parent());
		if(_par) {
			_par->setEditing(true);
		}
	}

	virtual void setModelData(QWidget *editor,
							  QAbstractItemModel *mModel,
							  const QModelIndex &index) const
	{
		QLineEdit* textBox = dynamic_cast<QLineEdit*>(editor);
		QString text = textBox->text();

		//获取后缀名
		QFileInfo tmp(text);
		QString suffix = tmp.suffix();

		//不允许修改扩展名
		if(*mStartSuffix != suffix) {
			QMessageBox::information(nullptr,QString(tr("重命名失败")),QString(tr("不允许修改扩展名，重命名失败，请重试！")),QMessageBox::Ok);
			return;
		} else {
			mModel->setData(index, text);
		}
	}

	virtual void destroyEditor(QWidget *editor, const QModelIndex &index) const
	{
		qDebug() << "destroyEditor";
		MyTreeView* _par = dynamic_cast<MyTreeView*>(parent());
		if(_par) {
			_par->setEditing(false);
		}
	}

protected:

	QString* mStartSuffix;
};

MyTreeView::MyTreeView(QWidget *parent):
	QTreeView(parent),
	bIsEditing(false)
{
	mDelegage = new MyTextEditorDelegate(this);
	setItemDelegate(mDelegage);
}

MyTreeView::~MyTreeView()
{
	delete mDelegage;
}

bool MyTreeView::isEditing()
{
	return bIsEditing;
}

void MyTreeView::setEditing(bool bEditing)
{
	bIsEditing = bEditing;
}

void MyTreeView::keyPressEvent(QKeyEvent *event)
{
	QTreeView::keyPressEvent(event);
	if(!isEditing()) {	//非编辑状态才发消息
		emit onKeyPressEvent(event);
	}
}

void MyTreeView::keyReleaseEvent(QKeyEvent *event)
{
	QTreeView::keyReleaseEvent(event);
	if(!isEditing()) {	//非编辑状态才发消息
		emit onKeyReleaseEvent(event);
	}
}

void MyTreeView::mousePressEvent(QMouseEvent *event)
{
	QTreeView::mousePressEvent(event);
	if(!isEditing()) {	//非编辑状态才发消息
		emit onMousePressEvent(event);
	}
}

void MyTreeView::mouseReleaseEvent(QMouseEvent *event)
{
	QTreeView::mouseReleaseEvent(event);
	if(!isEditing()) {	//非编辑状态才发消息
		emit onMouseReleaseEvent(event);
	}
}
