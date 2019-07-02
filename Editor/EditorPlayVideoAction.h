#pragma once
#include "EditorAction.h"
#include "PropertyPage.h"
#include "SourceType.h"

class EditorPlayVideoAction : public EditorAction
{
public:	
	virtual EditorAction * clone() const
	{
		return new EditorPlayVideoAction(*this);
	}
	virtual ActionPropertyPage * propertyPage() const;
	virtual boost::uuids::uuid actionType() const;

	virtual QString toString(Action * action) const;
};

///////////////////////////////////////////////////////////////////////////////

class EditorPlayVideoActionPropertyPage : public ActionPropertyPage
{
public:
	EditorPlayVideoActionPropertyPage();
	~EditorPlayVideoActionPropertyPage();
	
	virtual void setDocument(EditorDocument * document);

	virtual bool isValid() const;

private:
	void setRegionFieldsEnabled(bool enabled);

	void setSourceFieldsEnabled(SourceType souceType);

public slots:
	virtual void update();
	virtual void onChanged(Property * property);

private:
	
	
	ComboBoxProperty * videoType_;

	FileNameProperty * videoFileName_;
	EditProperty * url_;
	ComboBoxProperty * displayMode_;

	EditProperty * x_;
	EditProperty * y_;
	EditProperty * width_;
	EditProperty * height_;

	BoolProperty * showPlaybackControls_;

	QIntValidator validator_;
	EditProperty * startTime_;
	QDoubleValidator startTimeValidator_;
};