#pragma once
#include "SceneObject.h"
#include "Attrib.h"
#include "MathStuff.h"
#include "Camera.h"

///////////////////////////////////////////////////////////////////////////////
class Texture;
class Mesh;
class Material;
class TextureRenderer;


class Panorama : public SceneObject
{
	enum
	{
		Back, Down, Front, Left, Right, Top
	};

public:
	virtual const char * typeStr() const 
	{
		return "Panorama";
	}
	virtual boost::uuids::uuid type() const 
	{
		boost::uuids::string_generator gen;
		return gen("244321F2-AAA7-2799-1A12-56D12343872F");
	}
	virtual SceneObject * clone(ElementMapping * elementMapping) const 
	{return new Panorama(*this, elementMapping);}

	virtual void remapReferences(const ElementMapping & mapping) 
	{SceneObject::remapReferences(mapping);}

	virtual void referencedFiles(std::vector<std::string> * refFiles) const;
	virtual int setReferencedFiles(const std::string & baseDirectory,
		const std::vector<std::string> & refFiles, int index);

	bool update(float sceneTime);

	virtual bool asyncLoadUpdate();
	virtual bool isLoaded() const;

	Panorama();
	Panorama(const Panorama & rhs, ElementMapping * elementMapping);

	~Panorama();

	const std::string & backFile() const {return fileNames_[Back];}
	const std::string & downFile() const {return fileNames_[Down];}
	const std::string & frontFile() const {return fileNames_[Front];}
	const std::string & leftFile() const {return fileNames_[Left];}
	const std::string & rightFile() const {return fileNames_[Right];}
	const std::string & topFile() const {return fileNames_[Top];}

	void setBackFile(const std::string & file) {fileNames_[Back] = file;}
	void setDownFile(const std::string & file) {fileNames_[Down] = file;}
	void setFrontFile(const std::string & file) {fileNames_[Front] = file;}
	void setLeftFile(const std::string & file) {fileNames_[Left] = file;}
	void setRightFile(const std::string & file) {fileNames_[Right] = file;}
	void setTopFile(const std::string & file) {fileNames_[Top] = file;}

	virtual void drawObject(GfxRenderer * gl) const;
		
	virtual void init(GfxRenderer * gl, bool firstTime = false);
	virtual void uninit();

	virtual bool pressEvent(const Vector2 & startPos, int pressId);
	virtual bool releaseEvent(const Vector2 & pos, int pressId);
	virtual bool moveEvent(const Vector2 & pos, int pressId);

	virtual VisualAttrib * visualAttrib() {return &visualAttrib_;}

	virtual BoundingBox extents() const;

	virtual unsigned char version() const {return 1;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);
	virtual const char * xmlTag() const {return "Panorama";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void readXml(XmlReader & r, xmlNode * node);

	const float & width() const;
	void setWidth(const float & width) {width_ = width;}

	const float & height() const;
	void setHeight(const float & height) {height_ = height;}



	virtual SceneObject * intersect(Vector3 * intPt, const Ray & ray);

private:
	Panorama(const Panorama & rhs);
	Panorama & operator = (const Panorama & rhs);
	void create();

	Vector2 localCoords(const Vector2 & pos, 
		const Matrix & invTransMatrix) const;

	bool intersectRect(Vector3 * out, const Ray & ray) const;
private:
	Mesh * mesh_;
	float width_; 
	float height_;

	TextureRenderer * textureRenderer_;
	Texture * texture_;


	Material * materials_[6];
	Texture * textures_[6];		
	std::string fileNames_[6];

	VisualAttrib visualAttrib_;

	bool pressed_;
	bool startedMoving_;
	int curPressId_;	
	Vector2 pressPos_;
	float pressCamXAngle_;
	float pressCamYAngle_;
	Matrix invTransMatrix_;
	Matrix transMatrix_;

	float camXAngle_;
	float camYAngle_;
	Camera camera_;

	bool redrawTriggered_;

};

