#include "stdafx.h"
#include "ArticleManagerDlg.h"
#include "GenThumbsDlg.h"
#include "EditorDocument.h"
#include "Document.h"
#include "Scene.h"
#include "SceneObject.h"
#include "Global.h"
#include "EditorGlobal.h"
#include "EditorObject.h"
#include "GenericList.h"
#include "Utils.h"

using namespace std;


ArticleManagerDlg::ArticleManagerDlg(
	QWidget * parent, EditorDocument * document,
	const MagazineDocumentTemplate * articleManager) : QDialog(parent)
{	
	articleManager_ = articleManager;
	sceneSelected_ = pageSelected_ = false;
	ignoreSelChanged_ = false;
	ignoreLayoutChanged_ = false;
	document_ = document;
	ui.setupUi(this);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint); 

	setupList(&articlesList_, ui.articlesWidget);
	setupList(&pagesList_, ui.pagesWidget);
	setupList(&scenesList_, ui.scenesWidget);
	
	Document * sceneDoc = document->document();
	const vector<SceneSPtr> & scenes = sceneDoc->scenes();
	BOOST_FOREACH(SceneSPtr scene, scenes)
	{
		scenesList_->addTopLevelItem(
			stdStringToQString(scene->name()),
			scene.get());
	}

	pagesList_->setReorderable(true);
	
	articlesList_->setReorderable(true);
	articlesList_->setEditable(true);

	setScenesPaneEnabled(false);
	setPagesPaneEnabled(false);

	connect(articlesList_, SIGNAL(selectionChanged()), this, SLOT(onArticleSelectionChanged()));
	connect(articlesList_, SIGNAL(layoutChanged()), this, SLOT(onArticlesLayoutChanged()));
	connect(articlesList_, SIGNAL(itemEdited(GenericListItem *)), 
		this, SLOT(onArticleNameChanged(GenericListItem *)));
	connect(pagesList_, SIGNAL(selectionChanged()), this, SLOT(onPageSelectionChanged()));
	connect(scenesList_, SIGNAL(selectionChanged()), this, SLOT(onSceneSelectionChanged()));
	connect(ui.newArticleButton, SIGNAL(clicked()), this, SLOT(onNewArticle()));
	connect(ui.deleteArticleButton, SIGNAL(clicked()), this, SLOT(onDeleteArticles()));
	connect(ui.addPageButton, SIGNAL(clicked()), this, SLOT(onAddPage()));
	connect(ui.deletePageButton, SIGNAL(clicked()), this, SLOT(onDeletePage()));
	connect(pagesList_, SIGNAL(layoutChanged()), this, SLOT(onPagesLayoutChanged()));
	
	connect(ui.genThumbsCheckBox, SIGNAL(clicked()), this, SLOT(onGenThumbs()));
	connect(ui.thumbnailBrowseButton, SIGNAL(clicked()), this, SLOT(onThumbnailFolderBrowse()));
	connect(ui.uiBrowseButton, SIGNAL(clicked()), this, SLOT(onUiFolderBrowse()));
	connect(ui.toContentsCheckBox, SIGNAL(clicked()), this, SLOT(onContentsCheckBoxClicked()));	
	connect(ui.okButton, SIGNAL(clicked()), this, SLOT(onOk()));	
	connect(ui.contentsPageComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onContentsPageChanged(int)));

	QString curDir = QDir().current().absolutePath();
	if (curDir[curDir.size() - 1] != '/' && curDir[curDir.size() - 1] != '\\')
		curDir += '/';
	
	ui.thumbnailFolderEdit->setText(/*curDir +*/ "./Res/Thumbs");
	ui.uiFolderEdit->setText(/*curDir +*/ "./Res/articleManagerUi");	

	ui.splitter->setStretchFactor(0, 1);
	ui.splitter->setStretchFactor(1, 2);
	ui.splitter->setStretchFactor(2, 1);

	setArticles(articleManager->articles());

	ui.bookmarksCheckBox->setChecked(articleManager->doBookmarks());
	ui.aspectCheckBox->setChecked(articleManager->doAspect());

	ui.resetArticleCheckBox->setChecked(articleManager->resetArticleFirstPage());

	if (articleManager->homeButtonFunction() == MagazineDocumentTemplate::HomeButtonFirstPage)
		ui.homeFirstPageRadio->setChecked(true);	
	else
		ui.homeExitRadio->setChecked(true);

	ui.toContentsCheckBox->setChecked(articleManager->doToContentsPage());
	onContentsCheckBoxClicked();

	populateComboBox();
	ui.contentsPageComboBox->setCurrentIndex(articleManager->contentsArticleIndex());

	ui.transitionEffectComboBox->addItem(tr("Sliding"));
	ui.transitionEffectComboBox->addItem(tr("Inverted Page flip"));
	ui.transitionEffectComboBox->addItem(tr("Page flip"));
	ui.transitionEffectComboBox->setCurrentIndex((int)articleManager->transitionMode());
}

void ArticleManagerDlg::onContentsPageChanged(int index)
{
	const vector<GenericListItem *> & items = articlesList_->items();
	contentsPageIndex_ = ui.contentsPageComboBox->currentIndex();
	contentsPage_ = NULL;
	if (contentsPageIndex_ >= 0 && contentsPageIndex_ < (int)items.size())
	{
		contentsPage_ = (MagazineArticle *)items[contentsPageIndex_]->data;
	}
}

void ArticleManagerDlg::populateComboBox()
{
	const vector<GenericListItem *> & items = articlesList_->items();

	int index = ui.contentsPageComboBox->currentIndex();
	if (index >= (int)items.size()) --index;

	ui.contentsPageComboBox->clear();
	
	for (int i = 0; i < (int)items.size(); ++i)
	{
		if (items[i]->data == contentsPage_)
		{
			index = i;
		}
		ui.contentsPageComboBox->addItem(items[i]->string);
	}
	
	ui.contentsPageComboBox->setCurrentIndex(index);	
}

void ArticleManagerDlg::onContentsCheckBoxClicked()
{
	ui.contentsPageComboBox->setEnabled(ui.toContentsCheckBox->isChecked());
}

void ArticleManagerDlg::setupList(GenericList ** list, QWidget * container)
{
	*list = new GenericList();
	QHBoxLayout * layout = new QHBoxLayout();
	layout->setMargin(0);
	layout->setSpacing(0);
	layout->addWidget(*list);
	container->setLayout(layout);
}

ArticleManagerDlg::~ArticleManagerDlg()
{
}

int ArticleManagerDlg::toContentsIndex() const
{
	return ui.contentsPageComboBox->currentIndex();
}

void ArticleManagerDlg::setArticles(const vector<MagazineArticleSPtr> & articles)
{		
	articles_.clear();
	BOOST_FOREACH(MagazineArticleSPtr article, articles)
	{
		articles_.push_back(MagazineArticleSPtr(new MagazineArticle(*article)));
	}
	


	ignoreSelChanged_ = true;
	ignoreLayoutChanged_ = true;
	articlesList_->clearItems();
	BOOST_FOREACH(MagazineArticleSPtr & article, articles_)
	{	
		articlesList_->addTopLevelItem(
			stdStringToQString(article->name()), article.get());
	}
	ignoreLayoutChanged_ = false;
	ignoreSelChanged_ = false;
}

void ArticleManagerDlg::onNewArticle()
{	
	MagazineArticle * article = new MagazineArticle;
	articles_.push_back(MagazineArticleSPtr(article));
	articlesList_->addTopLevelItem(
		stdStringToQString(article->name()), article);

	populateComboBox();
}

void ArticleManagerDlg::onDeleteArticles()
{	
	articlesList_->deleteSelectedItems();
	refreshArticles();
	populateComboBox();
}

void ArticleManagerDlg::onAddPage()
{
	const vector<GenericListItem *> & selItems = scenesList_->selectedItems();
	MagazineArticle * article = curSelArticle();

	
	BOOST_FOREACH(GenericListItem * selItem, selItems)
	{
		Scene * scene = (Scene *)selItem->data;
	}

	BOOST_FOREACH(GenericListItem * selItem, selItems)
	{
		MagazinePage * page = new MagazinePage((Scene *)selItem->data, "");		
		article->addPage(MagazinePageSPtr(page));
		pagesList_->addTopLevelItem(selItem->string, page);
	}
}

MagazineArticle * ArticleManagerDlg::curSelArticle() const
{
	if (articlesList_->selectedItems().empty()) return 0;
	return (MagazineArticle *)articlesList_->selectedItems().front()->data;
}

std::vector<MagazineArticleSPtr> ArticleManagerDlg::articles() const
{
	return articles_;
}


void ArticleManagerDlg::onDeletePage()
{
	MagazineArticle * article = curSelArticle();
	pagesList_->deleteSelectedItems();	
	refreshPages(article);
}

void ArticleManagerDlg::refreshArticles()
{
	//temporary holding place for sptrs to prevent deletion during .clear()	
	std::map<MagazineArticle *, MagazineArticleSPtr> articlesBackup;
	BOOST_FOREACH(MagazineArticleSPtr & article, articles_)
	{
		articlesBackup[article.get()] = article;
	}

	articles_.clear();
	BOOST_FOREACH(GenericListItem * item, articlesList_->items())
	{
		articles_.push_back(articlesBackup[(MagazineArticle *)item->data]);		
	}
}

void ArticleManagerDlg::onArticlesLayoutChanged()
{
	if (ignoreLayoutChanged_) return;
	
	refreshArticles();
}

void ArticleManagerDlg::refreshPages(MagazineArticle * article)
{
	//temporary holding place for sptrs to prevent deletion during .clear()	
	std::map<MagazinePage *, MagazinePageSPtr> pagesBackup;
	BOOST_FOREACH(const MagazinePageSPtr & page, article->pages())
	{
		pagesBackup[page.get()] = page;
	}

	article->deletePages();
	BOOST_FOREACH(GenericListItem * item, pagesList_->items())
	{
		article->addPage(pagesBackup[(MagazinePage *)item->data]);		
	}
}

void ArticleManagerDlg::onPagesLayoutChanged()
{
	if (ignoreLayoutChanged_) return;
	if (articlesList_->selectedItems().empty()) return;

	MagazineArticle * article = curSelArticle();
	
	refreshPages(article);	
}

void ArticleManagerDlg::onArticleSelectionChanged()
{
	if (ignoreSelChanged_) return;
	const vector<GenericListItem *> & selItems = articlesList_->selectedItems();

	setPagesPaneEnabled(!selItems.empty());
	ignoreLayoutChanged_ = true;
	if (selItems.empty()) 
		pagesList_->clearItems();
	else
	{
		pagesList_->clearItems();
		MagazineArticle * article = curSelArticle();
		BOOST_FOREACH(MagazinePageSPtr page, article->pages())
		{
			pagesList_->addTopLevelItem(
				stdStringToQString(page->scene()->name()), page.get());
		}
	}
	ignoreLayoutChanged_ = false;
	onPagesLayoutChanged();

	setScenesPaneEnabled(!selItems.empty());
}

void ArticleManagerDlg::onPageSelectionChanged()
{
	const vector<GenericListItem *> & selItems = pagesList_->selectedItems();
	pageSelected_ = !selItems.empty();
	ui.deletePageButton->setEnabled(pageSelected_);
}

void ArticleManagerDlg::onSceneSelectionChanged()
{
	const vector<GenericListItem *> & selItems = scenesList_->selectedItems();
	sceneSelected_ = !selItems.empty();
	ui.addPageButton->setEnabled(sceneSelected_);
}

void ArticleManagerDlg::setPagesPaneEnabled(bool enabled)
{
	pagesList_->setEnabled(enabled);
	ui.deletePageButton->setEnabled(enabled?pageSelected_:0);
}

void ArticleManagerDlg::setScenesPaneEnabled(bool enabled)
{
	scenesList_->setEnabled(enabled);
	ui.addPageButton->setEnabled(enabled?sceneSelected_:0);
}

void ArticleManagerDlg::onArticleNameChanged(GenericListItem * item)
{
	MagazineArticle * article = (MagazineArticle *)item->data;
	article->setName(qStringToStdString(item->string));
	populateComboBox();
}

void ArticleManagerDlg::onOk()
{
	//copy ui images
	QString uiFolder = ui.uiFolderEdit->text();
	if (uiFolder[uiFolder.size() - 1] != '/' && 
		uiFolder[uiFolder.size() - 1] != '\\') uiFolder += '/';


	bool status = 1;

	typedef pair<QString, QString> FilePair;
	vector<FilePair> files;
	
	
	status &= QDir::current().mkpath(uiFolder);	

	files.push_back(make_pair(
		":/data/Resources/articleManager/scrollBarBg.png",
		uiFolder + "scrollBarBg.png"));

	files.push_back(make_pair(
		":/data/Resources/articleManager/scrollBarLeft.png",
		uiFolder + "scrollBarLeft.png"));

	files.push_back(make_pair(
		":/data/Resources/articleManager/scrollBarRight.png",
		uiFolder + "scrollBarRight.png"));


	files.push_back(make_pair(
		":/data/Resources/articleManager/menuBg.png",
		uiFolder + "menuBg.png"));

	files.push_back(make_pair(
		":/data/Resources/articleManager/layoutButton.png",
		uiFolder + "layoutButton.png"));

	files.push_back(make_pair(
		":/data/Resources/articleManager/homeButton.png",
		uiFolder + "homeButton.png"));

	files.push_back(make_pair(
		":/data/Resources/articleManager/scrollBarThumb.png",
		uiFolder + "scrollBarThumb.png"));	

	files.push_back(make_pair(
		":/data/Resources/articleManager/bg.png",
		uiFolder + "bg.png"));
		
	files.push_back(make_pair(
		":/data/Resources/articleManager/bookMarkActive.png",
		uiFolder + "bookmarkActive.png"));

	files.push_back(make_pair(
		":/data/Resources/articleManager/bookMarkInactive.png",
		uiFolder + "bookmarkInactive.png"));

	files.push_back(make_pair(
		":/data/Resources/articleManager/bookmarks.png",
		uiFolder + "bookmarkList.png"));

	files.push_back(make_pair(
		":/data/Resources/articleManager/bookmarksTitle.png",
		uiFolder + "bookmarksTitle.png"));

	files.push_back(make_pair(
		":/data/Resources/articleManager/aspect.png",
		uiFolder + "aspect.png"));

	files.push_back(make_pair(
		":/data/Resources/articleManager/aspectInactive.png",
		uiFolder + "aspectInactive.png"));

	files.push_back(make_pair(
		":/data/Resources/articleManager/toContents.png",
		uiFolder + "toContents.png"));

		
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

	/*
	if (!status)
	{
		QMessageBox::information(this, tr("Error"), tr("Failed to generate ui image"));
		return;
	}
	*/

	if (!ui.genThumbsCheckBox->isChecked())
	{
		accept();
	}
	else
	{
		QString thumbFolder = ui.thumbnailFolderEdit->text();
		QDir().mkpath(thumbFolder);

		std::vector<Scene *> scenesToRender;
		BOOST_FOREACH(MagazineArticleSPtr article, articles_)
		{
			BOOST_FOREACH(MagazinePageSPtr page, article->pages())
				scenesToRender.push_back(page->scene());
		}

		GenThumbsDlg dlg(
			document_,
			ui.thumbnailFolderEdit->text(),
			scenesToRender, 
			articleManager_->thumbWidth(), articleManager_->thumbHeight(), 
			this);

		if (dlg.numScenesToRender() == 0)
		{
			//no scenes to render thumbnails for
			accept();
		}
		else if (dlg.exec() == QDialog::Accepted)
		{	
			thumbFileMap_ = dlg.thumbFileMap();

			BOOST_FOREACH(MagazineArticleSPtr article, articles_)
			{				
				for (int i = 0; i < article->numPages(); ++i)
				{
					article->page(i)->setThumbnailFile( 
						qStringToStdString(thumbFileMap_[article->page(i)->scene()]));
				}					
			}


			accept();
		}
	}
}

void ArticleManagerDlg::onGenThumbs()
{
	bool isChecked = ui.genThumbsCheckBox->isChecked();
	ui.thumbnailFolderEdit->setEnabled(isChecked);
	ui.thumbnailBrowseButton->setEnabled(isChecked);
	ui.thumbnailFolderLabel->setEnabled(isChecked);
}


void ArticleManagerDlg::onThumbnailFolderBrowse()
{
	/*
	QString directory = QFileDialog::getExistingDirectory(this, QString(),
		ui.thumbnailFolderEdit->text());*/
	QString directory = getExistingDirectory(QString(),
		ui.thumbnailFolderEdit->text());
	
	if (!directory.isEmpty())
	{
		QDir dir(directory);
		ui.thumbnailFolderEdit->setText(dir.absolutePath());
	}
}

void ArticleManagerDlg::onUiFolderBrowse()
{	
	/*
	QString directory = QFileDialog::getExistingDirectory(this, QString(),
		ui.uiFolderEdit->text());
	*/

	QString directory = getExistingDirectory(QString(),
		ui.uiFolderEdit->text());


	if (!directory.isEmpty())
	{
		QDir dir(directory);
		ui.uiFolderEdit->setText(dir.absolutePath());
	}
}


const std::vector<std::string> & ArticleManagerDlg::uiImgFiles() const
{
	vector<string> uiImgFiles;
	

	return uiImgFiles_;
}
MagazineDocumentTemplate::HomeButtonFunction ArticleManagerDlg::homeButtonFunction() const
{
	if (ui.homeFirstPageRadio->isChecked())
		return MagazineDocumentTemplate::HomeButtonFirstPage;
	else
		return MagazineDocumentTemplate::HomeButtonExitToViewerLayer;

}

MagazineDocumentTemplate::TransitionMode ArticleManagerDlg::transitionMode() const
{
	return (MagazineDocumentTemplate::TransitionMode)ui.transitionEffectComboBox->currentIndex();
}