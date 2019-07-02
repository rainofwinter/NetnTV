#pragma once
#include "EditorAction.h"
#include "PropertyPage.h"

class EditorLaunchAppAction : public EditorAction
{
public:	
	virtual EditorAction * clone() const
	{
		return new EditorLaunchAppAction(*this);
	}
	virtual ActionPropertyPage * propertyPage() const;
	virtual boost::uuids::uuid actionType() const;

	virtual QString toString(Action * action) const;
};

///////////////////////////////////////////////////////////////////////////////

class EditorLaunchAppActionPropertyPage : public ActionPropertyPage
{
public:
	EditorLaunchAppActionPropertyPage();
	~EditorLaunchAppActionPropertyPage();
	
	virtual void setDocument(EditorDocument * document);

	virtual bool isValid() const;

public slots:
	virtual void update();
	virtual void onChanged(Property * property);

private:
	EditProperty * iosParams_;
	EditProperty * androidParams_;	

};