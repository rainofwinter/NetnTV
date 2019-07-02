#pragma once
#include "EditorAction.h"
#include "PropertyPage.h"

class EditorStopAnimationAction : public EditorAction
{
public:	
	virtual EditorAction * clone() const
	{
		return new EditorStopAnimationAction(*this);
	}
	virtual ActionPropertyPage * propertyPage() const;
	virtual boost::uuids::uuid actionType() const;

	virtual QString toString(Action * action) const;
};

///////////////////////////////////////////////////////////////////////////////

class EditorStopAnimationActionPropertyPage : public ActionPropertyPage
{
public:
	EditorStopAnimationActionPropertyPage();
	~EditorStopAnimationActionPropertyPage();
	
	virtual void setDocument(EditorDocument * document);

	virtual bool isValid() const;

public slots:
	virtual void update();
	virtual void onChanged(Property * property);

private:
	ComboBoxProperty * animation_;

};