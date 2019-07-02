#pragma once
#include "EditorEvent.h"
#include "PropertyPage.h"

class EditorReceiveMessageEvent : public EditorEvent
{
public:	
	virtual EditorEvent * clone() const
	{
		return new EditorReceiveMessageEvent(*this);
	}
	virtual EventPropertyPage * propertyPage() const;
	virtual boost::uuids::uuid eventType() const;

	virtual QString toString(Event * event) const;
};

///////////////////////////////////////////////////////////////////////////////

class EditorReceiveMessageEventPropertyPage : public EventPropertyPage
{
public:
	EditorReceiveMessageEventPropertyPage();
	~EditorReceiveMessageEventPropertyPage();
	
	virtual void setDocument(EditorDocument * document);

	virtual bool isValid() const;

public slots:
	virtual void update();
	virtual void onChanged(Property * property);

private:
	EditProperty * message_;

};