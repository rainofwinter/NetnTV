#pragma once
class Animation;
class EditorDocument;
class ListEditorWidget;
class PaneButton;

class ActionListModel : public QAbstractItemModel
{
	Q_OBJECT
public:
	ActionListModel();
	~ActionListModel();

	void setDocument(EditorDocument * document);
	virtual QStringList mimeTypes() const;
	virtual QMimeData * mimeData(const QModelIndexList & indexes) const;

	virtual QVariant data(const QModelIndex &index, int role) const;
	virtual Qt::ItemFlags flags(const QModelIndex &index) const;
	virtual Qt::DropActions supportedDropActions() const;
	virtual QVariant headerData(
		int section, Qt::Orientation orientation, int role) const;

	virtual QModelIndex index(int row, int column, const QModelIndex &parent) const;
	virtual QModelIndex parent(const QModelIndex &index) const;
	virtual int rowCount(const QModelIndex &parent) const;
	virtual int columnCount(const QModelIndex &parent) const;
	
	virtual bool dropMimeData(const QMimeData *data, 
		Qt::DropAction action, int row, int column, const QModelIndex &parent);

protected slots:
	void update();

private:
	EditorDocument * document_;
};

////////////////////////////////////////////////////////////////////////////////
class ActionListPane;
class AnimationChannel;

class ActionListDelegate : public QStyledItemDelegate
{
Q_OBJECT
public:
	ActionListDelegate(QTreeView * parent);
	virtual void paint(QPainter *painter, 
		const QStyleOptionViewItem &option, const QModelIndex &index) const;
	virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

	virtual QWidget * createEditor(QWidget * parent, const QStyleOptionViewItem & option, 
		const QModelIndex & index) const {return 0;}

private:
	QTreeView * parent_;
};

///////////////////////////////////////////////////////////////////////////////
class Action;

class ActionListTreeView : public QTreeView
{
Q_OBJECT
public:
	ActionListTreeView(ActionListPane * parent);
	~ActionListTreeView();
	void setDocument(EditorDocument * document);

protected slots:
	void updateSelection();
	void onListenerSelectionChanged();
	void onActionsCopy();
	void onActionsPaste();

protected:	
	virtual void selectionChanged(
		const QItemSelection &selected, const QItemSelection &deselected);

	virtual void mousePressEvent(QMouseEvent *event);

	void mouseDoubleClickEvent(QMouseEvent *event);

private:	
	QAction * actionsCopyAction_, * actionsPasteAction_;
	Action * clickedAction_;

	ActionListPane * parent_;
	EditorDocument * document_;	
	bool ignoreSelChanged_;

	QMenu * contextMenu_;
};

///////////////////////////////////////////////////////////////////////////////
class MainWindow;
class HeaderLabel;

class ActionListPane : public QWidget
{
Q_OBJECT
public:
	ActionListPane(MainWindow * mainWindow);
	~ActionListPane();
	void setDocument(EditorDocument * document);

public slots:
	void changeButtonsState();
	void onListenerSelectionChanged();
	void onNew();
	void onEdit();
	void onDelete();
protected:	
	virtual QSize sizeHint() const;
protected:
	ActionListTreeView * treeView_;
	ActionListModel * model_;
	ActionListDelegate * delegate_;
	EditorDocument * document_;

	QLabel * label_;

	PaneButton * newButton_, * editButton_, * delButton_;
};