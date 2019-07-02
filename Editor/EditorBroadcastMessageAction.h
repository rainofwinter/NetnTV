#pragma once
#include "EditorAction.h"
#include "PropertyPage.h"

class EditorBroadcastMessageAction : public EditorAction
{
public:	
	virtual EditorAction * clone() const
	{
		return new EditorBroadcastMessageAction(*this);
	}
	virtual ActionPropertyPage * propertyPage() const;
	virtual boost::uuids::uuid actionType() const;

	virtual QString toString(Action * action) const;
};

///////////////////////////////////////////////////////////////////////////////

class EditorBroadcastMessageActionPropertyPage : public ActionPropertyPage
{
public:
	EditorBroadcastMessageActionPropertyPage();
	~EditorBroadcastMessageActionPropertyPage();
	
	virtual void setDocument(EditorDocument * document);

	virtual bool isValid() const;

public slots:
	virtual void update();
	virtual void onChanged(Property * property);

private:
	ComboBoxProperty * scene_;
	EditProperty * msg_;

	ComboBoxProperty * targetType_;
	

};