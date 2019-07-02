#pragma once

class Scene;
class SceneObject;
class SceneObjectListPane;
class EditorDocument;
class EditorCustomObject;
class ListEditorWidget;
class PaneButton;

class PaneMenuIconStyle;
class EditorImage;
///////////////////////////////////////////////////////////////////////////////

class SceneObjectListModel : public QAbstractItemModel
{
	Q_OBJECT
public:
	SceneObjectListModel();
	~SceneObjectListModel();

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

class SceneObjectListDelegate : public QStyledItemDelegate
{
Q_OBJECT
public:
	SceneObjectListDelegate(SceneObjectListPane * parent);
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

	SceneObjectListPane * parent_;
};

///////////////////////////////////////////////////////////////////////////////

class SceneObjectListTreeView : public QTreeView
{
Q_OBJECT
public:
	SceneObjectListTreeView(SceneObjectListPane * parent);
	
	void setDocument(EditorDocument * document);
signals:
	void uiPropertiesChanged();
protected slots:
	void updateSelection();
	void updateSceneObjectList();

protected:	
	void updateSelection(QModelIndex parentIndex);
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseDoubleClickEvent(QMouseEvent * event);
	virtual void mouseReleaseEvent(QMouseEvent *event);
	virtual void mouseMoveEvent(QMouseEvent * event);

	virtual void selectionChanged(
		const QItemSelection &selected, const QItemSelection &deselected);

	virtual void paintEvent(QPaintEvent *event);

	virtual void dragEnterEvent(QDragEnterEvent *event);


private:	
	SceneObjectListPane * parent_;
	EditorDocument * document_;
	bool ignoreSelChanged_;
	bool mousePressOnUiIcon_;
};

///////////////////////////////////////////////////////////////////////////////

class SceneObjectListPaneTopBar : public QWidget
{
public:
	SceneObjectListPaneTopBar();
	~SceneObjectListPaneTopBar();
	virtual QSize sizeHint() const {return QSize(20, 20);}
	void setRightMargin(int size);
private:
	
	virtual void paintEvent(QPaintEvent * event);
	QPixmap eyeImg_, lockImg_;

	int rightMargin_;
};
///////////////////////////////////////////////////////////////////////////////
class MainWindow;
class HeaderLabel;

class SceneObjectListPane : public QWidget
{
Q_OBJECT
public:
	SceneObjectListPane(MainWindow * mainWindow);
	~SceneObjectListPane();

	void setDocument(EditorDocument * document);
	QSize sizeHint() const;
	MainWindow * parent() const {return mainWindow_;}

protected:
	virtual void keyPressEvent(QKeyEvent *);
	virtual void keyReleaseEvent(QKeyEvent *);
private:
	void setLabelText();
private slots:
	void onExpanded(const QModelIndex &index);
	void onCollapsed(const QModelIndex &index);
	void changeButtonsState();
	void onNew();
	void onDelete();
	void updateScene();
	void onQuickNew(QAction * action);
	void onNewImage();
	void onNewGroup();
	void onNewRectangle();
	void onNewSubScene();
	void onNewText();

protected:
	SceneObjectListTreeView * treeView_;
	SceneObjectListModel * model_;
	SceneObjectListDelegate * delegate_;
	EditorDocument * document_;

	MainWindow * mainWindow_;

//	HeaderLabel * label_;
	QLabel * label_;
	QMenu * newObjectMenu_;
	QToolButton * newObjectPopupBtn_;
	PaneButton * newImageBtn_, * newGroupBtn_, * newRectangleBtn_, * newSubSceneBtn_, * newTextBtn_;
	PaneButton * newButton_, * editButton_, * delButton_;

	QAction * newImage_, * newImageChanger_, * newImageGrid_, * newAudio_, * newVideo_, 
		* newText_, * newGroup_, * newRectangle_, * newSubScene_, * newSceneChanger_, 
		* newMultiChoiceQuestion_, * newPanorama_, * newTurntable_;

	PaneMenuIconStyle * iconStyle_;

	bool isCtrlKey_;
};


