#pragma once
#include "SceneObject.h"
#include "Attrib.h"

///////////////////////////////////////////////////////////////////////////////

class ParentObjectVisualAttrib : public VisualAttrib
{
public:
	ParentObjectVisualAttrib(SceneObject * sceneObject);

	virtual void onSetTransform();

	ParentObjectVisualAttrib & operator = (const ParentObjectVisualAttrib & rhs);
	ParentObjectVisualAttrib & operator = (const VisualAttrib & rhs);

	unsigned char version() const {return 0;}
	void write(Writer & writer) const;
	void read(Reader & reader, unsigned char version);

private:
	ParentObjectVisualAttrib(const ParentObjectVisualAttrib &);
private:
	SceneObject * sceneObject_;
};

///////////////////////////////////////////////////////////////////////////////

class Group : public SceneObject
{
	friend class MaskObject;
public:
	virtual JSObject * createScriptObjectProto(ScriptProcessor * s);
	virtual JSObject * getScriptObject(ScriptProcessor * s);

	virtual const char * typeStr() const 
	{
		return "Group";
	}
	typedef SceneObject base_type;
	virtual boost::uuids::uuid type() const 
	{
		boost::uuids::string_generator gen;
		return gen("24ED0DF2-ED37-2799-CA10-56D59733772F");
	}

	Group();
	Group(const Group & rhs, ElementMapping * elementMapping);
	virtual SceneObject * clone(ElementMapping * elementMapping) const 
	{return new Group(*this, elementMapping);}

	virtual void remapReferences(const ElementMapping & mapping) 
	{SceneObject::remapReferences(mapping);}

	virtual Group * group() {return this;}

	virtual void drawObject(GfxRenderer * gl) const;
	virtual VisualAttrib * visualAttrib() {return &visualAttrib_;}
	virtual BoundingBox extents() const;

	virtual unsigned char version() const {return 1;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);
	virtual const char * xmlTag() const {return "Group";}
	virtual void writeXml(XmlWriter & x) const;
	virtual void readXml(XmlReader & r, xmlNode * node);

	virtual void requestLocalSaveObject();
private:
	void readXmlChildren(XmlReader & r, xmlNode * node);

	Group(const Group & rhs);
	Group & operator = (const Group & rhs);

	ParentObjectVisualAttrib visualAttrib_;
};

