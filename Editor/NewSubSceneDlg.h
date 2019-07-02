#pragma once
#include "ui_newSubSceneDlg.h"

class EditorDocument;
class Document;

class NewSubSceneDlg : public QDialog
{
	Q_OBJECT
public:
	NewSubSceneDlg(QWidget * parent, EditorDocument * document);
	Scene * scene() const;

private slots:

	void onOk();
	void onCurrentIndexChanged(int index);
private:
	void okEnabled();

	Ui::NewSubSceneDlg ui;

	EditorDocument * document_;
	Document * sceneDoc_;
	mutable std::map<int, Scene *> comboBoxScenes_;
};