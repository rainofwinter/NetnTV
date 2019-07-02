#include "stdafx.h"
#include "GenericList.h"
#include "ListEditorWidget.h"
using namespace std;


GenericListModel::GenericListModel()
{
	editable_ = false;
	reorderable_ = false;
}

GenericListModel::~GenericListModel()
{
	BOOST_FOREACH(GenericListItem * item, items_)
		delete item;
}

void GenericListModel::addTopLevelItem(const QString & str, void * data)
{
	items_.push_back(new GenericListItem(str, data));
	emit layoutChanged();
}

void GenericListModel::clearItems()
{
	BOOST_FOREACH(GenericListItem * item, items_) delete item;
	items_.clear();
	selItems_.clear();
	emit layoutChanged();
}

QStringList GenericListModel::mimeTypes() const
{
    QStringList types;
    types << QLatin1String("application/generic-list-item-ptr");
    return types;
}


QMimeData * GenericListModel::mimeData(const QModelIndexList & indexes) const
{
	QMimeData * mimeData = new QMimeData();
	QByteArray encodedData;

	QDataStream stream(&encodedData, QIODevice::WriteOnly);
	QModelIndexList sortedIndices = indexes;
	
	std::sort(sortedIndices.begin(), sortedIndices.end());

	foreach (QModelIndex index, sortedIndices) {
		if (index.isValid()) {
			QVariant variant = data(index, Qt::DisplayRole);

			if (variant.canConvert<GenericListItem *>())
			{
				GenericListItem * item = variant.value<GenericListItem *>();
				stream.writeRawData((char *)&item, sizeof(GenericListItem *));
			}						
		}
	}
     mimeData->setData("application/generic-list-item-ptr", encodedData);
     return mimeData;
}


QVariant GenericListModel::data(const QModelIndex & index, int role) const
{	
	if (!index.isValid()) return QVariant();

	if (role == Qt::DisplayRole) 
	{
		if (index.column() == 0)
		{
			GenericListItem * obj = (GenericListItem *)index.internalPointer();
			return QVariant::fromValue(obj);
		}
		else
			return QVariant();
	}
	
	return QVariant();
}

Qt::ItemFlags GenericListModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
	{
		return Qt::ItemIsDropEnabled;
	}

	Qt::ItemFlags flags = 0;	
	flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	if (editable_) flags |= Qt::ItemIsEditable;
	if (reorderable_) flags |= Qt::ItemIsDragEnabled;		
	
	return flags;
		
}

Qt::DropActions GenericListModel::supportedDropActions() const
{
	return Qt::MoveAction;
}

QVariant GenericListModel::headerData(
	int section, Qt::Orientation orientation, int role) const
{
	return tr("Item");
}

QModelIndex GenericListModel::index(int row, int column, const QModelIndex &parent) const
{
	if (!hasIndex(row, column, parent)) return QModelIndex();
	if (column != 0) return QModelIndex();
	
	return createIndex(row, 0, items_[row]);
}

QModelIndex GenericListModel::parent(const QModelIndex & index) const
{
	return QModelIndex();	
}

int GenericListModel::rowCount(const QModelIndex & parent) const
{	
	if (!parent.isValid())
	{
		return items_.size();
	}
	else
		return 0;
	
}

int GenericListModel::columnCount(const QModelIndex & parent) const
{
	return 1;
}

bool GenericListModel::dropMimeData(const QMimeData * mimedata,
	Qt::DropAction action, int row, int column, const QModelIndex & parent)
{
	QStringList types = mimeTypes();
	QString format = types.at(0);
    QByteArray encoded = mimedata->data(format);
	QDataStream stream(&encoded, QIODevice::ReadOnly);
	
	vector<GenericListItem *> objs;
	
	while (!stream.atEnd()) {
		GenericListItem * obj;		
		stream.readRawData((char *)&obj, sizeof(GenericListItem *));	
		objs.push_back(obj);
	}
	GenericListItem * moveTarget = 0;

	QModelIndex idx = index(row, 0, parent);
	moveTarget = data(idx, Qt::DisplayRole).value<GenericListItem *>();

	BOOST_FOREACH(GenericListItem * item, objs)
		insertBefore(item, moveTarget);
	emit layoutChanged();
	return true;
}

void GenericListModel::insertBefore(
	GenericListItem * newObj, GenericListItem * refObj)
{
	vector<GenericListItem *>::iterator insertIter, removeIter, iter;
	if (newObj == refObj) return;
	
	removeIter = items_.end();
	insertIter = items_.end();	
	for (iter = items_.begin(); iter != items_.end(); ++iter)	
	{
		if ((*iter) == newObj) removeIter = iter;	
		if (*iter == refObj) insertIter = iter;
	}

	items_.erase(removeIter);

	insertIter = items_.end();	
	for (iter = items_.begin(); iter != items_.end(); ++iter)	
		if (*iter == refObj) insertIter = iter;	

	items_.insert(insertIter, newObj);
}

bool GenericListModel::isItemSelected(GenericListItem * item) const
{
	BOOST_FOREACH(GenericListItem * selItem, selItems_)
	{
		if (item == selItem) return true;
	}
	return false;
}

void GenericListModel::setEditable(bool editable)
{
	editable_ = editable;
}

void GenericListModel::setReorderable(bool reorderable)
{
	reorderable_ = reorderable;
}

void GenericListModel::deleteSelectedItems()
{
	vector<GenericListItem *>::iterator iter, selIter;
	for (iter = items_.begin(); iter != items_.end();)
	{
		bool isSelected = false;
		for (selIter = selItems_.begin(); selIter != selItems_.end(); ++selIter)
			if (*selIter == *iter) isSelected = true;

		if (isSelected)
		{
			delete *iter;
			iter = items_.erase(iter);
		}
		else
		{
			++iter;
		}
	}

	selItems_.clear();	
	emit layoutChanged();
}

////////////////////////////////////////////////////////////////////////////////
GenericListDelegate::GenericListDelegate() 
:
QStyledItemDelegate()
{
}

void GenericListDelegate::paint(QPainter *painter, 
	const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QStyleOptionViewItemV4 opt = option;
	opt.state &= ~QStyle::State_HasFocus;
	opt.state |= QStyle::State_Active;
	if (index.column() == 0)
	{		
		GenericListItem * obj = 
			index.data(Qt::DisplayRole).value<GenericListItem *>();
		QString text =  obj->string;		
		
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
		painter->drawText(x, y, text);		
		
		painter->restore();
	}
	else
		QStyledItemDelegate::paint(painter, option, index);
}

QSize GenericListDelegate::sizeHint(
	const QStyleOptionViewItem &option, const QModelIndex &index) const
{	
	return QSize(0, 20);
}

QWidget * GenericListDelegate::createEditor(QWidget * parent, 
	const QStyleOptionViewItem & option, const QModelIndex & index) const
{

	if (index.column() == 0)
	{		
		GenericListItem * obj = (GenericListItem *)index.internalPointer();		

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

void GenericListDelegate::setModelData(QWidget *editor, 
		QAbstractItemModel *model, const QModelIndex &index) const
{
	if (index.column() == 0)
	{
		GenericListItem * obj = 
			index.model()->data(index, Qt::DisplayRole).value<GenericListItem *>();

		ListEditorWidget * widget = 
			qobject_cast<ListEditorWidget *>(editor);

		obj->string = widget->GetLineEdit()->text();
		emit const_cast<GenericListDelegate *>(this)->itemEdited(obj);
		
	}
	else
		QStyledItemDelegate::setModelData(editor, model, index);
}

void GenericListDelegate::setEditorData(QWidget *editor, 
	const QModelIndex &index) const
{
	if (index.column() == 0)
	{
		GenericListItem * obj = 
			index.model()->data(index, Qt::DisplayRole).value<GenericListItem *>();

		ListEditorWidget * widget = 
			qobject_cast<ListEditorWidget *>(editor);
		widget->GetLineEdit()->setText(obj->string);

	}
	else
		QStyledItemDelegate::setEditorData(editor, index);
}

void GenericListDelegate::commitAndCloseEditor()
{
	ListEditorWidget * editor = qobject_cast<ListEditorWidget *>(sender());
	emit commitData(editor);
	emit closeEditor(editor);
}

void GenericListDelegate::cancelEdit()
{
	ListEditorWidget * editor = qobject_cast<ListEditorWidget *>(sender());
	emit closeEditor(editor);
}

///////////////////////////////////////////////////////////////////////////////
GenericList::GenericList()
{
	ignoreSelChanged_ = false;
	setIndentation(0);
	setRootIsDecorated(false);
	//setFrameShape(QFrame::NoFrame);
	model_ = new GenericListModel;
	setModel(model_);

	delegate_ = new GenericListDelegate;
	setItemDelegate(delegate_);

	setMode(MultipleMode);

	connect(model_, SIGNAL(layoutChanged()), this, SLOT(onLayoutChanged()));
	connect(model_, SIGNAL(layoutChanged()), this, SIGNAL(layoutChanged()));
	connect(delegate_, SIGNAL(itemEdited(GenericListItem *)), this, SIGNAL(itemEdited(GenericListItem *)));
}

void GenericList::setMode(SelectionMode mode)
{
	if (mode == SingleMode)
	{
		setSelectionMode(QAbstractItemView::SingleSelection);
	}
	else
	{
		setDragEnabled(true);
		setAcceptDrops(true);
		setDropIndicatorShown(true);
		setDefaultDropAction(Qt::MoveAction);
		setDragDropMode(QAbstractItemView::InternalMove);
		setSelectionMode(QAbstractItemView::ExtendedSelection);
	}

}

int GenericList::count() const
{
	return model_->rowCount(QModelIndex());
}

void GenericList::selectIndex(int index)
{
	QItemSelectionModel * selModel = selectionModel();
	QModelIndex modelIndex = model_->index(index, 0, QModelIndex());
	if (modelIndex.isValid())
		selModel->select(modelIndex, QItemSelectionModel::Select);
	else
		selModel->clearSelection();
}

int GenericList::selectedIndex() const
{
	QItemSelectionModel * selModel = selectionModel();
	QModelIndexList sels = selModel->selectedRows();
	if (sels.empty()) return -1;
	return sels.front().row();
}

GenericList::~GenericList()
{
	delete model_;
	delete delegate_;
}

void GenericList::onLayoutChanged()
{
	
	ignoreSelChanged_ = true;
	QItemSelectionModel * selModel = selectionModel();
	selModel->clearSelection();
		
	QAbstractItemModel * m = model();
	int numObjs = m->rowCount();
	for (int i = 0; i < numObjs; ++i)
	{
		QModelIndex index = m->index(i, 0);
		GenericListItem * obj = (GenericListItem *)index.internalPointer();
		if (model_->isItemSelected(obj))
			selModel->select(index, QItemSelectionModel::Select);
	}

	ignoreSelChanged_ = false;

}

void GenericList::mousePressEvent(QMouseEvent *event)
{
	QTreeView::mousePressEvent(event);
}

void GenericList::mouseDoubleClickEvent(QMouseEvent * event)
{
	QTreeView::mouseDoubleClickEvent(event);
}

void GenericList::expanded(const QModelIndex &index)
{
	QTreeView::expanded(index);
}

void GenericList::collapsed(const QModelIndex &index)
{
	QTreeView::collapsed(index);
}



void GenericList::selectionChanged(
		const QItemSelection &selected, const QItemSelection &deselected)
{
	
	QTreeView::selectionChanged(selected, deselected);
	
	if (ignoreSelChanged_) return;

	QItemSelectionModel * selModel = selectionModel();
	QModelIndexList selIndexList = selModel->selectedRows(0);
	
	vector<GenericListItem *> selObjs;
	foreach(QModelIndex index, selIndexList)
	{
		GenericListItem * selObj = index.data().value<GenericListItem *>();
		selObjs.push_back(selObj);
		
	}
	model_->setSelectedItems(selObjs);	

	emit selectionChanged();
}

void GenericList::addTopLevelItem(const QString & str, void * data)
{
	model_->addTopLevelItem(str, data);
}

void GenericList::clearItems()
{
	model_->clearItems();
	emit selectionChanged();
}

void GenericList::setEditable(bool editable)
{
	model_->setEditable(editable);
}

void GenericList::setReorderable(bool reorderable)
{
	model_->setReorderable(reorderable);
}

const std::vector<GenericListItem *> & GenericList::selectedItems() const
{
	return model_->selectedItems();
}

const std::vector<GenericListItem *> & GenericList::items() const
{
	return model_->items_;
}

void GenericList::deleteSelectedItems()
{
	model_->deleteSelectedItems();
	emit selectionChanged();
}
