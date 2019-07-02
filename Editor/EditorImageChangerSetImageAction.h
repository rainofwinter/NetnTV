#pragma once
#include "EditorAction.h"
#include "PropertyPage.h"

class Document;

class EditorImageChangerSetImageAction : public EditorAction
{
public:	
	virtual EditorAction * clone() const
	{
		return new EditorImageChangerSetImageAction(*this);
	}
	virtual ActionPropertyPage * propertyPage() const;
	virtual boost::uuids::uuid actionType() const;

	virtual QString toString(Action * action) const;
};

///////////////////////////////////////////////////////////////////////////////

class EditorImageChangerSetImagePropertyPage : public ActionPropertyPage
{
public:
	EditorImageChangerSetImagePropertyPage();
	~EditorImageChangerSetImagePropertyPage();
	
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