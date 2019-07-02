#pragma once
#include <QDialog>
#include "ui_articleManagerDlg.h"
#include "MagazineArticle.h"
#include "MagazineDocumentTemplate.h"
#include "GenericList.h"

///////////////////////////////////////////////////////////////////////////////
class EditorDocument;
class Document;
class Scene;
class SceneObject;
class GenericList;



class ArticleManagerDlg : public QDialog
{	

	Q_OBJECT
public:
	ArticleManagerDlg(
		QWidget * parent, EditorDocument * document,
		const MagazineDocumentTemplate * articleManager
		);
	~ArticleManagerDlg();
	
	void setArticles(const std::vector<MagazineArticleSPtr> & articles);
	std::vector<MagazineArticleSPtr> articles() const;

	const std::vector<std::string> & uiImgFiles() const;

	const std::map<Scene *, QString> & thumbFileMap() const 
	{
		return thumbFileMap_;
	}

	QString uiFolder() const {return ui.uiFolderEdit->text();}

	bool bookmarks() const {return ui.bookmarksCheckBox->isChecked();}
	bool aspect() const {return ui.aspectCheckBox->isChecked();}
	bool doToContents() const {return ui.toContentsCheckBox->isChecked();}
	int toContentsIndex() const;
	bool resetArticleFirstPage() const {return ui.resetArticleCheckBox->isChecked();}
	MagazineDocumentTemplate::HomeButtonFunction homeButtonFunction() const;
	
	MagazineDocumentTemplate::TransitionMode transitionMode() const;

private slots:
	void onNewArticle();
	void onDeleteArticles();
	void onArticleSelectionChanged();
	void onPageSelectionChanged();
	void onSceneSelectionChanged();
	void onAddPage();
	void onArticlesLayoutChanged();
	void onPagesLayoutChanged();
	void onDeletePage();
	void onArticleNameChanged(GenericListItem * item);
	void onOk();
	void onGenThumbs();
	void onThumbnailFolderBrowse();
	void onUiFolderBrowse();
	void onContentsCheckBoxClicked();
	void onContentsPageChanged(int index);

private:
	void refreshArticles();
	void refreshPages(MagazineArticle * article);
	void setupList(GenericList ** list, QWidget * container);
	void setPagesPaneEnabled(bool enabled);
	void setScenesPaneEnabled(bool enabled);
	MagazineArticle * curSelArticle() const;

	void populateComboBox();

private:
	Ui::ArticleManagerDlg ui;
	GenericList * articlesList_, * pagesList_, * scenesList_;

	EditorDocument * document_;
	
	bool ignoreSelChanged_;
	bool ignoreLayoutChanged_;
	const MagazineDocumentTemplate * articleManager_;

	std::vector<MagazineArticleSPtr> articles_;
	std::map<Scene *, QString> thumbFileMap_;

	std::vector<std::string> uiImgFiles_;

	bool pageSelected_;
	bool sceneSelected_;

	int contentsPageIndex_;
	MagazineArticle * contentsPage_;
	
};
///////////////////////////////////////////////////////////////////////////////
