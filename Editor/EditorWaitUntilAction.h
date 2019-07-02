#pragma once
#include "EditorAction.h"
#include "PropertyPage.h"

class EditorWaitUntilAction : public EditorAction
{
public:	
	virtual EditorAction * clone() const
	{
		return new EditorWaitUntilAction(*this);
	}
	virtual ActionPropertyPage * propertyPage() const;
	virtual boost::uuids::uuid actionType() const;

	virtual QString toString(Action * action) const;
};

///////////////////////////////////////////////////////////////////////////////

class EditorWaitUntilActionPropertyPage : public ActionPropertyPage
{
public:
	EditorWaitUntilActionPropertyPage();
	~EditorWaitUntilActionPropertyPage();
	
	virtual void setDocument(EditorDocument * document);

	virtual bool isValid() const;

public slots:
	virtual void update();
	virtual void onChanged(Property * property);

private:
	EditProperty * waitUntilTime_;
	QDoubleValidator validator_;

};