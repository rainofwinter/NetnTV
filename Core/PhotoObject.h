#pragma once

#include "SceneObject.h"
#include "Image.h"
#include "Attrib.h"
//#include "Texture.h"

class ScriptProcessor;
struct JSObject;

class PhotoObject : public Image
{
public:
	virtual JSObject * createScriptObjectProto(ScriptProcessor * s);
	virtual JSObject * getScriptObject(ScriptProcessor * s);

	virtual const char * typeStr() const
	{
		return "PhotoObject";
	}

	typedef SceneObject base_type;
	virtual boost::uuids::uuid type() const 
	{
		static boost::uuids::uuid uuid = 
			sUuidGen_("3EC98FD6-31D8-4FC2-81DB-EDC66941BA58");
		return uuid;
	}

	virtual SceneObject * clone(ElementMapping * elementMapping) const 
	{return new PhotoObject(*this, elementMapping);}

	virtual void remapReferences(const ElementMapping & mapping) 
	{SceneObject::remapReferences(mapping);}

	virtual void referencedFiles(std::vector<std::string> * refFiles) const;
	virtual int setReferencedFiles(const std::string & baseDirectory,
		const std::vector<std::string> & refFiles, int index);

	const float & x() const {return x_;}
	void setX(const float & x) 
	{
		x_ = x;
	}

	const float & y() const {return y_;}
	void setY(const float & y) {y_ = y;}

	const float & width() const {return width_;}
	void setWidth(const float & width);

	const float & height() const {return height_;}
	void setHeight(const float & height);

	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);
	virtual unsigned char version() const {return 0;}
	virtual BoundingBox extents() const;

	virtual VisualAttrib * visualAttrib() 
	{
		return &visualAttrib_;
	}
	void setPhotoName(const std::string & fileName);
	const std::string & getPhotoName() const {return fileName_;}
	void setFileName(const std::string & fileName);
	void photoDefaultImage();	
	void photoTakePicture();
	void photoCameraOn();
	void photoCameraOff();
	void photoSave();

	bool intersectRect(Vector3 * out, const Ray & ray);
	virtual SceneObject * intersect(Vector3 * intPt, const Ray & ray);

	const Vector2 & widHei() const {return trans_;}
	void setWidHei(const Vector2 & trans);

	PhotoObject();
	PhotoObject(const PhotoObject & rhs, ElementMapping * elementMapping);
	~PhotoObject();

private:
	PhotoObject(const PhotoObject & rhs);
	PhotoObject & operator = (const PhotoObject & rhs);
	void create();

private:
	VisualAttrib visualAttrib_;
	float width_;
	float height_;
	float x_;
	float y_;

	Vector2 trans_; 

	std::string fileName_;
	Texture::Format format_;
};
