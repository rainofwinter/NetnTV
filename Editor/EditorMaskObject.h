#pragma once
#include "EditorObject.h"
#include "PropertyPage.h"

class EditorMaskObject : public EditorObject
{
public:
	EditorMaskObject();
	virtual ~EditorMaskObject();
	virtual EditorObject * clone() const {return new EditorMaskObject(*this);}
	virtual SceneObject * createSceneObject(EditorDocument * document) const;
	virtual boost::uuids::uuid sceneObjectType() const;
	virtual PropertyPage * propertyPage() const;
	virtual void drawObject(GfxRenderer * gl, const EditorDocument * document) const;
	virtual void draw(GfxRenderer * gl, const EditorDocument * document) const;
	virtual void setExpanded(bool val) {expanded_ = val;}
	virtual bool expanded() const {return expanded_;}

	virtual SceneObject * intersect(const EditorDocument * document, const Ray & ray, 
		Vector3 * intPt);

	bool showMasking() const {return showMasking_;}
	void setShowMasking(bool showMasking) {showMasking_ = showMasking;}
private:
	bool expanded_;
	bool showMasking_;
};

///////////////////////////////////////////////////////////////////////////////

class EditorMaskObjectPropertyPage : public PropertyPage
{
public:
	EditorMaskObjectPropertyPage();
	~EditorMaskObjectPropertyPage();

	virtual void update();
	virtual void onChanged(Property * property);	
private:
	CorePropertiesPage * core_;
	VisualPropertiesPage * visual_;

	BoolProperty * showMasking_;

};