#pragma once
#include "Action.h"

class EditorScene;
class ActionPropertyPage;

class EditorAction
{
public:	
	EditorAction() {action_ = 0;}
	virtual ~EditorAction() {}
	virtual EditorAction * clone() const = 0;
	virtual ActionPropertyPage * propertyPage() const {return 0;}
	virtual boost::uuids::uuid actionType() const = 0;
	virtual QString toString(Action * action) const
	{
		return QString::fromLocal8Bit(action->typeStr());
	}

	void setAction(Action * action) {action_ = action;}
	Action * action() const {return action_;}

	void setScene(EditorScene * scene) {parentScene_ = scene;}
	EditorScene * parentScene() const {return parentScene_;}
protected:
	Action * action_;
	EditorScene * parentScene_;
};