#include "stdafx.h"
#include "EditorShowAllAction.h"
#include "ShowAllAction.h"

ActionPropertyPage * EditorShowAllAction::propertyPage() const
{
	return new EditorShowAllActionPropertyPage();
}

boost::uuids::uuid EditorShowAllAction::actionType() const 
{
	return ShowAllAction().type();
}

QString EditorShowAllAction::toString(Action * action) const
{
	ShowAllAction * showOnlyAction = (ShowAllAction *)action;
	QString typeStr = QString::fromLocal8Bit(showOnlyAction->typeStr());
	QString grpName = QString::fromLocal8Bit(showOnlyAction->targetObject()->id().c_str());


	QString str;
	if (showOnlyAction->visible())
		str = QObject::tr("show");
	else
		str = QObject::tr("hide");

	return typeStr + " - " + grpName + " - " + str;
	
}

///////////////////////////////////////////////////////////////////////////////
EditorShowAllActionPropertyPage::EditorShowAllActionPropertyPage()
{	
	setGroupBox(false);
	visibility_ = new BoolProperty(tr("visible"));		
	addProperty(visibility_, PropertyPage::Vertical);
}

EditorShowAllActionPropertyPage::~EditorShowAllActionPropertyPage()
{
}

void EditorShowAllActionPropertyPage::setDocument(EditorDocument * document)
{
	ActionPropertyPage::setDocument(document);	
}

void EditorShowAllActionPropertyPage::update()
{
	ShowAllAction * action = (ShowAllAction *)action_;
	visibility_->setValue(action->visible());
}

void EditorShowAllActionPropertyPage::onChanged(Property * property)
{
	PropertyPage::onChanged(property);

	ShowAllAction * action = (ShowAllAction *)action_;
	action->setVisible(visibility_->value());
	update();
}

bool EditorShowAllActionPropertyPage::isValid() const
{
	return true;
}