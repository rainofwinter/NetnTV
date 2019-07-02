#include "stdafx.h"
#include "ActionTargetListPane.h"
#include "EditorDocument.h"
#include "EditorGroup.h"
#include "EditorMaskObject.h"
#include "Scene.h"
#include "Root.h"
#include "Group.h"
#include "Types.h"
#include "ListEditorWidget.h"
#include "Animation.h"
#include "PaneLabel.h"
#include "Utils.h"
#include "MaskObject.h"

using namespace std;

Q_DECLARE_METATYPE(SceneObject *)

ActionTargetListModel::ActionTargetListModel()
{
	document_ = 0;
}

ActionTargetListModel::~ActionTargetListModel()
{
}

void ActionTargetListModel::setDocument(EditorDocument * document)
{
	document_ = document;
	emit layoutChanged();

}


QVariant ActionTargetListModel::data(const QModelIndex & index, int role) const
{	
	if (!index.isValid()) return QVariant();

	if (role == Qt::DisplayRole) 
	{
		if (index.column() == 0)
		{
			SceneObject * item = 
				(SceneObject *)index.internalPointer();
			return QVariant::fromValue(item);
		}
		else
			return QVariant();
	}
	
	return QVariant();
}

Qt::ItemFlags ActionTargetListModel::flags(const QModelIndex &index) const
{
	if (!index.isValid()) return 0;
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}


QVariant ActionTargetListModel::headerData(
	int section, Qt::Orientation orientation, int role) const
{
	return tr("Object");
}

QModelIndex ActionTargetListModel::index(int row, int column, const QModelIndex &parent) const
{
	if (!hasIndex(row, column, parent))
		return QModelIndex();

	if (column != 0) return QModelIndex();

	Scene * scene = document_->selectedScene();
	int numTopLevelObjs = scene->numTopLevelObjects();

	SceneObject * parentObj;
	if (parent.isValid()) parentObj = (SceneObject *)parent.internalPointer();
	else parentObj = (SceneObject *)document_->selectedScene()->root();

	int numChildren = parentObj->children().size();

	if (parent.isValid())
	{
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
	else if (row == 0)
	{		
		return createIndex(row, column, 0);
	}
	else
	{		
		
		SceneObject * obj = scene->root()->children()[numChildren - row].get();
		return createIndex(row, column, obj);
	}	

	return QModelIndex();
	
}

QModelIndex ActionTargetListModel::parent(const QModelIndex & index) const
{
	if (!index.isValid() || !document_) return QModelIndex();
	SceneObject * obj = (SceneObject *)index.internalPointer();
	if (!obj) return QModelIndex();

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

int ActionTargetListModel::rowCount(const QModelIndex & parent) const
{	
	if (!document_) return 0;
	Scene * scene = document_->selectedScene();
	if (!parent.isValid())
	{
		return scene->numTopLevelObjects() + 1;
	}
	else 
	{
		SceneObject * obj = (SceneObject *)parent.internalPointer();
		if (!obj) return 0;
		else if (obj->maskObject())
			return obj->children().size() + 1;
		else
			return obj->children().size();
	}
	
}

int ActionTargetListModel::columnCount(const QModelIndex & parent) const
{
	return 1;
}


////////////////////////////////////////////////////////////////////////////////
ActionTargetListDelegate::ActionTargetListDelegate(ActionTargetListPane * parent) 
:
QStyledItemDelegate()
{
	document_ = 0;	
	parent_ = parent;
}

void ActionTargetListDelegate::paint(QPainter *painter, 
	const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QStyleOptionViewItemV4 opt = option;
	opt.state &= ~QStyle::State_HasFocus;
	opt.state |= QStyle::State_Active;
	

	if (index.column() == 0)
	{		
		SceneObject * obj = 
			index.data(Qt::DisplayRole).value<SceneObject *>();

		QString text;
		if (obj)
		{
			text =  stdStringToQString(obj->id());		
		}
		else 
			text = tr("No target");
		
		
		initStyleOption(&opt, index);
		
		painter->save();

		const QStyle * style = 
			opt.SO_DockWidget ? opt.widget->style() : QApplication::style();
		const QWidget * widget = opt.widget;
	
		style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, widget);
/*
		QPixmap * screenShot = sceneImageMap_[scene];
		painter->drawPixmap(opt.rect.x() + thumbBorderMargin_, 
			opt.rect.y() + opt.rect.height()/2 - screenShot->height()/2, 
			*screenShot);

		painter->setFont(opt.font);
		qreal x = screenShot->width() + 2*thumbBorderMargin_ + thumbNameMargin_;		
		qreal y = opt.rect.y() + opt.rect.height()/2 + opt.fontMetrics.ascent()/2;
		painter->drawText(x, y, text);
		painter->restore();	*/

		QRect rect = opt.fontMetrics.tightBoundingRect(tr("A"));
		int fontHeight = rect.height();

		if (obj)
		{
			painter->setFont(opt.font);
		}
		else
		{
			QFont noneFont(opt.font);
			noneFont.setItalic(true);
			painter->setFont(noneFont);
		}
		qreal x = opt.rect.x() + 5;				
		qreal y = opt.rect.y() + opt.rect.height()/2 + fontHeight / 2;
		painter->drawText(x, y, text);

/*
		GlobalState & gs = GlobalState::Instance();
		const QPixmap * checkedImg = gs.ObjectCheckedPixmap();
		const QPixmap * uncheckedImg = gs.ObjectUncheckedPixmap();
		const QPixmap * drawImg;
		qreal ix, iy;
		float checkBoxMargin = parent_->GetCheckBoxMargin();
		ix = opt.rect.right() - checkedImg->width() - checkBoxMargin;
		iy = opt.rect.y() + (opt.rect.height() - checkedImg->height()) / 2;
		if (obj->IsThisLocked())
			drawImg = checkedImg;
		else
			drawImg = uncheckedImg;

		painter->drawPixmap(ix, iy, *drawImg);

		ix -= (checkedImg->width() + checkBoxMargin);

		if (obj->IsUIVisible())
			drawImg = checkedImg;
		else
			drawImg = uncheckedImg;

		painter->drawPixmap(ix, iy, *drawImg);
		*/

		painter->restore();
	}
	else
		QStyledItemDelegate::paint(painter, option, index);
}

QSize ActionTargetListDelegate::sizeHint(
	const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	return QSize(0, 20);
}

void ActionTargetListDelegate::setDocument(EditorDocument * document)
{
	document_ = document;
}

///////////////////////////////////////////////////////////////////////////////
ActionTargetListTreeView::ActionTargetListTreeView(ActionTargetListPane * parent)
{
	parent_ = parent;
	setIndentation(15);
	//setFrameShape(QFrame::NoFrame);
	document_ = 0;
}

void ActionTargetListTreeView::setDocument(EditorDocument * document)
{
	if (document_) document_->disconnect(this);
	document_ = document;

	connect(document_, SIGNAL(objectListChanged()),
		this, SLOT(onActionTargetListChanged()));

	onActionTargetListChanged();
}


void ActionTargetListTreeView::onActionTargetListChanged()
{
	QModelIndexList indexes = model()->match(
		model()->index(0,0, QModelIndex()), Qt::DisplayRole, "*", -1, 
		Qt::MatchWildcard|Qt::MatchRecursive);
		
	foreach (QModelIndex index, indexes)
	{
		SceneObject * obj = (SceneObject *)index.internalPointer();
		if (!obj) continue;
		Group * grp = obj->group();
		if (grp)
		{		
			EditorGroup * edGrp = (EditorGroup *)document_->editorObject(grp).get();
			if (edGrp->expanded())
				expand(index);
			else
				collapse(index);
		}

		MaskObject * maskObj = obj->maskObject();
		if (maskObj)
		{		
			EditorMaskObject * edGrp = (EditorMaskObject *)document_->editorObject(maskObj).get();
			if (edGrp->expanded())
				expand(index);
			else
				collapse(index);
		}
	}
}

void ActionTargetListTreeView::selectionChanged(
	const QItemSelection &selected, const QItemSelection &deselected)
{
	QTreeView::selectionChanged(selected, deselected);

	QItemSelectionModel * selModel = this->selectionModel();
	const QModelIndexList & selIndices = selModel->selectedIndexes();
	if (!selIndices.empty())
	{
		
		emit itemSelected((SceneObject *)selIndices.front().internalPointer());
	}
	
}


///////////////////////////////////////////////////////////////////////////////
ActionTargetListPane::ActionTargetListPane()
{	
	document_ = 0;
	delegate_ = 0;

	treeView_ = new ActionTargetListTreeView(this);
	model_ = 0;
	treeView_->setSelectionMode(QAbstractItemView::SingleSelection);

	QBoxLayout * layout = new QVBoxLayout(this);

	layout->setMargin(0);
	layout->setSpacing(0);
	label_ = new PaneLabel;
	label_->setText(tr("Action Target"));
	layout->addWidget(label_);
	
	layout->addWidget(treeView_);

	connect(treeView_, SIGNAL(itemSelected(SceneObject *)), 
		this, SIGNAL(itemSelected(SceneObject *)));
}

ActionTargetListPane::~ActionTargetListPane()
{
	delete model_;
	delete delegate_;
}



QSize ActionTargetListPane::sizeHint() const
{
	return QSize(150,150);
}

void ActionTargetListPane::setDocument(EditorDocument * document)
{
	document_ = document;

	delete model_;
	model_ = new ActionTargetListModel();		
	model_->setDocument(document);
		
	delete delegate_;
	delegate_ = new ActionTargetListDelegate(this);
	delegate_->setDocument(document);
	
	treeView_->setModel(model_);
	treeView_->setItemDelegate(delegate_);
	treeView_->setDocument(document);
	
}


void ActionTargetListPane::selectItem(int index)
{
	QItemSelectionModel * selModel = treeView_->selectionModel();
	selModel->clearSelection();
	selModel->select(
		model_->index(index, 0, QModelIndex()), QItemSelectionModel::Select);
}


void ActionTargetListPane::selectObject(SceneObject * object)
{
	QModelIndexList indexes = model_->match(
		model_->index(0,0, QModelIndex()), Qt::DisplayRole, QVariant::fromValue(object), 1, 
		Qt::MatchExactly|Qt::MatchRecursive);

	QItemSelectionModel * selModel = treeView_->selectionModel();
	selModel->clearSelection();
		
	foreach (QModelIndex index, indexes)
	{
		selModel->select(index, QItemSelectionModel::Select);		
	}
}