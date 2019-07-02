#pragma once
#include "EditorAction.h"
#include "PropertyPage.h"

class EditorOpenUrlAction : public EditorAction
{
public:	
	virtual EditorAction * clone() const
	{
		return new EditorOpenUrlAction(*this);
	}
	virtual ActionPropertyPage * propertyPage() const;
	virtual boost::uuids::uuid actionType() const;

	virtual QString toString(Action * action) const;
};

///////////////////////////////////////////////////////////////////////////////

class EditorOpenUrlActionPropertyPage : public ActionPropertyPage
{
public:
	EditorOpenUrlActionPropertyPage();
	~EditorOpenUrlActionPropertyPage();
	
	virtual void setDocument(EditorDocument * document);

	virtual bool isValid() const;

public slots:
	virtual void update();
	virtual void onChanged(Property * property);

private:
	EditProperty * url_;
//	ComboBoxProperty * mode_;

};