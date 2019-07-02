#pragma once
#include "EditorAction.h"
#include "PropertyPage.h"

class EditorResumeAnimationAction : public EditorAction
{
public:	
	virtual EditorAction * clone() const
	{
		return new EditorResumeAnimationAction(*this);
	}
	virtual ActionPropertyPage * propertyPage() const;
	virtual boost::uuids::uuid actionType() const;

	virtual QString toString(Action * action) const;
};

///////////////////////////////////////////////////////////////////////////////

class EditorResumeAnimationActionPropertyPage : public ActionPropertyPage
{
public:
	EditorResumeAnimationActionPropertyPage();
	~EditorResumeAnimationActionPropertyPage();
	
	virtual void setDocument(EditorDocument * document);

	virtual bool isValid() const;

public slots:
	virtual void update();
	virtual void onChanged(Property * property);

private:
	ComboBoxProperty * animation_;

};