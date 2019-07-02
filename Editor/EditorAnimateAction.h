#pragma once
#include "EditorAction.h"
#include "PropertyPage.h"

class EditorAnimateAction : public EditorAction
{
public:	
	virtual EditorAction * clone() const
	{
		return new EditorAnimateAction(*this);
	}
	virtual ActionPropertyPage * propertyPage() const;
	virtual boost::uuids::uuid actionType() const;

	virtual QString toString(Action * action) const;
};

///////////////////////////////////////////////////////////////////////////////

class EditorAnimateActionPropertyPage : public ActionPropertyPage
{
public:
	EditorAnimateActionPropertyPage();
	~EditorAnimateActionPropertyPage();
	
	virtual void setDocument(EditorDocument * document);

	virtual bool isValid() const;

public slots:
	virtual void update();
	virtual void onChanged(Property * property);

private:
	ComboBoxProperty * animation_;

};