#pragma once
#include "SceneObject.h"
#include "Color.h"
#include "Attrib.h"
///////////////////////////////////////////////////////////////////////////////
class Mesh;
class Material;
class ScriptProcessor;
struct JSObject;

namespace Studio
{

class Rectangle : public SceneObject
{
public:
	virtual JSObject * createScriptObjectProto(ScriptProcessor * s);
	virtual JSObject * getScriptObject(ScriptProcessor * s);

	virtual const char * typeStr() const 
	{
		return "Rectangle";
	}
	typedef SceneObject base_type;
	virtual boost::uuids::uuid type() const 
	{
		boost::uuids::string_generator gen;
		return gen("14EDEDF2-4CD7-2799-1A12-51D5377987AF");
	}
	virtual SceneObject * clone(ElementMapping * elementMapping) const 
	{
		return new Rectangle(*this, elementMapping);
	}

	virtual void remapReferences(const ElementMapping & mapping) 
	{SceneObject::remapReferences(mapping);}

	Rectangle();	
	Rectangle(const Rectangle & rhs, ElementMapping * elementMapping);
	
	~Rectangle();

	virtual void drawObject(GfxRenderer * gl) const;
		
	virtual void init(GfxRenderer * gl, bool firstTime = false);
	virtual void uninit();

	virtual VisualAttrib * visualAttrib() {return &visualAttrib_;}

	virtual BoundingBox extents() const;

	virtual unsigned char version() const {return 0;}
	virtual void write(Writer & writer) const;
	virtual const char * xmlTag() const {return "Rectangle";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void read(Reader & reader, unsigned char);
	virtual void readXml(XmlReader & r, xmlNode * node);

	const float & width() const {return width_;}
	const float & height() const {return height_;}

	void setWidth(const float & width) {width_ = width;}
	void setHeight(const float & height) {height_ = height;}

	const Color & color() const {return color_;}

	void setColor(const Color & color)
	{
		color_ = color;
	}

	virtual SceneObject * intersect(Vector3 * intPt, const Ray & ray);

private:
	void create();
	Rectangle(const Rectangle & rhs);
	Rectangle & operator = (const Rectangle & rhs);
private:
	float width_;
	float height_;
	Color color_;
	VisualAttrib visualAttrib_;
};

}