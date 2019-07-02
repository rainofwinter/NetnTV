#pragma once
#include "Action.h"

class CameraObject;

class SetCameraAction : public Action
{	
public:
	virtual boost::uuids::uuid type() const
	{
		boost::uuids::string_generator gen;
		return gen("342AD0FC-DFDF-5B22-9021-A14BE6ABE107");
	}

	virtual const char * typeStr() const
	{
		return "Set Camera";
	}

	SetCameraAction();
	~SetCameraAction();
	//default copy constructor / operator =
	virtual Action * clone() const {return new SetCameraAction(*this);}	
	virtual Action & equals(const Action & rhs);	
	virtual bool remapReferences(const ElementMapping & mapping);

	virtual bool supportsObject(SceneObject * obj) const;

	virtual bool start(SceneObject * object, float time);
	virtual bool hasDuration() const;
	virtual bool isExecuting() const;
	virtual bool update(SceneObject * object, float time);	

	CameraObject * cameraObject() const {return cameraObj_;}
	void setCameraObject(CameraObject * camObj) {cameraObj_ = camObj;}
		
	virtual bool dependsOn(Animation * animation) const {return false;}
	virtual bool dependsOn(Scene * scene) const;
	virtual bool dependsOn(SceneObject * obj) const;

	virtual unsigned char version() const {return 0;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);

	virtual const char * xmlTag() const {return "SetCamera";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void readXml(XmlReader & r, xmlNode * parent);

private:

	CameraObject * cameraObj_;

	Document * document_;
};

