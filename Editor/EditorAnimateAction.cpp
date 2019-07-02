#include "stdafx.h"
#include "EditorAnimateAction.h"
#include "PlayAnimationAction.h"
#include "Animation.h"
#include "Scene.h"
#include "EditorDocument.h"
#include "Utils.h"

ActionPropertyPage * EditorAnimateAction::propertyPage() const
{
	return new EditorAnimateActionPropertyPage();
}

boost::uuids::uuid EditorAnimateAction::actionType() const 
{
	return PlayAnimationAction().type();
}

QString EditorAnimateAction::toString(Action * action) const
{
	PlayAnimationAction * animAction = (PlayAnimationAction *)action;
	QString typeStr = stdStringToQString(animAction->typeStr());
	QString animName = stdStringToQString(animAction->animation()->name());
	return typeStr + " - " + animName;
	
}

///////////////////////////////////////////////////////////////////////////////
EditorAnimateActionPropertyPage::EditorAnimateActionPropertyPage()
{	
	setGroupBox(false);
	
	animation_ = new ComboBoxProperty(tr("animation"));

	addProperty(animation_, PropertyPage::Vertical);
	
}

EditorAnimateActionPropertyPage::~EditorAnimateActionPropertyPage()
{
}

void EditorAnimateActionPropertyPage::setDocument(EditorDocument * document)
{
	ActionPropertyPage::setDocument(document);

	Scene * scene = document_->selectedScene();
	
	animation_->clearItems();
	BOOST_FOREACH(const AnimationSPtr & anim, scene->animations())
		animation_->addItem(stdStringToQString(anim->name()));		
}

void EditorAnimateActionPropertyPage::update()
{
	PlayAnimationAction * action = (PlayAnimationAction *)action_;
	Animation * anim = action->animation();
	Scene * scene = document_->selectedScene();

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
	else
		animation_->setValue(-1);
}

void EditorAnimateActionPropertyPage::onChanged(Property * property)
{
	PropertyPage::onChanged(property);

	PlayAnimationAction * action = (PlayAnimationAction *)action_;
	Scene * scene = document_->selectedScene();

	int index = animation_->value();

	if (index >= 0)
		action->setAnimation(scene->animations()[index].get());
	update();
}




bool EditorAnimateActionPropertyPage::isValid() const
{
	return animation_->value() >= 0;
}