#pragma once

class ActionTargetListPane;
class EditorDocument;
class SceneObject;
class PaneLabel;


///////////////////////////////////////////////////////////////////////////////

class ActionTargetListModel : public QAbstractItemModel
{
	Q_OBJECT
public:
	ActionTargetListModel();
	~ActionTargetListModel();

	void setDocument(EditorDocument * document);

	virtual QVariant data(const QModelIndex &index, int role) const;
	virtual Qt::ItemFlags flags(const QModelIndex &index) const;
	virtual QVariant headerData(
		int section, Qt::Orientation orientation, int role) const;

	virtual QModelIndex index(int row, int column, const QModelIndex &parent) const;
	virtual QModelIndex parent(const QModelIndex &index) const;
	virtual int rowCount(const QModelIndex &parent) const;
	virtual int columnCount(const QModelIndex &parent) const;
	
private:
	EditorDocument * document_;

};

////////////////////////////////////////////////////////////////////////////////

class ActionTargetListDelegate : public QStyledItemDelegate
{
Q_OBJECT
public:
	ActionTargetListDelegate(ActionTargetListPane * parent);
	virtual void paint(QPainter *painter, 
		const QStyleOptionViewItem &option, const QModelIndex &index) const;
	virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

	void setDocument(EditorDocument * document);


private:
	EditorDocument * document_;

	ActionTargetListPane * parent_;
};

///////////////////////////////////////////////////////////////////////////////

class ActionTargetListTreeView : public QTreeView
{
Q_OBJECT
public:
	ActionTargetListTreeView(ActionTargetListPane * parent);
	
	void setDocument(EditorDocument * document);
signals:
	void itemSelected(SceneObject *);
protected slots:
	void onActionTargetListChanged();

protected:	
	void updateSelection(QModelIndex parentIndex);

	virtual void selectionChanged(
		const QItemSelection &selected, const QItemSelection &deselected);
private:	
	ActionTargetListPane * parent_;
	EditorDocument * document_;
};

///////////////////////////////////////////////////////////////////////////////

class ActionTargetListPane : public QWidget
{
Q_OBJECT
public:
	ActionTargetListPane();
	~ActionTargetListPane();
	void setDocument(EditorDocument * document);
	QSize sizeHint() const;

	void selectItem(int index);
	void selectObject(SceneObject * object);
signals:
	void itemSelected(SceneObject *);

protected:
	ActionTargetListTreeView * treeView_;
	ActionTargetListModel * model_;
	ActionTargetListDelegate * delegate_;
	EditorDocument * document_;

	PaneLabel * label_;

};


