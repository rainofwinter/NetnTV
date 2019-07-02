#pragma once
#include "ui_singleSceneDocumentDlg.h"
#include "Types.h"

class Document;
class SingleSceneDocumentTemplate;
class Scene;

class SingleSceneDocumentDlg : public QDialog
{
	Q_OBJECT
public:
	SingleSceneDocumentDlg(
		Document * document, SingleSceneDocumentTemplate * docTemplate, 
		QWidget * parent);

	Scene * scene() const;
private slots:
	void onOk();
private:
	Ui::SingleSceneDocumentDlg ui;
	const std::vector<SceneSPtr> & scenes_;
	SingleSceneDocumentTemplate * docTemplate_;
};