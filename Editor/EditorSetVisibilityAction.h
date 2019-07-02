#pragma once
#include "EditorAction.h"
#include "PropertyPage.h"

class EditorSetVisibilityAction : public EditorAction
{
public:	
	virtual EditorAction * clone() const
	{
		return new EditorSetVisibilityAction(*this);
	}
	virtual ActionPropertyPage * propertyPage() const;
	virtual boost::uuids::uuid actionType() const;

	virtual QString toString(Action * action) const;
};

///////////////////////////////////////////////////////////////////////////////

class EditorSetVisibilityActionPropertyPage : public ActionPropertyPage
{
public:
	EditorSetVisibilityActionPropertyPage();
	~EditorSetVisibilityActionPropertyPage();
	
	virtual void setDocument(EditorDocument * document);

	virtual bool isValid() const;


public slots:
	virtual void update();
	virtual void onChanged(Property * property);

private:
	BoolProperty * visibility_;
};