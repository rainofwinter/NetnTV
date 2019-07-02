#pragma once
#include "EditorAction.h"
#include "PropertyPage.h"

class Document;

class EditorAudioPlayAction : public EditorAction
{
public:	
	virtual EditorAction * clone() const
	{
		return new EditorAudioPlayAction(*this);
	}
	virtual ActionPropertyPage * propertyPage() const;
	virtual boost::uuids::uuid actionType() const;

	virtual QString toString(Action * action) const;
};

///////////////////////////////////////////////////////////////////////////////

class EditorAudioPlayPropertyPage : public ActionPropertyPage
{
public:
	EditorAudioPlayPropertyPage();
	~EditorAudioPlayPropertyPage();

	virtual void setDocument(EditorDocument * document);

	virtual bool isValid() const;

	public slots:
		virtual void update();
		virtual void onChanged(Property * property);

private:
	EditProperty * startTime_;
	QDoubleValidator startTimeValidator_;
};