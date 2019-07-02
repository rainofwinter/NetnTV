#include "stdafx.h"
#include "EventListPane.h"
#include "EditorDocument.h"
#include "EditorEvent.h"
#include "EditorObject.h"
#include "EditorGlobal.h"
#include "EventListener.h"
#include "Event.h"
#include "Command.h"
#include "Scene.h"
#include "Root.h"
#include "SceneObject.h"
#include "AppObject.h"
#include "ListEditorWidget.h"
#include "PaneButton.h"
#include "EventDlg.h"
#include "Utils.h"

using namespace std;
Q_DECLARE_METATYPE(EventListener *)

SceneObject * curObject(EditorDocument * document)
{
	SceneObject * obj = document->selectedObject();
	if (!obj) obj = document->selectedScene()->root();
	return obj;
}
AppObject * curAppObject(EditorDocument * document)
{
	AppObject * obj = document->selectedAppObject();
	if (!obj) obj = NULL;
	return obj;
}

EventListModel::EventListModel()
{
	document_ = 0;
}

EventListModel::~EventListModel()
{	
}

void EventListModel::setDocument(EditorDocument * document)
{
	if (document_) document_->disconnect(this);

	emit layoutAboutToBeChanged();
	document_ = document;
	emit layoutChanged();

	if (document_)
	{
		connect(document_, SIGNAL(objectSelectionChanged()),
			this, SLOT(update()));

		connect(document_, SIGNAL(appObjectSelectionChanged()),
			this, SLOT(update()));

		connect(document_, SIGNAL(listenerListChanged()),
			this, SLOT(update()));

		connect(document_, SIGNAL(listenerChanged()),
			this, SLOT(update()));
	}
}

void EventListModel::update()
{
	emit layoutChanged();	
}


QStringList EventListModel::mimeTypes() const
{
    QStringList types;
    types << QLatin1String("application/listener-ptr");
    return types;
}



QMimeData * EventListModel::mimeData(const QModelIndexList & indexes) const
{
	QMimeData * mimeData = new QMimeData();
	QByteArray encodedData;

	QDataStream stream(&encodedData, QIODevice::WriteOnly);
	QModelIndexList sortedIndices = indexes;
	
	std::sort(sortedIndices.begin(), sortedIndices.end());

	foreach (QModelIndex index, sortedIndices) {
		if (index.isValid()) {
			QVariant variant = data(index, Qt::DisplayRole);

			if (variant.canConvert<EventListener *>())
			{
				EventListener * channel = variant.value<EventListener *>();
				stream.writeRawData((char *)&channel, sizeof(EventListener *));
			}						
		}
	}
     mimeData->setData("application/listener-ptr", encodedData);
     return mimeData;
}


QVariant EventListModel::data(const QModelIndex & index, int role) const
{	
	if (!index.isValid()) return QVariant();

	if (role == Qt::DisplayRole) 
	{
		if (index.column() == 0)
		{
			EventListener * obj = (EventListener *)index.internalPointer();
			return QVariant::fromValue(obj);
		}
		else
			return QVariant();
	}
	return QVariant();
}

Qt::ItemFlags EventListModel::flags(const QModelIndex &index) const
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

Qt::DropActions EventListModel::supportedDropActions() const
{
	return Qt::MoveAction;
}

QVariant EventListModel::headerData(
	int section, Qt::Orientation orientation, int role) const
{
	return tr("Channel");
}

QModelIndex EventListModel::index(int row, int column, const QModelIndex &parent) const
{
	if (!hasIndex(row, column, parent)) return QModelIndex();
	if (column != 0) return QModelIndex();
	
	SceneObject * selObj = curObject(document_);
	AppObject* appObj = curAppObject(document_);

	if (appObj)
	{
		const vector<EventListenerSPtr> & objs = appObj->eventListeners();	

		return createIndex(row, column, objs[row].get());
	}
	else
	{
		const vector<EventListenerSPtr> & objs = selObj->eventListeners();	

		return createIndex(row, column, objs[row].get());
	}	
}

QModelIndex EventListModel::parent(const QModelIndex & index) const
{
	return QModelIndex();	
}

int EventListModel::rowCount(const QModelIndex & parent) const
{	
	if (!document_ || parent.isValid()) return 0;
	SceneObject * selObj = curObject(document_);
	AppObject* appObj = curAppObject(document_);

	if (appObj)
		return appObj->eventListeners().size();	
	else
		return selObj->eventListeners().size();	
}

int EventListModel::columnCount(const QModelIndex & parent) const
{
	return 1;
}

bool EventListModel::dropMimeData(const QMimeData * mimedata,
	Qt::DropAction action, int row, int column, const QModelIndex & parent)
{
	
	QStringList types = mimeTypes();
	QString format = types.at(0);
    QByteArray encoded = mimedata->data(format);
	QDataStream stream(&encoded, QIODevice::ReadOnly);
	
	vector<EventListener *> objs;
	
	while (!stream.atEnd()) {
		EventListener * obj;		
		stream.readRawData((char *)&obj, sizeof(EventListener *));	
		objs.push_back(obj);
	}
	EventListener * moveTarget = 0;

	QModelIndex idx = index(row, 0, parent);
	moveTarget = data(idx, Qt::DisplayRole).value<EventListener *>();
	
	SceneObject * sceneObj = curObject(document_);
	AppObject* appObj = curAppObject(document_);
	vector<EventListenerSPtr> objsToMove;
	if (appObj)
	{
		foreach(EventListener * obj, objs)
			objsToMove.push_back(appObj->findListener(obj));
	}
	else
	{
		foreach(EventListener * obj, objs)
			objsToMove.push_back(sceneObj->findListener(obj));
	}

	try
	{
	document_->doCommand(new MoveEventListenersCmd(
		sceneObj, objsToMove, moveTarget));
	}
	catch(...) {}
	return true;
}

////////////////////////////////////////////////////////////////////////////////
EventListDelegate::EventListDelegate(QTreeView * parent) 
:
QStyledItemDelegate()
{
	parent_ = parent;
}

void EventListDelegate::paint(QPainter *painter, 
	const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QStyleOptionViewItemV4 opt = option;
	opt.state &= ~QStyle::State_HasFocus;
	opt.state |= QStyle::State_Active;
	if (index.column() == 0)
	{		
		EventListener * obj = 
			index.data(Qt::DisplayRole).value<EventListener *>();

		Event * event = obj->event();
		EditorEvent * edEvent = EditorGlobal::instance().editorEvent(event);
		QString text;

		if (edEvent)
			text = edEvent->toString(event);
		else
			text = QString::fromLocal8Bit(event->typeStr());
		
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

QSize EventListDelegate::sizeHint(
	const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	return QSize(0, 20);
}


///////////////////////////////////////////////////////////////////////////////

void EventListTreeView::setDocument(EditorDocument * document)
{
	if (document_) document_->disconnect(this);
	document_ = document;
	
	if (document_)
	{
		connect(document_, SIGNAL(listenerSelectionChanged()), 
			this, SLOT(updateSelection()));	

		connect(document_, SIGNAL(objectSelectionChanged()),
			this, SLOT(onObjectSelectionChanged()));
		updateSelection();
	}
}

EventListTreeView::EventListTreeView(EventListPane * parent)
{
	document_ = 0;
	parent_ = parent;
	ignoreSelChanged_ = false;
	setIndentation(0);
	//setFrameShape(QFrame::NoFrame);
	setRootIsDecorated(false);
}

void EventListTreeView::updateSelection()
{
	
	ignoreSelChanged_ = true;
	QItemSelectionModel * selModel = selectionModel();
	selModel->clearSelection();
		
	QAbstractItemModel * m = model();
	int numObjs = m->rowCount();
	for (int i = 0; i < numObjs; ++i)
	{
		QModelIndex index = m->index(i, 0);
		EventListener * obj = (EventListener *)index.internalPointer();
		if (document_->isListenerSelected(obj))
			selModel->select(index, QItemSelectionModel::Select);
	}
	
	ignoreSelChanged_ = false;
	parent_->changeButtonsState();
	
}

void EventListTreeView::onObjectSelectionChanged()
{
	SceneObject * obj = document_->selectedObject();
	if (!obj) obj = document_->selectedScene()->root();


	QItemSelectionModel * selModel = selectionModel();
	if (selModel->selectedIndexes().isEmpty())
	{
		QAbstractItemModel * m = model();
		if (m->rowCount())
		{
			QModelIndex index = m->index(0, 0);
			EventListener * listener = (EventListener *)index.internalPointer();
			document_->setSelectedListeners(
				vector<EventListener *>(&listener, &listener + 1));			
		}
		
	}

}

void EventListTreeView::selectionChanged(
		const QItemSelection &selected, const QItemSelection &deselected)
{
	
	QTreeView::selectionChanged(selected, deselected);
	if (ignoreSelChanged_) return;

	QItemSelectionModel * selModel = selectionModel();
	QModelIndexList selIndexList = selModel->selectedRows(0);
	
	vector<EventListener *> selObjs;
	foreach(QModelIndex index, selIndexList)
	{
		EventListener * selObj = index.data().value<EventListener *>();
		selObjs.push_back(selObj);
		
	}
	document_->setSelectedListeners(selObjs);
	parent_->changeButtonsState();
	
}

void EventListTreeView::mouseDoubleClickEvent(QMouseEvent *event)
{
	QTreeView::mouseDoubleClickEvent(event);

	if(document_->selectedListeners().size() != 0)
		parent_->onEdit();
	else
		parent_->onNew();
}

///////////////////////////////////////////////////////////////////////////////
EventListPane::EventListPane(MainWindow * mainWindow)
{
	document_ = 0;
	delegate_ = 0;
	treeView_ = new EventListTreeView(this);
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
	label_->setMinimumWidth(50);
	label_->setMinimumHeight(25);
	label_->setMaximumHeight(25);	
	layout->addWidget(label_);
	layout->addWidget(treeView_);

	
	QBoxLayout * btnLayout = new QBoxLayout(QBoxLayout::LeftToRight);
	btnLayout->setMargin(0);
	btnLayout->setSpacing(0);
//	btnLayout->addWidget(newButton_ = new PaneButton(tr("New...")));
//	btnLayout->addWidget(delButton_ = new PaneButton(tr("Delete")));
//	btnLayout->addWidget(editButton_ = new PaneButton(tr("Edit...")));
	btnLayout->setAlignment(Qt::AlignRight);
	btnLayout->addWidget(newButton_ = new PaneButton(QPixmap(":/data/Resources/event_action/new.png"), QSize(30, 30), "New"));
	btnLayout->addWidget(delButton_ = new PaneButton(QPixmap(":/data/Resources/event_action/delete.png"), QSize(30, 30), "Delete"));
	btnLayout->addWidget(editButton_ = new PaneButton(QPixmap(":/data/Resources/event_action/edit.png"), QSize(30, 30), "Edit"));
	layout->addLayout(btnLayout);

	connect(treeView_->verticalScrollBar(), SIGNAL(rangeChanged(int, int)), 
		this, SIGNAL(rangeChanged(int, int)));

	connect(treeView_->verticalScrollBar(), SIGNAL(valueChanged(int)), 
		this, SIGNAL(valueChanged(int)));

	connect(newButton_, SIGNAL(clicked()), this, SLOT(onNew()));
	connect(delButton_, SIGNAL(clicked()), this, SLOT(onDelete()));
	connect(editButton_, SIGNAL(clicked()), this, SLOT(onEdit()));
}

EventListPane::~EventListPane()
{
	delete model_;
	delete delegate_;
}

void EventListPane::setDocument(EditorDocument * document)
{
	if (document_) document_->disconnect(this);
	document_ = document;

	delete model_;
	model_ = new EventListModel();
	treeView_->setModel(model_);
	
	model_->setDocument(document);
	treeView_->setDocument(document);

	connect(model_, SIGNAL(layoutChanged()), treeView_, SLOT(updateSelection()));
	
	delete delegate_;
	delegate_ = new EventListDelegate(treeView_);	
	treeView_->setItemDelegate(delegate_);

	if (document_)
	{
		setEnabled(true);
		connect(model_, SIGNAL(layoutChanged()), this, SLOT(changeButtonsState()));
		changeButtonsState();


		connect(document_, SIGNAL(objectSelectionChanged()), 
			this, SLOT(updateObject()));

		connect(document_, SIGNAL(objectChanged()),
			this, SLOT(updateObject()));
		connect(document_, SIGNAL(appObjectChanged()),
			this, SLOT(updateAppObject()));
		connect(document_, SIGNAL(appObjectSelectionChanged()),
			this, SLOT(updateAppObject()));
		updateObject();
	}
	else
	{
		setEnabled(false);
		label_->setText(QString());
	}
	
}


void EventListPane::setVScrollValue(int val)
{
	treeView_->verticalScrollBar()->setValue(val);
}


QSize EventListPane::sizeHint() const
{
	return QSize(150,150);
}

void EventListPane::changeButtonsState()
{
	QItemSelectionModel * selModel = treeView_->selectionModel();
	bool isSelected = 0;
	if (selModel)
	{
		QModelIndexList selIndexList = selModel->selectedRows(0);
		if (!selIndexList.empty()) isSelected = true;
	}
	
	delButton_->setEnabled(isSelected);
	editButton_->setEnabled(isSelected);
	newButton_->setEnabled(document_);
	
}

void EventListPane::updateAppObject()
{
	AppObject * appObj = document_->selectedAppObject();
	QString text = QString("<b>") + tr("Events: ") + "</b> ";
	if (appObj)
	{
		label_->setText(text + stdStringToQString(appObj->id()));
	}
	else
	{		
		label_->setText(text + "<i>" + tr("Scene Root") + "</i>");
	}
}

void EventListPane::updateObject()
{
	SceneObject * selObj = document_->selectedObject();
	QString text = QString("<b>") + tr("Events: ") + "</b> ";
	if (selObj)
	{
		label_->setText(text + stdStringToQString(selObj->id()));		
	}
	else
	{		
		label_->setText(text + "<i>" + tr("Scene Root") + "</i>");
	}
	
}

void EventListPane::onNew()
{
	QWidget * dlgParent = parentWidget();
	while (dlgParent->parent()) dlgParent = dlgParent->parentWidget();

	SceneObject * selObj = curObject(document_);
	AppObject * appObj = curAppObject(document_);
	if (appObj)
	{
		EventDlg dlg(document_, appObj, 0, dlgParent);
		if (dlg.exec() == QDialog::Accepted)
		{
			EventListenerSPtr listener;
			listener.reset(new EventListener(
				EventSPtr(dlg.createRequestedEvent())));

			document_->doCommand(new NewEventListenerCmd(
				appObj, listener));

			//select the newly created object
			vector<EventListener *> selObjs;
			selObjs.push_back(listener.get());
			document_->setSelectedListeners(selObjs);
		}
	}
	else
	{
		EventDlg dlg(document_, selObj, 0, dlgParent);
		if (dlg.exec() == QDialog::Accepted)
		{
			EventListenerSPtr listener;
			listener.reset(new EventListener(
				EventSPtr(dlg.createRequestedEvent())));

			document_->doCommand(new NewEventListenerCmd(
				selObj, listener));

			//select the newly created object
			vector<EventListener *> selObjs;
			selObjs.push_back(listener.get());
			document_->setSelectedListeners(selObjs);
		}
	}
}

void EventListPane::onDelete()
{	
	SceneObject * selObj = curObject(document_);
	AppObject* appObj = curAppObject(document_);
	vector<EventListenerSPtr> delObjs;

	if (appObj)
	{
		foreach(EventListener * obj, document_->selectedListeners())
			delObjs.push_back(appObj->findListener(obj));

		document_->doCommand(new DeleteEventListenersCmd(
			appObj, delObjs));
	}
	else
	{
		foreach(EventListener * obj, document_->selectedListeners())
			delObjs.push_back(selObj->findListener(obj));

		document_->doCommand(new DeleteEventListenersCmd(
			selObj, delObjs));
	}
}

void EventListPane::onEdit()
{
	QWidget * dlgParent = parentWidget();
	while (dlgParent->parent()) dlgParent = dlgParent->parentWidget();
	SceneObject * selObj = curObject(document_);
	AppObject* appObj = curAppObject(document_);
	if (appObj)
	{
		EventDlg dlg(document_, appObj, document_->selectedListener(), dlgParent);
		if (dlg.exec() == QDialog::Accepted)
		{
			EventSPtr newEvent(dlg.createRequestedEvent());

			document_->doCommand(new ChangeEventListenerCmd(
				document_->selectedListener(), newEvent));	

		}
	}
	else
	{
		EventDlg dlg(document_, selObj, document_->selectedListener(), dlgParent);
		if (dlg.exec() == QDialog::Accepted)
		{
			EventSPtr newEvent(dlg.createRequestedEvent());

			document_->doCommand(new ChangeEventListenerCmd(
				document_->selectedListener(), newEvent));	

		}
	}
}