#include "stdafx.h"
#include "EditorSetOpacityAction.h"
#include "SetOpacityAction.h"
#include "Utils.h"
#include "SceneObject.h"

ActionPropertyPage * EditorSetOpacityAction::propertyPage() const
{
	return new EditorSetOpacityActionPropertyPage();
}

boost::uuids::uuid EditorSetOpacityAction::actionType() const 
{
	return SetOpacityAction().type();
}

QString EditorSetOpacityAction::toString(Action * paction) const
{
	SetOpacityAction * action = (SetOpacityAction *)paction;
	QString typeStr = QString::fromLocal8Bit(action->typeStr());
	QString str;
	str.sprintf("%.4f", action->opacity());

	return typeStr + " - " + stdStringToQString(action->targetObject()->id()) + " - " + str;
	
}

///////////////////////////////////////////////////////////////////////////////
EditorSetOpacityActionPropertyPage::EditorSetOpacityActionPropertyPage() :
validator_(0, 1, 4, 0)
{	
	setGroupBox(false);	
	
	opacity_ = new EditProperty(tr("opacity"), &validator_);		
	addProperty(opacity_, PropertyPage::Vertical);
	
}

EditorSetOpacityActionPropertyPage::~EditorSetOpacityActionPropertyPage()
{
}

void EditorSetOpacityActionPropertyPage::setDocument(EditorDocument * document)
{
	ActionPropertyPage::setDocument(document);		
}

void EditorSetOpacityActionPropertyPage::update()
{
	SetOpacityAction * action = (SetOpacityAction *)action_;
	QString str;
	str.sprintf("%.4f", action->opacity());
	opacity_->setValue(str);

}

void EditorSetOpacityActionPropertyPage::onChanged(Property * property)
{
	PropertyPage::onChanged(property);

	SetOpacityAction * action = (SetOpacityAction *)action_;
	action->setOpacity(opacity_->value().toFloat());
	update();
}

bool EditorSetOpacityActionPropertyPage::isValid() const
{
	return !opacity_->value().isEmpty();
}