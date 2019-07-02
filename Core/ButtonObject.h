#pragma once

#include "SceneObject.h"
#include "Attrib.h"
#include "Texture.h"

class ButtonObject : public SceneObject
{

public:

	virtual const char * typeStr() const
	{
		return "ButtonObject";
	}

	typedef SceneObject base_type;
	virtual boost::uuids::uuid type() const 
	{
		static boost::uuids::uuid uuid = 
			sUuidGen_("B910BD5D-E034-4113-95C0-A0A9657CAFEC");
		return uuid;
	}

	virtual SceneObject * clone(ElementMapping * elementMapping) const 
	{return new ButtonObject(*this, elementMapping);}

	virtual void remapReferences(const ElementMapping & mapping) 
	{SceneObject::remapReferences(mapping);}

	virtual void referencedFiles(std::vector<std::string> * refFiles) const;
	virtual int setReferencedFiles(const std::string & baseDirectory,
		const std::vector<std::string> & refFiles, int index);
	bool update(float sceneTime);

	virtual bool asyncLoadUpdate();
	virtual bool isLoaded() const;

	ButtonObject();
	ButtonObject(const ButtonObject & rhs, ElementMapping * elementMapping);

	~ButtonObject();

	virtual void drawObject(GfxRenderer * gl) const;

	virtual void init(GfxRenderer * gl, bool firstTime = false);
	virtual void uninit();

	const float & width() const {return width_;}
	void setWidth(const float & width) {width_ = width;}

	const float & height() const {return height_;}
	void setHeight(const float & height) {height_ = height;}
	virtual VisualAttrib * visualAttrib() {return &visualAttrib_;}

	const std::vector<std::string> & fileNames() const {return fileNames_;}

	void setFileNames(const std::vector<std::string> & fileNames);
	virtual BoundingBox extents() const;

	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);
	virtual unsigned char version() const {return 0;}
	virtual const char * xmlTag() const {return "ButtonObject";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void readXml(XmlReader & r, xmlNode * node);
	const std::vector<TextureSPtr> & buffers() const {return textures_;}
	bool intersectRect(Vector3 * out, const Ray & ray);
	virtual SceneObject * intersect(Vector3 * intPt, const Ray & ray);

	bool pressEvent(const Vector2 & startPos, int pressId);
	bool releaseEvent(const Vector2 & pos, int pressId);

private:
	ButtonObject(const ButtonObject & rhs);
	ButtonObject & operator = (const ButtonObject & rhs);
	void create();

private:
	std::vector<TextureSPtr> textures_;
	std::vector<std::string> fileNames_;

	float width_;
	float height_;
	bool pressed_;

	Texture::Format format_;
	VisualAttrib visualAttrib_;
	SceneObject * sceneObject_;

	boost::scoped_ptr<Event> event_;
};
