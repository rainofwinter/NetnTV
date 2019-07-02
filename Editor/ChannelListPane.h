#pragma once
class Animation;
class EditorDocument;
class ListEditorWidget;
class PaneButton;

class ChannelListModel : public QAbstractItemModel
{
	Q_OBJECT
public:
	ChannelListModel();
	~ChannelListModel();

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
class ChannelListPane;
class AnimationChannel;

class ChannelListDelegate : public QStyledItemDelegate
{
Q_OBJECT
public:
	ChannelListDelegate(QTreeView * parent);
	virtual void paint(QPainter *painter, 
		const QStyleOptionViewItem &option, const QModelIndex &index) const;
	virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

	virtual QWidget * createEditor(QWidget * parent, const QStyleOptionViewItem & option, 
		const QModelIndex & index) const {return 0;}
private:
	QString displayString(AnimationChannel * obj) const;
	QTreeView * parent_;
};

///////////////////////////////////////////////////////////////////////////////
class ChannelListTreeView : public QTreeView
{
Q_OBJECT
public:
	ChannelListTreeView(ChannelListPane * parent);
	~ChannelListTreeView();
	void setDocument(EditorDocument * document);

protected slots:
	void updateSelection();
	void onCopy();
	void onPaste();

protected:	
	virtual void selectionChanged(
		const QItemSelection &selected, const QItemSelection &deselected);
	virtual void mousePressEvent(QMouseEvent *event);

private:	
	QAction * copyAction_, * pasteAction_;
	AnimationChannel * clickedChannel_;

	ChannelListPane * parent_;
	EditorDocument * document_;
	bool ignoreSelChanged_;
	QMenu * contextMenu_;
};

///////////////////////////////////////////////////////////////////////////////
class MainWindow;
class HeaderLabel;

class ChannelListPane : public QWidget
{
Q_OBJECT
public:
	ChannelListPane(MainWindow * mainWindow);
	~ChannelListPane();
	void setDocument(EditorDocument * document);

	void setVScrollValue(int val);
public slots:
	void changeButtonsState();
	void updateObject();
	void animationSelectionChanged();
	void onNew();
	void onDelete();
signals:
	void rangeChanged(int min, int max);
	void valueChanged(int value);
protected:	
	virtual void keyPressEvent(QKeyEvent * event);
	virtual void keyReleaseEvent(QKeyEvent * event);
	virtual QSize sizeHint() const;
protected:
	ChannelListTreeView * treeView_;
	ChannelListModel * model_;
	ChannelListDelegate * delegate_;
	EditorDocument * document_;

	QLabel * label_;

	PaneButton * newButton_, * delButton_;
};