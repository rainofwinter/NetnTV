#pragma once

class Scene;
class EditorDocument;
class ListEditorWidget;
class PaneButton;

class SceneListModel : public QAbstractItemModel
{
	Q_OBJECT
public:
	SceneListModel(QTreeView * treeView);
	~SceneListModel();

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


	void updateThumbnail(Scene * scene);

	void computeIndicesMap();

public slots:
	void onSceneListChanged();
	void update();
	void updateThumbnail();

private:
	QTreeView * treeView_;
	EditorDocument * document_;

	std::map<Scene *, int> sceneIndices_;
};

////////////////////////////////////////////////////////////////////////////////
class SceneListPane;
class TextureRenderer;
class Texture;
class Subscene;

class SceneListDelegate : public QStyledItemDelegate
{
Q_OBJECT
public:
	SceneListDelegate(QTreeView * parent, 
		SceneListPane * sceneListPane, SceneListModel * model);

	~SceneListDelegate();
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

	void deleteThumbs();
private:
	EditorDocument * document_;

	SceneListModel * model_;
	SceneListPane * sceneListPane_;

	std::map<Scene *, QPixmap *> sceneThumbs_;

	unsigned char * ssBuffer_;
	QTreeView * parent_;
};

///////////////////////////////////////////////////////////////////////////////
class SceneListTreeView : public QTreeView
{
Q_OBJECT
public:
	SceneListTreeView(SceneListPane * parent);
	~SceneListTreeView();
	
	void setDocument(EditorDocument * document);
	QModelIndex selectedSceneIndex() const {return selSceneIndex_;}

protected slots:
	void updateSelection();
	void onSceneProperties();
	void onSceneCopy();
	void onScenePaste();

protected:	
	virtual void selectionChanged(
		const QItemSelection &selected, const QItemSelection &deselected);
	virtual void mousePressEvent(QMouseEvent *event);
	
private:	
	QAction * sceneCopyAction_, * scenePasteAction_, * scenePropertiesAction_;
	Scene * clickedScene_;
	SceneListPane * parent_;
	EditorDocument * document_;
	bool ignoreSelChanged_;
	QModelIndex selSceneIndex_;

	QMenu * contextMenu_;
};

///////////////////////////////////////////////////////////////////////////////
class MainWindow;

class SceneListPane : public QWidget
{
Q_OBJECT
public:
	SceneListPane(MainWindow * mainWindow);
	~SceneListPane();
	void setDocument(EditorDocument * document);

	void setVScrollValue(int val);

	void asyncLoadUpdate();

	//thread 
	void operator ()();
	
	bool getScreenShot(
		const QModelIndex & index, Scene * scene, unsigned char * buffer);

	void requestScreenShot(const QModelIndex & index, Scene * scene);

	void resetScreenShots(bool clearScreenShots = true);

	unsigned int ssBufferSize() const {return ssBufferSize_;}

	MainWindow * parent() const {return mainWindow_;}

public slots:
	void triggerRedraw();
	void changeButtonsState();
	void onNew();
	void onDelete();
signals:
	void rangeChanged(int min, int max);
	void valueChanged(int value);
protected:	
	virtual QSize sizeHint() const;
	virtual void keyPressEvent(QKeyEvent * keyEvent);
	virtual void keyReleaseEvent(QKeyEvent * keyEvent);
	
	void newCachedSelScene(Scene * scene);
	bool doCachedScene(Scene * scene);

	void computeVisibleScenes(const QModelIndex & topLeftI, const QModelIndex & bottomLeftI);
	void pollScrollPos();

protected:
	SceneListTreeView * treeView_;
	SceneListModel * model_;
	SceneListDelegate * delegate_;
	EditorDocument * document_;

	PaneButton * newButton_, * delButton_;
	
	

	//these hold the same information, one stores information about order,
	//and one is easier to search
	std::deque<Scene *> scenesToScreenShot_;
	std::set<Scene *> scenesToScreenShotSet_;
	
	enum ScreenShotState
	{
		Initial,
		NewSceneToScreenShot
	} screenShotState_;

	Scene * curRequest_;
	std::vector<Subscene *> tempScreenShotScenes_;
	std::map<Scene *, Subscene *> cachedScenes_;
	Scene * curCachedSelScene_;

	std::map<Scene *, unsigned char *> screenShots_;
	int padding_;
	bool padCachedYet_;
	std::vector<int> padCacheIndices_;
	unsigned int ssBufferSize_;

	int curTopIndex_, curBottomIndex_;	

	std::set<Scene *> visibleScenes_;
	std::set<Scene *> almostVisibleScenes_;

	MainWindow * mainWindow_;

	bool isShiftKey_;
	bool isCtrlKey_;
};