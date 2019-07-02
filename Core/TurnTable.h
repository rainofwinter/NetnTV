#pragma once
#include "SceneObject.h"
#include "Attrib.h"
#include "PressEventData.h"
///////////////////////////////////////////////////////////////////////////////
class Texture;
class Mesh;
class Material;

class ScriptProcessor;

class TurnTable : public SceneObject
{
public:
	virtual JSObject * createScriptObjectProto(ScriptProcessor * s);
	virtual JSObject * getScriptObject(ScriptProcessor * s);

	virtual const char * typeStr() const 
	{
		return "TurnTable";
	}
	typedef SceneObject base_type;
	virtual boost::uuids::uuid type() const 
	{
		boost::uuids::string_generator gen;
		return gen("24ED0DF2-4D37-2799-4321-56133733872F");
	}
	virtual SceneObject * clone(ElementMapping * elementMapping) const 
	{return new TurnTable(*this, elementMapping);}

	virtual void remapReferences(const ElementMapping & mapping) 
	{SceneObject::remapReferences(mapping);}

	virtual void referencedFiles(std::vector<std::string> * refFiles) const;
	virtual int setReferencedFiles(const std::string & baseDirectory,
		const std::vector<std::string> & refFiles, int index);

	TurnTable();
	TurnTable(const TurnTable & rhs, ElementMapping * elementMapping);

	~TurnTable();

	void setFrameIndex(int index);
	int frameIndex() const {return curImgIndex_;}

	void setImgFileNames(const std::vector<std::string> & fileNames);
	const std::vector<std::string> & imgFileNames() const {return imgFileNames_;}

	virtual void drawObject(GfxRenderer * gl) const;
		
	virtual void init(GfxRenderer * gl, bool firstTime = false);
	virtual void uninit();

	const float & speed() const {return speed_;}
	void setSpeed(const float & speed) {speed_ = speed;}

	virtual VisualAttrib * visualAttrib() {return &visualAttrib_;}

	virtual BoundingBox extents() const;

	virtual unsigned char version() const {return 1;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);
	virtual const char * xmlTag() const {return "TurnTable";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void readXml(XmlReader & r, xmlNode * node);

	int width() const;
	int height() const;

	virtual bool asyncLoadUpdate();

	virtual SceneObject * intersect(Vector3 * intPt, const Ray & ray);

	virtual bool update(float sceneTime);
	virtual bool pressEvent(const Vector2 & startPos, int pressId);
	virtual bool moveEvent(const Vector2 & pos, int pressId);
	virtual bool releaseEvent(const Vector2 & pos, int pressId);

private:
	TurnTable(const TurnTable & rhs);
	TurnTable & operator = (const TurnTable & rhs);
	void create();

	Vector2 localCoords(
		const Vector2 & pos, const Matrix & invTransMatrix);
private:
	mutable Texture * texture_;
	mutable Texture * textureBack_;

	VisualAttrib visualAttrib_;

	bool redrawTriggered_;
/*
	bool isDragging_;
	int curPressId_;
	Vector2 pressPos_;
*/
	int pressId_;
	PressEventData pressEvent_;
	int pressImgIndex_;

	mutable std::vector<std::string> imgFileNames_;
	int curImgIndex_;

	Matrix invTransMatrix_;
	
	int width_, height_;

	float speed_;

};