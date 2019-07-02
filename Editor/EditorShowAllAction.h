#pragma once
#include "EditorAction.h"
#include "PropertyPage.h"

class EditorShowAllAction : public EditorAction
{
public:	
	virtual EditorAction * clone() const
	{
		return new EditorShowAllAction(*this);
	}
	virtual ActionPropertyPage * propertyPage() const;
	virtual boost::uuids::uuid actionType() const;

	virtual QString toString(Action * action) const;
};

///////////////////////////////////////////////////////////////////////////////

class EditorShowAllActionPropertyPage : public ActionPropertyPage
{
public:
	EditorShowAllActionPropertyPage();
	~EditorShowAllActionPropertyPage();
	
	virtual void setDocument(EditorDocument * document);

	virtual bool isValid() const;

public slots:
	virtual void update();
	virtual void onChanged(Property * property);

private:

	BoolProperty * visibility_;
};