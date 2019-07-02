#pragma once
#include "EditorObject.h"
#include "PropertyPage.h"

class Model;

class EditorLightObject : public EditorObject
{
public:
	EditorLightObject();
	virtual ~EditorLightObject();
	virtual EditorObject * clone() const {return new EditorLightObject(*this);}
	virtual SceneObject * createSceneObject(EditorDocument * document) const;
	virtual boost::uuids::uuid sceneObjectType() const;
	virtual void drawObject(GfxRenderer * gl, const EditorDocument * document) const;
	PropertyPage * propertyPage() const;

	virtual SceneObject * intersect(const EditorDocument * document, const Ray & ray, 
		Vector3 * intPt);

private:

	Model * lightModel_;
	Matrix lightModelMatrix_;
};

///////////////////////////////////////////////////////////////////////////////

class EditorLightObjectPropertyPage : public PropertyPage
{
public:
	EditorLightObjectPropertyPage();
	~EditorLightObjectPropertyPage();

	virtual void update();
	virtual void onChanged(Property * property);	

private:
	CorePropertiesPage * core_;
	VisualPropertiesPage * visual_;
	EditProperty * intensity_;
	EditProperty * specularIntensity_;
	QDoubleValidator intensityValidator_;

};