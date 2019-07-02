#pragma once
#include "EditorAction.h"
#include "PropertyPage.h"

class EditorShowOnlyAction : public EditorAction
{
public:	
	virtual EditorAction * clone() const
	{
		return new EditorShowOnlyAction(*this);
	}
	virtual ActionPropertyPage * propertyPage() const;
	virtual boost::uuids::uuid actionType() const;

	virtual QString toString(Action * action) const;
};

///////////////////////////////////////////////////////////////////////////////

class EditorShowOnlyActionPropertyPage : public ActionPropertyPage
{
public:
	EditorShowOnlyActionPropertyPage();
	~EditorShowOnlyActionPropertyPage();
	
	virtual void setDocument(EditorDocument * document);

	virtual bool isValid() const;

public slots:
	virtual void update();
	virtual void onChanged(Property * property);

private:
	ComboBoxProperty * showObject_;

};