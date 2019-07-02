#pragma once
#include "EditorEvent.h"
#include "PropertyPage.h"

class EditorAudioPlayTimeEvent : public EditorEvent
{
public:	
	virtual EditorEvent * clone() const
	{
		return new EditorAudioPlayTimeEvent(*this);
	}
	virtual EventPropertyPage * propertyPage() const;
	virtual boost::uuids::uuid eventType() const;

	virtual QString toString(Event * event) const;
};

///////////////////////////////////////////////////////////////////////////////

class EditorAudioPlayTimeEventPropertyPage : public EventPropertyPage
{
public:
	EditorAudioPlayTimeEventPropertyPage();
	~EditorAudioPlayTimeEventPropertyPage();
	
	virtual void setDocument(EditorDocument * document);

	virtual bool isValid() const;

public slots:
	virtual void update();
	virtual void onChanged(Property * property);

private:
	EditProperty * time_;
	QDoubleValidator validator_;

};