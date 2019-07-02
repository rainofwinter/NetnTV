#pragma once
#include "EditorAction.h"
#include "PropertyPage.h"

class EditorResetAnimationAction : public EditorAction
{
public:	
	virtual EditorAction * clone() const
	{
		return new EditorResetAnimationAction(*this);
	}
	virtual ActionPropertyPage * propertyPage() const;
	virtual boost::uuids::uuid actionType() const;

	virtual QString toString(Action * action) const;
};

///////////////////////////////////////////////////////////////////////////////

class EditorResetAnimationActionPropertyPage : public ActionPropertyPage
{
public:
	EditorResetAnimationActionPropertyPage();
	~EditorResetAnimationActionPropertyPage();
	
	virtual void setDocument(EditorDocument * document);
	virtual bool isValid() const;

public slots:
	virtual void update();
	virtual void onChanged(Property * property);

private:
	ComboBoxProperty * animation_;

	ComboBoxProperty * resetType_;

};