#pragma once
#include <QDialog>
#include "ui_sceneChangerDlg.h"
#include "GenericList.h"

///////////////////////////////////////////////////////////////////////////////
class Document;
class Scene;
class SceneObject;
class GenericList;
class SceneChanger;
class EditorDocument;

class SceneChangerDlg : public QDialog
{
	Q_OBJECT
public:
	SceneChangerDlg(
		QWidget * parent, EditorDocument * document,
		Scene * parentScene,
		const std::vector<Scene *> & scenes,
		bool allowDragging
		);
	~SceneChangerDlg();
	
	void setScenes(const std::vector<Scene *> & scenes);
	std::vector<Scene *> scenes() const;
	bool allowDragging() const;

private slots:
	void onPageSelectionChanged();
	void onSceneSelectionChanged();
	void onAddPage();
	void onPagesLayoutChanged();
	void onDeletePage();
	void onOk();
	void onGenThumbs();
	void onThumbFolderBrowse();

private:
	void refreshPages();
	void setupList(GenericList ** list, QWidget * container);
	void setPagesPaneEnabled(bool enabled);
	void setScenesPaneEnabled(bool enabled);

private:
	Ui::SceneChangerDlg ui;
	GenericList * pagesList_, * scenesList_;

	EditorDocument * document_;
	
	bool ignoreSelChanged_;
	bool ignoreLayoutChanged_;
	Scene * parentScene_;

	bool pageSelected_;
	bool sceneSelected_;

	std::vector<Scene *> scenes_;
	
};
///////////////////////////////////////////////////////////////////////////////
