#pragma once
#include "Attrib.h"
#include "AppObject.h"

class AppWebContent : public AppObject
{
public:
	enum Mode
	{
		ModeUrl,
		ModeHtml
	};
public:
	AppWebContent();
	virtual const char * typeStr() const 
	{
		return "Web Content";
	}
	virtual boost::uuids::uuid type() const 
	{
		boost::uuids::string_generator gen;
		return gen("99988DF2-4D37-9799-2232-35F55733872F");
	}

	virtual AppObject * clone() const 
	{
		return new AppWebContent();
	}

	virtual VisualAttrib * visualAttrib() 
	{
		return &visualAttrib_;
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

	virtual const char * xmlTag() const {return "AppWebContent";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void readXml(XmlReader & r, xmlNode * parent);

	void setUrl(const std::string & url);
	const std::string & url() const {return url_;}

	void setHtml();
	const std::string & html() const {return html_;}
	void setHtml(const std::string & html);

	const Mode & mode() const {return mode_;}
	void setMode(const Mode & mode) {mode_ = mode;}

private:
	void create();


private:

	VisualAttrib visualAttrib_;
	Mode mode_;
	std::string url_;
	std::string html_;

};