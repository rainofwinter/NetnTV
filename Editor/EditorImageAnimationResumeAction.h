#pragma once
#include "EditorAction.h"
#include "PropertyPage.h"

class Document;

class EditorImageAnimationResumeAction : public EditorAction
{
public:	
	virtual EditorAction * clone() const
	{
		return new EditorImageAnimationResumeAction(*this);
	}
	virtual ActionPropertyPage * propertyPage() const;
	virtual boost::uuids::uuid actionType() const;

	virtual QString toString(Action * action) const;
};

///////////////////////////////////////////////////////////////////////////////

class EditorImageAnimationResumePropertyPage : public ActionPropertyPage
{
public:
	EditorImageAnimationResumePropertyPage();
	~EditorImageAnimationResumePropertyPage();

	virtual void setDocument(EditorDocument * document);

	virtual bool isValid() const;

	public slots:
		virtual void update();
		virtual void onChanged(Property * property);

private:
};