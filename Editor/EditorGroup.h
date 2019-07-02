#pragma once
#include "EditorObject.h"
#include "PropertyPage.h"

class EditorGroup : public EditorObject
{
public:
	EditorGroup();
	virtual ~EditorGroup();
	virtual EditorObject * clone() const {return new EditorGroup(*this);}
	virtual SceneObject * createSceneObject(EditorDocument * document) const;
	virtual boost::uuids::uuid sceneObjectType() const;
	virtual PropertyPage * propertyPage() const;

	virtual void setExpanded(bool val) {expanded_ = val;}
	virtual bool expanded() const {return expanded_;}
private:
	bool expanded_;
};

///////////////////////////////////////////////////////////////////////////////
class Group;

class EditorGroupPropertyPage : public PropertyPage
{
public:
	EditorGroupPropertyPage(Group * group);
	~EditorGroupPropertyPage();

	virtual void update();

private:
	CorePropertiesPage * core_;
	VisualPropertiesPage * visual_;
	Group * group_;
};