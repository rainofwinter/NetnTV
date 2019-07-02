#pragma once
#include "SceneObject.h"
#include "Attrib.h"
#include "Types.h"

class MaskObject;

///////////////////////////////////////////////////////////////////////////////

class MaskObjectVisualAttrib : public VisualAttrib
{
public:
	MaskObjectVisualAttrib(MaskObject * sceneObject);

	MaskObjectVisualAttrib & operator = (const MaskObjectVisualAttrib & rhs);
	MaskObjectVisualAttrib & operator = (const VisualAttrib & rhs);

	virtual void onSetTransform();

	unsigned char version() const {return 0;}
	void write(Writer & writer) const;
	void read(Reader & reader, unsigned char version);
	const char * xmlTag() const {return "MaskObjectVisualAttrib";}
	//void writeXml(XmlWriter & w) const;
	//void readXml(XmlReader & r, xmlNode * node);

private:
	MaskObjectVisualAttrib(const MaskObjectVisualAttrib &);
private:
	MaskObject * sceneObject_;
};

///////////////////////////////////////////////////////////////////////////////

class MaskObject : public SceneObject
{
public:
	virtual const char * typeStr() const 
	{
		return "MaskObject";
	}
	typedef SceneObject base_type;
	virtual boost::uuids::uuid type() const 
	{
		static boost::uuids::uuid uuid = 
			sUuidGen_("d9351b10-814c-11e2-9e96-0800200c9a66");
		return uuid;
	}

	virtual MaskObject * maskObject() {return this;}

	virtual void init(GfxRenderer * gl, bool firstTime = false);
	virtual void uninit();
	virtual bool update(float sceneTime);
	virtual bool asyncLoadUpdate();

	MaskObject();
	MaskObject(const MaskObject & rhs, ElementMapping * elementMapping);
	virtual SceneObject * clone(ElementMapping * elementMapping) const 
	{return new MaskObject(*this, elementMapping);}

	virtual void remapReferences(const ElementMapping & mapping);
	virtual void referencedFiles(std::vector<std::string> * refFiles) const;
	virtual int setReferencedFiles(
		const std::string & baseDirectory,
		const std::vector<std::string> & refFiles, int startIndex);

	virtual void setParentScene(Scene * parentScene);

	virtual void draw(GfxRenderer * gl) const;
	virtual void drawObject(GfxRenderer * gl) const;
	virtual VisualAttrib * visualAttrib() {return &visualAttrib_;}
	virtual BoundingBox extents() const;

	virtual unsigned char version() const {return 0;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);
	virtual const char * xmlTag() const {return "MaskObject";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void readXml(XmlReader & r, xmlNode * node);

	Group * maskingObjectGroup() const {return maskingObjectGroup_.get();}


	void computeMaskRegion(GfxRenderer * gl, float & x0, float & y0, float & x1, float & y1) const;
private:
	MaskObject(const MaskObject & rhs);
	MaskObject & operator = (const MaskObject & rhs);

	MaskObjectVisualAttrib visualAttrib_;

	boost::shared_ptr<Group> maskingObjectGroup_;	
	mutable Vector3 tempVerts_[8];
};