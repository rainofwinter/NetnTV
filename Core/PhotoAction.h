#pragma once

#include "Action.h"

class PhotoTakePictureAction : public Action
{
public:

	virtual boost::uuids::uuid type() const
	{
		static boost::uuids::uuid uuid = boost::uuids::string_generator()			
			("C3081EFF-8637-4988-87D5-5460EA25AD55");
		return uuid;
	}

	virtual const char * typeStr() const
	{
		return "Photo Take a Picture";
	}
	PhotoTakePictureAction();
	~PhotoTakePictureAction();
	virtual bool needsScenePreStart() const {return true;}

	virtual Action * clone() const {return new PhotoTakePictureAction(*this);}	
	virtual Action & equals(const Action & rhs);

	virtual bool start(SceneObject * object, float time);
	virtual bool hasDuration() const {return false;}
	virtual bool isExecuting() const {return false;}
	virtual bool update(SceneObject * object, float time) {return false;}

	virtual unsigned char version() const {return 0;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);	

	virtual bool dependsOn(Animation * animation) const {return false;}
	virtual bool dependsOn(Scene * scene) const {return false;}
	virtual bool dependsOn(SceneObject * obj) const;

	virtual bool supportsObject(SceneObject * obj) const;
private:
};


////////////////////////////////////////////////////////////////////////////////

class PhotoCameraOnAction : public Action
{
public:

	virtual boost::uuids::uuid type() const
	{
		static boost::uuids::uuid uuid = boost::uuids::string_generator()			
			("E78A9F8E-8F4C-4D17-8C2F-A0631E0BBD33");
		return uuid;
	}

	virtual const char * typeStr() const
	{
		return "Photo Camera On";
	}
	PhotoCameraOnAction();
	~PhotoCameraOnAction();
	virtual bool needsScenePreStart() const {return true;}

	virtual Action * clone() const {return new PhotoCameraOnAction(*this);}	
	virtual Action & equals(const Action & rhs);

	virtual bool start(SceneObject * object, float time);
	virtual bool hasDuration() const {return false;}
	virtual bool isExecuting() const {return false;}
	virtual bool update(SceneObject * object, float time) {return false;}

	virtual unsigned char version() const {return 0;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);	

	virtual bool dependsOn(Animation * animation) const {return false;}
	virtual bool dependsOn(Scene * scene) const {return false;}
	virtual bool dependsOn(SceneObject * obj) const;

	virtual bool supportsObject(SceneObject * obj) const;
private:
};

////////////////////////////////////////////////////////////////////////////////

class PhotoCameraOffAction : public Action
{
public:

	virtual boost::uuids::uuid type() const
	{
		static boost::uuids::uuid uuid = boost::uuids::string_generator()			
			("3F8CB8FB-1E75-4B34-8116-51E0029D2030");
		return uuid;
	}

	virtual const char * typeStr() const
	{
		return "Photo Camera Off";
	}
	PhotoCameraOffAction();
	~PhotoCameraOffAction();
	virtual bool needsScenePreStart() const {return true;}

	virtual Action * clone() const {return new PhotoCameraOffAction(*this);}	
	virtual Action & equals(const Action & rhs);

	virtual bool start(SceneObject * object, float time);
	virtual bool hasDuration() const {return false;}
	virtual bool isExecuting() const {return false;}
	virtual bool update(SceneObject * object, float time) {return false;}

	virtual unsigned char version() const {return 0;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);	

	virtual bool dependsOn(Animation * animation) const {return false;}
	virtual bool dependsOn(Scene * scene) const {return false;}
	virtual bool dependsOn(SceneObject * obj) const;

	virtual bool supportsObject(SceneObject * obj) const;
private:
};

////////////////////////////////////////////////////////////////////////////////

class PhotoAlbumAction : public Action
{
public:

	virtual boost::uuids::uuid type() const
	{
		static boost::uuids::uuid uuid = boost::uuids::string_generator()			
			("9A6C2CAE-0C9F-4B76-8A20-F72E20791445");
		return uuid;
	}

	virtual const char * typeStr() const
	{
		return "Photo Album";
	}
	PhotoAlbumAction();
	~PhotoAlbumAction();
	virtual bool needsScenePreStart() const {return true;}

	virtual Action * clone() const {return new PhotoAlbumAction(*this);}	
	virtual Action & equals(const Action & rhs);

	virtual bool start(SceneObject * object, float time);
	virtual bool hasDuration() const {return false;}
	virtual bool isExecuting() const {return false;}
	virtual bool update(SceneObject * object, float time) {return false;}

	virtual unsigned char version() const {return 0;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);	

	virtual bool dependsOn(Animation * animation) const {return false;}
	virtual bool dependsOn(Scene * scene) const {return false;}
	virtual bool dependsOn(SceneObject * obj) const;

	virtual bool supportsObject(SceneObject * obj) const;

private:
};

////////////////////////////////////////////////////////////////////////////////

class PhotoCameraSwitchAction : public Action
{
public:

	virtual boost::uuids::uuid type() const
	{
		static boost::uuids::uuid uuid = boost::uuids::string_generator()			
			("276CFF19-4B3B-4739-BF2E-246E53DFAC7D");
		return uuid;
	}

	virtual const char * typeStr() const
	{
		return "Photo Camera Switch";
	}
	PhotoCameraSwitchAction();
	~PhotoCameraSwitchAction();
	virtual bool needsScenePreStart() const {return true;}

	virtual Action * clone() const {return new PhotoCameraSwitchAction(*this);}	
	virtual Action & equals(const Action & rhs);

	virtual bool start(SceneObject * object, float time);
	virtual bool hasDuration() const {return false;}
	virtual bool isExecuting() const {return false;}
	virtual bool update(SceneObject * object, float time) {return false;}

	virtual unsigned char version() const {return 0;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);	

	virtual bool dependsOn(Animation * animation) const {return false;}
	virtual bool dependsOn(Scene * scene) const {return false;}
	virtual bool dependsOn(SceneObject * obj) const;

	virtual bool supportsObject(SceneObject * obj) const;
private:
};

////////////////////////////////////////////////////////////////////////////////

class PhotoSave : public Action
{
public:

	virtual boost::uuids::uuid type() const
	{
		static boost::uuids::uuid uuid = boost::uuids::string_generator()			
			("73C0A9E4-6BBA-4F09-932B-3E76AFD76115");
		return uuid;
	}

	virtual const char * typeStr() const
	{
		return "Photo Save";
	}
	PhotoSave();
	~PhotoSave();
	virtual bool needsScenePreStart() const {return true;}

	virtual Action * clone() const {return new PhotoSave(*this);}	
	virtual Action & equals(const Action & rhs);

	virtual bool start(SceneObject * object, float time);
	virtual bool hasDuration() const {return false;}
	virtual bool isExecuting() const {return false;}
	virtual bool update(SceneObject * object, float time) {return false;}

	virtual unsigned char version() const {return 0;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);	

	virtual bool dependsOn(Animation * animation) const {return false;}
	virtual bool dependsOn(Scene * scene) const {return false;}
	virtual bool dependsOn(SceneObject * obj) const;

	virtual bool supportsObject(SceneObject * obj) const;
private:
};

////////////////////////////////////////////////////////////////////////////////
/*
#ifndef Photo_h__
#define Photo_h__

class Photo
{
public:
	Photo() {}
	virtual ~Photo() {}

	virtual void setFileName(const std::string & url);
	const std::string & fileName() const {return fileName_;}

private:
	std::string fileName_;
};

#endif // Photo_h__*/