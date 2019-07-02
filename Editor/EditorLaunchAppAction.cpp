#include "stdafx.h"
#include "EditorLaunchAppAction.h"
#include "LaunchAppAction.h"
#include "Animation.h"
#include "Scene.h"
#include "EditorDocument.h"
#include "Utils.h"

ActionPropertyPage * EditorLaunchAppAction::propertyPage() const
{
	return new EditorLaunchAppActionPropertyPage();
}

boost::uuids::uuid EditorLaunchAppAction::actionType() const 
{
	return LaunchAppAction().type();
}

QString EditorLaunchAppAction::toString(Action * paction) const
{
	LaunchAppAction * action = (LaunchAppAction *)paction;
	QString typeStr = stdStringToQString(action->typeStr());
	return typeStr;
	
}

///////////////////////////////////////////////////////////////////////////////
EditorLaunchAppActionPropertyPage::EditorLaunchAppActionPropertyPage()
{	
	setGroupBox(false);
	
	iosParams_ = new EditProperty(tr("IOS Parameters"));
	addProperty(iosParams_, PropertyPage::Vertical);

	addSpacing(4);

	androidParams_ = new EditProperty(tr("Android Parameters"));
	addProperty(androidParams_, PropertyPage::Vertical);
	
}

EditorLaunchAppActionPropertyPage::~EditorLaunchAppActionPropertyPage()
{
}

void EditorLaunchAppActionPropertyPage::setDocument(EditorDocument * document)
{
	ActionPropertyPage::setDocument(document);		
}

void EditorLaunchAppActionPropertyPage::update()
{
	LaunchAppAction * action = (LaunchAppAction *)action_;
	iosParams_->setValue(stdStringToQString(action->iosParams()));
	androidParams_->setValue(stdStringToQString(action->androidParams()));	
}

void EditorLaunchAppActionPropertyPage::onChanged(Property * property)
{
	PropertyPage::onChanged(property);

	LaunchAppAction * action = (LaunchAppAction *)action_;
	action->setIosParams(qStringToStdString(iosParams_->value()));
	action->setAndroidParams(qStringToStdString(androidParams_->value()));
	update();
}

bool EditorLaunchAppActionPropertyPage::isValid() const
{
	return !iosParams_->value().isEmpty() || !androidParams_->value().isEmpty();
}