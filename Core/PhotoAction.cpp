#include "StdAfx.h"
#include "PhotoAction.h"
#include "Writer.h"
#include "Reader.h"
#include "PhotoObject.h"
#include "ElementMapping.h"
#include "Global.h"
#include "Scene.h"
#include "Document.h"

PhotoTakePictureAction::PhotoTakePictureAction()
{
}


PhotoTakePictureAction::~PhotoTakePictureAction()
{
}

Action & PhotoTakePictureAction::equals(const Action & rhs)
{
	if (this == &rhs) return *this;
	*this = (PhotoTakePictureAction &)rhs;
	return *this;
}

bool PhotoTakePictureAction::dependsOn(SceneObject * obj) const 
{
	return Action::dependsOn(obj);
}

bool PhotoTakePictureAction::supportsObject(SceneObject * obj) const
{
	return obj && obj->type() == PhotoObject().type();
}

void PhotoTakePictureAction::write(Writer & writer) const
{
	writer.writeParent<Action>(this);	
}

void PhotoTakePictureAction::read(Reader & reader, unsigned char version)
{
	reader.readParent<Action>(this);
}

bool PhotoTakePictureAction::start(SceneObject * object, float time)
{	
	PhotoObject * obj = (PhotoObject *)targetObject_;
	Global::instance().photoTakePicture();
	return true;
}

////////////////////////////////////////////////////////////////////////////

PhotoCameraOnAction::PhotoCameraOnAction()
{
}


PhotoCameraOnAction::~PhotoCameraOnAction()
{
}

Action & PhotoCameraOnAction::equals(const Action & rhs)
{
	if (this == &rhs) return *this;
	*this = (PhotoCameraOnAction &)rhs;
	return *this;
}

bool PhotoCameraOnAction::dependsOn(SceneObject * obj) const 
{
	return Action::dependsOn(obj);
}

bool PhotoCameraOnAction::supportsObject(SceneObject * obj) const
{
	return obj && obj->type() == PhotoObject().type();
}

void PhotoCameraOnAction::write(Writer & writer) const
{
	writer.writeParent<Action>(this);	
}

void PhotoCameraOnAction::read(Reader & reader, unsigned char version)
{
	reader.readParent<Action>(this);
}

bool PhotoCameraOnAction::start(SceneObject * object, float time)
{	
	Scene * scene = object->parentScene();
	Document * document = scene->parentDocument();
	PhotoObject * obj = (PhotoObject *)targetObject_;
	VisualAttrib * attr = targetObject_->visualAttrib();

	if (!scene->isCurrentScene())
		return true;

	attr->setVisible(false);

	ContainerObjects objs;
	scene->getContainerObjects(&objs);
	
	document->photoCameraOn(obj, objs,
		obj->x(), obj->y(), obj->width(), obj->height());
	obj->photoCameraOn();

	return true;
}

////////////////////////////////////////////////////////////////////////////

PhotoCameraOffAction::PhotoCameraOffAction()
{
}


PhotoCameraOffAction::~PhotoCameraOffAction()
{
}

Action & PhotoCameraOffAction::equals(const Action & rhs)
{
	if (this == &rhs) return *this;
	*this = (PhotoCameraOffAction &)rhs;
	return *this;
}

bool PhotoCameraOffAction::dependsOn(SceneObject * obj) const 
{
	return Action::dependsOn(obj);
}

bool PhotoCameraOffAction::supportsObject(SceneObject * obj) const
{
	return obj && obj->type() == PhotoObject().type();
}

void PhotoCameraOffAction::write(Writer & writer) const
{
	writer.writeParent<Action>(this);	
}

void PhotoCameraOffAction::read(Reader & reader, unsigned char version)
{
	reader.readParent<Action>(this);
}

bool PhotoCameraOffAction::start(SceneObject * object, float time)
{	
	PhotoObject * obj = (PhotoObject *)targetObject_;
	VisualAttrib * attr = targetObject_->visualAttrib();		
	attr->setVisible(true);
	Global::instance().photoCameraOff();
	if(obj->getPhotoName() != "")
	{
		obj->setFileName(obj->getPhotoName());
		obj->setWidth(obj->width());
		obj->setHeight(obj->height());
	}	
	obj->photoCameraOff();
	return true;
}

////////////////////////////////////////////////////////////////////////////

PhotoAlbumAction::PhotoAlbumAction()
{
}


PhotoAlbumAction::~PhotoAlbumAction()
{
}

Action & PhotoAlbumAction::equals(const Action & rhs)
{
	if (this == &rhs) return *this;
	*this = (PhotoAlbumAction &)rhs;
	return *this;
}

bool PhotoAlbumAction::dependsOn(SceneObject * obj) const 
{
	return Action::dependsOn(obj);
}

bool PhotoAlbumAction::supportsObject(SceneObject * obj) const
{
	return obj && obj->type() == PhotoObject().type();
}

void PhotoAlbumAction::write(Writer & writer) const
{
	writer.writeParent<Action>(this);	
}

void PhotoAlbumAction::read(Reader & reader, unsigned char version)
{
	reader.readParent<Action>(this);
}

bool PhotoAlbumAction::start(SceneObject * object, float time)
{	
	PhotoObject * obj = (PhotoObject *)targetObject_;
	VisualAttrib * attr = targetObject_->visualAttrib();
	attr->setVisible(true);
	Global::instance().photoAlbum();
	return true;
}

////////////////////////////////////////////////////////////////////////////

PhotoCameraSwitchAction::PhotoCameraSwitchAction()
{
}


PhotoCameraSwitchAction::~PhotoCameraSwitchAction()
{
}

Action & PhotoCameraSwitchAction::equals(const Action & rhs)
{
	if (this == &rhs) return *this;
	*this = (PhotoCameraSwitchAction &)rhs;
	return *this;
}

bool PhotoCameraSwitchAction::dependsOn(SceneObject * obj) const 
{
	return Action::dependsOn(obj);
}

bool PhotoCameraSwitchAction::supportsObject(SceneObject * obj) const
{
	return obj && obj->type() == PhotoObject().type();
}

void PhotoCameraSwitchAction::write(Writer & writer) const
{
	writer.writeParent<Action>(this);	
}

void PhotoCameraSwitchAction::read(Reader & reader, unsigned char version)
{
	reader.readParent<Action>(this);
}

bool PhotoCameraSwitchAction::start(SceneObject * object, float time)
{	
	PhotoObject * obj = (PhotoObject *)targetObject_;
	Global::instance().photoCameraSwitch();
	return true;
}

////////////////////////////////////////////////////////////////////////////

PhotoSave::PhotoSave()
{
}


PhotoSave::~PhotoSave()
{
}

Action & PhotoSave::equals(const Action & rhs)
{
	if (this == &rhs) return *this;
	*this = (PhotoSave &)rhs;
	return *this;
}

bool PhotoSave::dependsOn(SceneObject * obj) const 
{
	return Action::dependsOn(obj);
}

bool PhotoSave::supportsObject(SceneObject * obj) const
{
	return obj && obj->type() == PhotoObject().type();
}

void PhotoSave::write(Writer & writer) const
{
	writer.writeParent<Action>(this);	
}

void PhotoSave::read(Reader & reader, unsigned char version)
{
	reader.readParent<Action>(this);
}

bool PhotoSave::start(SceneObject * object, float time)
{	
	Scene * scene = object->parentScene();
	Document * document = scene->parentDocument();
	PhotoObject * obj = (PhotoObject *)targetObject_;

	if (!scene->isCurrentScene())
		return true;

	ContainerObjects objs;
	scene->getContainerObjects(&objs);
	
	document->photoSave(obj, objs,
		obj->x(), obj->y(), obj->width(), obj->height());
	return true;
}

/////////////////////////////////////////////////////////////////