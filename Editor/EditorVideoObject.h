#pragma once
#include "EditorObject.h"
#include "PropertyPage.h"
#include "Tool.h"

class VideoObject;

class EditorVideoObject : public EditorObject
{
public:
	EditorVideoObject();
	virtual ~EditorVideoObject();
	virtual EditorObject * clone() const {return new EditorVideoObject(*this);}
	virtual SceneObject * createSceneObject(EditorDocument * document) const;
	virtual boost::uuids::uuid sceneObjectType() const;
	virtual void drawObject(GfxRenderer * gl, const EditorDocument * document) const;
	PropertyPage * propertyPage() const;

	virtual SceneObject * intersect(const EditorDocument * document, const Ray & ray, 
		Vector3 * intPt);

private:
};

///////////////////////////////////////////////////////////////////////////////
class VideoRegionTool;

class EditorVideoObjectPropertyPage : public PropertyPage
{
	Q_OBJECT
public:
	EditorVideoObjectPropertyPage();
	~EditorVideoObjectPropertyPage();

	virtual void update();
	virtual void onChanged(Property * property);

	virtual void setDocument(EditorDocument * document);
private slots:
	void onInteractiveRegion();
	void onToolChanged();
	void onUrl();
private:
	ComboBoxProperty * mode_;
	CorePropertiesPage * core_;
	FileNameProperty * fileName_;
	EditProperty * url_;

	BoolProperty * showControls_;
	ComboBoxProperty * playMode_;

	QPushButton * interactiveRegionBtn_;
	EditProperty * x_, * y_, * width_, *height_;

	QIntValidator posValidator_, dimValidator_;

	boost::scoped_ptr<VideoRegionTool> regionTool_;

	QPushButton * urlPush_;

	std::vector<std::string> sourceName_;
	std::vector<int> sourceMode_;
};

