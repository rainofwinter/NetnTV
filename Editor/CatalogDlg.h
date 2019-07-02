#pragma once
#include <QDialog>
#include "ui_catalogDlg.h"
#include "GenericList.h"


///////////////////////////////////////////////////////////////////////////////
class Document;
class Scene;
class SceneObject;
class GenericList;
class EditorDocument;

class CatalogDlg : public QDialog
{
	Q_OBJECT
public:
	CatalogDlg(
		QWidget * parent, EditorDocument * document,
		const std::vector<Scene *> & scenes
		);
	~CatalogDlg();

	void setScenes(const std::vector<Scene *> & scenes);
	std::vector<Scene *> scenes() const;

	const std::vector<std::string> & uiImgFiles() const {return uiImgFiles_;}

	const std::map<int, std::string> thumbFileMap() const;
	
private slots:
	void onPageSelectionChanged();
	void onSceneSelectionChanged();
	void onAddPage();
	void onPagesLayoutChanged();
	void onDeletePage();
	void onOk();
	

private:
	void genThumbs();
	void refreshPages();
	void setupList(GenericList ** list, QWidget * container);
	void setPagesPaneEnabled(bool enabled);
	void setScenesPaneEnabled(bool enabled);

private:
	Ui::CatalogDlg ui;
	GenericList * pagesList_, * scenesList_;

	EditorDocument * document_;
	
	bool ignoreSelChanged_;
	bool ignoreLayoutChanged_;

	bool pageSelected_;
	bool sceneSelected_;

	std::vector<Scene *> scenes_;

	std::map<Scene *, QString> thumbFileMap_;

	std::vector<std::string> uiImgFiles_;
	
};