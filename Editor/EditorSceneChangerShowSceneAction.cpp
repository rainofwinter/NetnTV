#include "stdafx.h"
#include "EditorSceneChangerShowSceneAction.h"
#include "SceneChangerShowSceneAction.h"
#include "Scene.h"
#include "EditorDocument.h"
#include "Document.h"
#include "SceneChanger.h"
#include "Utils.h"

using namespace std;

ActionPropertyPage * EditorSceneChangerShowSceneAction::propertyPage() const
{
	return new EditorSceneChangerShowScenePropertyPage();
}

boost::uuids::uuid EditorSceneChangerShowSceneAction::actionType() const 
{
	return SceneChangerShowSceneAction().type();
}

QString EditorSceneChangerShowSceneAction::toString(Action * pAction) const
{
	SceneChangerShowSceneAction * action = (SceneChangerShowSceneAction *)pAction;
	QString typeStr = QString::fromLocal8Bit(action->typeStr());	
	
	if (action->mode() == SceneChangerShowSceneAction::Specific)
	{
		QString sceneName = QString::fromLocal8Bit(action->scene()->name().c_str());
		return typeStr + " - " + stdStringToQString(action->targetObject()->id()) + " - " + sceneName;
	}
	else
	{
		if (action->direction() == SceneChangerShowSceneAction::Previous)
			return typeStr + " - Previous";
		else
			return typeStr + " - Next";
	}	
}

///////////////////////////////////////////////////////////////////////////////
EditorSceneChangerShowScenePropertyPage::EditorSceneChangerShowScenePropertyPage()
{	
	setGroupBox(false);
		
	mode_ = new ComboBoxProperty(tr("mode"));
	addProperty(mode_);
	mode_->addItem(tr("Specific"));
	mode_->addItem(tr("Adjacent"));

	addSpacing(4);	
	scene_ = new ComboBoxProperty(tr("scene"));
	addProperty(scene_, PropertyPage::Vertical);	

	addSpacing(4);
	direction_ = new ComboBoxProperty(tr("direction"));
	addProperty(direction_, PropertyPage::Vertical);
	direction_->addItem(tr("Previous"));
	direction_->addItem(tr("Next"));

	setModeEnabled();
}

void EditorSceneChangerShowScenePropertyPage::setModeEnabled()
{
	SceneChangerShowSceneAction::ChangeMode mode = (SceneChangerShowSceneAction::ChangeMode)mode_->value();
	if (mode == SceneChangerShowSceneAction::Specific)
	{
		scene_->setEnabled(true);
		direction_->setEnabled(false);
	}
	else
	{
		scene_->setEnabled(false);
		direction_->setEnabled(true);
	}
}

EditorSceneChangerShowScenePropertyPage::~EditorSceneChangerShowScenePropertyPage()
{
}

void EditorSceneChangerShowScenePropertyPage::setDocument(EditorDocument * document)
{
	ActionPropertyPage::setDocument(document);
	
	SceneChanger * obj = (SceneChanger *)action_->targetObject();
	scenesList_.clear();
	scenesMap_.clear();
	scene_->clearItems();	

	int index = 0;

	BOOST_FOREACH(Scene * scene, obj->scenes())
	{				
		map<Scene *, int>::iterator iter = scenesMap_.find(scene);
		if (iter == scenesMap_.end())
		{
			scene_->addItem(QString::fromLocal8Bit(scene->name().c_str()));
			scenesMap_[scene] = index++;
			scenesList_.push_back(scene);
		}
	}

}

void EditorSceneChangerShowScenePropertyPage::update()
{
	SceneChangerShowSceneAction * action = (SceneChangerShowSceneAction *)action_;
	Scene * scene = action->scene();	

	if (scene)
	{
		int i = 0;
		BOOST_FOREACH(Scene * curScene, scenesList_)
		{
			if (curScene == scene) break;
			++i;
		}	
		scene_->setValue(i);	
	}	
	else 
		scene_->setValue(-1);

	mode_->setValue((int)action->mode());
	direction_->setValue((int)action->direction());
	setModeEnabled();
}

void EditorSceneChangerShowScenePropertyPage::onChanged(Property * property)
{
	PropertyPage::onChanged(property);
	SceneChangerShowSceneAction * action = (SceneChangerShowSceneAction *)action_;

	int index = scene_->value();
	if (index >= 0)
		action->setScene(scenesList_[index]);

	action->setMode((SceneChangerShowSceneAction::ChangeMode)mode_->value());
	action->setDirection((SceneChangerShowSceneAction::Direction)direction_->value());	

	update();
}

bool EditorSceneChangerShowScenePropertyPage::isValid() const
{
	return 
		(mode_->value() == (int)SceneChangerShowSceneAction::Specific && scene_->value() >= 0) ||
		(mode_->value() == (int)SceneChangerShowSceneAction::Adjacent && direction_->value() >= 0);
}