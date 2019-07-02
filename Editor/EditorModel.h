#pragma once
#include "EditorObject.h"
#include "PropertyPage.h"

class EditorModel : public EditorObject
{
public:
	EditorModel();
	virtual ~EditorModel();
	virtual EditorObject * clone() const {return new EditorModel(*this);}
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

class EditorModelPropertyPage : public PropertyPage
{
	Q_OBJECT
public:
	EditorModelPropertyPage();
	~EditorModelPropertyPage();

	virtual void update();
	virtual void onChanged(Property * property);

	virtual void setDocument(EditorDocument * document);

private slots:
	void onInspectComponentTool();
	void onModelAnimationPanel();
	void onModelAnimationPanelClosed();
	void onToolChanged();


private:
	CorePropertiesPage * core_;
	VisualPropertiesPage * visual_;
	QPushButton * selectJointButton_;
	QPushButton * modelAnimationPanelButton_;
	
	static ModelAnimationPanel * modelAnimationPanel_;
	boost::scoped_ptr<InspectComponentTool> inspectComponentTool_;	

};