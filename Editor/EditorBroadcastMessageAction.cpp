#include "stdafx.h"
#include "EditorBroadcastMessageAction.h"
#include "BroadcastMessageAction.h"
#include "Scene.h"
#include "EditorDocument.h"
#include "Utils.h"

ActionPropertyPage * EditorBroadcastMessageAction::propertyPage() const
{
	return new EditorBroadcastMessageActionPropertyPage();
}

boost::uuids::uuid EditorBroadcastMessageAction::actionType() const 
{
	return BroadcastMessageAction().type();
}

QString EditorBroadcastMessageAction::toString(Action * paction) const
{
	BroadcastMessageAction * action = (BroadcastMessageAction *)paction;
	QString typeStr = QString::fromLocal8Bit(action->typeStr());
	return typeStr + " - " + stdStringToQString(action->message());
	
}

///////////////////////////////////////////////////////////////////////////////
EditorBroadcastMessageActionPropertyPage::EditorBroadcastMessageActionPropertyPage()
{	
	setGroupBox(false);

	startGroup(tr("target"));
	targetType_ = new ComboBoxProperty(tr("target type"));
	targetType_->addItem(tr("Scene"));
	targetType_->addItem(tr("External"));
	addProperty(targetType_, PropertyPage::Vertical);
	addSpacing(4);

	scene_ = new ComboBoxProperty(tr("scene"));
	addProperty(scene_, PropertyPage::Vertical);
	

	endGroup();

	addSpacing(4);
	
	msg_ = new EditProperty(tr("message"));
	addProperty(msg_, PropertyPage::Vertical);
	
}

EditorBroadcastMessageActionPropertyPage::~EditorBroadcastMessageActionPropertyPage()
{
}

void EditorBroadcastMessageActionPropertyPage::setDocument(EditorDocument * document)
{
	ActionPropertyPage::setDocument(document);	

	scene_->clearItems();
	BOOST_FOREACH(const SceneSPtr & scene, document->scenes())
		scene_->addItem(stdStringToQString(scene->name()));
}

void EditorBroadcastMessageActionPropertyPage::update()
{
	BroadcastMessageAction * action = (BroadcastMessageAction *)action_;
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

	BroadcastMessageAction::TargetType type = action->targetType();
	scene_->setEnabled(type == BroadcastMessageAction::SceneTarget);
	targetType_->setValue((int)type);

	msg_->setValue(stdStringToQString(action->message()));
	
}

void EditorBroadcastMessageActionPropertyPage::onChanged(Property * property)
{
	PropertyPage::onChanged(property);

	BroadcastMessageAction * action = (BroadcastMessageAction *)action_;	

	if (property == msg_)
	{
		action->setMessage(qStringToStdString(msg_->value()));
	}
	
	if (property == scene_)
	{
		int index = scene_->value();
		if (index >= 0)
			action->setScene(document_->scenes()[index].get());
	}

	if (property == targetType_)
	{
		BroadcastMessageAction::TargetType type = 
			(BroadcastMessageAction::TargetType)targetType_->value();		
		action->setTargetType(type);
	}

	update();
}


bool EditorBroadcastMessageActionPropertyPage::isValid() const
{

	BroadcastMessageAction::TargetType type = 
		(BroadcastMessageAction::TargetType)targetType_->value();

	bool validTarget = type == BroadcastMessageAction::ExternalTarget ||
		scene_->value() > -1;

	return !msg_->value().isEmpty() && validTarget;
}