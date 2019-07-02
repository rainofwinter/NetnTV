#include "stdafx.h"
#include "ChannelListPane.h"
#include "EditorDocument.h"
#include "EditorObject.h"
#include "Animation.h"
#include "AnimationChannel.h"
#include "Command.h"
#include "Scene.h"
#include "SceneObject.h"
#include "ListEditorWidget.h"
#include "PaneButton.h"
#include "NewChannelDlg.h"
#include "Utils.h"
#include "EditorGlobal.h"

using namespace std;
Q_DECLARE_METATYPE(AnimationChannel *)

ChannelListModel::ChannelListModel()
{
	document_ = 0;
}

ChannelListModel::~ChannelListModel()
{	
}

void ChannelListModel::setDocument(EditorDocument * document)
{
	if (document_) document_->disconnect(this);
	emit layoutAboutToBeChanged();
	document_ = document;
	emit layoutChanged();

	if (document_)
	{

		connect(document_, SIGNAL(animationSelectionChanged()), 
			this, SLOT(update()));

		connect(document_, SIGNAL(objectSelectionChanged()),
			this, SLOT(update()));

		connect(document_, SIGNAL(channelListChanged()),
			this, SLOT(update()));
	}
}

void ChannelListModel::update()
{
	emit layoutChanged();	
}


QStringList ChannelListModel::mimeTypes() const
{
    QStringList types;
    types << QLatin1String("application/channel-element-ptr");
    return types;
}



QMimeData * ChannelListModel::mimeData(const QModelIndexList & indexes) const
{
	QMimeData * mimeData = new QMimeData();
	QByteArray encodedData;

	QDataStream stream(&encodedData, QIODevice::WriteOnly);
	QModelIndexList sortedIndices = indexes;
	
	std::sort(sortedIndices.begin(), sortedIndices.end());

	foreach (QModelIndex index, sortedIndices) {
		if (index.isValid()) {
			QVariant variant = data(index, Qt::DisplayRole);

			if (variant.canConvert<AnimationChannel *>())
			{
				AnimationChannel * channel = variant.value<AnimationChannel *>();
				stream.writeRawData((char *)&channel, sizeof(AnimationChannel *));
			}						
		}
	}
     mimeData->setData("application/channel-element-ptr", encodedData);
     return mimeData;
}


QVariant ChannelListModel::data(const QModelIndex & index, int role) const
{	
	if (!index.isValid()) return QVariant();

	if (role == Qt::DisplayRole) 
	{
		if (index.column() == 0)
		{
			AnimationChannel * obj = (AnimationChannel *)index.internalPointer();
			return QVariant::fromValue(obj);
		}
		else
			return QVariant();
	}
	return QVariant();
}

Qt::ItemFlags ChannelListModel::flags(const QModelIndex &index) const
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

Qt::DropActions ChannelListModel::supportedDropActions() const
{
	return Qt::MoveAction;
}

QVariant ChannelListModel::headerData(
	int section, Qt::Orientation orientation, int role) const
{
	return tr("Channel");
}

QModelIndex ChannelListModel::index(int row, int column, const QModelIndex &parent) const
{
	if (!hasIndex(row, column, parent)) return QModelIndex();
	if (column != 0) return QModelIndex();

	SceneObject * selObj = document_->selectedObject();

	const vector<AnimationChannelSPtr> & objs = 
		*document_->selectedAnimation()->channels(selObj);		

	return createIndex(row, column, objs[row].get());	
}

QModelIndex ChannelListModel::parent(const QModelIndex & index) const
{
	return QModelIndex();	
}

int ChannelListModel::rowCount(const QModelIndex & parent) const
{	
	if (!document_ || parent.isValid()) return 0;
	Animation * anim = document_->selectedAnimation();
	if (!anim) return 0;
	if (!document_->selectedObject()) return 0;
	SceneObject * selObj = document_->selectedObject();

	const vector<AnimationChannelSPtr> * channels = anim->channels(selObj);
	if (!channels) return 0;
	int count = 0;
	foreach(AnimationChannelSPtr channel, *channels) 
		if (channel->object() == selObj) ++count;
	return count;	
}

int ChannelListModel::columnCount(const QModelIndex & parent) const
{
	return 1;
}

bool ChannelListModel::dropMimeData(const QMimeData * mimedata,
	Qt::DropAction action, int row, int column, const QModelIndex & parent)
{
	QStringList types = mimeTypes();
	QString format = types.at(0);
    QByteArray encoded = mimedata->data(format);
	QDataStream stream(&encoded, QIODevice::ReadOnly);
	
	vector<AnimationChannel *> objs;
	
	while (!stream.atEnd()) {
		AnimationChannel * obj;		
		stream.readRawData((char *)&obj, sizeof(AnimationChannel *));	
		objs.push_back(obj);
	}
	AnimationChannel * moveTarget = 0;

	QModelIndex idx = index(row, 0, parent);
	moveTarget = data(idx, Qt::DisplayRole).value<AnimationChannel *>();
	
	Animation * anim = document_->selectedAnimation();
	vector<AnimationChannelSPtr> objsToMove;
	foreach(AnimationChannel * obj, objs)
		objsToMove.push_back(anim->findChannel(obj));

	try
	{
	document_->doCommand(new MoveChannelsCmd(
		document_->selectedAnimation(), objsToMove, moveTarget));
	}
	catch(...) {}
	return true;
}

////////////////////////////////////////////////////////////////////////////////
ChannelListDelegate::ChannelListDelegate(QTreeView * parent) 
:
QStyledItemDelegate()
{
	parent_ = parent;
}

void ChannelListDelegate::paint(QPainter *painter, 
	const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QStyleOptionViewItemV4 opt = option;
	opt.state &= ~QStyle::State_HasFocus;
	opt.state |= QStyle::State_Active;
	if (index.column() == 0)
	{		
		AnimationChannel * obj = 
			index.data(Qt::DisplayRole).value<AnimationChannel *>();
		QString text = displayString(obj);
		initStyleOption(&opt, index);
		
		painter->save();

		const QStyle * style = 
			opt.SO_DockWidget ? opt.widget->style() : QApplication::style();
		const QWidget * widget = opt.widget;
		QSlider * rulerSlider;
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

QString ChannelListDelegate::displayString(AnimationChannel * obj) const
{
	return QString::fromLocal8Bit(obj->name());
}

QSize ChannelListDelegate::sizeHint(
	const QStyleOptionViewItem &option, const QModelIndex &index) const
{		
	QStyleOptionViewItemV4 opt = option;
	opt.state &= ~QStyle::State_HasFocus;

	AnimationChannel * obj = 
			index.data(Qt::DisplayRole).value<AnimationChannel *>();
	QFontMetrics metrics(opt.font);

	int windowWidth = parent_->width() - 2;
	int itemWidth = metrics.width(displayString(obj)) + 10;
	return QSize(max(itemWidth, windowWidth), 20);
}


///////////////////////////////////////////////////////////////////////////////
void ChannelListTreeView::setDocument(EditorDocument * document)
{
	if (document_) document_->disconnect(this);
	
	document_ = document;
	
	if (document_)
	{
		connect(document_, SIGNAL(channelSelectionChanged()), 
			this, SLOT(updateSelection()));

		updateSelection();
	}
}

ChannelListTreeView::ChannelListTreeView(ChannelListPane * parent)
{
	document_ = 0;
	parent_ = parent;
	ignoreSelChanged_ = false;
	setIndentation(0);
	//setFrameShape(QFrame::NoFrame);
	setRootIsDecorated(false);
	copyAction_ = new QAction(tr("Copy"), this);	
	connect(copyAction_, SIGNAL(triggered()), this, SLOT(onCopy()));
	pasteAction_ = new QAction(tr("Paste"), this);
	connect(pasteAction_, SIGNAL(triggered()), this, SLOT(onPaste()));
	contextMenu_ = new QMenu(this);
	QPalette Pal(QApplication::palette());
	//get rid of weird drop shadow effect on disabled items
	Pal.setColor(QPalette::Light, QColor(92, 92, 92, 255));	
	
	contextMenu_->setPalette(Pal);
	contextMenu_->addAction(copyAction_);
	contextMenu_->addAction(pasteAction_);
}

ChannelListTreeView::~ChannelListTreeView()
{
	delete copyAction_;
	delete pasteAction_;
	delete contextMenu_;
}

void ChannelListTreeView::updateSelection()
{
	ignoreSelChanged_ = true;
	QItemSelectionModel * selModel = selectionModel();
	selModel->clearSelection();
		
	QAbstractItemModel * m = model();	
	int numObjs = m->rowCount();
	for (int i = 0; i < numObjs; ++i)
	{
		QModelIndex index = m->index(i, 0);
		AnimationChannel * obj = (AnimationChannel *)index.internalPointer();
		if (document_->isChannelSelected(obj))
			selModel->select(index, QItemSelectionModel::Select);
	}

	ignoreSelChanged_ = false;
}

void ChannelListTreeView::selectionChanged(
		const QItemSelection &selected, const QItemSelection &deselected)
{
	QTreeView::selectionChanged(selected, deselected);
	if (ignoreSelChanged_) return;

	QItemSelectionModel * selModel = selectionModel();
	QModelIndexList selIndexList = selModel->selectedRows(0);
	
	vector<AnimationChannel *> selObjs;
	foreach(QModelIndex index, selIndexList)
	{
		AnimationChannel * selObj = index.data().value<AnimationChannel *>();
		selObjs.push_back(selObj);
		
	}	
	document_->setSelectedChannels(selObjs);
	parent_->changeButtonsState();
}

void ChannelListTreeView::mousePressEvent(QMouseEvent *event)
{
	QTreeView::mousePressEvent(event);
	QModelIndex clickIndex = indexAt(event->pos());

	if (clickIndex.isValid())
		clickedChannel_ = (AnimationChannel *)clickIndex.internalPointer();
	else clickedChannel_ = 0;

	if (event->button() == Qt::RightButton && document_->selectedAnimation())
	{
		copyAction_->setEnabled(clickedChannel_ != 0);		
		pasteAction_->setEnabled(EditorGlobal::instance().pasteAnimChannelsAvailable());		
		contextMenu_->popup(QCursor::pos());		
	}
}


void ChannelListTreeView::onCopy()
{
	EditorGlobal::instance().copyAnimChannels(document_->selectedChannels());
}

void ChannelListTreeView::onPaste()
{
	Document * sceneDoc = document_->document();
	try
	{
	document_->doCommand(new PasteAnimChannelsCmd(
		document_,
		EditorGlobal::instance().pasteAnimChannels()
		));
	} catch (Exception & e)
	{
		QMessageBox::information(0, tr("Error"), 
			e.what());
	}
}

///////////////////////////////////////////////////////////////////////////////
ChannelListPane::ChannelListPane(MainWindow * mainWindow)
{
	document_ = 0;
	delegate_ = 0;
	treeView_ = new ChannelListTreeView(this);
	model_ = 0;
	
	treeView_->setDragEnabled(true);
	treeView_->setAcceptDrops(true);
	treeView_->setDropIndicatorShown(true);
	treeView_->setDefaultDropAction(Qt::MoveAction);
	treeView_->setDragDropMode(QAbstractItemView::InternalMove);
	treeView_->setSelectionMode(QAbstractItemView::ExtendedSelection);
	treeView_->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	treeView_->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	treeView_->setUniformRowHeights(true);
	
	QBoxLayout * layout = new QBoxLayout(QBoxLayout::TopToBottom, this);

	layout->setMargin(0);
	layout->setSpacing(0);
	//layout->addSpacing(32);
	label_ = new QLabel(tr("Animation Attributes"));
	label_->setMinimumWidth(50);	
	label_->setMinimumHeight(20);
	layout->addWidget(label_);
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
}

ChannelListPane::~ChannelListPane()
{
	delete model_;
	delete delegate_;
}

void ChannelListPane::setDocument(EditorDocument * document)
{
	if (document_) document_->disconnect(this);

	document_ = document;

	delete model_;
	model_ = new ChannelListModel();
	treeView_->setModel(model_);
	model_->setDocument(document);

	treeView_->setDocument(document);

	connect(model_, SIGNAL(layoutChanged()), treeView_, SLOT(updateSelection()));
	
	delete delegate_;
	delegate_ = new ChannelListDelegate(treeView_);		
	treeView_->setItemDelegate(delegate_);

	/*
	//Make the treeview scroll horizontally as necessary
	treeView_->header()->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
	treeView_->header()->setResizeMode(0, QHeaderView::ResizeToContents);
	treeView_->header()->setStretchLastSection(false);
	*/	
	
	if (document_)
	{
		setEnabled(true);
		connect(model_, SIGNAL(layoutChanged()), this, SLOT(changeButtonsState()));
		connect(document_, SIGNAL(channelSelectionChanged()), this, SLOT(changeButtonsState()));

		changeButtonsState();

		connect(document_, SIGNAL(objectSelectionChanged()), 
			this, SLOT(updateObject()));

		connect(document_, SIGNAL(objectChanged()),
			this, SLOT(updateObject()));

		connect(document_, SIGNAL(animationSelectionChanged()),
			this, SLOT(animationSelectionChanged()));
			
		animationSelectionChanged();
	}
	else
	{
		setEnabled(false);
		label_->setText(QString());
	}
}


void ChannelListPane::setVScrollValue(int val)
{
	treeView_->verticalScrollBar()->setValue(val);
}

void ChannelListPane::keyPressEvent(QKeyEvent * event)
{
	QWidget::keyReleaseEvent(event);

	if(event->key() == Qt::Key_Delete)
		onDelete();
}

void ChannelListPane::keyReleaseEvent(QKeyEvent * event)
{
	QWidget::keyReleaseEvent(event);
}

QSize ChannelListPane::sizeHint() const
{
	return QSize(150,150);
}

void ChannelListPane::changeButtonsState()
{
	QItemSelectionModel * selModel = treeView_->selectionModel();
	bool isSelected = 0;
	if (selModel)
	{
		QModelIndexList selIndexList = selModel->selectedRows(0);
		if (!selIndexList.empty()) isSelected = true;
	}
	
	
	delButton_->setEnabled(isSelected);
	newButton_->setEnabled(document_ && document_->selectedObject());
	
}

void ChannelListPane::updateObject()
{
	if (!document_->selectedAnimation()) return;
	SceneObject * selObj = document_->selectedObject();
	setEnabled(selObj != 0);

	if (selObj)
	{
		label_->setText(
			QString("<b>") + tr("Attributes: ") + "</b> " + 
			stdStringToQString(selObj->id()));		
	}
	else
	{		
		label_->setText(tr("No object selected"));
	}
}

void ChannelListPane::animationSelectionChanged()
{
	if (!document_->selectedAnimation())
	{
		setEnabled(false);
		label_->setText(tr("No animation selected"));		
	}
	else
	{
		setEnabled(true);
		updateObject();		
	}
}

void ChannelListPane::onNew()
{
	QWidget * dlgParent = parentWidget();
	while (dlgParent->parent()) dlgParent = dlgParent->parentWidget();

	SceneObject * selObj = document_->selectedObject();
	Animation * anim = document_->selectedAnimation();
	NewChannelDlg dlg(dlgParent, document_->selectedAnimation(), selObj);
	if (dlg.exec() == QDialog::Accepted)
	{
		AnimationChannelSPtr newChannel(dlg.createRequestedChannel());
		document_->doCommand(new NewChannelCmd(
			anim, selObj, newChannel));

		//select the newly created object
		vector<AnimationChannel *> selObjs;
		selObjs.push_back(newChannel.get());
		document_->setSelectedChannels(selObjs);
	}
}

void ChannelListPane::onDelete()
{
	SceneObject * selObj = document_->selectedObject();
	Animation * anim = document_->selectedAnimation();

	vector<AnimationChannelSPtr> delChans;
	foreach(AnimationChannel * chan, document_->selectedChannels())
		delChans.push_back(anim->findChannel(chan));

	document_->doCommand(new DeleteChannelsCmd(anim, selObj, delChans));
}