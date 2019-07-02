#include "stdafx.h"
#include "SceneObject.h"
#include "EditorImageAnimationResumeAction.h"
#include "ImageAnimationResumeAction.h"
#include "Utils.h"

using namespace std;

ActionPropertyPage * EditorImageAnimationResumeAction::propertyPage() const
{
	return new EditorImageAnimationResumePropertyPage();
}

boost::uuids::uuid EditorImageAnimationResumeAction::actionType() const 
{
	return ImageAnimationResumeAction().type();
}

QString EditorImageAnimationResumeAction::toString(Action * pAction) const
{
	ImageAnimationResumeAction * action = (ImageAnimationResumeAction *)pAction;

	QString typeStr =  
		QString::fromLocal8Bit(action->typeStr());
	return typeStr;
}

///////////////////////////////////////////////////////////////////////////////
EditorImageAnimationResumePropertyPage::EditorImageAnimationResumePropertyPage()
{	
	setGroupBox(false);
}

EditorImageAnimationResumePropertyPage::~EditorImageAnimationResumePropertyPage()
{
}

void EditorImageAnimationResumePropertyPage::setDocument(EditorDocument * document)
{
	ActionPropertyPage::setDocument(document);	

}

void EditorImageAnimationResumePropertyPage::update()
{
	ImageAnimationResumeAction * action = (ImageAnimationResumeAction *)action_;
}

void EditorImageAnimationResumePropertyPage::onChanged(Property * property)
{
	PropertyPage::onChanged(property);
	ImageAnimationResumeAction * action = (ImageAnimationResumeAction *)action_;

	update();
}

bool EditorImageAnimationResumePropertyPage::isValid() const
{
	return true;
}