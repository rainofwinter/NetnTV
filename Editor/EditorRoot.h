#pragma once
#include "EditorObject.h"
#include "PropertyPage.h"

class EditorRoot : public EditorObject
{
public:
	EditorRoot();
	virtual ~EditorRoot();
	virtual EditorObject * clone() const {return new EditorRoot(*this);}
	virtual SceneObject * createSceneObject(EditorDocument * document) const;
	virtual boost::uuids::uuid sceneObjectType() const;
	PropertyPage * propertyPage() const;

	void setExpanded(bool val) {expanded_ = val;}
	bool expanded() const {return expanded_;}
private:
	bool expanded_;
};

///////////////////////////////////////////////////////////////////////////////
class CameraObject;

class EditorRootPropertyPage : public PropertyPage
{
	Q_OBJECT
public:
	EditorRootPropertyPage();
	~EditorRootPropertyPage();

	virtual void setDocument(EditorDocument * document);
	virtual void update();

	void setCameraIndex(int index, EditorDocument * document);

private slots:
	virtual void onChanged(Property * property);	
	void onResetCamera();
	void onResetCamera100();
	void onResetEnableChange();

private:
	void populateCameras();

private:
	EditProperty * screenWidth_;
	EditProperty * screenHeight_;
	ColorProperty * bgColor_;

	//ComboBoxProperty * objectSorting_;
	//TODO make this into a property 
	QPushButton * resetCamButton_;
	QPushButton * resetCam100Button_;
	QDoubleValidator doubleValidator_, intensityValidator_;

	BoolProperty * zoom_;

	ComboBoxProperty * camera_;

	std::vector<CameraObject *> cameras_;
	std::map<CameraObject *, int> cameraIndices_;

	BoolProperty * zBuffer_;
	ComboBoxProperty * zoomMag_;
	EditProperty * trackingUrl_;

	EditProperty * lightIntensity_;
	EditProperty * lightSpecularIntensity_;
	EditProperty * lightAmbient_;

};