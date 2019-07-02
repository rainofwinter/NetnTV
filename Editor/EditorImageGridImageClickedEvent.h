#pragma once
#include "EditorEvent.h"
#include "PropertyPage.h"

class EditorImageGridImageClickedEvent : public EditorEvent
{
public:	
	virtual EditorEvent * clone() const
	{
		return new EditorImageGridImageClickedEvent(*this);
	}
	virtual EventPropertyPage * propertyPage() const;
	virtual boost::uuids::uuid eventType() const;

	virtual QString toString(Event * event) const;
};

///////////////////////////////////////////////////////////////////////////////

class EditorImageGridImageClickedEventPropertyPage : public EventPropertyPage
{
public:
	EditorImageGridImageClickedEventPropertyPage();
	~EditorImageGridImageClickedEventPropertyPage();
	
	virtual void setDocument(EditorDocument * document);

	virtual bool isValid() const;

public slots:
	virtual void update();
	virtual void onChanged(Property * property);

private:
	EditProperty * index_;
	QIntValidator validator_;

};