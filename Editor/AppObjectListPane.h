#pragma once

class AppObjectListPane;
class EditorDocument;
class ListEditorWidget;
class PaneButton;
///////////////////////////////////////////////////////////////////////////////

class AppObjectListModel : public QAbstractItemModel
{
	Q_OBJECT
public:
	AppObjectListModel();
	~AppObjectListModel();

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

class AppObjectListDelegate : public QStyledItemDelegate
{
Q_OBJECT
public:
	AppObjectListDelegate(AppObjectListPane * parent);
	virtual void paint(QPainter *painter, 
		const QStyleOptionViewItem &option, const QModelIndex &index) const;
	virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

	virtual QWidget * createEditor(QWidget * parent, const QStyleOptionViewItem & option, 
		const QModelIndex & index) const;

	virtual void setModelData(QWidget *editor, 
		QAbstractItemModel *model, const QModelIndex &index) const;

	virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;

	void setDocument(EditorDocument * document);

	ListEditorWidget * GetCurEditor() const {return curEditor_;}
protected:
	
protected slots:
	
	void editorClosed(QWidget * editor, QAbstractItemDelegate::EndEditHint hint);
	void commitAndCloseEditor();
	void cancelEdit();

private:
	mutable ListEditorWidget * curEditor_;
	EditorDocument * document_;
	QPixmap visibleOnImg_, visibleOffImg_;	
	QPixmap lockOnImg_, lockOffImg_;

	//int thumbBorderMargin_, thumbNameMargin_;
	//SceneImageMap & sceneImageMap_;

	AppObjectListPane * parent_;
};

///////////////////////////////////////////////////////////////////////////////

class AppObjectListTreeView : public QTreeView
{
Q_OBJECT
public:
	AppObjectListTreeView(AppObjectListPane * parent);
	
	void setDocument(EditorDocument * document);
signals:
	void uiPropertiesChanged();
protected slots:
	void updateSelection();
	void updateAppObjectList();

protected:	
	void updateSelection(QModelIndex parentIndex);
	virtual void mousePressEvent(QMouseEvent *event);

	virtual void selectionChanged(
		const QItemSelection &selected, const QItemSelection &deselected);

private:	
	AppObjectListPane * parent_;
	EditorDocument * document_;
	bool ignoreSelChanged_;
	bool mousePressOnUiIcon_;
};

////////////////////////////////////////////////////////////////////////////////
class MainWindow;
class HeaderLabel;

class AppObjectListPane : public QWidget
{
Q_OBJECT
public:
	AppObjectListPane(MainWindow * mainWindow);
	~AppObjectListPane();
	void setDocument(EditorDocument * document);
	QSize sizeHint() const;
	MainWindow * parent() const {return mainWindow_;}
private:
	void setLabelText();
private slots:
	void changeButtonsState();
	void onNew();
	void onDelete();
	void updateScene();
protected:
	void focusInEvent(QFocusEvent * event);
	virtual void keyPressEvent(QKeyEvent * event);

protected:
	AppObjectListTreeView * treeView_;
	AppObjectListModel * model_;
	AppObjectListDelegate * delegate_;
	EditorDocument * document_;

	MainWindow * mainWindow_;

//	HeaderLabel * label_;
	QLabel * label_;

	PaneButton * newButton_, * editButton_, * delButton_;
};


