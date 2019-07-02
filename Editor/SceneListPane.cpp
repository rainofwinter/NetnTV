#include "stdafx.h"
#include "SceneListPane.h"
#include "EditorDocument.h"
#include "EditorObject.h"
#include "Animation.h"
#include "Command.h"
#include "Document.h"
#include "DocumentTemplate.h"
#include "Scene.h"
#include "SceneObject.h"
#include "ListEditorWidget.h"
#include "PaneButton.h"
#include "ScenePropertiesDlg.h"
#include "EditorGlobal.h"
#include "Utils.h"
#include "MainWindow.h"
#include "Subscene.h"
#include "Root.h"
#include "Camera.h"
#include "GLWidget.h"

using namespace std;
using namespace boost;
Q_DECLARE_METATYPE(Scene *)

SceneListModel::SceneListModel(QTreeView * treeView)
{
	document_ = 0;
	treeView_ = treeView;
}

SceneListModel::~SceneListModel()
{	
}

void SceneListModel::computeIndicesMap()
{
	sceneIndices_.clear();
	const vector<SceneSPtr> & scenes = document_->scenes();
	for (int i = 0; i < (int)scenes.size(); ++i)
	{
		sceneIndices_[scenes[i].get()] = i;
	}
}

void SceneListModel::setDocument(EditorDocument * document)
{
	if (document_) document_->disconnect(this);

	document_ = document;
	
	if (document_)
	{
		connect(document_, SIGNAL(sceneListChanged()),
			this, SLOT(onSceneListChanged()));
		
		//when the current scene is drawn, updateSelection will redraw the 
		//corresponding thumbnail
		connect(document_, SIGNAL(sceneDrawn()), 
			this, SLOT(updateThumbnail()));

		computeIndicesMap();

	}	
		
	emit layoutChanged();
}

void SceneListModel::update()
{
	emit layoutChanged();
}


void SceneListModel::onSceneListChanged()
{
	//note this is expensive so it should be called as infrequently as possible
	computeIndicesMap();
	emit layoutChanged();
}

void SceneListModel::updateThumbnail()
{
	const vector<SceneSPtr> & scenes= document_->scenes();
	int sceneIndex = -1;

	QModelIndex topLeftI = 
		treeView_->indexAt(treeView_->viewport()->rect().topLeft());

	QModelIndex bottomLeftI = 
		treeView_->indexAt(treeView_->viewport()->rect().bottomLeft());

	int minI = 0;
	int maxI = (int)scenes.size() - 1;

	if (topLeftI.row() >= 0) minI = topLeftI.row();
	if (bottomLeftI.row() >= 0) maxI = bottomLeftI.row();
	
	for (int i = minI; i <= maxI; ++i)
	{
		bool isScene = scenes[i].get() == document_->selectedScene();
		bool dependsOnScene = scenes[i]->dependsOn(document_->selectedScene());
		if (isScene || dependsOnScene)
		{		
			//trigger repaint of this item
			emit dataChanged(
				index(i, 0, QModelIndex()), index(i, 0, QModelIndex()));
		}
	}
}

void SceneListModel::updateThumbnail(Scene * scene)
{
	map<Scene *, int>::iterator iter = sceneIndices_.find(scene);

	if (iter != sceneIndices_.end())
	{
		int iIndex = (*iter).second;
		QModelIndex qIndex = index(iIndex, 0, QModelIndex());
		emit dataChanged(qIndex, qIndex);
	}
}

QStringList SceneListModel::mimeTypes() const
{
    QStringList types;
    types << QLatin1String("application/scene-ptr");
    return types;
}


QMimeData * SceneListModel::mimeData(const QModelIndexList & indexes) const
{
	QMimeData * mimeData = new QMimeData();
	QByteArray encodedData;

	QDataStream stream(&encodedData, QIODevice::WriteOnly);
	QModelIndexList sortedIndices = indexes;
	
	std::sort(sortedIndices.begin(), sortedIndices.end());

	foreach (QModelIndex index, sortedIndices) {
		if (index.isValid()) {
			QVariant variant = data(index, Qt::DisplayRole);

			if (variant.canConvert<Scene *>())
			{
				Scene * obj = variant.value<Scene *>();
				stream.writeRawData((char *)&obj, sizeof(Scene *));
			}						
		}
	}
     mimeData->setData("application/scene-ptr", encodedData);
     return mimeData;
}


QVariant SceneListModel::data(const QModelIndex & index, int role) const
{	
	if (!index.isValid()) return QVariant();

	if (role == Qt::DisplayRole) 
	{
		if (index.column() == 0)
		{
			Scene * obj = (Scene *)index.internalPointer();
			return QVariant::fromValue(obj);
		}
		else
			return QVariant();
	}
	return QVariant();
}

Qt::ItemFlags SceneListModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
	{
		return Qt::ItemIsDropEnabled;
	}

	Qt::ItemFlags flags = 0;	
	flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable |
		Qt::ItemIsDragEnabled;	
	
	return flags;
		
}

Qt::DropActions SceneListModel::supportedDropActions() const
{
	return Qt::MoveAction;
}

QVariant SceneListModel::headerData(
	int section, Qt::Orientation orientation, int role) const
{
	return tr("Scene");
}

QModelIndex SceneListModel::index(int row, int column, const QModelIndex &parent) const
{
	if (!hasIndex(row, column, parent))
		return QModelIndex();

	if (column == 0)
	{
		const vector<SceneSPtr> & objs = document_->scenes();

		if (row >= 0 && row < (int)objs.size())
			return createIndex(row, column, objs[row].get());
		else
			return QModelIndex();
	}	
	
	return QModelIndex();
}

QModelIndex SceneListModel::parent(const QModelIndex & index) const
{
	return QModelIndex();	
}

int SceneListModel::rowCount(const QModelIndex & parent) const
{	
	if (!document_ || parent.isValid()) return 0;

	return document_->scenes().size();	
}

int SceneListModel::columnCount(const QModelIndex & parent) const
{
	return 1;
}

bool SceneListModel::dropMimeData(const QMimeData * mimedata,
	Qt::DropAction action, int row, int column, const QModelIndex & parent)
{
	
	QStringList types = mimeTypes();
	QString format = types.at(0);
    QByteArray encoded = mimedata->data(format);
	QDataStream stream(&encoded, QIODevice::ReadOnly);
	
	vector<Scene *> objs;
	
	while (!stream.atEnd()) {
		Scene * obj;		
		stream.readRawData((char *)&obj, sizeof(Scene *));	
		objs.push_back(obj);
	}
	Scene * moveTarget = 0;

	QModelIndex idx = index(row, 0, parent);
	moveTarget = data(idx, Qt::DisplayRole).value<Scene *>();
	
	try
	{
	document_->doCommand(new MoveScenesCmd(
		document_->document(), objs, moveTarget));
	}
	catch(...) {}
	return true;

}

////////////////////////////////////////////////////////////////////////////////

int texWidth = 75;
int texHeight = 75;
SceneListDelegate::SceneListDelegate(
	QTreeView * parent, SceneListPane * sceneListPane, SceneListModel * model)
: 
QStyledItemDelegate()
{
	model_ = model;
	sceneListPane_ = sceneListPane;
	parent_ = parent;
	//textureRenderer_ = new TextureRenderer;
	//texture_ = new Texture;	


	ssBuffer_ = (unsigned char *)malloc(
		sceneListPane->ssBufferSize());
}


SceneListDelegate::~SceneListDelegate()
{
	//delete textureRenderer_;
	//delete texture_;
	free(ssBuffer_);
}

void SceneListDelegate::paint(QPainter *painter, 
	const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QStyleOptionViewItemV4 opt = option;
	opt.state &= ~QStyle::State_HasFocus;
	opt.state |= QStyle::State_Active;

	if (index.column() == 0)
	{
		Scene * scene = 
			index.data(Qt::DisplayRole).value<Scene *>();

		if (scene->name() == "Top")
		{
			int a = 4;
		}
		initStyleOption(&opt, index);

		painter->save();

		const QStyle * style = 
			opt.SO_DockWidget ? opt.widget->style() : QApplication::style();
		const QWidget * widget = opt.widget;
	
		style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, widget);

		QRect rect = opt.fontMetrics.tightBoundingRect(tr("A"));
		int fontHeight = rect.height();

		painter->setFont(opt.font);
		qreal x = opt.rect.x() + 5;				
		qreal y = opt.rect.y() + opt.rect.height()/2 + fontHeight / 2;
		bool ssGotten = sceneListPane_->getScreenShot(
			index, scene, ssBuffer_);
		if (ssGotten)
		{
			QImage image(ssBuffer_, texWidth, texHeight, QImage::Format_RGB16);

			painter->drawImage(opt.rect.x() + 5, opt.rect.y() + 5, image);
		}
		else 
		{
			sceneListPane_->requestScreenShot(index, scene);
			painter->setBrush(QColor(192, 192, 192));
			painter->drawRect(opt.rect.x() + 5, opt.rect.y() + 5, texWidth, texHeight);
		}
		painter->setBrush(Qt::NoBrush);
		painter->setPen(QPen(QColor(96, 96, 96, 255)));
		painter->drawRect(opt.rect.x() + 5, opt.rect.y() + 5, texWidth, texHeight);

		painter->setPen(QPen(QColor(0, 0, 0)));
		x += texWidth + 5;
		painter->drawText(x, y, stdStringToQString(scene->name()));
		
		painter->restore();
	}
	else
		QStyledItemDelegate::paint(painter, option, index);
}


QSize SceneListDelegate::sizeHint(
	const QStyleOptionViewItem &option, const QModelIndex &index) const
{		
	return QSize(0, 10 + texHeight);
}

//------------------------------------------------------------------------------

QWidget * SceneListDelegate::createEditor(QWidget * parent, 
	const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	if (index.column() == 0)
	{
		ListEditorWidget * widget = new ListEditorWidget(texWidth + 10, 0, parent);
			
		connect(widget, SIGNAL(editingFinished()),
			this, SLOT(commitAndCloseEditor()));

		connect(widget, SIGNAL(editingCancelled()),
			this, SLOT(cancelEdit()));

		return widget;
	}
	else  
		return QStyledItemDelegate::createEditor(parent, option, index);
}

void SceneListDelegate::setModelData(QWidget *editor, 
		QAbstractItemModel *model, const QModelIndex &index) const
{
	if (!document_) return;

	if (index.column() == 0)
	{
		Scene * obj = 
			index.model()->data(index, Qt::DisplayRole).value<Scene *>();

		ListEditorWidget * widget = 
			qobject_cast<ListEditorWidget *>(editor);
		
		string newName = qStringToStdString(widget->GetLineEdit()->text());

		if (newName != obj->name())
		{
			document_->doCommand(makeChangePropertyCmd(ChangeSceneProperty,
				obj, newName, &Scene::name, &Scene::setName));
		}		
		
	}
	else
		QStyledItemDelegate::setModelData(editor, model, index);
}

void SceneListDelegate::setEditorData(QWidget *editor, 
	const QModelIndex &index) const
{
	if (index.column() == 0)
	{
		Scene * obj = 
			index.model()->data(index, Qt::DisplayRole).value<Scene *>();

		ListEditorWidget * widget = 
			qobject_cast<ListEditorWidget *>(editor);

		widget->setText(obj->name());	

	}
	else
		QStyledItemDelegate::setEditorData(editor, index);
}

void SceneListDelegate::setDocument(EditorDocument * document)
{
	document_ = document;
}


void SceneListDelegate::commitAndCloseEditor()
{
	ListEditorWidget * editor = qobject_cast<ListEditorWidget *>(sender());
	emit commitData(editor);
	emit closeEditor(editor);
}

void SceneListDelegate::cancelEdit()
{
	ListEditorWidget * editor = qobject_cast<ListEditorWidget *>(sender());
	emit closeEditor(editor);
}

void SceneListDelegate::deleteThumbs()
{
	map<Scene *, QPixmap *>::iterator iter = sceneThumbs_.begin();
	
}

///////////////////////////////////////////////////////////////////////////////
void SceneListTreeView::setDocument(EditorDocument * document)
{
	if (document_) document_->disconnect(this);
	document_ = document;
	
	if (document_)
	{
		connect(document_, SIGNAL(sceneSelectionChanged()), 
			this, SLOT(updateSelection()));

		updateSelection();
	}
}

SceneListTreeView::SceneListTreeView(SceneListPane * parent)
{
	document_ = 0;
	parent_ = parent;
	ignoreSelChanged_ = false;
	setIndentation(0);
	setRootIsDecorated(false);
	//setFrameShape(QFrame::NoFrame);
	sceneCopyAction_ = new QAction(tr("Copy"), this);	
	connect(sceneCopyAction_, SIGNAL(triggered()), this, SLOT(onSceneCopy()));
	scenePasteAction_ = new QAction(tr("Paste"), this);
	connect(scenePasteAction_, SIGNAL(triggered()), this, SLOT(onScenePaste()));
	scenePropertiesAction_ = new QAction(tr("Properties..."), this);
	connect(scenePropertiesAction_, SIGNAL(triggered()), this, SLOT(onSceneProperties()));

	contextMenu_ = new QMenu(this);
	QPalette Pal(QApplication::palette());
	//get rid of weird drop shadow effect on disabled items
	Pal.setColor(QPalette::Light, QColor(92, 92, 92, 255));	
	contextMenu_->setPalette(Pal);

	contextMenu_->addAction(sceneCopyAction_);
	contextMenu_->addAction(scenePasteAction_);
	contextMenu_->addSeparator();	
	contextMenu_->addAction(scenePropertiesAction_);
}

SceneListTreeView::~SceneListTreeView()
{
	delete sceneCopyAction_;
	delete scenePasteAction_;
	delete scenePropertiesAction_;
	delete contextMenu_;
}

void SceneListTreeView::updateSelection()
{
	ignoreSelChanged_ = true;
	QItemSelectionModel * selModel = selectionModel();
	selModel->clearSelection();
		
	QAbstractItemModel * m = model();
	int numObjs = m->rowCount();
	for (int i = 0; i < numObjs; ++i)
	{
		QModelIndex index = m->index(i, 0);
		Scene * obj = (Scene *)index.internalPointer();
		if (document_->isSceneSelected(obj))
			selModel->select(index, QItemSelectionModel::Select);
	}

	ignoreSelChanged_ = false;
}

void SceneListTreeView::selectionChanged(
		const QItemSelection &selected, const QItemSelection &deselected)
{
	
	QTreeView::selectionChanged(selected, deselected);
	if (ignoreSelChanged_) return;

	QItemSelectionModel * selModel = selectionModel();
	QModelIndexList selIndexList = selModel->selectedRows(0);
	
	vector<Scene *> selObjs;
	if (!selIndexList.empty()) selSceneIndex_ = selIndexList.front();
	foreach(QModelIndex index, selIndexList)
	{
		Scene * selObj = index.data().value<Scene *>();
		selObjs.push_back(selObj);		
	}	

	//disallow scene selection from being empty
	if (selObjs.empty())
	{
		updateSelection();
	}
	else
	{
		Scene * prevSelScene = document_->selectedScene();
		vector<Scene *> prevSelScenes = document_->selectedScenes();
		document_->setSelectedScenes(selObjs);	
		if (document_->selectedScene() != prevSelScene)
		{
			document_->addCommand(new ChangeSceneCmd(
				document_, prevSelScenes, selObjs));
		}
		parent_->changeButtonsState();
	}
}

void SceneListTreeView::mousePressEvent(QMouseEvent *event)
{
	QTreeView::mousePressEvent(event);

	QModelIndex clickIndex = indexAt(event->pos());
	
	if (clickIndex.isValid())
		clickedScene_ = (Scene *)clickIndex.internalPointer();
	else clickedScene_ = 0;

	if (event->button() == Qt::RightButton)
	{
		sceneCopyAction_->setEnabled(clickedScene_ != 0);		
		scenePropertiesAction_->setEnabled(clickedScene_ != 0);
		scenePasteAction_->setEnabled(EditorGlobal::instance().pasteScenesAvailable());

		contextMenu_->popup(QCursor::pos());
	}
}

void SceneListTreeView::onSceneProperties()
{
	QWidget * dlgParent = this;
	while (dlgParent->parent()) dlgParent = dlgParent->parentWidget();
	ScenePropertiesDlg dlg(clickedScene_, dlgParent);
	if (dlg.exec() == QDialog::Accepted)
	{
		document_->doCommand(new ChangeScenesPropertiesCmd(
			document_->selectedScenes(), dlg.width(), dlg.height(), dlg.zoom(), dlg.color()));
	}
}

void SceneListTreeView::onSceneCopy()
{
	EditorGlobal::instance().copyScenes(document_->selectedScenes());
}

void SceneListTreeView::onScenePaste()
{
	Document * sceneDoc = document_->document();
	document_->doCommand(new PasteScenesCmd(
		document_, 
		document_->selectedScene(), 
		EditorGlobal::instance().pasteScenes(sceneDoc)));
}

///////////////////////////////////////////////////////////////////////////////
SceneListPane::SceneListPane(MainWindow * parent)
{
	mainWindow_ = parent;
	setWindowTitle(tr("Scenes"));

	padding_ = 30;
	document_ = 0;
	delegate_ = 0;
	treeView_ = new SceneListTreeView(this);
	model_ = 0;

	isShiftKey_ = false;
	isCtrlKey_ = false;

	treeView_->setDragEnabled(true);
	treeView_->setAcceptDrops(true);
	treeView_->setDropIndicatorShown(true);
	treeView_->setDefaultDropAction(Qt::MoveAction);
	treeView_->setDragDropMode(QAbstractItemView::InternalMove);
	treeView_->setSelectionMode(QAbstractItemView::ExtendedSelection);
	QBoxLayout * layout = new QBoxLayout(QBoxLayout::TopToBottom, this);

	layout->setMargin(0);
	layout->setSpacing(0);	
	layout->addWidget(treeView_);
	//layout->addSpacing(::GetSystemMetrics(SM_CYHSCROLL));

	QBoxLayout * btnLayout = new QBoxLayout(QBoxLayout::LeftToRight);
	btnLayout->setMargin(0);
	btnLayout->setSpacing(0);
	btnLayout->setAlignment(Qt::AlignRight);
	btnLayout->addWidget(newButton_ = new PaneButton(QPixmap(":/data/Resources/Scene_menu/scene_new.png"), QSize(30, 30), "New"));
	btnLayout->addWidget(delButton_ = new PaneButton(QPixmap(":/data/Resources/Scene_menu/scene_del.png"), QSize(30, 30), "Delete"));
	layout->addLayout(btnLayout);

	connect(treeView_->verticalScrollBar(), SIGNAL(rangeChanged(int, int)), 
		this, SIGNAL(rangeChanged(int, int)));

	connect(treeView_->verticalScrollBar(), SIGNAL(valueChanged(int)), 
		this, SIGNAL(valueChanged(int)));

	connect(newButton_, SIGNAL(clicked()), this, SLOT(onNew()));
	connect(delButton_, SIGNAL(clicked()), this, SLOT(onDelete()));

	ssBufferSize_ = (texWidth+1) * (texHeight+1) * 2;
	resetScreenShots();	
}

SceneListPane::~SceneListPane()
{
	resetScreenShots();

	delete model_;
	delete delegate_;
}

void SceneListPane::setDocument(EditorDocument * document)
{
	resetScreenShots();
	document_ = document;

	delete model_;
	model_ = new SceneListModel(treeView_);
	treeView_->setModel(model_);
	model_->setDocument(document);
	
	delete delegate_;
	delegate_ = new SceneListDelegate(treeView_, this, model_);
	delegate_->setDocument(document);	

	treeView_->setItemDelegate(delegate_);
	treeView_->setDocument(document);	

	if (document_)
	{
		setEnabled(true);
		connect(model_, SIGNAL(layoutChanged()), treeView_, SLOT(updateSelection()));
		connect(model_, SIGNAL(layoutChanged()), this, SLOT(changeButtonsState()));
		changeButtonsState();

		//indices might have changed, so we need to recompute them
		connect(model_, SIGNAL(layoutChanged()), this, SLOT(triggerRedraw()));

		setVScrollValue(document_->sceneListScrollPos());
	}
	else
	{
		setEnabled(false);
	}
}

void SceneListPane::triggerRedraw()
{
	resetScreenShots(false);
}

void SceneListPane::setVScrollValue(int val)
{
	treeView_->verticalScrollBar()->setValue(val);
}

void SceneListPane::newCachedSelScene(Scene * scene)
{
	map<Scene *, Subscene *>::iterator citer;
	for (citer = cachedScenes_.begin(); citer != cachedScenes_.end(); ++citer)	
	{
		Subscene * cached = (*citer).second;
		//save a static screen shot before deleting from the cache
		if (cached->isLoaded())
		{
			GLWidget * renderer = document_->glWidget();
			
			int index = -1;
			for (int i = 0; i < (int)document_->scenes().size(); ++i)
			{
				if (document_->scenes()[i].get() == cached->scene())
				{
					index = i;
					break;
				}
			}

			Scene * req = cached->scene();

			map<Scene *, unsigned char*>::iterator miter;
			
			unsigned char * ssBufferNew = 0;
			miter = screenShots_.find(req);
			if (miter != screenShots_.end())
				ssBufferNew = (*miter).second;
			else
			{
				ssBufferNew = (unsigned char *)malloc(ssBufferSize_);
				screenShots_[req] = ssBufferNew;
			}

			renderer->screenShot(
				ssBufferNew, cached->scene(), texWidth, texHeight, true);					
					
		}

		delete (*citer).second;		
	}
	cachedScenes_.clear();
	curCachedSelScene_ = scene;

	doCachedScene(scene);
}

bool SceneListPane::doCachedScene(Scene * scene)
{
	//cache the scene if it is the currently selected scene
	//Also cached scenes contained in the selected scene
	bool sceneAlreadyCached = 
		cachedScenes_.find(scene) != cachedScenes_.end();

	bool needsCaching = 
		scene->isShowing(document_->selectedScene()) || 
		scene == document_->selectedScene();

	if (!sceneAlreadyCached && needsCaching)
	{
		set<Scene *> showingScenes;
		showingScenes.insert(scene);
		scene->showingScenes(&showingScenes);		

		BOOST_FOREACH(Scene * scene, showingScenes)
		{
			if (cachedScenes_.find(scene) == cachedScenes_.end())
			{
				Subscene * newS = new Subscene;
				newS->init(document_->renderer());
				newS->setScene(scene->curContainerObject(), scene);
				cachedScenes_[scene] = newS;
			}
		}

		QString windowTitleName = QString::fromStdString(scene->name());
		this->setWindowTitle(tr("Scene - ") + windowTitleName);
		return true;
	}

	return sceneAlreadyCached;
}

void SceneListPane::computeVisibleScenes(
	const QModelIndex & topLeftI, const QModelIndex & bottomLeftI)
{
	visibleScenes_.clear();

	const vector<SceneSPtr> & scenes = document_->scenes();

	int topIndex = topLeftI.row();
	int bottomIndex = bottomLeftI.row();

	if (topIndex < 0) topIndex = 0;
	if (bottomIndex < 0) bottomIndex = (int)scenes.size() - 1;

	for (int i = topIndex; i <= bottomIndex; ++i)
		visibleScenes_.insert(scenes[i].get());


	almostVisibleScenes_.clear();

	int padTopIndex = topIndex - padding_;
	int padBottomIndex = bottomIndex + padding_;

	if (padTopIndex < 0) padTopIndex = 0;
	if (padBottomIndex >= (int)scenes.size()) padBottomIndex = (int)scenes.size() - 1;

	for (int i = padTopIndex; i < topIndex; ++i)
		almostVisibleScenes_.insert(scenes[i].get());

	for (int i = bottomIndex + 1; i <= padBottomIndex; ++i)
		almostVisibleScenes_.insert(scenes[i].get());
}

void SceneListPane::pollScrollPos()
{
	if (!document_) return;

	if (treeView_->verticalScrollBar())
		document_->setSceneListScrollPos(treeView_->verticalScrollBar()->value());
	else
		document_->setSceneListScrollPos(0);
}

void SceneListPane::asyncLoadUpdate()
{
	if (document_->selectedScene() != curCachedSelScene_)
	{		
		newCachedSelScene(document_->selectedScene());
	}

	map<Scene *, Subscene *>::iterator iter;
	for (iter = cachedScenes_.begin(); iter != cachedScenes_.end(); ++iter)
	{
		(*iter).second->asyncLoadUpdate();
	}

	if (screenShotState_ == Initial)
	{
		
		QModelIndex topLeftI = 
			treeView_->indexAt(treeView_->viewport()->rect().topLeft());

		QModelIndex bottomLeftI = 
			treeView_->indexAt(treeView_->viewport()->rect().bottomLeft());

		int topRow = topLeftI.row();
		int bottomRow = bottomLeftI.row();

		if (curTopIndex_ != topRow || curBottomIndex_ != bottomRow)
		{
			padCachedYet_ = false;
			curTopIndex_ = topRow;
			curBottomIndex_ = bottomRow;

			computeVisibleScenes(topLeftI, bottomLeftI);
			pollScrollPos();
		}

		//Prune ss requests that are for scenes that are not currently visible
		deque<Scene *>::iterator iter;
		if (!padCachedYet_)
		{
			for (iter = scenesToScreenShot_.begin(); iter != scenesToScreenShot_.end();)
			{
				Scene * scene = *iter;
				bool invisible = 
					visibleScenes_.find(scene) == visibleScenes_.end();					
				if (invisible)
				{				
					scenesToScreenShotSet_.erase(scene);
					iter = scenesToScreenShot_.erase(iter);				
				}
				else
					++iter;
			}
		}

		//prune cached screen shots that are for scenes that are not current 
		//visible, or almost visible
		
		map<Scene *, unsigned char *>::iterator ssiter;
		for (ssiter = screenShots_.begin(); ssiter != screenShots_.end();)
		{
			Scene * scene = (*ssiter).first;

			bool invisible = 
				visibleScenes_.find(scene) == visibleScenes_.end() &&
				almostVisibleScenes_.find(scene) == almostVisibleScenes_.end();
				
			if (invisible)
			{				
				free((*ssiter).second);
				screenShots_.erase(ssiter++);
			}
			else
				++ssiter;
		}

		//if there are no pending requests
		//go a little bit into the just beyond visible region of the list pane
		//and cache screenshots of those scenes
		if (scenesToScreenShot_.empty() && !padCachedYet_)
		{
			padCachedYet_ = true;
			const vector<SceneSPtr> & scenes = document_->scenes();

			int curTopIndex = topRow;
			int curBottomIndex = bottomRow;

			padCacheIndices_.clear();
			while(padCacheIndices_.size() < 2*padding_)
			{
				bool added = false;
				if (curTopIndex >= 0)
				{
					--curTopIndex;
					if (curTopIndex >= 0)
						padCacheIndices_.push_back(curTopIndex);
					added = true;
				}

				if (curBottomIndex >= 0 && curBottomIndex < (int)scenes.size())
				{
					++curBottomIndex;
					if (curBottomIndex < (int)scenes.size())
						padCacheIndices_.push_back(curBottomIndex);
					added = true;
				}

				if (!added) break;
			}


			BOOST_FOREACH(int i, padCacheIndices_)
			{
				requestScreenShot(
					model_->index(i, 0, QModelIndex()),
					scenes[i].get());
			}
			
		}


		//process next pending request

		Scene * req = 0;
		if (!scenesToScreenShot_.empty())
		{
			req = scenesToScreenShot_.front();
			scenesToScreenShot_.pop_front();
			scenesToScreenShotSet_.erase(req);
		}
		
		if (req)
		{
			curRequest_ = req;

			set<Scene *> screenShotScenes;
			req->showingScenes(&screenShotScenes);

			BOOST_FOREACH(Subscene * s, tempScreenShotScenes_) delete s;
			tempScreenShotScenes_.clear();

			Subscene * subscene = new Subscene;
			subscene->init(document_->renderer());
			subscene->setScene(req->curContainerObject(), req);
			tempScreenShotScenes_.push_back(subscene);
			BOOST_FOREACH(Scene * scene, screenShotScenes)
			{
				Subscene * subscene = new Subscene;
				subscene->init(document_->renderer());
				subscene->setScene(scene->curContainerObject(), scene);
				tempScreenShotScenes_.push_back(subscene);
			}		
			
			screenShotState_ = NewSceneToScreenShot;
		}			
	}
	else if (screenShotState_ == NewSceneToScreenShot)
	{
		bool everythingLoaded = true;

		BOOST_FOREACH(Subscene * subscene, tempScreenShotScenes_)
		{
			everythingLoaded &= subscene->scene()->asyncLoadUpdate();
			everythingLoaded &= subscene->asyncLoadUpdate();
		}

		Subscene * ssSubscene = tempScreenShotScenes_.front();
		Scene * ssScene = ssSubscene->scene();
		
		if (ssSubscene->isLoaded()) 
		{
			//take the actual screen shot
			GLWidget * renderer = document_->glWidget();

			unsigned char * ssBufferNew = 0;
		
			if(screenShots_.find(curRequest_) == screenShots_.end())
			{
				ssBufferNew = (unsigned char *)malloc(ssBufferSize_);
				screenShots_[curRequest_] = ssBufferNew;			
			}
			else
				ssBufferNew = screenShots_[curRequest_];
			
			renderer->screenShot(
				ssBufferNew, ssScene, texWidth, texHeight, true);
			//trigger Scene List pane thumbnail update for the newly captured 
			//scene 
			
			model_->updateThumbnail(curRequest_);
			screenShotState_ = Initial;
		}

		doCachedScene(ssScene);
	}
}


QSize SceneListPane::sizeHint() const
{
	return QSize(150,150);
}

void SceneListPane::changeButtonsState()
{
	QItemSelectionModel * selModel = treeView_->selectionModel();
	int numSel = 0;
	int numTotal = 0;
	if (selModel)
	{
		QModelIndexList selIndexList = selModel->selectedRows(0);
		numTotal = treeView_->model()->rowCount();
		if (!selIndexList.empty()) numSel = selIndexList.size();
	}
	
	delButton_->setEnabled(numSel > 0 && numTotal - numSel > 0);	
}


void SceneListPane::keyPressEvent(QKeyEvent * keyEvent)
{
	QWidget::keyPressEvent(keyEvent);

	if (keyEvent->key() == Qt::Key_Shift) isShiftKey_ = true;
	if (keyEvent->key() == Qt::Key_Control) isCtrlKey_ = true;

	/*
	if (isCtrlKey_ && isShiftKey_ && (keyEvent->key() == Qt::Key_N))
		onNew();
	*/
	if (keyEvent->key() == Qt::Key_Delete)
		onDelete();
}

void SceneListPane::keyReleaseEvent(QKeyEvent * keyEvent)
{
	QWidget::keyReleaseEvent(keyEvent);

	if (keyEvent->key() == Qt::Key_Shift) isShiftKey_ = false;
	if (keyEvent->key() == Qt::Key_Control) isCtrlKey_ = false;
}

void SceneListPane::onNew()
{
	int width = document_->selectedScene()->screenWidth();
	int height = document_->selectedScene()->screenHeight();
	document_->doCommand(new NewSceneCmd(document_, width, height));
}
//-----------------------------------------------------------------------------
struct SceneLinkage
{
	std::vector<SceneLinkage *> parents;
	std::vector<SceneLinkage *> children;
	Scene * scene;
};


typedef std::map<Scene *, SceneLinkage *> SceneLinkages;


SceneLinkage * makeSceneLinkageNode(Scene * scene, SceneLinkages * linkages)
{
	SceneLinkage * item = new SceneLinkage;
	item->scene = scene;
	SceneLinkages::iterator iter = linkages->find(scene);

	if (iter != linkages->end())
	{
		//already exists, return existing
		delete item;		
		return (*iter).second;
	}
	else
	{
		(*linkages)[scene] =item;

		//get children
		vector<Scene *> depScenes;
		scene->dependsOnScenes(&depScenes);
		BOOST_FOREACH(Scene * depScene, depScenes)
		{
			item->children.push_back(makeSceneLinkageNode(
				depScene, linkages));
		}

		
		return item;
	}
}

void addToEraseSet(
	std::set<SceneLinkage *> * eraseSet, 
	const std::set<Scene *> & delSet, 
	SceneLinkage * linkage)
{
	if (delSet.find(linkage->scene) == delSet.end()) return;
	eraseSet->insert(linkage);
	BOOST_FOREACH(SceneLinkage * child, linkage->children)
		addToEraseSet(eraseSet, delSet, child);
}

void SceneListPane::onDelete()
{
	SceneLinkages linkages;
	set<Scene *> selSet;
	set<Scene *> allScenes;
	set<Scene *> delSet;
	
	struct EraseReason
	{
		Scene * scene;
		std::string reason;
		EraseReason(Scene * scene, std::string reason) 
		{
			this->scene = scene; this->reason = reason;
		}
	};

	std::vector<EraseReason> reasons;

	BOOST_FOREACH(Scene * selScene, document_->selectedScenes())	
		selSet.insert(selScene);	

	delSet = selSet;

	BOOST_FOREACH(SceneSPtr scene, document_->scenes()) 
		allScenes.insert(scene.get());

	//create linkages list, fill in children
	BOOST_FOREACH(Scene * selScene, allScenes)
		makeSceneLinkageNode(selScene, &linkages);

	//fill in parents
	typedef std::pair<Scene *, SceneLinkage *> SceneLinkagePair;
	BOOST_FOREACH(SceneLinkagePair item, linkages)
	{
		BOOST_FOREACH(SceneLinkage * child, item.second->children)
			child->parents.push_back(item.second);
	}

	set<Scene *>::iterator itert;
	for (itert = delSet.begin(); itert != delSet.end();)
	{		
		Scene * retScene = 
			document_->document()->documentTemplate()->dependsOn(*itert);
		if (retScene)
		{
			if (retScene == *itert)
			{
				reasons.push_back(EraseReason(*itert, "referenced in document template"));
			}
			else
			{
				stringstream ss;
				ss << "referenced in " << retScene->name() << " which is referenced in the document template";
				reasons.push_back(EraseReason(*itert, ss.str()));
			}
			
			
			delSet.erase(itert++);
		}	
		else
			++itert;
	}

	

	set<SceneLinkage *> eraseSet;
	
	set<Scene *>::iterator iter = delSet.begin();
	for (; iter != delSet.end(); ++iter)
	{
		Scene * delScene = *iter;
		SceneLinkage * linkage = linkages[delScene];
		bool hasNonSelParent = false;

		BOOST_FOREACH(SceneLinkage * parent, linkage->parents)
			if (delSet.find(parent->scene) == delSet.end()) hasNonSelParent = true;			
		
		if (hasNonSelParent)		
		{
			addToEraseSet(&eraseSet, delSet, linkage);
		}		
	}

	BOOST_FOREACH(SceneLinkage * eraseItem, eraseSet)
	{
		stringstream ss;
		ss << "referenced in the following scenes: ";

		for (int i = 0; i < (int)eraseItem->parents.size(); ++i)
		{
			ss << eraseItem->parents[i]->scene->name();
			if (i < (int)eraseItem->parents.size() - 1)
				ss << ", ";
		}

		reasons.push_back(EraseReason(eraseItem->scene, ss.str()));
		delSet.erase(eraseItem->scene);
	}
	
	if (!reasons.empty())
	{
		stringstream ss;
		ss << "Couldn't delete following scenes:\n";
		BOOST_FOREACH(EraseReason & reason, reasons)
		{
			ss << reason.scene->name() << " : ";
			ss << reason.reason << "\n";
		}

		QMessageBox::information(0, tr("Message"), stdStringToQString(ss.str()));
	}


	if (linkages.size() > 0)
	{	
		std::vector<Scene *> delScenes;
		BOOST_FOREACH(Scene * delScene, delSet)
			delScenes.push_back(delScene);

		try
		{
		
		document_->doCommand(new DeleteScenesCmd(
			document_, delScenes));
		} catch (Exception & e)
		{
			QMessageBox::information(0, tr("Error"), tr(e.what()));
		}
	}
	

	//cleanup
	BOOST_FOREACH(SceneLinkagePair item, linkages)
	{
		delete item.second;
	}
}

//-----------------------------------------------------------------------------

bool SceneListPane::getScreenShot(
	const QModelIndex & index, Scene * scene, unsigned char * destBuffer)
{
	doCachedScene(scene);

	map<Scene *, unsigned char *>::iterator ssiter;

	map<Scene *, Subscene *>::iterator iter = cachedScenes_.find(scene);
	bool sceneCached = false;
	if (iter != cachedScenes_.end())
	{
		sceneCached = true;
		Subscene * cached = (*iter).second;
		if (cached->isLoaded())
		{
			GLWidget * renderer = document_->glWidget();
			renderer->screenShot(
				destBuffer, cached->scene(), texWidth, texHeight, true);

			//save screenshot as well
			/*
			bug fix, when a new scene is created, the rendering will revert to
			the saved screen shot, so we must keep the saved screen shot 
			current
			*/
			ssiter = screenShots_.find(cached->scene());
			if (ssiter != screenShots_.end())
			{
				unsigned char * buffer = (*ssiter).second;
				memcpy(buffer, destBuffer, ssBufferSize_);
				return true;
			}

			return true;			
		}
	}

	
	ssiter = screenShots_.find(scene);
	unsigned char * buffer = 0;

	if (ssiter != screenShots_.end())
	{
		buffer = (*ssiter).second;
		memcpy(destBuffer, buffer, ssBufferSize_);
		return true;
	}

	return false;
}


void SceneListPane::requestScreenShot(
	const QModelIndex & index, Scene * scene)
{	
	//bail if scene already exists
	if (scenesToScreenShotSet_.find(scene) != scenesToScreenShotSet_.end())
		return;
	
	scenesToScreenShot_.push_back(scene);
	scenesToScreenShotSet_.insert(scene);
}

void SceneListPane::resetScreenShots(bool clearScreenShots)
{
	
	scenesToScreenShot_.clear();
	scenesToScreenShotSet_.clear();
	
	BOOST_FOREACH(Subscene * subscene, tempScreenShotScenes_) delete subscene;
	tempScreenShotScenes_.clear();

	map<Scene *, Subscene *>::iterator citer;
	for (citer = cachedScenes_.begin(); citer != cachedScenes_.end(); ++citer)	
		delete (*citer).second;		
	cachedScenes_.clear();
	curCachedSelScene_ = 0;
		
	if (clearScreenShots)
	{
		map<Scene *, unsigned char *>::iterator iter;
		for (iter = screenShots_.begin(); iter != screenShots_.end(); ++iter)	
			free((*iter).second);		
		screenShots_.clear();
	}
		
	padCachedYet_ = false;
	curTopIndex_ = -1;
	curBottomIndex_ = -1;
		
	screenShotState_ = Initial;
}