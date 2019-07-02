#include "stdafx.h"
#include "CatalogDlg.h"
#include "Document.h"
#include "Scene.h"
#include "SceneObject.h"
#include "Global.h"
#include "EditorGlobal.h"
#include "EditorObject.h"
#include "GenericList.h"
#include "Utils.h"
#include "GenThumbsDlg.h"
#include "EditorDocument.h"
#include "CatalogDocumentTemplate.h"


using namespace std;

CatalogDlg::CatalogDlg(QWidget * parent, EditorDocument * document,
					   const std::vector<Scene *> & scenes) 
: QDialog(parent)
{
	sceneSelected_ = pageSelected_ = false;
	ignoreSelChanged_ = false;
	ignoreLayoutChanged_ = false;
	document_ = document;
	ui.setupUi(this);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint); 
	
	setupList(&pagesList_, ui.pagesWidget);
	setupList(&scenesList_, ui.scenesWidget);
	
	const vector<SceneSPtr> & docScenes = document->scenes();
	BOOST_FOREACH(SceneSPtr scene, docScenes)
	{
		scenesList_->addTopLevelItem(
			stdStringToQString(scene->name()),
			scene.get());
	}

	pagesList_->setReorderable(true);
	
	connect(pagesList_, SIGNAL(selectionChanged()), this, SLOT(onPageSelectionChanged()));
	connect(scenesList_, SIGNAL(selectionChanged()), this, SLOT(onSceneSelectionChanged()));
	connect(ui.addPageButton, SIGNAL(clicked()), this, SLOT(onAddPage()));
	connect(ui.deletePageButton, SIGNAL(clicked()), this, SLOT(onDeletePage()));
	connect(pagesList_, SIGNAL(layoutChanged()), this, SLOT(onPagesLayoutChanged()));
	connect(ui.okButton, SIGNAL(clicked()), this, SLOT(onOk()));

	//ui.thumbWidthEdit->setValidator(new QIntValidator(1, 512, NULL));
	//ui.thumbHeightEdit->setValidator(new QIntValidator(1, 512, NULL));

	ui.splitter->setStretchFactor(0, 1);
	ui.splitter->setStretchFactor(1, 1);

	setScenes(scenes);	
}

void CatalogDlg::setupList(GenericList ** list, QWidget * container)
{
	*list = new GenericList();
	QHBoxLayout * layout = new QHBoxLayout();
	layout->setMargin(0);
	layout->setSpacing(0);
	layout->addWidget(*list);
	container->setLayout(layout);
}

CatalogDlg::~CatalogDlg()
{
}

void CatalogDlg::setScenes(const vector<Scene *> & scenes)
{		
	scenes_ = scenes;

	ignoreSelChanged_ = true;
	ignoreLayoutChanged_ = true;

	pagesList_->clearItems();
	BOOST_FOREACH(Scene * scene, scenes_)
	{	
		pagesList_->addTopLevelItem(
			stdStringToQString(scene->name()), scene);
	}
	ignoreLayoutChanged_ = false;
	ignoreSelChanged_ = false;
}


void CatalogDlg::onAddPage()
{
	const vector<GenericListItem *> & selItems = scenesList_->selectedItems();

	BOOST_FOREACH(GenericListItem * selItem, selItems)
	{
		Scene * page = (Scene *)selItem->data;
		scenes_.push_back(page);
		pagesList_->addTopLevelItem(selItem->string, page);
	}
}


std::vector<Scene *> CatalogDlg::scenes() const
{
	return scenes_;
}


void CatalogDlg::onDeletePage()
{
	pagesList_->deleteSelectedItems();	
	refreshPages();
}



void CatalogDlg::refreshPages()
{
	scenes_.clear();
	BOOST_FOREACH(GenericListItem * item, pagesList_->items())
	{
		scenes_.push_back((Scene *)item->data);		
	}
}

void CatalogDlg::onPagesLayoutChanged()
{
	if (ignoreLayoutChanged_) return;	
	refreshPages();	
}

void CatalogDlg::onPageSelectionChanged()
{
	const vector<GenericListItem *> & selItems = pagesList_->selectedItems();
	pageSelected_ = !selItems.empty();
	ui.deletePageButton->setEnabled(pageSelected_);
}

void CatalogDlg::onSceneSelectionChanged()
{
	const vector<GenericListItem *> & selItems = scenesList_->selectedItems();
	sceneSelected_ = !selItems.empty();
	ui.addPageButton->setEnabled(sceneSelected_);
}

void CatalogDlg::setPagesPaneEnabled(bool enabled)
{
	pagesList_->setEnabled(enabled);
	ui.deletePageButton->setEnabled(enabled?pageSelected_:0);
}

void CatalogDlg::setScenesPaneEnabled(bool enabled)
{
	scenesList_->setEnabled(enabled);
	ui.addPageButton->setEnabled(enabled?sceneSelected_:0);
}


void CatalogDlg::onOk()
{
	genThumbs();
	bool status = 1;


	QString uiFolder = QString("./Res/Temp/");

	typedef pair<QString, QString> FilePair;
	vector<FilePair> files;

	status &= QDir::current().mkpath(uiFolder);	
	

	files.push_back(make_pair(
		":/Catalog/Resources/catalog/bt_thumbnail.png",
		uiFolder + "layoutButton.png"));
	files.push_back(make_pair(
		":/Catalog/Resources/catalog/viewer_thumbnail_box.png",
		uiFolder + "thumbnailNormalBarFile_.png"));

	files.push_back(make_pair(
		":/Catalog/Resources/catalog/viewer_width_thumbnail_box.png",
		uiFolder + "thumbnailWidthBarFile_.png"));

	files.push_back(make_pair(
		":/Catalog/Resources/catalog/viewer_thumbnail_bottom.png",
		uiFolder + "thumbnailNormalClosedFile_.png"));

	files.push_back(make_pair(
		":/Catalog/Resources/catalog/viewer_thumbnail_top.png",
		uiFolder + "thumbnailNormalOpenedFile_.png"));	

	files.push_back(make_pair(
		":/Catalog/Resources/catalog/bookmark.png",
		uiFolder + "bookmark.png"));

	files.push_back(make_pair(
		":/Catalog/Resources/catalog/viewer_photobox.png",
		uiFolder + "thumbnailShadow.png"));

	files.push_back(make_pair(
		":/Catalog/Resources/catalog/viewer_width_thumbnail_bottom.png",
		uiFolder + "thumbnailWidthClosedFile_.png"));

	files.push_back(make_pair(
		":/Catalog/Resources/catalog/viewer_width_thumbnail_top.png",
		uiFolder + "thumbnailWidthOpenedFile_.png"));	

		
	BOOST_FOREACH(const FilePair & filePair, files)
	{		
		if (QFile::exists(filePair.second)) 
		{
			QFile::setPermissions(filePair.second, (QFile::Permissions)0xFFF);
			QFile::remove(filePair.second);		
		}
		status &= QFile::copy(filePair.first, filePair.second);
	}

	uiImgFiles_.clear();
	BOOST_FOREACH(const FilePair & filePair, files)
	{
		uiImgFiles_.push_back(qStringToStdString(filePair.second));
	}
	accept();
}


void CatalogDlg::genThumbs()
{
	QString thumbFolder = QString("./Res/Temp");
	QDir dir;
	dir.setPath(thumbFolder);
	thumbFolder = dir.absolutePath();
	dir.mkpath(thumbFolder);
	int thumbWidth, thumbHeight;

	thumbWidth = 159;
	thumbHeight = 212;

	if (thumbFolder.isEmpty() || !QDir(thumbFolder).exists())
	{
		QMessageBox::information(0, tr("Error"), tr("Could not locate thumbnail folder"));
		return;
	}

	if (thumbWidth <= 0 || thumbHeight <= 0)
	{
		QMessageBox::information(0, tr("Error"), tr("Invalid thumbnail dimensions"));
		return;
	}

	GenThumbsDlg dlg(
		document_,
		thumbFolder,
		scenes_, 
		thumbWidth, thumbHeight, 
		this);

	if (dlg.numScenesToRender() == 0) return;
	else if (dlg.exec() == QDialog::Accepted)
	{	
		thumbFileMap_ = dlg.thumbFileMap();
		//CatalogDocumentTemplate* cdt = (CatalogDocumentTemplate*)document_->document()->documentTemplate();
		//cdt->setThumbFileMap(thumbFileMap());
		//for (int i = 0 ; i < scenes_.size() ; i++)
		//{
		//	cdt->setThumbnailFile(i, 
		//		qStringToStdString(thumbFileMap_[scenes_[i]]));
		//}
	}

	//dlg.exec();
}

const std::map<int, std::string> CatalogDlg::thumbFileMap() const
{
	std::map<int, std::string> thumbFileMap;
	for (int i = 0 ; i < scenes_.size() ; i++)
		thumbFileMap[i] = qStringToStdString(thumbFileMap_.find(scenes_[i])->second);

	return thumbFileMap;
}