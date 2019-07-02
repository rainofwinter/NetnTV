#pragma once
#include "EditorAction.h"
#include "PropertyPage.h"

class EditorEffectAction : public EditorAction
{
public:	
	virtual EditorAction * clone() const
	{
		return new EditorEffectAction(*this);
	}
	virtual ActionPropertyPage * propertyPage() const;
	virtual boost::uuids::uuid actionType() const;

	virtual QString toString(Action * action) const;
};

///////////////////////////////////////////////////////////////////////////////

class EditorEffectActionPropertyPage : public ActionPropertyPage
{
public:
	EditorEffectActionPropertyPage();
	~EditorEffectActionPropertyPage();
	
	virtual void setDocument(EditorDocument * document);

	virtual bool isValid() const;

public slots:
	virtual void update();
	virtual void onChanged(Property * property);

private:
	EditProperty * duration_;
	ComboBoxProperty * effect_;
	QDoubleValidator validator_;

};