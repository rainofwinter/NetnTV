#pragma once
#include "EditorAction.h"
#include "PropertyPage.h"

class EditorImageSetFileAction : public EditorAction
{
public:	
	virtual EditorAction * clone() const
	{
		return new EditorImageSetFileAction(*this);
	}
	virtual ActionPropertyPage * propertyPage() const;
	virtual boost::uuids::uuid actionType() const;

	virtual QString toString(Action * action) const;
};

///////////////////////////////////////////////////////////////////////////////

class EditorImageSetFileActionPropertyPage : public ActionPropertyPage
{
public:
	EditorImageSetFileActionPropertyPage();
	~EditorImageSetFileActionPropertyPage();
	
	virtual void setDocument(EditorDocument * document);

	virtual bool isValid() const;


public slots:
	virtual void update();
	virtual void onChanged(Property * property);

private:
	FileNameProperty * fileName_;
};