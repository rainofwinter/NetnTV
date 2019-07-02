#pragma once
class EditorDocument;
class ListEditorWidget;
class PaneButton;
class AnimationChannel;

class AnimationListItem
{
public:
	AnimationListItem(const QList<QVariant> &data, AnimationListItem *parent = 0);
	~AnimationListItem();

	void appendChild(AnimationListItem *child);

	AnimationListItem * child(int row);
	int childCount() const;
	int columnCount() const;
	QVariant data(int column) const;
	int row() const;
	AnimationListItem * parent();

private:
	QList<AnimationListItem *> childItems;
	QList<QVariant> itemData;
	AnimationListItem * parentItem;
};

class AnimationListModel : public QAbstractItemModel
{
	Q_OBJECT
public:
	AnimationListModel();
	~AnimationListModel();

	void setDocument(EditorDocument * document);
	void setupModelData(AnimationListItem * parent);

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
	AnimationListItem * animationListItem;
};



////////////////////////////////////////////////////////////////////////////////
class AnimationListPane;

class AnimationListDelegate : public QStyledItemDelegate
{
Q_OBJECT
public:
	AnimationListDelegate(AnimationListPane * parent);
	virtual void paint(QPainter *painter, 
		const QStyleOptionViewItem &option, const QModelIndex &index) const;
	virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

	virtual QWidget * createEditor(QWidget * parent, const QStyleOptionViewItem & option, 
		const QModelIndex & index) const;

	virtual void setModelData(QWidget *editor, 
		QAbstractItemModel *model, const QModelIndex &index) const;

	virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;

	void setDocument(EditorDocument * document);

protected slots:

	void commitAndCloseEditor();
	void cancelEdit();

private:
	EditorDocument * document_;

	//int thumbBorderMargin_, thumbNameMargin_;
	//SceneImageMap & sceneImageMap_;

	AnimationListPane * parent_;

	QPixmap repeatOnImg_, repeatOffImg_;
};


///////////////////////////////////////////////////////////////////////////////

class AnimationListTreeView : public QTreeView
{
Q_OBJECT
public:
	AnimationListTreeView(AnimationListPane * parent);
	~AnimationListTreeView();
	
	void setDocument(EditorDocument * document);

protected slots:
	void layoutChanged();
	void updateSelection();
	void onProperties();

protected:	
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseMoveEvent(QMouseEvent * event);
	virtual void mouseDoubleClickEvent(QMouseEvent * event);
	virtual void expanded(const QModelIndex &index);
	virtual void collapsed(const QModelIndex &index);
		
	virtual void selectionChanged(
		const QItemSelection &selected, const QItemSelection &deselected);
private:	
	AnimationListPane * parent_;
	EditorDocument * document_;
	bool ignoreSelChanged_;

	bool mousePressOnUiIcon_;

	Animation * clickedAnimation_;
	QAction * propertiesAction_;

	QMenu * contextMenu_;
};

///////////////////////////////////////////////////////////////////////////////
class MainWindow;
class HeaderLabel;

class AnimationListPane : public QWidget
{
Q_OBJECT
public:
	AnimationListPane(MainWindow * mainWindow);
	~AnimationListPane();
	void setDocument(EditorDocument * document);


public slots:
	void changeButtonsState();
	void onNew();
	void onDelete();
	void updateScene();
protected:
	virtual QSize sizeHint() const;
	virtual void keyPressEvent(QKeyEvent * event);
	virtual void keyReleaseEvent(QKeyEvent * event);
protected:
	AnimationListTreeView * treeView_;
	AnimationListModel * model_;
	AnimationListDelegate * delegate_;
	EditorDocument * document_;

	QLabel * label_;

	PaneButton * newButton_, * editButton_, * delButton_;
};