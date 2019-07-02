#pragma once
#include "EditorObject.h"
#include "PropertyPage.h"

class EditorMap : public EditorObject
{
public:
	EditorMap();
	virtual ~EditorMap();
	virtual EditorObject * clone() const {return new EditorMap(*this);}
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

class EditorMapPropertyPage : public PropertyPage
{
	Q_OBJECT
public:
	EditorMapPropertyPage();
	~EditorMapPropertyPage();

	virtual void update();
	virtual void onChanged(Property * property);

	virtual void setDocument(EditorDocument * document);

private:
	CorePropertiesPage * core_;
	VisualPropertiesPage * visual_;

	FileNameProperty * meshFileName_;
	FileNameProperty * pathMeshFileName_;
};