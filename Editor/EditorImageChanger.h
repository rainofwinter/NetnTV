#pragma once
#include "EditorObject.h"
#include "PropertyPage.h"
#include "Types.h"

class EditorImageChanger : public EditorObject
{
public:
	EditorImageChanger();
	virtual ~EditorImageChanger();
	virtual EditorObject * clone() const {return new EditorImageChanger(*this);}
	virtual SceneObject * createSceneObject(EditorDocument * document) const;
	virtual boost::uuids::uuid sceneObjectType() const;
	virtual void drawObject(GfxRenderer * gl, const EditorDocument * document) const;
	PropertyPage * propertyPage() const;
private:

};

///////////////////////////////////////////////////////////////////////////////
class Scene;


class EditorImageChangerPropertyPage : public PropertyPage
{
	Q_OBJECT
public:
	EditorImageChangerPropertyPage();
	~EditorImageChangerPropertyPage();

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

	QPushButton * imagesButton_;
	EditProperty * width_, * height_;
	QDoubleValidator validator_;

	BoolProperty * showMarkers_;
	ComboBoxProperty * markerLocation_;
	FileNameProperty * activeMarker_;
	FileNameProperty * inactiveMarker_;

	BoolProperty * allowDrag_;
	ComboBoxProperty * scrollDirection_;

	EditProperty * autoTransitionTime_;
	QDoubleValidator autoTransitionValidator_;
};