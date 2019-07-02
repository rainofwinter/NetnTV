#include "stdafx.h"
#include "ActionListPane.h"
#include "EditorDocument.h"
#include "EditorObject.h"
#include "EventListener.h"
#include "Event.h"
#include "EditorAction.h"
#include "Action.h"
#include "Command.h"
#include "Scene.h"
#include "SceneObject.h"
#include "ListEditorWidget.h"
#include "PaneButton.h"
#include "ActionDlg.h"
#include "EditorGlobal.h"
#include "Exception.h"

using namespace std;
Q_DECLARE_METATYPE(Action *)

ActionListModel::ActionListModel()
{
	document_ = 0;
}

ActionListModel::~ActionListModel()
{	
}

void ActionListModel::setDocument(EditorDocument * document)
{
	if (document_) document_->disconnect(this);

	emit layoutAboutToBeChanged();
	document_ = document;
	emit layoutChanged();

	if (document_)
	{

		connect(document_, SIGNAL(listenerSelectionChanged()),
			this, SLOT(update()));

		connect(document_, SIGNAL(actionListChanged()),
			this, SLOT(update()));

		//when ChangeActionCmd is issued, the pointer value of the changed action will
		//change, so the internalPointer of the tree item should be updated
		connect(document_, SIGNAL(actionChanged()),
			this, SLOT(update()));

	}
}

void ActionListModel::update()
{
	emit layoutChanged();	
}


QStringList ActionListModel::mimeTypes() const
{
    QStringList types;
    types << QLatin1String("application/action-ptr");
    return types;
}



QMimeData * ActionListModel::mimeData(const QModelIndexList & indexes) const
{
	QMimeData * mimeData = new QMimeData();
	QByteArray encodedData;

	QDataStream stream(&encodedData, QIODevice::WriteOnly);
	QModelIndexList sortedIndices = indexes;
	
	std::sort(sortedIndices.begin(), sortedIndices.end());

	foreach (QModelIndex index, sortedIndices) {
		if (index.isValid()) {
			QVariant variant = data(index, Qt::DisplayRole);

			if (variant.canConvert<Action *>())
			{
				Action * channel = variant.value<Action *>();
				stream.writeRawData((char *)&channel, sizeof(Action *));
			}						
		}
	}
     mimeData->setData("application/action-ptr", encodedData);
     return mimeData;
}


QVariant ActionListModel::data(const QModelIndex & index, int role) const
{	
	if (!index.isValid()) return QVariant();

	if (role == Qt::DisplayRole) 
	{
		if (index.column() == 0)
		{
			Action * obj = (Action *)index.internalPointer();
			return QVariant::fromValue(obj);
		}
		else
			return QVariant();
	}
	return QVariant();
}

Qt::ItemFlags ActionListModel::flags(const QModelIndex &index) const
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

Qt::DropActions ActionListModel::supportedDropActions() const
{
	return Qt::MoveAction;
}

QVariant ActionListModel::headerData(
	int section, Qt::Orientation orientation, int role) const
{
	return tr("Channel");
}

QModelIndex ActionListModel::index(int row, int column, const QModelIndex &parent) const
{
	if (!hasIndex(row, column, parent)) return QModelIndex();
	if (column != 0) return QModelIndex();
	EventListener * selObj = document_->selectedListener();
	const vector<ActionSPtr> & objs = selObj->actions();		
	return createIndex(row, column, objs[row].get());	

}

QModelIndex ActionListModel::parent(const QModelIndex & index) const
{
	return QModelIndex();	
}

int ActionListModel::rowCount(const QModelIndex & parent) const
{	
	if (!document_ || parent.isValid()) return 0;
	EventListener * selObj = document_->selectedListener();
	if (!selObj) return 0;

	
	return selObj->actions().size();	
}

int ActionListModel::columnCount(const QModelIndex & parent) const
{
	return 1;
}

bool ActionListModel::dropMimeData(const QMimeData * mimedata,
	Qt::DropAction action, int row, int column, const QModelIndex & parent)
{
	
	QStringList types = mimeTypes();
	QString format = types.at(0);
    QByteArray encoded = mimedata->data(format);
	QDataStream stream(&encoded, QIODevice::ReadOnly);
	
	vector<Action *> objs;
	
	while (!stream.atEnd()) {
		Action * obj;		
		stream.readRawData((char *)&obj, sizeof(Action *));	
		objs.push_back(obj);
	}
	Action * moveTarget = 0;

	QModelIndex idx = index(row, 0, parent);
	moveTarget = data(idx, Qt::DisplayRole).value<Action *>();
	
	EventListener * listener = document_->selectedListener();
	vector<ActionSPtr> objsToMove;
	foreach(Action * obj, objs)
		objsToMove.push_back(listener->findAction(obj));

	try
	{
	document_->doCommand(new MoveActionsCmd(
		listener, objsToMove, moveTarget));
	}
	catch(...) {}
	return true;
}

////////////////////////////////////////////////////////////////////////////////
ActionListDelegate::ActionListDelegate(QTreeView * parent) 
:
QStyledItemDelegate()
{
	parent_ = parent;
}

void ActionListDelegate::paint(QPainter *painter, 
	const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QStyleOptionViewItemV4 opt = option;
	opt.state &= ~QStyle::State_HasFocus;
	opt.state |= QStyle::State_Active;
	if (index.column() == 0)
	{		
		Action * obj = 
			index.data(Qt::DisplayRole).value<Action *>();

		EditorAction * edAction = EditorGlobal::instance().editorAction(obj);
		QString text;
		if (edAction)		
			text = edAction->toString(obj);		
		else 
			text = obj->typeStr();
		
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

QSize ActionListDelegate::sizeHint(
	const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	return QSize(0, 20);
}


///////////////////////////////////////////////////////////////////////////////

void ActionListTreeView::setDocument(EditorDocument * document)
{
	if (document_) document_->disconnect(this);
	document_ = document;
	if (document_)
	{
		connect(document_, SIGNAL(actionSelectionChanged()), 
			this, SLOT(updateSelection()));

		connect(document_, SIGNAL(listenerSelectionChanged()), 
			this, SLOT(onListenerSelectionChanged()));

		updateSelection();
	}
}

ActionListTreeView::ActionListTreeView(ActionListPane * parent)
{	
	document_ = 0;
	clickedAction_ = 0;
	parent_ = parent;
	ignoreSelChanged_ = false;
	setIndentation(0);
	//setFrameShape(QFrame::NoFrame);
	setRootIsDecorated(false);
	actionsCopyAction_ = new QAction(tr("Copy"), this);	
	connect(actionsCopyAction_, SIGNAL(triggered()), this, SLOT(onActionsCopy()));
	actionsPasteAction_ = new QAction(tr("Paste"), this);
	connect(actionsPasteAction_, SIGNAL(triggered()), this, SLOT(onActionsPaste()));

	contextMenu_ = new QMenu(this);		
	QPalette Pal(QApplication::palette());
	//get rid of weird drop shadow effect on disabled items
	Pal.setColor(QPalette::Light, QColor(92, 92, 92, 255));	
	contextMenu_->setPalette(Pal);
	
	contextMenu_->addAction(actionsCopyAction_);
	contextMenu_->addAction(actionsPasteAction_);
}

ActionListTreeView::~ActionListTreeView()
{
	delete actionsCopyAction_;
	delete actionsPasteAction_;
	delete contextMenu_;
}

void ActionListTreeView::updateSelection()
{
	
	ignoreSelChanged_ = true;
	QItemSelectionModel * selModel = selectionModel();
	selModel->clearSelection();
		
	QAbstractItemModel * m = model();
	int numObjs = m->rowCount();
	for (int i = 0; i < numObjs; ++i)
	{
		QModelIndex index = m->index(i, 0);
		Action * obj = (Action *)index.internalPointer();
		if (document_->isActionSelected(obj))
			selModel->select(index, QItemSelectionModel::Select);
	}

	ignoreSelChanged_ = false;
	parent_->changeButtonsState();
	
}

void ActionListTreeView::onListenerSelectionChanged()
{
	EventListener * obj = document_->selectedListener();
	if (!obj) return;

	QItemSelectionModel * selModel = selectionModel();
	if (selModel->selectedIndexes().isEmpty())
	{
		QAbstractItemModel * m = model();
		if (m->rowCount())
		{
			QModelIndex index = m->index(0, 0);
			Action * action = (Action *)index.internalPointer();
			document_->setSelectedActions(
				vector<Action *>(&action, &action + 1));			
		}
		
	}

}

void ActionListTreeView::selectionChanged(
		const QItemSelection &selected, const QItemSelection &deselected)
{
	QTreeView::selectionChanged(selected, deselected);
	if (ignoreSelChanged_) return;

	QItemSelectionModel * selModel = selectionModel();
	QModelIndexList selIndexList = selModel->selectedRows(0);
	
	vector<Action *> selObjs;
	foreach(QModelIndex index, selIndexList)
	{
		Action * selObj = index.data().value<Action *>();
		selObjs.push_back(selObj);		
	}	
	document_->setSelectedActions(selObjs);
	parent_->changeButtonsState();	
}

void ActionListTreeView::mousePressEvent(QMouseEvent *event)
{
	QTreeView::mousePressEvent(event);
	QModelIndex clickIndex = indexAt(event->pos());

	if (clickIndex.isValid())
		clickedAction_ = (Action *)clickIndex.internalPointer();
	else clickedAction_ = 0;

	if (event->button() == Qt::RightButton && document_->selectedListener())
	{
		actionsCopyAction_->setEnabled(clickedAction_ != 0);		
		actionsPasteAction_->setEnabled(EditorGlobal::instance().pasteActionsAvailable());	

		contextMenu_->popup(QCursor::pos());	
	}
}

void ActionListTreeView::mouseDoubleClickEvent(QMouseEvent *event)
{
	QTreeView::mouseDoubleClickEvent(event);
	if(document_->selectedListeners().size() != 0)
	{
		if(document_->selectedActions().size()!=0)
			parent_->onEdit();
		else
			parent_->onNew();
	}
}


void ActionListTreeView::onActionsCopy()
{
	EditorGlobal::instance().copyActions(document_->selectedActions());
}

void ActionListTreeView::onActionsPaste()
{
	Document * sceneDoc = document_->document();
	try
	{
	document_->doCommand(new PasteActionsCmd(
		document_, 
		document_->selectedListener(),
		EditorGlobal::instance().pasteActions(sceneDoc, document_->selectedScene())
		));
	} catch (Exception & e)
	{
		QMessageBox::information(0, tr("Error"), 
			e.what());
	}
}

///////////////////////////////////////////////////////////////////////////////
ActionListPane::ActionListPane(MainWindow * mainWindow)
{
	document_ = 0;
	delegate_ = 0;
	treeView_ = new ActionListTreeView(this);
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

	
	connect(newButton_, SIGNAL(clicked()), this, SLOT(onNew()));
	connect(delButton_, SIGNAL(clicked()), this, SLOT(onDelete()));
	connect(editButton_, SIGNAL(clicked()), this, SLOT(onEdit()));
}

ActionListPane::~ActionListPane()
{
	delete model_;
	delete delegate_;
}

void ActionListPane::setDocument(EditorDocument * document)
{
	if (document_) document_->disconnect(this);
	document_ = document;

	delete model_;
	model_ = new ActionListModel();
	
	model_->setDocument(document);
	treeView_->setModel(model_);
	treeView_->setDocument(document);
	
	delete delegate_;
	delegate_ = new ActionListDelegate(treeView_);	
	treeView_->setItemDelegate(delegate_);

	if (document_)
	{
		setEnabled(true);
		connect(document_, SIGNAL(listenerSelectionChanged()), 
			this, SLOT(onListenerSelectionChanged()));

		/*
		For example, the user might change the event type by clicking the Edit
		button in the EventsListPane. Then the caption of the ActionListPane 
		must change accordingly.
		*/
		connect(document_, SIGNAL(listenerChanged()), this, SLOT(onListenerSelectionChanged()));

		onListenerSelectionChanged();

		connect(model_, SIGNAL(layoutChanged()), 
			this, SLOT(changeButtonsState()));

		changeButtonsState();
	}
	else
	{
		setEnabled(false);
		label_->setText(QString());
	}

	
}


QSize ActionListPane::sizeHint() const
{
	return QSize(150,150);
}

void ActionListPane::changeButtonsState()
{
	QItemSelectionModel * selModel = treeView_->selectionModel();
	bool isSelected = 0;
	if (selModel)
	{
		QModelIndexList selIndexList = selModel->selectedRows(0);
		if (!selIndexList.empty()) isSelected = true;
	}
	
	newButton_->setEnabled(document_->selectedListener());
	delButton_->setEnabled(isSelected);	
	editButton_->setEnabled(isSelected);
	
}

void ActionListPane::onListenerSelectionChanged()
{
	EventListener * selObj = document_->selectedListener();
	QString text = QString("<b>") + tr("Actions: ") + "</b> ";
	if (selObj)
	{
		label_->setText(text + QString::fromLocal8Bit(selObj->event()->typeStr()));		
	}
	else
	{		
		label_->setText(tr("No event selected"));
	}	

	changeButtonsState();
}

void ActionListPane::onNew()
{

	QWidget * dlgParent = parentWidget();
	while (dlgParent->parent()) dlgParent = dlgParent->parentWidget();

	ActionDlg dlg(document_, 0, dlgParent);
	if (dlg.exec() == QDialog::Accepted)
	{
		ActionSPtr newAction(dlg.createRequestedAction());
		document_->doCommand(new NewActionCmd(
			document_->selectedListener(), newAction));
			
	}
	
}

void ActionListPane::onEdit()
{
	QWidget * dlgParent = parentWidget();
	while (dlgParent->parent()) dlgParent = dlgParent->parentWidget();
	
	ActionDlg dlg(document_, document_->selectedAction(), dlgParent);
	if (dlg.exec() == QDialog::Accepted)
	{
		ActionSPtr newAction(dlg.createRequestedAction());

		document_->doCommand(new ChangeActionCmd(
			document_->selectedListener(), 
			document_->selectedAction(), newAction));	
			
	}
}

void ActionListPane::onDelete()
{
	
	SceneObject * selObj = document_->selectedObject();
	EventListener * listener = document_->selectedListener();

	vector<ActionSPtr> delObjs;
	foreach(Action * chan, document_->selectedActions())
		delObjs.push_back(listener->findAction(chan));

	document_->doCommand(new DeleteActionsCmd(document_, listener, delObjs));
}