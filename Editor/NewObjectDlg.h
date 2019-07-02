#pragma once
#include <QDialog>
#include "ui_newObjectDlg.h"

///////////////////////////////////////////////////////////////////////////////
class EditorDocument;
class Scene;
class SceneObject;
class EditorCustomObject;

class NewObjectDlg : public QDialog
{
	Q_OBJECT
public:
	NewObjectDlg(EditorDocument * document, QWidget * parent);
	
	SceneObject * createSceneObject() const;

private slots:
	void onItemSelectionChanged(QTreeWidgetItem *, QTreeWidgetItem *);
	void onItemDoubleClicked(QTreeWidgetItem * item, int col);
private:
	Ui::NewObjectDlg ui;
	EditorDocument * document_;
	std::vector<SceneObject *> sceneObjs_;
	std::vector<EditorCustomObject *> editorCustomObjects_;
	
};

