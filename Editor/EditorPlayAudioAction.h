#pragma once
#include "EditorAction.h"
#include "PropertyPage.h"
#include "SourceType.h"

class EditorPlayAudioAction : public EditorAction
{
public:	
	virtual EditorAction * clone() const
	{
		return new EditorPlayAudioAction(*this);
	}
	virtual ActionPropertyPage * propertyPage() const;
	virtual boost::uuids::uuid actionType() const;

	virtual QString toString(Action * action) const;
};

///////////////////////////////////////////////////////////////////////////////

class EditorPlayAudioActionPropertyPage : public ActionPropertyPage
{
public:
	EditorPlayAudioActionPropertyPage();
	~EditorPlayAudioActionPropertyPage();
	
	virtual void setDocument(EditorDocument * document);

	virtual bool isValid() const;


public slots:
	virtual void update();
	virtual void onChanged(Property * property);

private:
	
	FileNameProperty * videoFileName_;
	EditProperty * startTime_;
	QDoubleValidator validator_;
};