#include "stdafx.h"
#include "SceneObjectListPane.h"
#include "EditorDocument.h"
#include "EditorGroup.h"
#include "EditorGlobal.h"
#include "Scene.h"
#include "Root.h"
#include "Group.h"
#include "Types.h"
#include "ListEditorWidget.h"
#include "Command.h"
#include "PaneButton.h"
#include "NewObjectDlg.h"
#include "Image.h"
#include "Utils.h"
#include "FileUtils.h"
#include "Animation.h"
#include "MainWindow.h"
#include "SceneObject.h"
#include "CustomObject.h"
#include "MaskObject.h"
#include "PaneMenuIconStyle.h"

using namespace std;

Q_DECLARE_METATYPE(SceneObject *)

SceneObjectListModel::SceneObjectListModel()
{
	document_ = 0;
}

SceneObjectListModel::~SceneObjectListModel()
{
}

void SceneObjectListModel::setDocument(EditorDocument * document)
{
	if (document_) document_->disconnect(this);
	document_ = document;
	emit layoutChanged();

	if (document_)
	{
		connect(document_, SIGNAL(sceneSelectionChanged()),
			this, SLOT(update()));

		connect(document_, SIGNAL(objectListChanged()), 
			this, SLOT(update()));

		connect(document_, SIGNAL(objectChanged()), 
			this, SLOT(update()));

		connect(document_, SIGNAL(animationSelectionChanged()), 
			this, SLOT(update()));
	}
}

QStringList SceneObjectListModel::mimeTypes() const
{
    QStringList types;
    types << QLatin1String("application/x-scene-object-ptr");
	types << QLatin1String("text/uri-list");
    return types;
}


class IndexComparer
{
public:
	bool operator ()(QModelIndex lhs, QModelIndex rhs) const
	{
		if (lhs.parent() == rhs.parent()) return lhs < rhs;

		lParents.clear();		
		QModelIndex curLIndex = lhs;
		while (curLIndex.isValid())
		{
			lParents.push_back(curLIndex);
			curLIndex = curLIndex.parent();
		}
		lParents.push_back(QModelIndex());
		int lIndex = lParents.size() - 1;
		/////
		rParents.clear();
		QModelIndex curRIndex = rhs;
		while (curRIndex.isValid())
		{
			rParents.push_back(curRIndex);
			curRIndex = curRIndex.parent();
		}
		rParents.push_back(QModelIndex());		
		int rIndex = rParents.size() - 1;
		/////
		while(lIndex > 0 && rIndex > 0)
		{
			if (lParents[lIndex] != rParents[rIndex]) break;
			lIndex--;
			rIndex--;
		}

		if (lParents[lIndex]!= rParents[rIndex]) 
			return lParents[lIndex] < rParents[rIndex];
		
		return lParents.size() < rParents.size();	
		
	}
private:
	mutable vector<QModelIndex> lParents;
	mutable vector<QModelIndex> rParents;
};

QMimeData * SceneObjectListModel::mimeData(const QModelIndexList & indexes) const
{
	QMimeData * mimeData = new QMimeData();
	QByteArray encodedData;

	QDataStream stream(&encodedData, QIODevice::WriteOnly);
	QModelIndexList sortedIndices = indexes;
	
	std::sort(sortedIndices.begin(), sortedIndices.end(), IndexComparer());

	BOOST_REVERSE_FOREACH (QModelIndex index, sortedIndices) {
		if (index.isValid()) {
			QVariant variant = data(index, Qt::DisplayRole);

			if (variant.canConvert<SceneObject *>())
			{
				SceneObject * obj = variant.value<SceneObject *>();
				stream.writeRawData((char *)&obj, sizeof(SceneObject *));
			}						
		}
	}
     mimeData->setData("application/x-scene-object-ptr", encodedData);
     return mimeData;
}


QVariant SceneObjectListModel::data(const QModelIndex & index, int role) const
{	
	if (!index.isValid()) return QVariant();

	if (role == Qt::DisplayRole) 
	{
		if (index.column() == 0)
		{
			SceneObject * obj = (SceneObject *)index.internalPointer();
			return QVariant::fromValue(obj);
		}
		else
			return QVariant();
	}
	
	return QVariant();
}

Qt::ItemFlags SceneObjectListModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
	{
		return Qt::ItemIsDropEnabled;
	}

	Qt::ItemFlags flags = 0;	
	SceneObject * obj = static_cast<SceneObject *>(index.internalPointer());

	flags = Qt::ItemIsEnabled | Qt::ItemIsEditable |
			Qt::ItemIsDragEnabled;

	if (obj->parent())
	{	
		MaskObject * parentMaskObj = obj->parent()->maskObject();
		if (parentMaskObj && parentMaskObj->maskingObjectGroup() == obj)
		{
			flags &= ~(Qt::ItemIsDragEnabled | Qt::ItemIsEditable);
		}
	}
	
	EditorObject *edObj = document_->editorObject(obj).get();
	if (!edObj || edObj->isThisAndAncestorsUnlocked())
	{		
		flags |= Qt::ItemIsSelectable;		
		if (obj->group() || obj->maskObject())
			flags |= Qt::ItemIsDropEnabled;
	}
	return flags;
}

Qt::DropActions SceneObjectListModel::supportedDropActions() const
{
	return Qt::MoveAction | Qt::CopyAction;
}

QVariant SceneObjectListModel::headerData(
	int section, Qt::Orientation orientation, int role) const
{
	return tr("Object");
}

QModelIndex SceneObjectListModel::index(int row, int column, const QModelIndex &parent) const
{
	if (!hasIndex(row, column, parent))
		return QModelIndex();

	if (column != 0) return QModelIndex();

	SceneObject * parentObj;
	if (parent.isValid()) parentObj = (SceneObject *)parent.internalPointer();
	else parentObj = (SceneObject *)document_->selectedScene()->root();		

	int numChildren = parentObj->children().size();
	
	MaskObject * maskObj = parentObj->maskObject();
	if (maskObj)
	{
		if (row == 0)
			return createIndex(row, column, maskObj->maskingObjectGroup());
		return createIndex(row, column, maskObj->children()[numChildren - row].get());
	}
	else
	{
		return createIndex(row, column, parentObj->children()[numChildren - row - 1].get());	
	}
}

QModelIndex SceneObjectListModel::parent(const QModelIndex & index) const
{
	if (!index.isValid() || !document_) return QModelIndex();
	SceneObject * obj = static_cast<SceneObject *>(index.internalPointer());
	
	SceneObject * parent = obj->parent();
	if (!parent || parent == document_->selectedScene()->root()) 
		return QModelIndex();

	SceneObject * parentsParent = parent->parent();
	if (!parentsParent) return QModelIndex();

	MaskObject * maskObj = parentsParent->maskObject();
	if (maskObj)
	{		
		if (maskObj->maskingObjectGroup() == parent)
			return createIndex(0, 0, parent);

		int childIndex = 1;
		BOOST_REVERSE_FOREACH(const SceneObjectSPtr & child, parentsParent->children())
		{
			if (child.get() == parent) return createIndex(childIndex, 0, parent);
			++childIndex;
		}

	}
	else
	{
		int childIndex = 0;

		BOOST_REVERSE_FOREACH(const SceneObjectSPtr & child, parentsParent->children())
		{
			if (child.get() == parent) return createIndex(childIndex, 0, parent);
			++childIndex;
		}

	}
	
	return QModelIndex();
}

int SceneObjectListModel::rowCount(const QModelIndex & parent) const
{	
	if (!document_) return 0;
	Scene * scene = document_->selectedScene();
	if (!parent.isValid())
	{
		return (scene->root()->children().size());
	}
	else 
	{
		SceneObject * obj = (SceneObject *)parent.internalPointer();
		
		if (obj->maskObject())
			return obj->children().size() + 1;
		else
			return obj->children().size();
	}
	
}

int SceneObjectListModel::columnCount(const QModelIndex & parent) const
{
	return 1;
}

bool SceneObjectListModel::dropMimeData(const QMimeData * mimedata,
	Qt::DropAction action, int row, int column, const QModelIndex & parent)
{	
	QStringList types = mimeTypes();


	SceneObject * parentObj;
	if (parent.isValid()) parentObj = (SceneObject *)parent.internalPointer();
	else parentObj = (SceneObject *)document_->selectedScene()->root();
	
	int numChildren = rowCount(parent);
	int index = numChildren - row;

	if (parentObj->maskObject())
	{
		if (index > parentObj->children().size())
			index--;
	}

	if (mimedata->hasUrls())
	{
		QList<QUrl> urls = mimedata->urls();
		vector<SceneObjectSPtr> newObjs;
		BOOST_FOREACH(QUrl url, urls)
		{
			QString localFile = url.toLocalFile();

			Image * img = new Image();
			img->setFileName(convertToRelativePath(qStringToStdString(localFile)));	
			img->init(document_->renderer());
			img->setId(getFileTitle(qStringToStdString(localFile)));
			
			int trWidth, trHeight;
			Global::instance().getImageDims(img->fileName(), &trWidth, &trHeight);

			ImageSPtr imgSPtr(img);

			if (trWidth > 0 && trHeight > 0)			
				newObjs.push_back(imgSPtr);
		}
		if (!newObjs.empty()) document_->doCommand(new NewObjectsCmd(
			document_, parentObj, index, newObjs));
	}
	else
	{
		QByteArray encoded = mimedata->data(types[0]);
		QDataStream stream(&encoded, QIODevice::ReadOnly);
		
		vector<SceneObject *> objs;
		while (!stream.atEnd()) {
			SceneObject * obj;		
			stream.readRawData((char *)&obj, sizeof(SceneObject *));	
			objs.push_back(obj);
		}			

		try
		{
		
		document_->doCommand(new MoveSceneObjectsCmd(
			document_, parentObj, index, objs));
		} catch (...) {}
	}

	return true;
}

void SceneObjectListModel::update()
{
	emit layoutChanged();
}

////////////////////////////////////////////////////////////////////////////////
const int iconDim = 20;
SceneObjectListDelegate::SceneObjectListDelegate(SceneObjectListPane * parent) 
:
QStyledItemDelegate()
{
	document_ = 0;	
	curEditor_ = 0;
	parent_ = parent;

	visibleOnImg_.load(":/data/Resources/visibleOn.png");
	visibleOffImg_.load(":/data/Resources/visibleOff.png");

	lockOnImg_.load(":/data/Resources/lockOn.png");
	lockOffImg_.load(":/data/Resources/lockOff.png");
}

void SceneObjectListDelegate::paint(QPainter *painter, 
	const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QStyleOptionViewItemV4 opt = option;
	opt.state &= ~QStyle::State_HasFocus;
	opt.state |= QStyle::State_Active;
	

	if (index.column() == 0)
	{		
		SceneObject * obj = index.data(Qt::DisplayRole).value<SceneObject *>();
		QString text =  stdStringToQString(obj->id());		
		initStyleOption(&opt, index);
		
		painter->save();

		bool objIsAnimatedBySelAnim = false;
		BOOST_FOREACH(Animation * selAnim, document_->selectedAnimations())
		{
			if (selAnim->hasObject(obj))
				objIsAnimatedBySelAnim = true;
		}
		if (objIsAnimatedBySelAnim)
		{
			painter->fillRect(opt.rect, QColor(0, 0, 0, 20));
		}

		const QStyle * style = 
			opt.SO_DockWidget ? opt.widget->style() : QApplication::style();
		const QWidget * widget = opt.widget;
	
		style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, widget);

		int fontHeight = opt.fontMetrics.height();


		painter->setFont(opt.font);
		qreal x = opt.rect.x() + 5;				
		qreal y = opt.rect.y() + opt.rect.height()/2 - fontHeight/2;
		painter->drawText(x, y, 
			opt.rect.width() - 5 - 2*iconDim,
			opt.rect.height(), Qt::AlignLeft, text);

		const QPixmap * drawImg;
		qreal ix, iy;
		ix = opt.rect.right() - iconDim/2 - visibleOnImg_.width()/2;
		iy = opt.rect.y() + (opt.rect.height() - visibleOnImg_.height()) / 2;

		EditorObject * edObj = document_->editorObject(obj).get();
		if (edObj && edObj->isUiLocked())
			drawImg = &lockOnImg_;
		else
			drawImg = &lockOffImg_;

		painter->drawPixmap(ix, iy, *drawImg);

		ix -= iconDim;

		bool drawEyeIcon = obj->visualAttrib() && obj->visualAttrib()->isVisible();

		if (drawEyeIcon)
			drawImg = &visibleOnImg_;
		else
			drawImg = &visibleOffImg_;

		painter->drawPixmap(ix, iy, *drawImg);
		

		painter->restore();
	}
	else
		QStyledItemDelegate::paint(painter, option, index);
}

QSize SceneObjectListDelegate::sizeHint(
	const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	return QSize(0, 20);
}

QWidget * SceneObjectListDelegate::createEditor(QWidget * parent, 
	const QStyleOptionViewItem & option, const QModelIndex & index) const
{

	if (index.column() == 0)
	{		
		SceneObject * obj = (SceneObject *)index.internalPointer();		

		ListEditorWidget * widget = new ListEditorWidget(
			0, 0, parent);
			
		connect(widget, SIGNAL(editingFinished()),
			this, SLOT(commitAndCloseEditor()));

		connect(widget, SIGNAL(editingCancelled()),
			this, SLOT(cancelEdit()));

		curEditor_ = widget;
		return widget;
	}
	else  
		return QStyledItemDelegate::createEditor(parent, option, index);
}

void SceneObjectListDelegate::setModelData(QWidget *editor, 
		QAbstractItemModel *model, const QModelIndex &index) const
{

	if (index.column() == 0)
	{
		SceneObject * obj = 
			index.model()->data(index, Qt::DisplayRole).value<SceneObject *>();

		ListEditorWidget * widget = 
			qobject_cast<ListEditorWidget *>(editor);
		
		string newName = qStringToStdString(widget->GetLineEdit()->text());

		if (newName != obj->id())
		{
			document_->doCommand(makeChangePropertyCmd(ChangeObjectProperty,
				obj, newName, &SceneObject::id, &SceneObject::setId));
		}
	}
	else
		QStyledItemDelegate::setModelData(editor, model, index);
}

void SceneObjectListDelegate::setEditorData(QWidget *editor, 
	const QModelIndex &index) const
{
	if (index.column() == 0)
	{
		SceneObject * obj = 
			index.model()->data(index, Qt::DisplayRole).value<SceneObject *>();

		ListEditorWidget * widget = 
			qobject_cast<ListEditorWidget *>(editor);
		widget->GetLineEdit()->setText(stdStringToQString(obj->id()));

	}
	else
		QStyledItemDelegate::setEditorData(editor, index);
}

void SceneObjectListDelegate::setDocument(EditorDocument * document)
{
	document_ = document;
}


void SceneObjectListDelegate::editorClosed(QWidget * editor, EndEditHint hint)
{
	curEditor_ = 0;
}

void SceneObjectListDelegate::commitAndCloseEditor()
{
	ListEditorWidget * editor = qobject_cast<ListEditorWidget *>(sender());
	emit commitData(editor);
	emit closeEditor(editor);
}

void SceneObjectListDelegate::cancelEdit()
{
	ListEditorWidget * editor = qobject_cast<ListEditorWidget *>(sender());
	emit closeEditor(editor);
}

///////////////////////////////////////////////////////////////////////////////

SceneObjectListTreeView::SceneObjectListTreeView(SceneObjectListPane * parent)
{
	parent_ = parent;
	ignoreSelChanged_ = false;
	setIndentation(15);
	//setFrameShape(QFrame::NoFrame);
	document_ = 0;
}	

void SceneObjectListTreeView::setDocument(EditorDocument * document)
{
	mousePressOnUiIcon_ = false;

	if (document_) 
	{
		disconnect(
			this, SIGNAL(uiPropertiesChanged()), 
			document_, SIGNAL(objectChanged()));
		document_->disconnect(this);
	}

	document_ = document;
	if (document_)
	{
		connect(document_, SIGNAL(objectSelectionChanged()), 
			this, SLOT(updateSelection()));

		updateSelection();

		connect(document_, SIGNAL(objectListChanged()),
			this, SLOT(updateSceneObjectList()));

		connect(document_, SIGNAL(sceneSelectionChanged()),
			this, SLOT(updateSceneObjectList()));

		connect(this, SIGNAL(uiPropertiesChanged()), document_,
			SIGNAL(objectChanged()));

		updateSceneObjectList();
	}
}

void SceneObjectListTreeView::updateSelection()
{
	ignoreSelChanged_ = true;
	QItemSelectionModel * selModel = selectionModel();
	selModel->clearSelection();
	updateSelection(QModelIndex());
	ignoreSelChanged_ = false;
}

void SceneObjectListTreeView::updateSelection(QModelIndex parentIndex)
{
	QItemSelectionModel * selModel = selectionModel();
	
		
	QAbstractItemModel * m = model();
	int numObjs = m->rowCount(parentIndex);
	for (int i = 0; i < numObjs; ++i)
	{
		QModelIndex childIndex = m->index(i, 0, parentIndex);

		SceneObject * obj = (SceneObject *)childIndex.internalPointer();
		if (document_->isObjectSelected(obj))
			selModel->select(childIndex, QItemSelectionModel::Select);
		
		updateSelection(childIndex);		
	}
}

void SceneObjectListTreeView::updateSceneObjectList()
{
	QModelIndexList indexes = model()->match(
		model()->index(0,0, QModelIndex()), Qt::DisplayRole, "*", -1, 
		Qt::MatchWildcard|Qt::MatchRecursive);
	
	foreach (QModelIndex index, indexes)
	{
		
		SceneObject * obj = (SceneObject *)index.internalPointer();	

		if (model()->hasChildren(index))
		{		
			EditorGroup * edGrp = 
				(EditorGroup *)document_->editorObject(obj).get();
			if (edGrp->expanded())
				expand(index);
			else
				collapse(index);
		}
	}
}

void SceneObjectListTreeView::mousePressEvent(QMouseEvent *event)
{
	mousePressOnUiIcon_ = false;
	if (event->button() == Qt::LeftButton)
	{
		float x = event->pos().x();
		int width = columnWidth(0);
		int lockLeft, lockRight, visLeft, visRight;
		QModelIndex index = indexAt(event->pos());	

		visLeft = width - 2*iconDim;
		visRight = width - iconDim;
		lockLeft = visRight;
		lockRight = width;

		SceneObject * obj = (SceneObject *)index.internalPointer();

		if (obj && lockLeft < x && x < lockRight)
		{
			EditorObject * edObj = document_->editorObject(obj).get();
			if (edObj)
			{
				bool locked = !edObj->isUiLocked();
				edObj->setUiLocked(locked);
				document_->setSelectedRecurse(obj, false);
				emit uiPropertiesChanged();
				update(index);
			}
			
			mousePressOnUiIcon_ = true;
		}
		else if (obj && visLeft < x && x < visRight)
		{
			EditorObject * edObj = document_->editorObject(obj).get();
			if (edObj)
			{
				/*
				bool vis = !edObj->isUiVisible();
				edObj->setUiVisible(vis);			
				*/
				
				VisualAttrib * attr = obj->visualAttrib();
				if (attr)
				{
					bool vis = !attr->isVisible();
					attr->setVisible(vis);
				}


				emit uiPropertiesChanged();	
				update(index);
			}						
			mousePressOnUiIcon_ = true;
		}
		else QTreeView::mousePressEvent(event);
	}
	else QTreeView::mousePressEvent(event);

	MainWindow * mainWindow = parent_->parent();
	if (mainWindow->objectMode() != MainWindow::ModeSceneObject)
		mainWindow->setObjectMode(MainWindow::ModeSceneObject);
	
}

void SceneObjectListTreeView::mouseDoubleClickEvent(QMouseEvent * event)
{
	float width = columnWidth(0);
	if (event->pos().x() < width - iconDim * 2)
		QTreeView::mouseDoubleClickEvent(event);	
}

void SceneObjectListTreeView::mouseReleaseEvent(QMouseEvent *event)
{
	mousePressOnUiIcon_ = false;
	QTreeView::mouseReleaseEvent(event);
}


void SceneObjectListTreeView::mouseMoveEvent(QMouseEvent * event)
{
	if (!mousePressOnUiIcon_)
		QTreeView::mouseMoveEvent(event);
}


void SceneObjectListTreeView::selectionChanged(
		const QItemSelection &selected, const QItemSelection &deselected)
{
	QTreeView::selectionChanged(selected, deselected);
	if (ignoreSelChanged_) return;

	QItemSelectionModel * selModel = selectionModel();
	QModelIndexList selIndexList = selModel->selectedRows(0);
	
	std::sort(selIndexList.begin(), selIndexList.end(), IndexComparer());

	vector<SceneObject *> selObjs;
	foreach(QModelIndex index, selIndexList)
	{
		SceneObject * selObj = index.data().value<SceneObject *>();
		selObjs.push_back(selObj);
		
	}
	document_->setSelectedObjects(selObjs);
	
	MainWindow * mainWindow = parent_->parent();
	if (mainWindow->objectMode() != MainWindow::ModeSceneObject)
		mainWindow->setObjectMode(MainWindow::ModeSceneObject);
}

void SceneObjectListTreeView::paintEvent(QPaintEvent *event)
{
	QTreeView::paintEvent(event);
	
}

void SceneObjectListTreeView::dragEnterEvent(QDragEnterEvent *event)
{
	const QMimeData * data = event->mimeData();
	QStringList formats = data->formats();
	
	QStringList modelTypes = model()->mimeTypes();
	bool has = data->hasFormat(modelTypes[1]);
	Qt::DropAction action = event->dropAction();
	QTreeView::dragEnterEvent(event);

}

///////////////////////////////////////////////////////////////////////////////


SceneObjectListPaneTopBar::SceneObjectListPaneTopBar()
{
	setStyleSheet("border: 0px;");

	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	lockImg_.load(":/data/Resources/lock.png");
	eyeImg_.load(":/data/Resources/visible.png");
	rightMargin_ = 0;
}

SceneObjectListPaneTopBar::~SceneObjectListPaneTopBar()
{
}

void SceneObjectListPaneTopBar::setRightMargin(int size)
{
	rightMargin_ = size;
}

void SceneObjectListPaneTopBar::paintEvent(QPaintEvent * event)
{
	QPainter p(this);
	
	
	p.drawPixmap(
		width() - 2 * iconDim - rightMargin_, height()/2-iconDim/2, 
		iconDim, iconDim, eyeImg_);
	p.drawPixmap(
		width() - iconDim - rightMargin_, height()/2-iconDim/2, 
		iconDim, iconDim, lockImg_);

}

///////////////////////////////////////////////////////////////////////////////
SceneObjectListPane::SceneObjectListPane(MainWindow * mainWindow)
{	
	setWindowTitle(tr("Objects"));

	mainWindow_ = mainWindow;

	document_ = 0;
	delegate_ = 0;

	treeView_ = new SceneObjectListTreeView(this);
	model_ = 0;

	isCtrlKey_ = false;

	treeView_->setDragEnabled(true);
	treeView_->setAcceptDrops(true);
	treeView_->setDropIndicatorShown(true);
	treeView_->setDefaultDropAction(Qt::MoveAction);
	treeView_->setDragDropMode(QAbstractItemView::DragDrop);	
	treeView_->setSelectionMode(QAbstractItemView::ExtendedSelection);

	QBoxLayout * layout = new QVBoxLayout(this);
		
	layout->setMargin(0);
	layout->setSpacing(0);

	QBoxLayout * shortCutButtonLayout = new QHBoxLayout(this);
	shortCutButtonLayout->setMargin(0);
	shortCutButtonLayout->setSpacing(0);

	shortCutButtonLayout->addWidget(newImageBtn_ = new PaneButton(QPixmap(":/data/Resources/Object/imageObject.png"), QSize(38, 29), "Image"));
	shortCutButtonLayout->addWidget(newGroupBtn_ = new PaneButton(QPixmap(":/data/Resources/Object/groupObject.png"), QSize(38, 29), "Group"));
	shortCutButtonLayout->addWidget(newRectangleBtn_ = new PaneButton(QPixmap(":/data/Resources/Object/rectangleObject.png"), QSize(38, 29), "Rectangle"));
	shortCutButtonLayout->addWidget(newSubSceneBtn_ = new PaneButton(QPixmap(":/data/Resources/Object/subSceneObject.png"), QSize(38, 29), "Sub Scene"));
	shortCutButtonLayout->addWidget(newTextBtn_ = new PaneButton(QPixmap(":/data/Resources/Object/textObject.png"), QSize(38, 29), "Text"));
	layout->addLayout(shortCutButtonLayout);

	QBoxLayout * firstLineLayout = new QHBoxLayout(this);

//	label_ = new HeaderLabel(mainWindow);
	label_ = new QLabel;
	label_->setMinimumWidth(10);
	label_->setMinimumHeight(25);
	label_->setMaximumHeight(25);
	
	firstLineLayout->setMargin(0);
	firstLineLayout->setSpacing(0);
	firstLineLayout->addWidget(label_, Qt::AlignLeft);
	
	newImage_ = new QAction(QIcon(QPixmap(":/data/Resources/Object/imageObject.png")), "Image", this);
	newImageChanger_ = new QAction(QIcon(QPixmap(":/data/Resources/Object/imageChanger.png")), "Image Changer", this);
	newImageGrid_ = new QAction(QIcon(QPixmap(":/data/Resources/Object/imageGrid.png")), "Image Grid", this);
	newAudio_ = new QAction(QIcon(QPixmap(":/data/Resources/Object/AudioObject.png")), "Audio", this);
	newVideo_ = new QAction(QIcon(QPixmap(":/data/Resources/Object/videoObject.png")), "Video", this);
	newText_ = new QAction(QIcon(QPixmap(":/data/Resources/Object/textObject.png")), "Text", this);
	newGroup_ = new QAction(QIcon(QPixmap(":/data/Resources/Object/groupObject.png")), "Group", this);
	newRectangle_ = new QAction(QIcon(QPixmap(":/data/Resources/Object/rectangleObject.png")), "Rectangle", this);
	newSubScene_ = new QAction(QIcon(QPixmap(":/data/Resources/Object/subSceneObject.png")), "Sub Scene", this);
	newSceneChanger_ = new QAction(QIcon(QPixmap(":/data/Resources/Object/sceneChangerObject.png")), "Scene Changer", this);
//	newMultiChoiceQuestion_ = new QAction(QIcon(QPixmap(":/data/Resources/Object/questionObject.png")), "Multi Choice Question", this);
	newPanorama_ = new QAction(QIcon(QPixmap(":/data/Resources/Object/panoramaObject.png")), "Panorama", this);
	newTurntable_ = new QAction(QIcon(QPixmap(":/data/Resources/Object/turntableObject.png")), "Turntable", this); 

	newImage_->setData("Image");
	newImageChanger_->setData("Image Changer");
	newImageGrid_->setData("Image Grid");
	newAudio_->setData("Audio");
	newVideo_->setData("Video");
	newText_->setData("Text");
	newGroup_->setData("Group");
	newRectangle_->setData("Rectangle");
	newSubScene_->setData("Sub Scene");
	newSceneChanger_->setData("Scene Changer");
//	newMultiChoiceQuestion_->setData("Multi Choice Question");
	newPanorama_->setData("Panorama");
	newTurntable_->setData("Turntable");

	newObjectMenu_ = new QMenu(this);
	newObjectMenu_->addAction(newImage_);
	newObjectMenu_->addAction(newImageChanger_);
	newObjectMenu_->addAction(newImageGrid_);
	newObjectMenu_->addAction(newAudio_);
	newObjectMenu_->addAction(newVideo_);
	newObjectMenu_->addAction(newText_);
	newObjectMenu_->addAction(newGroup_);
	newObjectMenu_->addAction(newRectangle_);
	newObjectMenu_->addAction(newSubScene_);
	newObjectMenu_->addAction(newSceneChanger_);
	//newObjectMenu_->addAction(newMultiChoiceQuestion_);
	newObjectMenu_->addAction(newPanorama_);
	newObjectMenu_->addAction(newTurntable_);	
	
	newObjectMenu_->setStyleSheet(
		"QMenu{border: 1px solid rgb(170, 170, 170);}"
		"QMenu::item{padding-left: 45px; height: 29px; color:rgb(55, 55, 55);}");

	iconStyle_ = new PaneMenuIconStyle;
	newObjectMenu_->setStyle(iconStyle_);

	newImageBtn_->addAction(newImage_);
	newGroupBtn_->addAction(newGroup_);
	newRectangleBtn_->addAction(newRectangle_);
	newSubSceneBtn_->addAction(newSubScene_);
	newTextBtn_->addAction(newText_);

	QIcon newButtonIcon = QIcon(QPixmap(":/data/Resources/Object_menu/object_new.png"));
	newObjectPopupBtn_ = new QToolButton(this);
	newObjectPopupBtn_->setMenu(newObjectMenu_);
	newObjectPopupBtn_->setIcon(newButtonIcon);
	newObjectPopupBtn_->setPopupMode(QToolButton::InstantPopup);
	newObjectPopupBtn_->setIconSize(QSize(62, 20));
	newObjectPopupBtn_->	setStyleSheet("QToolButton{background-color: none; border: none;}" 
		"QToolButton:pressed:hover{background-color: rgb(170, 170, 170);}" 
		"QToolButton:hover{background-color: rgb(190, 190, 190);}");
	firstLineLayout->addWidget(newObjectPopupBtn_, Qt::AlignRight);
	layout->addLayout(firstLineLayout);
	//layout->addWidget(iconBar_ = new SceneObjectListPaneTopBar);

	QBoxLayout * btnLayout = new QBoxLayout(QBoxLayout::LeftToRight);
	btnLayout->setMargin(0);
	btnLayout->setSpacing(0);
	btnLayout->setAlignment(Qt::AlignRight);
	btnLayout->addWidget(newButton_ = new PaneButton(QPixmap(":/data/Resources/Scene_menu/scene_new.png"), QSize(30, 30), "New"));
	btnLayout->addWidget(delButton_ = new PaneButton(QPixmap(":/data/Resources/Scene_menu/scene_del.png"), QSize(30, 30), "Delete"));
	
	layout->addWidget(treeView_);
	layout->addLayout(btnLayout);
	
	connect(newObjectMenu_, SIGNAL(triggered(QAction *)), this, SLOT(onQuickNew(QAction *)));

	connect(newImageBtn_, SIGNAL(clicked()), this, SLOT(onNewImage()));
	connect(newGroupBtn_, SIGNAL(clicked()), this, SLOT(onNewGroup()));
	connect(newRectangleBtn_, SIGNAL(clicked()), this, SLOT(onNewRectangle()));
	connect(newSubSceneBtn_, SIGNAL(clicked()), this, SLOT(onNewSubScene()));
	connect(newTextBtn_, SIGNAL(clicked()), this, SLOT(onNewText()));
	
	connect(newButton_, SIGNAL(clicked()), this, SLOT(onNew()));
	connect(delButton_, SIGNAL(clicked()), this, SLOT(onDelete()));
	
	connect(treeView_, SIGNAL(expanded(const QModelIndex &)), 
		this, SLOT(onExpanded(const QModelIndex &)));
	connect(treeView_, SIGNAL(collapsed(const QModelIndex &)), 
		this, SLOT(onCollapsed(const QModelIndex &)));
}

SceneObjectListPane::~SceneObjectListPane()
{
	delete newObjectMenu_;
	delete model_;
	delete delegate_;
	delete iconStyle_;
}

void SceneObjectListPane::onExpanded(const QModelIndex &index)
{
	SceneObject * obj = (SceneObject *)index.internalPointer();
	EditorGroup * edGrp = (EditorGroup *)document_->editorObject(obj).get();
	edGrp->setExpanded(true);
}

void SceneObjectListPane::onCollapsed(const QModelIndex &index)
{
	SceneObject * obj = (SceneObject *)index.internalPointer();
	EditorGroup * edGrp = (EditorGroup *)document_->editorObject(obj).get();
	edGrp->setExpanded(false);
}


QSize SceneObjectListPane::sizeHint() const
{
	return QSize(200,150);
}


void SceneObjectListPane::changeButtonsState()
{
	QItemSelectionModel * selModel = treeView_->selectionModel();
	int numSel = 0;
	if (selModel)
	{
		QModelIndexList selIndexList = selModel->selectedRows(0);
		if (!selIndexList.empty()) numSel = selIndexList.size();
	}
	
	delButton_->setEnabled(numSel > 0);	
}

void SceneObjectListPane::setDocument(EditorDocument * document)
{
	if (document_) document_->disconnect(this);

	document_ = document;

	delete model_;
	model_ = new SceneObjectListModel();
	treeView_->setModel(model_);

	model_->setDocument(document);
	treeView_->setDocument(document);
	connect(model_, SIGNAL(layoutChanged()), treeView_, SLOT(updateSelection()));
	delete delegate_;
	delegate_ = new SceneObjectListDelegate(this);
	
	
	treeView_->setItemDelegate(delegate_);
	delegate_->setDocument(document);

	if (document_)
	{
		setEnabled(true);
		connect(document_, SIGNAL(objectSelectionChanged()), this, SLOT(changeButtonsState()));
		connect(model_, SIGNAL(layoutChanged()), this, SLOT(changeButtonsState()));
		changeButtonsState();

		connect(document_, SIGNAL(sceneSelectionChanged()), this, SLOT(updateScene()));
		connect(document_, SIGNAL(sceneChanged()), this, SLOT(updateScene()));
		updateScene();
	}
	else
	{
		setEnabled(false);
		setLabelText();
	}
}

void SceneObjectListPane::onNew()
{
	QWidget * dlgParent = parentWidget();
	while (dlgParent->parent()) dlgParent = dlgParent->parentWidget();

	NewObjectDlg dlg(document_, dlgParent);
	if (dlg.exec() == QDialog::Accepted)
	{
		SceneObject * newObjPtr = dlg.createSceneObject();
		if (newObjPtr)
		{
			vector<SceneObjectSPtr> newObjs;
			newObjs.push_back( SceneObjectSPtr(newObjPtr));
			document_->doCommand(new NewObjectsCmd(
				document_, document_->selectedScene()->root(), -1, newObjs));

			// scenechanger일때 zoom in/out 해제
			string typestr = string(newObjPtr->typeStr());
			if (typestr == "SceneChanger")
			{
				document_->selectedScene()->setZoom(false);
			}

			if (mainWindow_->objectMode() != MainWindow::ModeSceneObject)
				mainWindow_->setObjectMode(MainWindow::ModeSceneObject);
		}
	}
}

void SceneObjectListPane::onDelete()
{
	try {
	document_->doCommand(new DeleteObjectsCmd(
		document_, document_->selectedScene(),
		document_->selectedObjects()));
	} catch (const Exception & e) 
	{
		QMessageBox::information(0, tr("Error"), tr(e.what()));
	}		
}

void SceneObjectListPane::onNewImage()
{
	onQuickNew(newImage_);
}

void SceneObjectListPane::onNewGroup()
{
	onQuickNew(newGroup_);
}

void SceneObjectListPane::onNewRectangle()
{
	onQuickNew(newRectangle_);
}

void SceneObjectListPane::onNewSubScene()
{
	onQuickNew(newSubScene_);
}

void SceneObjectListPane::onNewText()
{
	onQuickNew(newText_);
}

void SceneObjectListPane::setLabelText()
{
	QString labelText = QString("<b>") + tr("Scene Objects: ") + "</b> ";
	if (document_)
	{
		Scene * selScene = document_->selectedScene();
		labelText += stdStringToQString(selScene->name());
	}
	else
	{
		labelText.clear();
	}
	
	label_->setText(labelText);	
}

void SceneObjectListPane::updateScene()
{
	if (!isEnabled()) return;
	setLabelText();
}

void SceneObjectListPane::onQuickNew(QAction * action)
{
	QString objType = action->data().toString();
	int objIndex;
	string typestr;
	bool isSceneOrCustomObj = true;

	if(objType == "Image") typestr = "Image";
	else if(objType == "Image Changer") typestr = "ImageChanger";
	else if(objType == "Image Grid") typestr = "ImageGrid";
	else if(objType == "Audio") typestr = "Audio";
	else if(objType == "Video") typestr = "Video";
	else if(objType == "Text") typestr = "TextObject";
	else if(objType == "Group") typestr = "Group";
	else if(objType == "Rectangle") typestr = "Rectangle";
	else if(objType == "Sub Scene") typestr = "Subscene";
	else if(objType == "Scene Changer") typestr = "SceneChanger";
	else if(objType == "Panorama") typestr = "Panorama";
	else if(objType == "Turntable") typestr = "TurnTable";
	//else if(objType == "Multi Choice Question")
	//{
	//	typestr = "MultiChoiceQuestion";
	//	objIndex = 0;
	//	isSceneOrCustomObj = false;
	//}

	SceneObject * newObjPtr, * curObjPtr;

	if(isSceneOrCustomObj)
	{
		EditorScene * edScene = 
			document_->editorScene(document_->selectedScene()).get();
		std::vector<SceneObject *> sceneObjs;
		Global::instance().sceneObjects(&sceneObjs);

		std::vector<SceneObject *>::iterator iter;
		for(iter = sceneObjs.begin(); iter != sceneObjs.end(); ++iter)
		{
			SceneObject * obj = *iter;
			if(obj->typeStr() == typestr)
			{
				curObjPtr = obj;
				break;
			}
		}

		EditorObject * edObj = 
			EditorGlobal::instance().createEditorObject(edScene, curObjPtr);
		if (edObj) 
		{
			SceneObject * obj = edObj->createSceneObject(document_);
			delete edObj;
			newObjPtr = obj;
		}
		else 
		{
			SceneObject * newObj = curObjPtr->clone(0);
			newObj->init(document_->document()->renderer());
			newObjPtr = newObj;
		}
	}

	else if(!isSceneOrCustomObj)
	{
		EditorScene * edScene = 
			document_->editorScene(document_->selectedScene()).get();
		std::vector<EditorCustomObject *> custObjs;
		EditorGlobal::instance().editorCustomObjects(&custObjs);
		EditorCustomObject * edObj =
			EditorGlobal::instance().createEditorCustomObject(edScene, custObjs[objIndex]);
		SceneObject * obj = edObj->createSceneObject(document_);
		delete edObj;
		newObjPtr = obj;
	}

	if (newObjPtr)
	{
		vector<SceneObjectSPtr> newObjs;
		newObjs.push_back( SceneObjectSPtr(newObjPtr));
		document_->doCommand(new NewObjectsCmd(
			document_, document_->selectedScene()->root(), -1, newObjs));

		if (mainWindow_->objectMode() != MainWindow::ModeSceneObject)
			mainWindow_->setObjectMode(MainWindow::ModeSceneObject);
	}
}

void SceneObjectListPane::keyPressEvent(QKeyEvent * keyEvent)
{
	QWidget::keyPressEvent(keyEvent);

	if (keyEvent->key() == Qt::Key_Control) isCtrlKey_ = true;

	std::vector<SceneObject *> selectedObj = document_->selectedObjects();
	if (isCtrlKey_ && (keyEvent->key() == Qt::Key_G) &&
		(!selectedObj.empty()))
	{
		SceneObject * newObjPtr;
		std::vector<SceneObject *> sceneObjs;
		Global::instance().sceneObjects(&sceneObjs);
		EditorScene * edScene = 
			document_->editorScene(document_->selectedScene()).get();
		EditorObject * edObj = 
			EditorGlobal::instance().createEditorObject(edScene, sceneObjs[4]);
		if (edObj) 
		{
			SceneObject * obj = edObj->createSceneObject(document_);
			delete edObj;
			newObjPtr = obj;
		}
		else 
		{
			SceneObject * newObj = sceneObjs[2]->clone(0);
			newObj->init(document_->document()->renderer());
			newObjPtr = newObj;
		}
		vector<SceneObjectSPtr> newObjs;
		newObjs.push_back( SceneObjectSPtr(newObjPtr));
		document_->doCommand(new NewObjectsCmd(
			document_, document_->selectedScene()->root(), -1, newObjs));

		int numChildren = newObjPtr->children().size();
		int index = numChildren + 1;
		document_->doCommand(new MoveSceneObjectsCmd(
			document_, newObjPtr, index, selectedObj));
	}

	if(keyEvent->key() == Qt::Key_Delete)
	{
		onDelete();
	}
}

void SceneObjectListPane::keyReleaseEvent(QKeyEvent * keyEvent)
{
	QWidget::keyReleaseEvent(keyEvent);

	if (keyEvent->key() == Qt::Key_Control) isCtrlKey_ = false;
}