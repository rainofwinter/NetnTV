#pragma once
#include "EditorAction.h"
#include "PropertyPage.h"

class Document;

class EditorSceneChangerSceneSetAction : public EditorAction
{
public:	
	virtual EditorAction * clone() const
	{
		return new EditorSceneChangerSceneSetAction(*this);
	}
	virtual ActionPropertyPage * propertyPage() const;
	virtual boost::uuids::uuid actionType() const;

	virtual QString toString(Action * action) const;
};

///////////////////////////////////////////////////////////////////////////////

class EditorSceneChangerSceneSetPropertyPage : public ActionPropertyPage
{
public:
	EditorSceneChangerSceneSetPropertyPage();
	~EditorSceneChangerSceneSetPropertyPage();
	
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
	std::map<Scene *, int> scenesMap_;
	std::vector<Scene *> scenesList_;

};