#pragma once
#include "EditorAction.h"
#include "PropertyPage.h"

class Document;

class EditorImageChangerShowImageAction : public EditorAction
{
public:	
	virtual EditorAction * clone() const
	{
		return new EditorImageChangerShowImageAction(*this);
	}
	virtual ActionPropertyPage * propertyPage() const;
	virtual boost::uuids::uuid actionType() const;

	virtual QString toString(Action * action) const;
};

///////////////////////////////////////////////////////////////////////////////

class EditorImageChangerShowImagePropertyPage : public ActionPropertyPage
{
public:
	EditorImageChangerShowImagePropertyPage();
	~EditorImageChangerShowImagePropertyPage();
	
	virtual void setDocument(EditorDocument * document);

	virtual bool isValid() const;

public slots:
	virtual void update();
	virtual void onChanged(Property * property);

private:
	void setModeEnabled();
	ComboBoxProperty * mode_;
	ComboBoxProperty * direction_;
	EditProperty * imageIndex_;

	QIntValidator validator_;

};