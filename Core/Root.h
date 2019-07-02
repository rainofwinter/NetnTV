#pragma once
#include "SceneObject.h"

class Root : public SceneObject
{
public:
	
	virtual const char * typeStr() const 
	{
		return "Root";
	}
	virtual boost::uuids::uuid type() const 
	{
		static boost::uuids::uuid uuid = boost::uuids::string_generator()
			("A4E102F2-ED37-2799-C038-CBD597337F29");
		return uuid;
	}

	Root();
	Root(const Root & rhs, ElementMapping * elementMapping);

	virtual SceneObject * clone(ElementMapping * elementMapping) const 
	{return new Root(*this, elementMapping);}

	virtual void remapReferences(const ElementMapping & mapping) 
	{SceneObject::remapReferences(mapping);}

	virtual Group * group() {return 0;}

	virtual void draw(GfxRenderer * gl) const;

	virtual BoundingBox extents() const;

	virtual unsigned char version() const {return 0;}
	virtual void write(Writer & writer) const;
	virtual void writeXml(XmlWriter & w) const;
	virtual void read(Reader & reader, unsigned char);
	virtual void readXml(XmlReader & r, xmlNode * node);
	virtual const char * xmlTag() const {return "Root";}

	virtual VisualAttrib * visualAttrib() {return &visualAttrib_;}

	virtual void requestLocalSaveObject();

private:

	void readXmlChildren(XmlReader & r, xmlNode * node);

	Root & operator = (const Root &);
	Root(const Root &);

	//So that supportsObject(...) for MoveEvent, ReleaseEvent, etc.. will return true .
	VisualAttrib visualAttrib_;
};