#pragma once
#include "EditorObject.h"
#include "PropertyPage.h"

class EditorPanorama : public EditorObject
{
public:
	EditorPanorama();
	virtual ~EditorPanorama();
	virtual EditorObject * clone() const {return new EditorPanorama(*this);}
	virtual SceneObject * createSceneObject(EditorDocument * document) const;
	virtual boost::uuids::uuid sceneObjectType() const;
	virtual void drawObject(GfxRenderer * gl, const EditorDocument * document) const;
	PropertyPage * propertyPage() const;
private:
	static QString lastDir_;
};

///////////////////////////////////////////////////////////////////////////////

class EditorPanoramaPropertyPage : public PropertyPage
{
public:
	EditorPanoramaPropertyPage();
	~EditorPanoramaPropertyPage();

	virtual void update();
	virtual void onChanged(Property * property);

private:
	CorePropertiesPage * core_;
	VisualPropertiesPage * visual_;

	EditProperty * width_, * height_;
	QDoubleValidator validator_;
	FileNameProperty * front_, *back_, *left_, *right_, *top_, *bottom_;

};