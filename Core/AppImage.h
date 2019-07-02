#pragma once
#include "Attrib.h"
#include "AppObject.h"

class AppImage : public AppObject
{
public:
	AppImage();
	virtual const char * typeStr() const 
	{
		return "Image";
	}
	virtual boost::uuids::uuid type() const 
	{
		boost::uuids::string_generator gen;
		return gen("99988DF2-4D37-2799-1111-55555733872F");
	}

	virtual AppObject * clone() const 
	{
		return new AppImage();
	}

	virtual bool intersect(Vector3 * intPt, const Ray & ray);

	/**
	refFiles is added onto, and is not cleared.
	*/
	virtual void referencedFiles(std::vector<std::string> * refFiles) const;
	/**
	@return end index (= to next object's start index)
	*/
	virtual int setReferencedFiles(const std::string & baseDirectory,
		const std::vector<std::string> & refFiles, int startIndex);

	virtual BoundingBox extents() const;
	
	virtual unsigned char version() const {return 1;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);

	virtual const char * xmlTag() const {return "Image";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void readXml(XmlReader & r, xmlNode * parent);

	/**
	Default behavior is to set width and height to the size of the image file
	*/
	void setFileName(const std::string & fileName, bool resetDims = true);
	const std::string & fileName() const {return fileName_;}

private:
	void create();


private:

	std::string fileName_;

};