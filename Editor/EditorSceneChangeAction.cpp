#include "stdafx.h"
#include "EditorSceneChangeAction.h"
#include "SceneChangeAction.h"
#include "Animation.h"
#include "Scene.h"
#include "EditorDocument.h"
#include "Document.h"

ActionPropertyPage * EditorSceneChangeAction::propertyPage() const
{
	return new EditorSceneChangePropertyPage();
}

boost::uuids::uuid EditorSceneChangeAction::actionType() const 
{
	return SceneChangeAction().type();
}

QString EditorSceneChangeAction::toString(Action * action) const
{
	SceneChangeAction * sceneChangeAction = (SceneChangeAction *)action;
	QString typeStr = QString::fromLocal8Bit(sceneChangeAction->typeStr());

	if (sceneChangeAction->mode() == SceneChangeAction::Specific)
	{
		QString sceneName = QString::fromLocal8Bit(sceneChangeAction->scene()->name().c_str());
		return typeStr + " - " + sceneName;
	}
	else
	{
		if (sceneChangeAction->direction() == SceneChangeAction::Previous)
			return typeStr + " - Previous";
		else
			return typeStr + " - Next";
	}
	
}

///////////////////////////////////////////////////////////////////////////////
EditorSceneChangePropertyPage::EditorSceneChangePropertyPage()
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

void EditorSceneChangePropertyPage::setModeEnabled()
{
	SceneChangeAction::ChangeMode mode = (SceneChangeAction::ChangeMode)mode_->value();
	if (mode == SceneChangeAction::Specific)
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

EditorSceneChangePropertyPage::~EditorSceneChangePropertyPage()
{
}

void EditorSceneChangePropertyPage::setDocument(EditorDocument * document)
{
	ActionPropertyPage::setDocument(document);
	
	scene_->clearItems();
	BOOST_FOREACH(const SceneSPtr & scene, document->scenes())
		scene_->addItem(QString::fromLocal8Bit(scene->name().c_str()));
}

void EditorSceneChangePropertyPage::update()
{
	SceneChangeAction * action = (SceneChangeAction *)action_;
	Scene * scene = action->scene();

	if (scene)
	{
		int i = 0;
		BOOST_FOREACH(const SceneSPtr & curScene, document_->scenes())
		{
			if (curScene.get() == scene) break;
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

void EditorSceneChangePropertyPage::onChanged(Property * property)
{
	PropertyPage::onChanged(property);

	SceneChangeAction * action = (SceneChangeAction *)action_;

	int index = scene_->value();
	if (index >= 0)
		action->setScene(document_->scenes()[index].get());

	action->setMode((SceneChangeAction::ChangeMode)mode_->value());
	action->setDirection((SceneChangeAction::Direction)direction_->value());	

	update();
}

bool EditorSceneChangePropertyPage::isValid() const
{
	return 
		(mode_->value() == (int)SceneChangeAction::Specific && scene_->value() >= 0) ||
		(mode_->value() == (int)SceneChangeAction::Adjacent && direction_->value() >= 0);
}