#pragma once
#include "EditorAction.h"
#include "PropertyPage.h"

class Document;

class EditorSceneChangeAction : public EditorAction
{
public:	
	virtual EditorAction * clone() const
	{
		return new EditorSceneChangeAction(*this);
	}
	virtual ActionPropertyPage * propertyPage() const;
	virtual boost::uuids::uuid actionType() const;

	virtual QString toString(Action * action) const;
};

///////////////////////////////////////////////////////////////////////////////

class EditorSceneChangePropertyPage : public ActionPropertyPage
{
public:
	EditorSceneChangePropertyPage();
	~EditorSceneChangePropertyPage();
	
	virtual void setDocument(EditorDocument * document);
	virtual bool isValid() const;

public slots:
	virtual void update();
	virtual void onChanged(Property * property);

private:
	void setModeEnabled();
	ComboBoxProperty * mode_;
	ComboBoxProperty * direction_;

	ComboBoxProperty * scene_;
	

};