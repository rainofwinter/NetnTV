#include "stdafx.h"
#include "EditorSceneChangerSceneSetAction.h"
#include "SceneChangerSceneSetAction.h"
#include "Scene.h"
#include "EditorDocument.h"
#include "Document.h"
#include "SceneChanger.h"
#include "Utils.h"

using namespace std;

ActionPropertyPage * EditorSceneChangerSceneSetAction::propertyPage() const
{
	return new EditorSceneChangerSceneSetPropertyPage();
}

boost::uuids::uuid EditorSceneChangerSceneSetAction::actionType() const 
{
	return SceneChangerSceneSetAction().type();
}

QString EditorSceneChangerSceneSetAction::toString(Action * pAction) const
{
	SceneChangerSceneSetAction * action = (SceneChangerSceneSetAction *)pAction;
	QString typeStr = QString::fromLocal8Bit(action->typeStr());
	
	if (action->mode() == SceneChangerSceneSetAction::Specific)
	{
		QString sceneName = QString::fromLocal8Bit(action->scene()->name().c_str());
		return typeStr + " - " + stdStringToQString(action->targetObject()->id()) + " - " + sceneName;
	}
	else
	{
		if (action->direction() == SceneChangerSceneSetAction::Previous)
			return typeStr + " - Previous";
		else
			return typeStr + " - Next";
	}	
}

///////////////////////////////////////////////////////////////////////////////
EditorSceneChangerSceneSetPropertyPage::EditorSceneChangerSceneSetPropertyPage()
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

void EditorSceneChangerSceneSetPropertyPage::setModeEnabled()
{
	SceneChangerSceneSetAction::ChangeMode mode = (SceneChangerSceneSetAction::ChangeMode)mode_->value();
	if (mode == SceneChangerSceneSetAction::Specific)
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

EditorSceneChangerSceneSetPropertyPage::~EditorSceneChangerSceneSetPropertyPage()
{
}

void EditorSceneChangerSceneSetPropertyPage::setDocument(EditorDocument * document)
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

void EditorSceneChangerSceneSetPropertyPage::update()
{
	SceneChangerSceneSetAction * action = (SceneChangerSceneSetAction *)action_;
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

void EditorSceneChangerSceneSetPropertyPage::onChanged(Property * property)
{
	PropertyPage::onChanged(property);
	SceneChangerSceneSetAction * action = (SceneChangerSceneSetAction *)action_;

	int index = scene_->value();
	if (index >= 0)
		action->setScene(scenesList_[index]);

	action->setMode((SceneChangerSceneSetAction::ChangeMode)mode_->value());
	action->setDirection((SceneChangerSceneSetAction::Direction)direction_->value());	

	update();
}

bool EditorSceneChangerSceneSetPropertyPage::isValid() const
{
	return 
		(mode_->value() == (int)SceneChangerSceneSetAction::Specific && scene_->value() >= 0) ||
		(mode_->value() == (int)SceneChangerSceneSetAction::Adjacent && direction_->value() >= 0);
}