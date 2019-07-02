#include "stdafx.h"
#include "EditorResumeAnimationAction.h"
#include "ResumeAnimationAction.h"
#include "Animation.h"
#include "Scene.h"
#include "EditorDocument.h"
#include "Utils.h"

ActionPropertyPage * EditorResumeAnimationAction::propertyPage() const
{
	return new EditorResumeAnimationActionPropertyPage();
}

boost::uuids::uuid EditorResumeAnimationAction::actionType() const 
{
	return ResumeAnimationAction().type();
}

QString EditorResumeAnimationAction::toString(Action * action) const
{
	ResumeAnimationAction * animAction = (ResumeAnimationAction *)action;
	QString typeStr = stdStringToQString(animAction->typeStr());
	QString animName = stdStringToQString(animAction->animation()->name());
	return typeStr + " - " + animName;
	
}

///////////////////////////////////////////////////////////////////////////////
EditorResumeAnimationActionPropertyPage::EditorResumeAnimationActionPropertyPage()
{	
	setGroupBox(false);
	
	animation_ = new ComboBoxProperty(tr("animation"));

	addProperty(animation_, PropertyPage::Vertical);
	
}

EditorResumeAnimationActionPropertyPage::~EditorResumeAnimationActionPropertyPage()
{
}

void EditorResumeAnimationActionPropertyPage::setDocument(EditorDocument * document)
{
	ActionPropertyPage::setDocument(document);

	Scene * scene = document_->selectedScene();
	
	animation_->clearItems();
	BOOST_FOREACH(const AnimationSPtr & anim, scene->animations())
		animation_->addItem(stdStringToQString(anim->name()));		
}

void EditorResumeAnimationActionPropertyPage::update()
{
	ResumeAnimationAction * action = (ResumeAnimationAction *)action_;
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

void EditorResumeAnimationActionPropertyPage::onChanged(Property * property)
{
	PropertyPage::onChanged(property);

	ResumeAnimationAction * action = (ResumeAnimationAction *)action_;
	Scene * scene = document_->selectedScene();

	int index = animation_->value();

	if (index >= 0)
		action->setAnimation(scene->animations()[index].get());
	update();
}

bool EditorResumeAnimationActionPropertyPage::isValid() const
{
	return animation_->value() >= 0;
}