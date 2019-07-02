#include "stdafx.h"
#include "EditorSetVisibilityAction.h"
#include "SetVisibilityAction.h"
#include "Utils.h"
#include "SceneObject.h"

ActionPropertyPage * EditorSetVisibilityAction::propertyPage() const
{
	return new EditorSetVisibilityActionPropertyPage();
}

boost::uuids::uuid EditorSetVisibilityAction::actionType() const 
{
	return SetVisibilityAction().type();
}

QString EditorSetVisibilityAction::toString(Action * paction) const
{
	SetVisibilityAction * action = (SetVisibilityAction *)paction;
	QString typeStr = QString::fromLocal8Bit(action->typeStr());
	QString str;
	if (action->visibility())
		str = QObject::tr("true");
	else
		str = QObject::tr("false");
	
	return typeStr + " - " + stdStringToQString(action->targetObject()->id()) + " - " + str;
	
}

///////////////////////////////////////////////////////////////////////////////
EditorSetVisibilityActionPropertyPage::EditorSetVisibilityActionPropertyPage()
{	
	setGroupBox(false);	
	
	visibility_ = new BoolProperty(tr("visibility"));		
	addProperty(visibility_, PropertyPage::Vertical);
	
}

EditorSetVisibilityActionPropertyPage::~EditorSetVisibilityActionPropertyPage()
{
}

void EditorSetVisibilityActionPropertyPage::setDocument(EditorDocument * document)
{
	ActionPropertyPage::setDocument(document);		
}

void EditorSetVisibilityActionPropertyPage::update()
{
	SetVisibilityAction * action = (SetVisibilityAction *)action_;
	visibility_->setValue(action->visibility());

}

void EditorSetVisibilityActionPropertyPage::onChanged(Property * property)
{
	PropertyPage::onChanged(property);

	SetVisibilityAction * action = (SetVisibilityAction *)action_;
	action->setVisibility(visibility_->value());
	update();
}

bool EditorSetVisibilityActionPropertyPage::isValid() const
{
	return true;
}