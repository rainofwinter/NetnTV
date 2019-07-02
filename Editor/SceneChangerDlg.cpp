#include "stdafx.h"
#include "SceneChangerDlg.h"
#include "SceneChanger.h"
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

using namespace std;

SceneChangerDlg::SceneChangerDlg(QWidget * parent, EditorDocument * document,
	Scene * parentScene, const std::vector<Scene *> & sceneChangerScenes,
	bool allowDragging) 
: QDialog(parent)
{
	parentScene_ = parentScene;

	sceneSelected_ = pageSelected_ = false;
	ignoreSelChanged_ = false;
	ignoreLayoutChanged_ = false;
	document_ = document;
	ui.setupUi(this);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint); 
	
	setupList(&pagesList_, ui.pagesWidget);
	setupList(&scenesList_, ui.scenesWidget);
	
	const vector<SceneSPtr> & scenes = document->scenes();
	BOOST_FOREACH(SceneSPtr scene, scenes)
	{
		scenesList_->addTopLevelItem(
			stdStringToQString(scene->name()),
			scene.get());
	}

	pagesList_->setReorderable(true);

	ui.allowDraggingCheckBox->setChecked(allowDragging);
	
	connect(pagesList_, SIGNAL(selectionChanged()), this, SLOT(onPageSelectionChanged()));
	connect(scenesList_, SIGNAL(selectionChanged()), this, SLOT(onSceneSelectionChanged()));
	connect(ui.addPageButton, SIGNAL(clicked()), this, SLOT(onAddPage()));
	connect(ui.deletePageButton, SIGNAL(clicked()), this, SLOT(onDeletePage()));
	connect(pagesList_, SIGNAL(layoutChanged()), this, SLOT(onPagesLayoutChanged()));
	connect(ui.okButton, SIGNAL(clicked()), this, SLOT(onOk()));
	//connect(ui.genThumbsBtn, SIGNAL(clicked()), this, SLOT(onGenThumbs()));
	//connect(ui.thumbFolderBrowse, SIGNAL(clicked()), this, SLOT(onThumbFolderBrowse()));

	//ui.thumbWidthEdit->setValidator(new QIntValidator(1, 512, NULL));
	//ui.thumbHeightEdit->setValidator(new QIntValidator(1, 512, NULL));

	ui.splitter->setStretchFactor(0, 1);
	ui.splitter->setStretchFactor(1, 1);

	setScenes(sceneChangerScenes);	
}

void SceneChangerDlg::setupList(GenericList ** list, QWidget * container)
{
	*list = new GenericList();
	QHBoxLayout * layout = new QHBoxLayout();
	layout->setMargin(0);
	layout->setSpacing(0);
	layout->addWidget(*list);
	container->setLayout(layout);
}

SceneChangerDlg::~SceneChangerDlg()
{
}

bool SceneChangerDlg::allowDragging() const
{
	return ui.allowDraggingCheckBox->isChecked();
}

void SceneChangerDlg::setScenes(const vector<Scene *> & scenes)
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

void SceneChangerDlg::onAddPage()
{
	const vector<GenericListItem *> & selItems = scenesList_->selectedItems();

	BOOST_FOREACH(GenericListItem * selItem, selItems)
	{
		Scene * scene = (Scene *)selItem->data;
		Scene * docScene = parentScene_;
		if (docScene)
		{
			if (scene == docScene || scene->dependsOn(docScene))
			{
				QMessageBox::information(this, tr("Error"), 
					tr("Could not add scene because doing so would result in a cycle."));
				return;
			}
		}
	}

	BOOST_FOREACH(GenericListItem * selItem, selItems)
	{
		Scene * page = (Scene *)selItem->data;
		scenes_.push_back(page);
		pagesList_->addTopLevelItem(selItem->string, page);
	}
}


std::vector<Scene *> SceneChangerDlg::scenes() const
{
	return scenes_;
}


void SceneChangerDlg::onDeletePage()
{
	pagesList_->deleteSelectedItems();	
	refreshPages();
}



void SceneChangerDlg::refreshPages()
{
	scenes_.clear();
	BOOST_FOREACH(GenericListItem * item, pagesList_->items())
	{
		scenes_.push_back((Scene *)item->data);		
	}
}

void SceneChangerDlg::onPagesLayoutChanged()
{
	if (ignoreLayoutChanged_) return;	
	refreshPages();	
}

void SceneChangerDlg::onPageSelectionChanged()
{
	const vector<GenericListItem *> & selItems = pagesList_->selectedItems();
	pageSelected_ = !selItems.empty();
	ui.deletePageButton->setEnabled(pageSelected_);
}

void SceneChangerDlg::onSceneSelectionChanged()
{
	const vector<GenericListItem *> & selItems = scenesList_->selectedItems();
	sceneSelected_ = !selItems.empty();
	ui.addPageButton->setEnabled(sceneSelected_);
}

void SceneChangerDlg::setPagesPaneEnabled(bool enabled)
{
	pagesList_->setEnabled(enabled);
	ui.deletePageButton->setEnabled(enabled?pageSelected_:0);
}

void SceneChangerDlg::setScenesPaneEnabled(bool enabled)
{
	scenesList_->setEnabled(enabled);
	ui.addPageButton->setEnabled(enabled?sceneSelected_:0);
}


void SceneChangerDlg::onOk()
{
	accept();
}


void SceneChangerDlg::onGenThumbs()
{
	/*QString thumbFolder;
	int thumbWidth, thumbHeight;

	thumbFolder = ui.thumbFolderEdit->text();
	thumbWidth = ui.thumbWidthEdit->text().toInt();
	thumbHeight = ui.thumbHeightEdit->text().toInt();

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

	dlg.exec();*/
}

void SceneChangerDlg::onThumbFolderBrowse()
{
	//QString dir = getExistingDirectory(tr("Thumbnail directory"));
	//ui.thumbFolderEdit->setText(dir);
}