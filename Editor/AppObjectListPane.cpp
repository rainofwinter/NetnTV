#include "stdafx.h"
#include "AppObjectListPane.h"
#include "EditorDocument.h"
#include "EditorGroup.h"
#include "Scene.h"
#include "Root.h"
#include "Group.h"
#include "Types.h"
#include "ListEditorWidget.h"
#include "Command.h"
#include "PaneButton.h"
#include "NewAppObjectDlg.h"
#include "AppImage.h"
#include "Utils.h"
#include "FileUtils.h"

#include "AppObject.h"
#include "MainWindow.h"

using namespace std;

Q_DECLARE_METATYPE(AppObject *)

AppObjectListModel::AppObjectListModel()
{
	document_ = 0;
}

AppObjectListModel::~AppObjectListModel()
{
}

void AppObjectListModel::setDocument(EditorDocument * document)
{
	if (document_) document_->disconnect(this);
	document_ = document;
	emit layoutChanged();

	if (document_)
	{
		connect(document_, SIGNAL(sceneSelectionChanged()),
			this, SLOT(update()));

		connect(document_, SIGNAL(appObjectListChanged()), 
			this, SLOT(update()));

		connect(document_, SIGNAL(appObjectChanged()), 
			this, SLOT(update()));
	}
}

QStringList AppObjectListModel::mimeTypes() const
{
    QStringList types;
    types << QLatin1String("application/x-app-object-ptr");
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

QMimeData * AppObjectListModel::mimeData(const QModelIndexList & indexes) const
{
	QMimeData * mimeData = new QMimeData();
	QByteArray encodedData;

	QDataStream stream(&encodedData, QIODevice::WriteOnly);
	QModelIndexList sortedIndices = indexes;
	
	std::sort(sortedIndices.begin(), sortedIndices.end(), IndexComparer());

	BOOST_REVERSE_FOREACH (QModelIndex index, sortedIndices) {
		if (index.isValid()) {
			QVariant variant = data(index, Qt::DisplayRole);

			if (variant.canConvert<AppObject *>())
			{
				AppObject * obj = variant.value<AppObject *>();
				stream.writeRawData((char *)&obj, sizeof(AppObject *));
			}						
		}
	}
     mimeData->setData("application/x-app-object-ptr", encodedData);
     return mimeData;
}


QVariant AppObjectListModel::data(const QModelIndex & index, int role) const
{	
	if (!index.isValid()) return QVariant();

	if (role == Qt::DisplayRole) 
	{
		if (index.column() == 0)
		{
			AppObject * obj = (AppObject *)index.internalPointer();
			return QVariant::fromValue(obj);
		}
		else
			return QVariant();
	}
	
	return QVariant();
}

Qt::ItemFlags AppObjectListModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
	{
		return Qt::ItemIsDropEnabled;
	}

	Qt::ItemFlags flags = 0;	
	AppObject * obj = static_cast<AppObject *>(index.internalPointer());

	//TODO

	flags = Qt::ItemIsEnabled | Qt::ItemIsEditable |
		Qt::ItemIsDragEnabled | Qt::ItemIsSelectable;
	/*	
	EditorObject *edObj = document_->editorObject(obj).get();
	if (!edObj || edObj->isThisAndAncestorsUnlocked())
	{		
		flags |= Qt::ItemIsSelectable;
		if (obj->group())
			flags |= Qt::ItemIsDropEnabled;
	}
	*/
	return flags;
}

Qt::DropActions AppObjectListModel::supportedDropActions() const
{
	return Qt::MoveAction | Qt::CopyAction;
}

QVariant AppObjectListModel::headerData(
	int section, Qt::Orientation orientation, int role) const
{
	return tr("App Object");
}

QModelIndex AppObjectListModel::index(int row, int column, const QModelIndex &parent) const
{
	if (!hasIndex(row, column, parent))
		return QModelIndex();

	if (column != 0) return QModelIndex();

	AppObject * parentObj;
	if (parent.isValid()) parentObj = (AppObject *)parent.internalPointer();
	else parentObj = (AppObject *)document_->selectedScene()->root();		

	Scene * scene = document_->selectedScene();
	int numChildren = scene->numAppObjects();
	return createIndex(row, column, scene->appObjects()[numChildren - row - 1].get());	
}

QModelIndex AppObjectListModel::parent(const QModelIndex & index) const
{
	return QModelIndex();
}

int AppObjectListModel::rowCount(const QModelIndex & parent) const
{	
	if (!document_) return 0;
	Scene * scene = document_->selectedScene();
	if (!parent.isValid())
	{
		return scene->numAppObjects();
	}
	else 
		return 0;
	
	
}

int AppObjectListModel::columnCount(const QModelIndex & parent) const
{
	return 1;
}

bool AppObjectListModel::dropMimeData(const QMimeData * mimedata,
	Qt::DropAction action, int row, int column, const QModelIndex & parent)
{	
	QStringList types = mimeTypes();

	Scene * scene = document_->selectedScene();

	int numChildren = scene->numAppObjects();
	int index = numChildren - row;

	if (mimedata->hasUrls())
	{
		QList<QUrl> urls = mimedata->urls();
		vector<AppObjectSPtr> newObjs;
		BOOST_FOREACH(QUrl url, urls)
		{
			QString localFile = url.toLocalFile();			
			QImage qimage(localFile);			

			if (qimage.width() > 0 && qimage.height() > 0)	
			{
				AppImage * img = new AppImage();				
				img->setFileName(convertToRelativePath(qStringToStdString(localFile)));	
				img->setId(getFileTitle(qStringToStdString(localFile)));
				img->setWidth(qimage.width());
				img->setHeight(qimage.height());
				img->init();
			
				AppObjectSPtr imgSPtr(img);
				newObjs.push_back(imgSPtr);
			}
		}
		try
		{
			if (!newObjs.empty()) document_->doCommand(new NewAppObjectsCmd(
				document_, index, newObjs));
		}
		catch (const Exception & e)
		{
			QMessageBox::information(0, tr("Error"), e.what());
		}
	}
	else
	{
		QByteArray encoded = mimedata->data(types[0]);
		QDataStream stream(&encoded, QIODevice::ReadOnly);
		
		vector<AppObject *> objs;
		while (!stream.atEnd()) {
			AppObject * obj;		
			stream.readRawData((char *)&obj, sizeof(AppObject *));	
			objs.push_back(obj);
		}			

		try
		{
		
		document_->doCommand(new MoveAppObjectsCmd(
			document_, index, objs));
		} catch (...) {}
	}

	return true;
}

void AppObjectListModel::update()
{
	emit layoutChanged();
}

////////////////////////////////////////////////////////////////////////////////
const int iconDim = 20;
AppObjectListDelegate::AppObjectListDelegate(AppObjectListPane * parent) 
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

void AppObjectListDelegate::paint(QPainter *painter, 
	const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QStyleOptionViewItemV4 opt = option;
	opt.state &= ~QStyle::State_HasFocus;
	opt.state |= QStyle::State_Active;
	

	if (index.column() == 0)
	{		
		AppObject * obj = index.data(Qt::DisplayRole).value<AppObject *>();
		QString text =  stdStringToQString(obj->id());		
		
		initStyleOption(&opt, index);
		
		painter->save();

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


		/*
		//TODO AppObject
		const QPixmap * drawImg;
		qreal ix, iy;
		ix = opt.rect.right() - iconDim/2 - visibleOnImg_.width()/2;
		iy = opt.rect.y() + (opt.rect.height() - visibleOnImg_.height()) / 2;

		EditorObject * edObj = document_->editorObject(obj).get();
		if (!edObj || edObj->isUiLocked())
			drawImg = &lockOnImg_;
		else
			drawImg = &lockOffImg_;

		painter->drawPixmap(ix, iy, *drawImg);

		ix -= iconDim;

		if (!edObj || edObj->isUiVisible())
			drawImg = &visibleOnImg_;
		else
			drawImg = &visibleOffImg_;

		painter->drawPixmap(ix, iy, *drawImg);
		*/
		

		painter->restore();
	}
	else
		QStyledItemDelegate::paint(painter, option, index);
}

QSize AppObjectListDelegate::sizeHint(
	const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	return QSize(0, 20);
}

QWidget * AppObjectListDelegate::createEditor(QWidget * parent, 
	const QStyleOptionViewItem & option, const QModelIndex & index) const
{

	if (index.column() == 0)
	{		
		AppObject * obj = (AppObject *)index.internalPointer();		

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

void AppObjectListDelegate::setModelData(QWidget *editor, 
		QAbstractItemModel *model, const QModelIndex &index) const
{

	if (index.column() == 0)
	{
		AppObject * obj = 
			index.model()->data(index, Qt::DisplayRole).value<AppObject *>();

		ListEditorWidget * widget = 
			qobject_cast<ListEditorWidget *>(editor);
		
		string newName = qStringToStdString(widget->GetLineEdit()->text());

		if (newName != obj->id())
		{
			document_->doCommand(makeChangePropertyCmd(ChangeAppObjectProperty,
				obj, newName, &AppObject::id, &AppObject::setId));
		}
		 
		
	}
	else
		QStyledItemDelegate::setModelData(editor, model, index);
}

void AppObjectListDelegate::setEditorData(QWidget *editor, 
	const QModelIndex &index) const
{
	if (index.column() == 0)
	{
		AppObject * obj = 
			index.model()->data(index, Qt::DisplayRole).value<AppObject *>();

		ListEditorWidget * widget = 
			qobject_cast<ListEditorWidget *>(editor);
		widget->GetLineEdit()->setText(stdStringToQString(obj->id()));

	}
	else
		QStyledItemDelegate::setEditorData(editor, index);
}

void AppObjectListDelegate::setDocument(EditorDocument * document)
{
	document_ = document;
}


void AppObjectListDelegate::editorClosed(QWidget * editor, EndEditHint hint)
{
	curEditor_ = 0;
}

void AppObjectListDelegate::commitAndCloseEditor()
{
	ListEditorWidget * editor = qobject_cast<ListEditorWidget *>(sender());
	emit commitData(editor);
	emit closeEditor(editor);
}

void AppObjectListDelegate::cancelEdit()
{
	ListEditorWidget * editor = qobject_cast<ListEditorWidget *>(sender());
	emit closeEditor(editor);
}

///////////////////////////////////////////////////////////////////////////////

AppObjectListTreeView::AppObjectListTreeView(AppObjectListPane * parent)
{
	parent_ = parent;
	ignoreSelChanged_ = false;
	setIndentation(15);
	//setFrameShape(QFrame::NoFrame);
	document_ = 0;

}

void AppObjectListTreeView::setDocument(EditorDocument * document)
{
	mousePressOnUiIcon_ = false;

	if (document_) 
	{
		disconnect(
			this, SIGNAL(uiPropertiesChanged()), 
			document_, SIGNAL(appObjectChanged()));
		document_->disconnect(this);
	}

	document_ = document;
	if (document_)
	{
		connect(document_, SIGNAL(appObjectSelectionChanged()), 
			this, SLOT(updateSelection()));

		updateSelection();

		connect(document_, SIGNAL(appObjectListChanged()),
			this, SLOT(updateAppObjectList()));

		connect(document_, SIGNAL(sceneSelectionChanged()),
			this, SLOT(updateAppObjectList()));

		connect(this, SIGNAL(uiPropertiesChanged()), document_,
			SIGNAL(appObjectChanged()));

		updateAppObjectList();
	}
}
void AppObjectListTreeView::updateSelection()
{
	ignoreSelChanged_ = true;
	QItemSelectionModel * selModel = selectionModel();
	selModel->clearSelection();
	updateSelection(QModelIndex());
	ignoreSelChanged_ = false;
}

void AppObjectListTreeView::updateSelection(QModelIndex parentIndex)
{
	QItemSelectionModel * selModel = selectionModel();	
		
	QAbstractItemModel * m = model();
	int numObjs = m->rowCount(parentIndex);
	for (int i = 0; i < numObjs; ++i)
	{
		QModelIndex childIndex = m->index(i, 0, parentIndex);

		AppObject * obj = (AppObject *)childIndex.internalPointer();
		if (document_->isAppObjectSelected(obj))
			selModel->select(childIndex, QItemSelectionModel::Select);
		
		updateSelection(childIndex);		
	}
}


void AppObjectListTreeView::mousePressEvent(QMouseEvent *event)
{
	QTreeView::mousePressEvent(event);
	MainWindow * mainWindow = parent_->parent();
	if (mainWindow->objectMode() != MainWindow::ModeAppObject)
		mainWindow->setObjectMode(MainWindow::ModeAppObject);
}
void AppObjectListTreeView::updateAppObjectList()
{
	//TODO
	//In the Scene Object List, this function handles the appropriate expanding/
	//unexpanding of groups
}


void AppObjectListTreeView::selectionChanged(
		const QItemSelection &selected, const QItemSelection &deselected)
{
	QTreeView::selectionChanged(selected, deselected);
	if (ignoreSelChanged_) return;

	QItemSelectionModel * selModel = selectionModel();
	QModelIndexList selIndexList = selModel->selectedRows(0);
	
	vector<AppObject *> selObjs;
	foreach(QModelIndex index, selIndexList)
	{
		AppObject * selObj = index.data().value<AppObject *>();
		selObjs.push_back(selObj);
		
	}
	document_->setSelectedAppObjects(selObjs);

	MainWindow * mainWindow = parent_->parent();
	if (mainWindow->objectMode() != MainWindow::ModeAppObject)
		mainWindow->setObjectMode(MainWindow::ModeAppObject);
}


////////////////////////////////////////////////////////////////////////////////
AppObjectListPane::AppObjectListPane(MainWindow * mainWindow)
{	
	setFocusPolicy(Qt::StrongFocus);


	mainWindow_ = mainWindow;
	document_ = 0;

	delegate_ = 0;
	

	treeView_ = new AppObjectListTreeView(this);
	model_ = 0;

	treeView_->setDragEnabled(true);
	treeView_->setAcceptDrops(true);
	treeView_->setDropIndicatorShown(true);
	treeView_->setDefaultDropAction(Qt::MoveAction);
	treeView_->setDragDropMode(QAbstractItemView::DragDrop);	
	treeView_->setSelectionMode(QAbstractItemView::ExtendedSelection);

	QBoxLayout * layout = new QVBoxLayout(this);

	layout->setMargin(0);
	layout->setSpacing(0);
//	label_ = new HeaderLabel(mainWindow);
	label_ = new QLabel;
	label_->setMinimumWidth(10);
	label_->setMinimumHeight(25);
	label_->setMaximumHeight(25);
	layout->addWidget(label_);
	
	//layout->addWidget(iconBar_ = new AppObjectListPaneTopBar);

	layout->addWidget(treeView_);
	
	QBoxLayout * btnLayout = new QBoxLayout(QBoxLayout::LeftToRight);
	btnLayout->setMargin(0);
	btnLayout->setSpacing(0);
	btnLayout->setAlignment(Qt::AlignRight);
	btnLayout->addWidget(newButton_ = new PaneButton(QPixmap(":/data/Resources/Scene_menu/scene_new.png"), QSize(30, 30), "New"));
	btnLayout->addWidget(delButton_ = new PaneButton(QPixmap(":/data/Resources/Scene_menu/scene_del.png"), QSize(30, 30), "Delete"));
	layout->addLayout(btnLayout);
	connect(newButton_, SIGNAL(clicked()), this, SLOT(onNew()));
	connect(delButton_, SIGNAL(clicked()), this, SLOT(onDelete()));

}

AppObjectListPane::~AppObjectListPane()
{
	delete model_;
	delete delegate_;
}



QSize AppObjectListPane::sizeHint() const
{
	return QSize(200,150);
}

void AppObjectListPane::changeButtonsState()
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

void AppObjectListPane::setDocument(EditorDocument * document)
{
	if (document_) document_->disconnect(this);

	document_ = document;

	delete model_;
	model_ = new AppObjectListModel();
	treeView_->setModel(model_);

	model_->setDocument(document);
	treeView_->setDocument(document);
	connect(model_, SIGNAL(layoutChanged()), treeView_, SLOT(updateSelection()));
	delete delegate_;
	delegate_ = new AppObjectListDelegate(this);
	
	
	treeView_->setItemDelegate(delegate_);
	delegate_->setDocument(document);

	if (document_)
	{
		setEnabled(true);
		connect(document_, SIGNAL(appObjectSelectionChanged()), this, SLOT(changeButtonsState()));
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

void AppObjectListPane::onNew()
{
	QWidget * dlgParent = parentWidget();
	while (dlgParent->parent()) dlgParent = dlgParent->parentWidget();
	try{
	if (document_->selectedScene()->userCamera() != NULL)
	{
		throw Exception("Scenes with custom cameras cannot contain app objects");
	}

	if (document_->selectedScene()->zBuffer())
	{
		throw Exception("Scenes z-buffering enabled cannot contain app objects");
	}

	NewAppObjectDlg dlg(document_, dlgParent);
	if (dlg.exec() == QDialog::Accepted)
	{
		AppObject * newObjPtr = dlg.createAppObject();
		if (newObjPtr)
		{
			vector<AppObjectSPtr> newObjs;
			newObjs.push_back(AppObjectSPtr(newObjPtr));
			document_->doCommand(new NewAppObjectsCmd(
				document_, -1, newObjs));

			if (mainWindow_->objectMode() != MainWindow::ModeAppObject)
				mainWindow_->setObjectMode(MainWindow::ModeAppObject);
		}
	}
	}catch(Exception e){
		QMessageBox::information(0, tr("Error"), e.what());
	}
}

void AppObjectListPane::onDelete()
{
	document_->doCommand(new DeleteAppObjectsCmd(
		document_, document_->selectedScene(),
		document_->selectedAppObjects()));
}

void AppObjectListPane::setLabelText()
{
	QString labelText = QString("<b>") + tr("App Objects: ") + "</b> ";
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

void AppObjectListPane::updateScene()
{
	if (!isEnabled()) return;
	setLabelText();
}


void AppObjectListPane::focusInEvent(QFocusEvent * event)
{
	if (mainWindow_->objectMode() != MainWindow::ModeAppObject)
		mainWindow_->setObjectMode(MainWindow::ModeAppObject);
}

void AppObjectListPane::keyPressEvent(QKeyEvent * event)
{
	if(event->key() == Qt::Key_Delete)
	{
		onDelete();
	}
}
