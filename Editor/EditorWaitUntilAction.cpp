#include "stdafx.h"
#include "EditorWaitUntilAction.h"
#include "WaitUntilAction.h"

ActionPropertyPage * EditorWaitUntilAction::propertyPage() const
{
	return new EditorWaitUntilActionPropertyPage();
}

boost::uuids::uuid EditorWaitUntilAction::actionType() const 
{
	return WaitUntilAction().type();
}

QString EditorWaitUntilAction::toString(Action * action) const
{
	WaitUntilAction * waitUntilAction = (WaitUntilAction *)action;
	QString typeStr = QString::fromLocal8Bit(waitUntilAction->typeStr());
	QString str;
	str.sprintf("%.4f", waitUntilAction->waitUntilTime());	
	return typeStr + " - " + str;
	
}

///////////////////////////////////////////////////////////////////////////////
EditorWaitUntilActionPropertyPage::EditorWaitUntilActionPropertyPage()
{	
	setGroupBox(false);
	
	waitUntilTime_ = new EditProperty(tr("time"));

	addProperty(waitUntilTime_, PropertyPage::Vertical);
	
}

EditorWaitUntilActionPropertyPage::~EditorWaitUntilActionPropertyPage()
{
}

void EditorWaitUntilActionPropertyPage::setDocument(EditorDocument * document)
{
	ActionPropertyPage::setDocument(document);

	WaitUntilAction * action = (WaitUntilAction *)action_;
}

void EditorWaitUntilActionPropertyPage::update()
{
	WaitUntilAction * action = (WaitUntilAction *)action_;
	QString str;
	str.sprintf("%.4f", action->waitUntilTime());
	waitUntilTime_->setValue(str);
	
}

void EditorWaitUntilActionPropertyPage::onChanged(Property * property)
{
	PropertyPage::onChanged(property);

	WaitUntilAction * action = (WaitUntilAction *)action_;
	
	action->setWaitUntilTime(waitUntilTime_->value().toFloat());

	update();
}

bool EditorWaitUntilActionPropertyPage::isValid() const
{
	return true;
}