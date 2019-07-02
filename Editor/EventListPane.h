#pragma once
class Animation;
class EditorDocument;
class ListEditorWidget;
class PaneButton;

class EventListModel : public QAbstractItemModel
{
	Q_OBJECT
public:
	EventListModel();
	~EventListModel();

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
class EventListPane;
class AnimationChannel;

class EventListDelegate : public QStyledItemDelegate
{
Q_OBJECT
public:
	EventListDelegate(QTreeView * parent);
	virtual void paint(QPainter *painter, 
		const QStyleOptionViewItem &option, const QModelIndex &index) const;
	virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

	virtual QWidget * createEditor(QWidget * parent, const QStyleOptionViewItem & option, 
		const QModelIndex & index) const {return 0;}
private:
	QTreeView * parent_;
};

///////////////////////////////////////////////////////////////////////////////
class EventListTreeView : public QTreeView
{
Q_OBJECT
public:
	EventListTreeView(EventListPane * parent);
	
	void setDocument(EditorDocument * document);

protected slots:
	void updateSelection();
	void onObjectSelectionChanged();
protected:	
	virtual void selectionChanged(
		const QItemSelection &selected, const QItemSelection &deselected);

	void mouseDoubleClickEvent(QMouseEvent *event);
private:	
	EventListPane * parent_;
	EditorDocument * document_;
	bool ignoreSelChanged_;
};

///////////////////////////////////////////////////////////////////////////////
class MainWindow;
class HeaderLabel;

class EventListPane : public QWidget
{
Q_OBJECT
public:
	EventListPane(MainWindow * mainWindow);
	~EventListPane();
	void setDocument(EditorDocument * document);

	void setVScrollValue(int val);
public slots:
	void changeButtonsState();
	void updateObject();
	void updateAppObject();
	void onNew();
	void onDelete();
	void onEdit();
signals:
	void rangeChanged(int min, int max);
	void valueChanged(int value);
protected:	
	virtual QSize sizeHint() const;
protected:
	EventListTreeView * treeView_;
	EventListModel * model_;
	EventListDelegate * delegate_;
	EditorDocument * document_;

	QLabel * label_;

	PaneButton * newButton_, * delButton_, * editButton_;
};