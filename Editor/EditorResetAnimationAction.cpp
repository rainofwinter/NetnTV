#include "stdafx.h"
#include "EditorResetAnimationAction.h"
#include "ResetAnimationAction.h"
#include "Animation.h"
#include "Scene.h"
#include "EditorDocument.h"
#include "Utils.h"

ActionPropertyPage * EditorResetAnimationAction::propertyPage() const
{
	return new EditorResetAnimationActionPropertyPage();
}

boost::uuids::uuid EditorResetAnimationAction::actionType() const 
{
	return ResetAnimationAction().type();
}

QString EditorResetAnimationAction::toString(Action * action) const
{
	ResetAnimationAction * animAction = (ResetAnimationAction *)action;
	QString typeStr = QString::fromLocal8Bit(animAction->typeStr());
	QString animName;
	if (animAction->animation())
		animName = stdStringToQString(animAction->animation()->name());
	else
		animName = QString("ALL");
	return typeStr + " - " + animName;
	
}

///////////////////////////////////////////////////////////////////////////////
EditorResetAnimationActionPropertyPage::EditorResetAnimationActionPropertyPage()
{	
	setGroupBox(false);
	
	animation_ = new ComboBoxProperty(tr("animation"));

	addProperty(animation_, PropertyPage::Vertical);
	addSpacing(4);

	resetType_ = new ComboBoxProperty(tr("target type"));
	resetType_->addItem(tr("Any"));
	resetType_->addItem(tr("All"));
	addProperty(resetType_, PropertyPage::Vertical);
	
}

EditorResetAnimationActionPropertyPage::~EditorResetAnimationActionPropertyPage()
{
}

void EditorResetAnimationActionPropertyPage::setDocument(EditorDocument * document)
{
	ActionPropertyPage::setDocument(document);

	Scene * scene = document_->selectedScene();
	
	animation_->clearItems();
	BOOST_FOREACH(const AnimationSPtr & anim, scene->animations())
		animation_->addItem(stdStringToQString(anim->name()));		
}

void EditorResetAnimationActionPropertyPage::update()
{
	ResetAnimationAction * action = (ResetAnimationAction *)action_;
	Animation * anim = action->animation();
	Scene * scene = document_->selectedScene();

	animation_->setValue(-1);
	if (anim)
	{
		int i = 0;
		BOOST_FOREACH(const AnimationSPtr & curAnim, scene->animations())
		{
			if (curAnim.get() == anim)
			{			
				break;
			}
			++i;
		}	
		animation_->setValue(i);	
	}

	ResetAnimationAction::ResetType type = action->resetType();
	animation_->setEnabled(type == ResetAnimationAction::Any);
	resetType_->setValue((int)type);
}

void EditorResetAnimationActionPropertyPage::onChanged(Property * property)
{
	PropertyPage::onChanged(property);

	ResetAnimationAction * action = (ResetAnimationAction *)action_;
	Scene * scene = document_->selectedScene();

	if (property == animation_)
	{
		int index = animation_->value();

		if (index >= 0)
			action->setAnimation(scene->animations()[index].get());
	}

	if (property == resetType_)
	{
		ResetAnimationAction::ResetType type = 
			(ResetAnimationAction::ResetType)resetType_->value();		
		action->setResetType(type);
	}
	update();
}

bool EditorResetAnimationActionPropertyPage::isValid() const
{
	ResetAnimationAction::ResetType type = 
		(ResetAnimationAction::ResetType)resetType_->value();

	bool validReset = type == ResetAnimationAction::All;

	return animation_->value() >= 0 || validReset;
}