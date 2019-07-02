#include "stdafx.h"
#include "SingleSceneDocumentDlg.h"
#include "Document.h"
#include "Scene.h"
#include "Utils.h"
#include "SingleSceneDocumentTemplate.h"

using namespace std;

SingleSceneDocumentDlg::SingleSceneDocumentDlg(
	Document * document, SingleSceneDocumentTemplate * docTemplate, 
	QWidget * parent)
: QDialog(parent), scenes_(document->scenes()), docTemplate_(docTemplate)
{
	ui.setupUi(this);	
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint); 


	Scene * origScene = docTemplate->scene();
	int selIndex = -1;
	int i  = 0;
	BOOST_FOREACH(SceneSPtr scene, scenes_)
	{
		ui.sceneComboBox->addItem(stdStringToQString(scene->name()));
		if (scene.get() == origScene) selIndex = i;
		++i;
	}

	ui.sceneComboBox->setCurrentIndex(selIndex);

	connect(ui.okButton, SIGNAL(clicked()), this, SLOT(onOk()));
}

void SingleSceneDocumentDlg::onOk()
{
	if (ui.sceneComboBox->currentIndex() >= 0)
	{
		accept();
	}
	else
	{
		QMessageBox::information(this, tr("Error"), tr("No scene selected"));
	}
}

Scene * SingleSceneDocumentDlg::scene() const
{
	int index = ui.sceneComboBox->currentIndex();
	
	return scenes_[index].get();
}