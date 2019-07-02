#include "stdafx.h"
#include "NewSubSceneDlg.h"
#include "EditorGlobal.h"
#include "Utils.h"
#include "EditorDocument.h"
#include "Document.h"

NewSubSceneDlg::NewSubSceneDlg(
	QWidget * parent, EditorDocument * document) : QDialog(parent)
{
	ui.setupUi(this);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint); 

	document_ = document;

	int index = 0;
	BOOST_FOREACH(SceneSPtr scene, document->scenes())
	{
		if (scene.get() == document->selectedScene()) continue;
		ui.sceneComboBox->addItem(stdStringToQString(scene->name()));
		comboBoxScenes_[index++] = scene.get();
	}

	ui.sceneComboBox->setCurrentIndex(0);

	connect(ui.okButton, SIGNAL(clicked()), this, SLOT(onOk()));
	connect(ui.sceneComboBox, SIGNAL(currentIndexChanged(int)), 
		this, SLOT(onCurrentIndexChanged(int)));

	okEnabled();
}

Scene * NewSubSceneDlg::scene() const
{
	int index = ui.sceneComboBox->currentIndex();
	return comboBoxScenes_[index];
}

void NewSubSceneDlg::okEnabled()
{
	ui.okButton->setEnabled(ui.sceneComboBox->currentIndex() >= 0);
}

void NewSubSceneDlg::onCurrentIndexChanged(int index)
{
	ui.okButton->setEnabled(index >= 0);
	okEnabled();
}

void NewSubSceneDlg::onOk()
{	
	accept();
}