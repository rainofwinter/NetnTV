#include "stdafx.h"
#include "EditorShowOnlyAction.h"
#include "ShowOnlyAction.h"
#include "Animation.h"
#include "Scene.h"
#include "SceneObject.h"
#include "Group.h"
#include "EditorDocument.h"
#include "Utils.h"

ActionPropertyPage * EditorShowOnlyAction::propertyPage() const
{
	return new EditorShowOnlyActionPropertyPage();
}

boost::uuids::uuid EditorShowOnlyAction::actionType() const 
{
	return ShowOnlyAction().type();
}

QString EditorShowOnlyAction::toString(Action * action) const
{
	ShowOnlyAction * showOnlyAction = (ShowOnlyAction *)action;
	QString typeStr = QString::fromLocal8Bit(showOnlyAction->typeStr());
	QString objName = QString::fromLocal8Bit(showOnlyAction->showObject()->id().c_str());
	QString grpName = QString::fromLocal8Bit(showOnlyAction->targetObject()->id().c_str());
	return typeStr + " - " + grpName + " : " + objName;
	
}

///////////////////////////////////////////////////////////////////////////////
EditorShowOnlyActionPropertyPage::EditorShowOnlyActionPropertyPage()
{	
	setGroupBox(false);
	
	showObject_ = new ComboBoxProperty(tr("object to show"));

	addProperty(showObject_, PropertyPage::Vertical);
	
}

EditorShowOnlyActionPropertyPage::~EditorShowOnlyActionPropertyPage()
{
}

void EditorShowOnlyActionPropertyPage::setDocument(EditorDocument * document)
{
	ActionPropertyPage::setDocument(document);

	ShowOnlyAction * action = (ShowOnlyAction *)action_;

	Scene * scene = document_->selectedScene();
	


	showObject_->clearItems();
	BOOST_REVERSE_FOREACH(const SceneObjectSPtr & child, 
		action->targetObject()->children())
	{
		showObject_->addItem(stdStringToQString(child->id()));	
	}
}

void EditorShowOnlyActionPropertyPage::update()
{
	ShowOnlyAction * action = (ShowOnlyAction *)action_;
	SceneObject * obj = action->showObject();
	Group * targetObj = action->targetObject()->group();

	if (obj)
	{
		int i = 0;
		int numChildren = (int)targetObj->children().size();
		for (i = 0; i < (int)targetObj->children().size(); ++i)		
		{
			
			if (targetObj->children()[numChildren - i - 1].get() == obj) break;
		}	
		showObject_->setValue(i);	
	}	
	else
	{
		showObject_->setValue(-1);
	}
}

void EditorShowOnlyActionPropertyPage::onChanged(Property * property)
{
	PropertyPage::onChanged(property);

	ShowOnlyAction * action = (ShowOnlyAction *)action_;
	SceneObject * obj = action->showObject();
	Group * targetObj = action->targetObject()->group();

	int index = showObject_->value();

	if (index >= 0)
	{
		int numChildren = (int)targetObj->children().size();
		action->setShowObject(targetObj->children()[numChildren - index - 1].get());
	}
	update();
}

bool EditorShowOnlyActionPropertyPage::isValid() const
{
	return showObject_->value() >= 0;
}