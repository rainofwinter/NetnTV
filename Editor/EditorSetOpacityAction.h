#pragma once
#include "EditorAction.h"
#include "PropertyPage.h"

class EditorSetOpacityAction : public EditorAction
{
public:	
	virtual EditorAction * clone() const
	{
		return new EditorSetOpacityAction(*this);
	}
	virtual ActionPropertyPage * propertyPage() const;
	virtual boost::uuids::uuid actionType() const;

	virtual QString toString(Action * action) const;
};

///////////////////////////////////////////////////////////////////////////////

class EditorSetOpacityActionPropertyPage : public ActionPropertyPage
{
public:
	EditorSetOpacityActionPropertyPage();
	~EditorSetOpacityActionPropertyPage();
	
	virtual void setDocument(EditorDocument * document);

	virtual bool isValid() const;


public slots:
	virtual void update();
	virtual void onChanged(Property * property);

private:
	EditProperty * opacity_;
	QDoubleValidator validator_;
};