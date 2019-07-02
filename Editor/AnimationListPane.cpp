#include "stdafx.h"
#include "AnimationListPane.h"
#include "EditorDocument.h"
#include "EditorObject.h"
#include "Animation.h"
#include "AnimationChannel.h"
#include "Command.h"
#include "Scene.h"
#include "SceneObject.h"
#include "ListEditorWidget.h"
#include "PaneButton.h"
#include "Utils.h"
#include "AnimationPropertiesDlg.h"

using namespace std;

AnimationListItem::AnimationListItem(const QList<QVariant> &data, AnimationListItem * parent)
{
	parentItem = parent;
	itemData = data;
}

AnimationListItem::~AnimationListItem()
{
	qDeleteAll(childItems);
}

void AnimationListItem::appendChild(AnimationListItem *item)
{
	childItems.append(item);
}

AnimationListItem * AnimationListItem::child(int row)
{
	return childItems.value(row);
}

int AnimationListItem::columnCount() const
{
	return itemData.count();
}

QVariant AnimationListItem::data(int column) const
{
	return itemData.value(column);
}

AnimationListItem * AnimationListItem::parent()
{
	return parentItem;
}

int AnimationListItem::row() const
{
	if(parentItem)
		return parentItem->childItems.indexOf(const_cast<AnimationListItem *>(this));

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
Q_DECLARE_METATYPE(Animation *)
AnimationListModel::AnimationListModel()
{
	document_ = 0;
}

AnimationListModel::~AnimationListModel()
{
}

void AnimationListModel::setupModelData(AnimationListItem * parent)
{

}

void AnimationListModel::setDocument(EditorDocument * document)
{
	if (document_) disconnect(this);

	emit layoutAboutToBeChanged();
	document_ = document;
	emit layoutChanged();

	if (document_)
	{
		connect(document_, SIGNAL(sceneSelectionChanged()),
			this, SLOT(update()));

		connect(document_, SIGNAL(animationListChanged()), 
			this, SLOT(update()));

		connect(document_, SIGNAL(animationChanged()), 
			this, SLOT(update()));

		connect(document_, SIGNAL(objectSelectionChanged()),
			this, SLOT(update()));
		
		connect(document_, SIGNAL(channelListChanged()), 
			this, SLOT(update()));
	}
}

QStringList AnimationListModel::mimeTypes() const
{
    QStringList types;
    types << QLatin1String("application/animation-element-ptr");
    return types;
}


QMimeData * AnimationListModel::mimeData(const QModelIndexList & indexes) const
{
	QMimeData * mimeData = new QMimeData();
	QByteArray encodedData;

	QDataStream stream(&encodedData, QIODevice::WriteOnly);
	QModelIndexList sortedIndices = indexes;
	
	std::sort(sortedIndices.begin(), sortedIndices.end());

	foreach (QModelIndex index, sortedIndices) {
		if (index.isValid()) {
			QVariant variant = data(index, Qt::DisplayRole);

			if (variant.canConvert<Animation *>())
			{
				Animation * animation = variant.value<Animation *>();
				stream.writeRawData((char *)&animation, sizeof(Animation *));
			}						
		}
	}
     mimeData->setData("application/animation-element-ptr", encodedData);
     return mimeData;
}


QVariant AnimationListModel::data(const QModelIndex & index, int role) const
{	
	if (!index.isValid()) return QVariant();

	if (role == Qt::DisplayRole) 
	{
		if (index.column() == 0)
		{
			Animation * obj = (Animation *)index.internalPointer();
			return QVariant::fromValue(obj);
		}
		else
			return QVariant();
	}
	
	return QVariant();
}

Qt::ItemFlags AnimationListModel::flags(const QModelIndex &index) const
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

Qt::DropActions AnimationListModel::supportedDropActions() const
{
	return Qt::MoveAction;
}

QVariant AnimationListModel::headerData(
	int section, Qt::Orientation orientation, int role) const
{
	return tr("Animation");
}

QModelIndex AnimationListModel::index(int row, int column, const QModelIndex &parent) const
{
	if (!hasIndex(row, column, parent)) return QModelIndex();
	if (column != 0) return QModelIndex();
	
	Animation * parentObj;
	
	const vector<AnimationSPtr> & animations = 
		document_->selectedScene()->animations();
	
	return createIndex(row, column, animations[row].get());
}

QModelIndex AnimationListModel::parent(const QModelIndex & index) const
{
	return QModelIndex();	
}

int AnimationListModel::rowCount(const QModelIndex & parent) const
{	
	if (!document_) return 0;
	if (!parent.isValid())
	{
		Scene * scene = document_->selectedScene();
		return scene->animations().size();
	}
	else
		return 0;
	
}

int AnimationListModel::columnCount(const QModelIndex & parent) const
{
	return 1;
}

bool AnimationListModel::dropMimeData(const QMimeData * mimedata,
	Qt::DropAction action, int row, int column, const QModelIndex & parent)
{
	QStringList types = mimeTypes();
	QString format = types.at(0);
    QByteArray encoded = mimedata->data(format);
	QDataStream stream(&encoded, QIODevice::ReadOnly);
	
	vector<Animation *> animations;
	
	while (!stream.atEnd()) {
		Animation * animation;		
		stream.readRawData((char *)&animation, sizeof(Animation *));	
		animations.push_back(animation);
	}
	Animation * moveTarget = 0;

	QModelIndex idx = index(row, 0, parent);
	moveTarget = data(idx, Qt::DisplayRole).value<Animation *>();

	
	document_->doCommand(new MoveAnimationsCmd(
		document_->selectedScene(), animations, moveTarget));

	return true;
}

void AnimationListModel::update()
{
	emit layoutChanged();
}



////////////////////////////////////////////////////////////////////////////////
const int iconDim = 25;
AnimationListDelegate::AnimationListDelegate(AnimationListPane * parent) 
:
QStyledItemDelegate()
{
	document_ = 0;	
	parent_ = parent;

	repeatOnImg_.load(":/data/Resources/timeline/repeatOn.png");
	repeatOffImg_.load(":/data/Resources/timeline/repeatOff.png");
}

void AnimationListDelegate::paint(QPainter *painter, 
	const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QStyleOptionViewItemV4 opt = option;
	opt.state &= ~QStyle::State_HasFocus;
	opt.state |= QStyle::State_Active;
	if (index.column() == 0)
	{		
		Animation * obj = index.data(Qt::DisplayRole).value<Animation *>();
		QString text =  stdStringToQString(obj->name());		
		
		initStyleOption(&opt, index);
		
		painter->save();

		bool selObjIsAnimated = false;
		BOOST_FOREACH(SceneObject * selObj, document_->selectedObjects())
		{
			if (obj->hasObject(selObj))
				selObjIsAnimated = true;
		}
		if (selObjIsAnimated)
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
		qreal y = opt.rect.y() + (opt.rect.height() - fontHeight)/2;
		painter->drawText(x, y,
			opt.rect.width() - 5 - iconDim,
			opt.rect.height(), Qt::AlignLeft, text);

		const QPixmap * drawImg;
		qreal ix, iy;
		ix = opt.rect.right() - iconDim/2 - repeatOnImg_.width()/2;
		iy = opt.rect.y() + (opt.rect.height() - repeatOnImg_.height()) / 2;

		if (obj->repeat())
			drawImg = &repeatOnImg_;
		else
			drawImg = &repeatOffImg_;

		painter->drawPixmap(ix, iy, *drawImg);
		painter->restore();
	}
	else
		QStyledItemDelegate::paint(painter, option, index);
}

QSize AnimationListDelegate::sizeHint(
	const QStyleOptionViewItem &option, const QModelIndex &index) const
{	
	return QSize(0, 20);
}

QWidget * AnimationListDelegate::createEditor(QWidget * parent, 
	const QStyleOptionViewItem & option, const QModelIndex & index) const
{

	if (index.column() == 0)
	{		
		Animation * obj = (Animation *)index.internalPointer();		

		ListEditorWidget * widget = new ListEditorWidget(0, 0, parent);
			
		connect(widget, SIGNAL(editingFinished()),
			this, SLOT(commitAndCloseEditor()));

		connect(widget, SIGNAL(editingCancelled()),
			this, SLOT(cancelEdit()));

		return widget;
	}
	else  
		return QStyledItemDelegate::createEditor(parent, option, index);
}

void AnimationListDelegate::setModelData(QWidget *editor, 
		QAbstractItemModel *model, const QModelIndex &index) const
{
	if (!document_) return;

	if (index.column() == 0)
	{
		Animation * obj = 
			index.model()->data(index, Qt::DisplayRole).value<Animation *>();

		ListEditorWidget * widget = 
			qobject_cast<ListEditorWidget *>(editor);
		
		string newName = qStringToStdString(widget->GetLineEdit()->text());

		if (newName != obj->name())
		{
			document_->doCommand(makeChangePropertyCmd(ChangeAnimationProperty, 
				obj, newName, &Animation::name, &Animation::setName));
		}
		
		
	}
	else
		QStyledItemDelegate::setModelData(editor, model, index);
}

void AnimationListDelegate::setEditorData(QWidget *editor, 
	const QModelIndex &index) const
{
	if (index.column() == 0)
	{
		Animation * obj = 
			index.model()->data(index, Qt::DisplayRole).value<Animation *>();

		ListEditorWidget * widget = 
			qobject_cast<ListEditorWidget *>(editor);
		widget->GetLineEdit()->setText(stdStringToQString(obj->name()));

	}
	else
		QStyledItemDelegate::setEditorData(editor, index);
}

void AnimationListDelegate::setDocument(EditorDocument * document)
{
	document_ = document;
}


void AnimationListDelegate::commitAndCloseEditor()
{
	ListEditorWidget * editor = qobject_cast<ListEditorWidget *>(sender());
	emit commitData(editor);
	emit closeEditor(editor);
}

void AnimationListDelegate::cancelEdit()
{
	ListEditorWidget * editor = qobject_cast<ListEditorWidget *>(sender());
	emit closeEditor(editor);
}

///////////////////////////////////////////////////////////////////////////////
void AnimationListTreeView::setDocument(EditorDocument * document)
{
	mousePressOnUiIcon_ = false;

	if (document_) 
	{
		document_->disconnect(this);
	}

	document_ = document;	
	
	if (document_)
	{
		connect(document_, SIGNAL(animationSelectionChanged()), 
			this, SLOT(updateSelection()));

		updateSelection();
	}
}

AnimationListTreeView::AnimationListTreeView(AnimationListPane * parent)
{
	document_ = 0;
	parent_ = parent;
	ignoreSelChanged_ = false;
	clickedAnimation_ = NULL;
	setIndentation(0);	
	//setFrameShape(QFrame::NoFrame);
	setRootIsDecorated(false);
	propertiesAction_ = new QAction(tr("Properties..."), this);
	connect(propertiesAction_, SIGNAL(triggered()), this, SLOT(onProperties()));

	contextMenu_ = new QMenu(this);
	QPalette Pal(QApplication::palette());
	//get rid of weird drop shadow effect on disabled items
	Pal.setColor(QPalette::Light, QColor(240, 240, 240, 255));	
	contextMenu_->setPalette(Pal);

	contextMenu_->addAction(propertiesAction_);
}

AnimationListTreeView::~AnimationListTreeView()
{
	delete contextMenu_;
}

void AnimationListTreeView::layoutChanged()
{
	ignoreSelChanged_ = true;
	QItemSelectionModel * selModel = selectionModel();
	selModel->clearSelection();
		
	QAbstractItemModel * m = model();
	int numObjs = m->rowCount();
	for (int i = 0; i < numObjs; ++i)
	{
		QModelIndex index = m->index(i, 0);
		Animation * anim = (Animation *)index.internalPointer();
		if (document_->isAnimationSelected(anim))
			selModel->select(index, QItemSelectionModel::Select);
	}

	ignoreSelChanged_ = false;	

}

void AnimationListTreeView::onProperties()
{
	AnimationPropertiesDlg dlg(clickedAnimation_, this);
	if (dlg.exec())
	{
		document_->doCommand(
			new AnimationPropertiesCmd(clickedAnimation_, dlg.repeat(), dlg.syncObj()));
	}
}

void AnimationListTreeView::mousePressEvent(QMouseEvent *event)
{
	QTreeView::mousePressEvent(event);

	mousePressOnUiIcon_ = false;
	
	QModelIndex index = indexAt(event->pos());	

	if (index.isValid())
		clickedAnimation_ = (Animation *)index.internalPointer();
	else clickedAnimation_ = 0;

	//if (event->button() == Qt::LeftButton)
	//allow selection on right button right before context menu is displayed
	{
		float x = event->pos().x();
		int width = columnWidth(0);
		int repeatLeft, repeatRight;
		

		repeatLeft = width - iconDim;
		repeatRight = width;

		Animation * obj = (Animation *)index.internalPointer();

		if (obj && repeatLeft < x && x < repeatRight)
		{				
			document_->doCommand(
				makeChangePropertyCmd(ChangeAnimationProperty,
				obj, !obj->repeat(),
				&Animation::repeat, &Animation::setRepeat));

			//emit uiPropertiesChanged();
			update(index);			
			
			mousePressOnUiIcon_ = true;
		}	
		else QTreeView::mousePressEvent(event);
	}

	if (event->button() == Qt::RightButton)
	{			
		propertiesAction_->setEnabled(clickedAnimation_ != 0);	
		contextMenu_->popup(QCursor::pos());
	}
}

void AnimationListTreeView::mouseMoveEvent(QMouseEvent * event)
{
	if (!mousePressOnUiIcon_)
		QTreeView::mouseMoveEvent(event);
}

void AnimationListTreeView::mouseDoubleClickEvent(QMouseEvent * event)
{
	float width = columnWidth(0);
	if (event->pos().x() < width - iconDim)
		QTreeView::mouseDoubleClickEvent(event);	
}

void AnimationListTreeView::expanded(const QModelIndex &index)
{
	mousePressOnUiIcon_ = false;
	QTreeView::expanded(index);
}

void AnimationListTreeView::collapsed(const QModelIndex &index)
{
	QTreeView::collapsed(index);
}

void AnimationListTreeView::updateSelection()
{
	ignoreSelChanged_ = true;
	QItemSelectionModel * selModel = selectionModel();
	selModel->clearSelection();
		
	QAbstractItemModel * m = model();
	int numObjs = m->rowCount();
	for (int i = 0; i < numObjs; ++i)
	{
		QModelIndex index = m->index(i, 0);
		Animation * obj = (Animation *)index.internalPointer();
		if (document_->isAnimationSelected(obj))
			selModel->select(index, QItemSelectionModel::Select);
	}

	ignoreSelChanged_ = false;
}

void AnimationListTreeView::selectionChanged(
		const QItemSelection &selected, const QItemSelection &deselected)
{
	QTreeView::selectionChanged(selected, deselected);
	if (ignoreSelChanged_) return;

	QItemSelectionModel * selModel = selectionModel();
	QModelIndexList selIndexList = selModel->selectedRows(0);
	
	vector<Animation *> selObjs;
	foreach(QModelIndex index, selIndexList)
	{
		Animation * selObj = index.data().value<Animation *>();
		selObjs.push_back(selObj);
		
	}
	document_->setSelectedAnimations(selObjs);
	parent_->changeButtonsState();

}

///////////////////////////////////////////////////////////////////////////////
AnimationListPane::AnimationListPane(MainWindow * mainWindow)
{
	document_ = 0;
	delegate_ = 0;

	treeView_ = new AnimationListTreeView(this);
	model_ = 0;

	treeView_->setDragEnabled(true);
	treeView_->setAcceptDrops(true);
	treeView_->setDropIndicatorShown(true);
	treeView_->setDefaultDropAction(Qt::MoveAction);
	treeView_->setDragDropMode(QAbstractItemView::InternalMove);
	treeView_->setSelectionMode(QAbstractItemView::ExtendedSelection);

	QBoxLayout * layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
	
	layout->setMargin(0);
	layout->setSpacing(0);	
	label_ = new QLabel;
	label_->setMinimumWidth(10);
	label_->setMinimumHeight(20);
	layout->addWidget(label_);
	layout->addWidget(treeView_);
		
	QBoxLayout * btnLayout = new QBoxLayout(QBoxLayout::LeftToRight);
	btnLayout->setMargin(0);
	btnLayout->setSpacing(0);
	btnLayout->setAlignment(Qt::AlignRight);
	btnLayout->addWidget(newButton_ = new PaneButton(QPixmap(":/data/Resources/Scene_menu/scene_new.png"), QSize(30, 30), "New"));
	btnLayout->addWidget(delButton_ = new PaneButton(QPixmap(":/data/Resources/Scene_menu/scene_del.png"), QSize(30, 30), "Delete"));
	//btnLayout->addWidget(editButton_ = new PaneButton(tr("Details...")));	
	layout->addLayout(btnLayout);

	connect(newButton_, SIGNAL(clicked()), this, SLOT(onNew()));
	connect(delButton_, SIGNAL(clicked()), this, SLOT(onDelete()));

	changeButtonsState();
}

AnimationListPane::~AnimationListPane()
{
	delete model_;
	delete delegate_;
}

void AnimationListPane::setDocument(EditorDocument * document)
{
	if (document_) document_->disconnect(this);
	document_ = document;

	delete model_;
	model_ = new AnimationListModel();
	treeView_->setModel(model_);
	model_->setDocument(document);
	
	delete delegate_;
	delegate_ = new AnimationListDelegate(this);	
	treeView_->setItemDelegate(delegate_);
	delegate_->setDocument(document);

	treeView_->setDocument(document);

	if (document_)
	{
		connect(model_, SIGNAL(layoutChanged()), treeView_, SLOT(layoutChanged()));
		connect(model_, SIGNAL(layoutChanged()), this, SLOT(changeButtonsState()));
		connect(document_, SIGNAL(sceneSelectionChanged()), this, SLOT(updateScene()));
		connect(document_, SIGNAL(sceneChanged()), this, SLOT(updateScene()));
		updateScene();
	}
	else
	{
		label_->setText(QString());
	}
}

void AnimationListPane::keyPressEvent(QKeyEvent * event)
{
	QWidget::keyPressEvent(event);

	if(event->key() == Qt::Key_Delete)
		onDelete();
}

void AnimationListPane::keyReleaseEvent(QKeyEvent * event)
{
	QWidget::keyReleaseEvent(event);
}

QSize AnimationListPane::sizeHint() const
{
	return QSize(150,150);
}


void AnimationListPane::changeButtonsState()
{
	QItemSelectionModel * selModel = treeView_->selectionModel();
	bool isSelected = 0;
	if (selModel)
	{
		QModelIndexList selIndexList = selModel->selectedRows(0);
		if (!selIndexList.empty()) isSelected = true;
	}
	
	delButton_->setEnabled(isSelected);
	//editButton_->setEnabled(isSelected);
}

void AnimationListPane::onNew()
{
	document_->doCommand(new NewAnimationCmd(
		document_->selectedScene(), AnimationSPtr(new Animation)));
}

void AnimationListPane::onDelete()
{
	document_->doCommand(new DeleteAnimationsCmd(
		document_->selectedScene(), document_->selectedAnimations()));
}

void AnimationListPane::updateScene()
{
	if (!isEnabled()) return;

	Scene * selScene = document_->selectedScene();

	label_->setText(
		QString("<b>") + tr("Animations: ") + "</b> " + 
		stdStringToQString(selScene->name()));		

}
