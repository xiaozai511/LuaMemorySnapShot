#ifndef MYTREEVIEW_H
#define MYTREEVIEW_H

#include <QTreeView>
#include <QMenu>
#include <QMouseEvent>
#include <QObject>

class MyTextEditorDelegate;

class MyTreeView : public QTreeView
{
	Q_OBJECT
	friend class MyTextEditorDelegate;

public:
	MyTreeView(QWidget *parent = Q_NULLPTR);
	virtual ~MyTreeView();

	bool isEditing();

signals:
	//将事件转发出去，统一在dock中处理
	void onKeyPressEvent(QKeyEvent *event);
	void onKeyReleaseEvent(QKeyEvent *event);
	void onMousePressEvent(QMouseEvent *event);
	void onMouseReleaseEvent(QMouseEvent *event);

protected:
	virtual void keyPressEvent(QKeyEvent* event);
	virtual void keyReleaseEvent(QKeyEvent* event);
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);

protected:
	void setEditing(bool bEditing);

private:
	MyTextEditorDelegate* mDelegage;
	bool bIsEditing;
};

#endif // MYTREEVIEW_H
