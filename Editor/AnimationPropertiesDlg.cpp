#include "stdafx.h"
#include "AnimationPropertiesDlg.h"
#include "Animation.h"
#include "VideoObject.h"
#include "AudioObject.h"
#include "Utils.h"

AnimationPropertiesDlg::AnimationPropertiesDlg(Animation * animation, QWidget * parent) : QDialog(parent)
{
	ui.setupUi(this);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint); 

	ui.repeatCheckBox->setChecked(animation->repeat());

	Scene * scene = animation->parentScene();
	
	sceneSyncObjs_.push_back(NULL);
	scene->getSceneObjectsByID(VideoObject().type(), &sceneSyncObjs_);
	scene->getSceneObjectsByID(AudioObject().type(), &sceneSyncObjs_);

	ui.syncComboBox->addItem(tr("<none>"));
	BOOST_FOREACH(SceneObject * obj, sceneSyncObjs_)
	{
		if (!obj) continue;
		ui.syncComboBox->addItem(stdStringToQString(obj->id()));
	}

	ui.syncComboBox->setCurrentIndex(getSyncObjIndex(scene->syncObject(animation)));

	connect(ui.okButton, SIGNAL(clicked()), this, SLOT(onOk()));
}

int AnimationPropertiesDlg::getSyncObjIndex(SceneObject * obj) const
{
	int ret = 0;
	for (int i = 0; i < (int)sceneSyncObjs_.size(); ++i)
	{
		if (sceneSyncObjs_[i] == obj) ret = i;
	}
	return ret;
}

void AnimationPropertiesDlg::onOk()
{
	accept();
}

bool AnimationPropertiesDlg::repeat() const
{
	return ui.repeatCheckBox->isChecked();
}

SceneObject * AnimationPropertiesDlg::syncObj() const
{
	return sceneSyncObjs_[ui.syncComboBox->currentIndex()];
}

