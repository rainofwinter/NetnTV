#pragma once
#include "EditorAction.h"
#include "PropertyPage.h"

class Document;

class EditorImageAnimationPauseAction : public EditorAction
{
public:	
	virtual EditorAction * clone() const
	{
		return new EditorImageAnimationPauseAction(*this);
	}
	virtual ActionPropertyPage * propertyPage() const;
	virtual boost::uuids::uuid actionType() const;

	virtual QString toString(Action * action) const;
};

///////////////////////////////////////////////////////////////////////////////

class EditorImageAnimationPausePropertyPage : public ActionPropertyPage
{
public:
	EditorImageAnimationPausePropertyPage();
	~EditorImageAnimationPausePropertyPage();

	virtual void setDocument(EditorDocument * document);

	virtual bool isValid() const;

	public slots:
		virtual void update();
		virtual void onChanged(Property * property);

private:
};