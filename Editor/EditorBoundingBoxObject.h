#pragma once
#include "EditorObject.h"
#include "PropertyPage.h"

class EditorBoundingBoxObject : public EditorObject
{
public:
	EditorBoundingBoxObject();
	virtual ~EditorBoundingBoxObject();
	virtual EditorObject * clone() const {return new EditorBoundingBoxObject(*this);}
	virtual SceneObject * createSceneObject(EditorDocument * document) const;
	virtual boost::uuids::uuid sceneObjectType() const;
	virtual void drawObject(GfxRenderer * gl, const EditorDocument * document) const;
	PropertyPage * propertyPage() const;
private:
	static QString lastDir_;
};

///////////////////////////////////////////////////////////////////////////////
class InspectComponentTool;
class ModelAnimationPanel;

class EditorBoundingBoxObjectPropertyPage : public PropertyPage
{
	Q_OBJECT
public:
	EditorBoundingBoxObjectPropertyPage();
	~EditorBoundingBoxObjectPropertyPage();

	virtual void update();
	virtual void onChanged(Property * property);

	virtual void setDocument(EditorDocument * document);
	
private:
	CorePropertiesPage * core_;
	VisualPropertiesPage * visual_;

};