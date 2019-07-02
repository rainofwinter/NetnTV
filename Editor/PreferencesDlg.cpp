#include "stdafx.h"
#include "PreferencesDlg.h"
#include "EditorGlobal.h"
#include "Utils.h"
#include "PropertyPage.h"

PreferencesDlg::PreferencesDlg(QWidget * parent) :
QDialog(parent)
{
	ui.setupUi(this);	
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint); 
	
	ui.propertiesWidget;

	layout = new QVBoxLayout;
	layout->setMargin(0);
	layout->setSpacing(0);
	ppage = new PropertyPage;

	playerLocation_ = new FileNameProperty(tr("Player Location"), tr("Executable Files (*.exe)"));
	ppage->setGroupBox(false);
	ppage->addProperty(playerLocation_);

	ppage->addSpacing(4);
	sceneOutlineColor_ = new ColorProperty(tr("Scene outline color"));
	ppage->addProperty(sceneOutlineColor_);

	ppage->startGroup("Grid");
	showGrid_ = new BoolProperty(tr("Show grid"));
	ppage->addProperty(showGrid_);
	gridSpacing_ = new EditProperty(tr("Grid spacing"), &QIntValidator(0, 10000, NULL));	
	ppage->addProperty(gridSpacing_);
	ppage->endGroup();

	ppage->addSpacing(4);
	tempfileSaveInterval_ = new EditProperty(tr("Tempfile interval"), &QIntValidator(0, 10000, NULL));	
	ppage->addProperty(tempfileSaveInterval_);

	ppage->addSpring();
	layout->addWidget(ppage->widget());
	
	ui.propertiesWidget->setLayout(layout);


	playerLocation_->setValue(EditorGlobal::instance().playerLocation());
	showGrid_->setValue(EditorGlobal::instance().showGrid());
	std::stringstream ss;
	ss << (int)EditorGlobal::instance().gridSpacing();	
	gridSpacing_->setValue(stdStringToQString(ss.str()));
	sceneOutlineColor_->setValue(&EditorGlobal::instance().sceneOutlineColor());
	std::stringstream tempss;
	tempss << (int)EditorGlobal::instance().tempfileInterval();	
	tempfileSaveInterval_->setValue(stdStringToQString(tempss.str()));

	connect(ui.okButton, SIGNAL(clicked()), this, SLOT(onOk()));

}

PreferencesDlg::~PreferencesDlg()
{
	delete ppage;
	delete layout;
}

void PreferencesDlg::onOk()
{
	//EditorGlobal::instance().setPlayerLocation(ui.playerEdit->text());
	EditorGlobal::instance().setPlayerLocation(playerLocation_->value());
	EditorGlobal::instance().setShowGrid(showGrid_->value());
	EditorGlobal::instance().setSceneOutlineColor(sceneOutlineColor_->value());
	EditorGlobal::instance().setGridSpacing(gridSpacing_->value().toFloat());
	EditorGlobal::instance().setTempfileInterval(tempfileSaveInterval_->value().toInt());

	accept();
}

