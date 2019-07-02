#pragma once
#include "EditorEvent.h"
#include "PropertyPage.h"

class EditorVideoPlayTimeEvent : public EditorEvent
{
public:	
	virtual EditorEvent * clone() const
	{
		return new EditorVideoPlayTimeEvent(*this);
	}
	virtual EventPropertyPage * propertyPage() const;
	virtual boost::uuids::uuid eventType() const;

	virtual QString toString(Event * event) const;
};

///////////////////////////////////////////////////////////////////////////////

class EditorVideoPlayTimeEventPropertyPage : public EventPropertyPage
{
public:
	EditorVideoPlayTimeEventPropertyPage();
	~EditorVideoPlayTimeEventPropertyPage();
	
	virtual void setDocument(EditorDocument * document);

	virtual bool isValid() const;

public slots:
	virtual void update();
	virtual void onChanged(Property * property);

private:
	EditProperty * time_;
	QDoubleValidator validator_;

};