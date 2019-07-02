#include "stdafx.h"
#include "EditorStopAnimationAction.h"
#include "StopAnimationAction.h"
#include "Animation.h"
#include "Scene.h"
#include "EditorDocument.h"
#include "Utils.h"

ActionPropertyPage * EditorStopAnimationAction::propertyPage() const
{
	return new EditorStopAnimationActionPropertyPage();
}

boost::uuids::uuid EditorStopAnimationAction::actionType() const 
{
	return StopAnimationAction().type();
}

QString EditorStopAnimationAction::toString(Action * action) const
{
	StopAnimationAction * animAction = (StopAnimationAction *)action;
	QString typeStr = QString::fromLocal8Bit(animAction->typeStr());
	QString animName = stdStringToQString(animAction->animation()->name());
	return typeStr + " - " + animName;
	
}

///////////////////////////////////////////////////////////////////////////////
EditorStopAnimationActionPropertyPage::EditorStopAnimationActionPropertyPage()
{	
	setGroupBox(false);
	
	animation_ = new ComboBoxProperty(tr("animation"));

	addProperty(animation_, PropertyPage::Vertical);
	
}

EditorStopAnimationActionPropertyPage::~EditorStopAnimationActionPropertyPage()
{
}

void EditorStopAnimationActionPropertyPage::setDocument(EditorDocument * document)
{
	ActionPropertyPage::setDocument(document);

	Scene * scene = document_->selectedScene();
	
	animation_->clearItems();
	BOOST_FOREACH(const AnimationSPtr & anim, scene->animations())
		animation_->addItem(stdStringToQString(anim->name()));		
}

void EditorStopAnimationActionPropertyPage::update()
{
	StopAnimationAction * action = (StopAnimationAction *)action_;
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
}

void EditorStopAnimationActionPropertyPage::onChanged(Property * property)
{
	PropertyPage::onChanged(property);

	StopAnimationAction * action = (StopAnimationAction *)action_;
	Scene * scene = document_->selectedScene();

	int index = animation_->value();

	if (index >= 0)
		action->setAnimation(scene->animations()[index].get());
	update();
}

bool EditorStopAnimationActionPropertyPage::isValid() const
{
	return animation_->value() >= 0;
}