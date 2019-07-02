#include "stdafx.h"
#include "SceneObject.h"
#include "EditorImageAnimationPauseAction.h"
#include "ImageAnimationPauseAction.h"
#include "Utils.h"

using namespace std;

ActionPropertyPage * EditorImageAnimationPauseAction::propertyPage() const
{
	return new EditorImageAnimationPausePropertyPage();
}

boost::uuids::uuid EditorImageAnimationPauseAction::actionType() const 
{
	return ImageAnimationPauseAction().type();
}

QString EditorImageAnimationPauseAction::toString(Action * pAction) const
{
	ImageAnimationPauseAction * action = (ImageAnimationPauseAction *)pAction;

	QString typeStr =  
		QString::fromLocal8Bit(action->typeStr());
	return typeStr;
}

///////////////////////////////////////////////////////////////////////////////
EditorImageAnimationPausePropertyPage::EditorImageAnimationPausePropertyPage()
{	
	setGroupBox(false);
}

EditorImageAnimationPausePropertyPage::~EditorImageAnimationPausePropertyPage()
{
}

void EditorImageAnimationPausePropertyPage::setDocument(EditorDocument * document)
{
	ActionPropertyPage::setDocument(document);	

}

void EditorImageAnimationPausePropertyPage::update()
{
	ImageAnimationPauseAction * action = (ImageAnimationPauseAction *)action_;
}

void EditorImageAnimationPausePropertyPage::onChanged(Property * property)
{
	PropertyPage::onChanged(property);
	ImageAnimationPauseAction * action = (ImageAnimationPauseAction *)action_;

	update();
}

bool EditorImageAnimationPausePropertyPage::isValid() const
{
	return true;
}