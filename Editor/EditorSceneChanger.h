#pragma once
#include "EditorObject.h"
#include "PropertyPage.h"
#include "Types.h"

class EditorSceneChanger : public EditorObject
{
public:
	EditorSceneChanger();
	virtual ~EditorSceneChanger();
	virtual EditorObject * clone() const {return new EditorSceneChanger(*this);}
	virtual SceneObject * createSceneObject(EditorDocument * document) const;
	virtual boost::uuids::uuid sceneObjectType() const;
	virtual void drawObject(GfxRenderer * gl, const EditorDocument * document) const;
	PropertyPage * propertyPage() const;
private:

};

///////////////////////////////////////////////////////////////////////////////
class Scene;


class EditorSceneChangerPropertyPage : public PropertyPage
{
	Q_OBJECT
public:
	EditorSceneChangerPropertyPage();
	~EditorSceneChangerPropertyPage();

	virtual void update();
	virtual void onChanged(Property * property);

	virtual void setDocument(EditorDocument * document);

private slots:

	void onArticles();


private:
	void onShowMarkers(bool val);
	void onAllowDrag(bool val);

	CorePropertiesPage * core_;
	VisualPropertiesPage * visual_;

	QPushButton * scenesButton_;
	EditProperty * width_, * height_;
	QDoubleValidator validator_;

	BoolProperty * showMarkers_;
	ComboBoxProperty * markerLocation_;
	FileNameProperty * activeMarker_;
	FileNameProperty * inactiveMarker_;

	BoolProperty * transparentBg_;
	ComboBoxProperty * cameraMode_;
	BoolProperty * linkWithViewerLevelObjects_;

	BoolProperty * allowDrag_;
	ComboBoxProperty * scrollDirection_;

	BoolProperty * preloadAdjacent_;

	ComboBoxProperty * transitionEffect_;
};