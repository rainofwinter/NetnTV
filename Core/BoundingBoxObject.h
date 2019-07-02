#pragma once
#include "BoundingBox.h"

#include "SceneObject.h"
#include "Attrib.h"

///////////////////////////////////////////////////////////////////////////////
/**
A bounding box starting at (0, 0, 0) with sides of length 1
with transform applied
*/


class BoundingBoxObject : public SceneObject
{
public:
	virtual JSObject * createScriptObjectProto(ScriptProcessor * s);
	virtual JSObject * getScriptObject(ScriptProcessor * s);

	virtual const char * typeStr() const 
	{
		return "Bounding Box";
	}
	typedef SceneObject base_type;
	virtual boost::uuids::uuid type() const 
	{
		static boost::uuids::uuid uuid = boost::uuids::string_generator()			
			("911DF180-0BA0-95BC-8AFC-6A80A840DCEF");
		return uuid;
	}
	virtual SceneObject * clone(ElementMapping * elementMapping) const 
	{return new BoundingBoxObject(*this, elementMapping);}

	bool update(float sceneTime);
	
	virtual bool asyncLoadUpdate();
	virtual bool isLoaded() const;

	BoundingBoxObject();
	BoundingBoxObject(const BoundingBoxObject & rhs, ElementMapping * elementMapping);

	~BoundingBoxObject();

	virtual void drawObject(GfxRenderer * gl) const;
		
	virtual void init(GfxRenderer * gl, bool firstTime = false);
	virtual void uninit();

	virtual BoundingBox extents() const;

	virtual unsigned char version() const {return 0;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);	
	virtual const char * xmlTag() const {return "TextTable";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void readXml(XmlReader & r, xmlNode * node);

	virtual SceneObject * intersect(Vector3 * intPt, const Ray & ray);

	bool intersect(BoundingBoxObject * bbo) const;

	virtual VisualAttrib * visualAttrib() {return &visualAttrib_;}

	OBoundingBox toObb() const;

private:
	BoundingBoxObject(const BoundingBoxObject & rhs);
	BoundingBoxObject & operator = (const BoundingBoxObject & rhs);
	void create();	

private:
	VisualAttrib visualAttrib_;
};

