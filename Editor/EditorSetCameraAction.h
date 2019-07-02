#pragma once
#include "EditorAction.h"
#include "PropertyPage.h"

class Document;

class EditorSetCameraAction : public EditorAction
{
public:	
	virtual EditorAction * clone() const
	{
		return new EditorSetCameraAction(*this);
	}
	virtual ActionPropertyPage * propertyPage() const;
	virtual boost::uuids::uuid actionType() const;

	virtual QString toString(Action * action) const;
};

///////////////////////////////////////////////////////////////////////////////

class EditorSetCameraPropertyPage : public ActionPropertyPage
{
public:
	EditorSetCameraPropertyPage();
	~EditorSetCameraPropertyPage();
	
	virtual bool isValid() const;

	virtual void setDocument(EditorDocument * document);

private:
	void populateCameras();

public slots:
	virtual void update();
	virtual void onChanged(Property * property);

private:	
	ComboBoxProperty * cameraObj_;
	std::vector<CameraObject *> cameraObjs_;

};