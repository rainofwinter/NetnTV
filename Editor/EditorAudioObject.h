#pragma once
#include "EditorObject.h"
#include "PropertyPage.h"

class Model;

class EditorAudioObject : public EditorObject
{
public:
	EditorAudioObject();
	virtual ~EditorAudioObject();
	virtual EditorObject * clone() const {return new EditorAudioObject(*this);}
	virtual SceneObject * createSceneObject(EditorDocument * document) const;
	virtual boost::uuids::uuid sceneObjectType() const;
	virtual void drawObject(GfxRenderer * gl, const EditorDocument * document) const;
	PropertyPage * propertyPage() const;

	virtual SceneObject * intersect(const EditorDocument * document, const Ray & ray, 
		Vector3 * intPt);

private:
};

///////////////////////////////////////////////////////////////////////////////

class EditorAudioObjectPropertyPage : public PropertyPage
{
public:
	EditorAudioObjectPropertyPage();
	~EditorAudioObjectPropertyPage();

	virtual void update();
	virtual void onChanged(Property * property);	

private:
	CorePropertiesPage * core_;
	FileNameProperty * fileName_;
	BoolProperty * repeat_;

	ComboBoxProperty * sourceType_;
};