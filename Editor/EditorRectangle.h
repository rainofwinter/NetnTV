#pragma once
#include "EditorObject.h"
#include "PropertyPage.h"

class EditorRectangle : public EditorObject
{
public:
	EditorRectangle();
	virtual ~EditorRectangle();
	virtual EditorObject * clone() const {return new EditorRectangle(*this);}
	virtual SceneObject * createSceneObject(EditorDocument * document) const;
	virtual boost::uuids::uuid sceneObjectType() const;
	virtual void drawObject(GfxRenderer * gl, const EditorDocument * document) const;
	PropertyPage * propertyPage() const;
private:

};

///////////////////////////////////////////////////////////////////////////////

class EditorRectanglePropertyPage : public PropertyPage
{
public:
	EditorRectanglePropertyPage();
	~EditorRectanglePropertyPage();

	virtual void update();
	virtual void onChanged(Property * property);

private:
	CorePropertiesPage * core_;
	VisualPropertiesPage * visual_;
	ColorProperty * color_;

	EditProperty * width_, * height_;
	QIntValidator dimValidator_;
};