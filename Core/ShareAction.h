#pragma once
#include "Action.h"

class Animation;

class ShareAction : public Action
{
public:
	enum TargetType
	{
		TextTarget,
		UrlTarget,
		ImageTarget
	};

	virtual boost::uuids::uuid type() const
	{
		boost::uuids::string_generator gen;
		return gen("C4604F8E-2B02-4B28-A73A-E59679F1160A");
	}

	virtual const char * typeStr() const
	{
		return "Share";
	}

	ShareAction();
	~ShareAction();
	
	virtual Action * clone() const {return new ShareAction(*this);}
	virtual bool remapReferences(const ElementMapping & mapping)
	{return Action::remapReferences(mapping);}

	virtual bool supportsObject(SceneObject * obj) const
	{
		return obj == 0;
	}	

	virtual Action & equals(const Action & rhs);

	virtual bool start(SceneObject * object, float time);
	virtual bool hasDuration() const;
	virtual bool isExecuting() const;
	virtual bool update(SceneObject * object, float time);

	virtual bool dependsOn(Animation * animation) const {return false;}
	virtual bool dependsOn(Scene * scene) const {return false;}
	virtual bool dependsOn(SceneObject * obj) const {return false;}

	virtual unsigned char version() const {return 1;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);

	virtual const char * xmlTag() const {return "Share";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void readXml(XmlReader & r, xmlNode * parent);

	
	virtual void referencedFiles(std::vector<std::string> * refFiles) const;
	virtual int setReferencedFiles(const std::string & baseDirectory,
		const std::vector<std::string> & refFiles, int index);

	const TargetType & targetType() const 
	{return targetType_;}

	void setTargetType(const TargetType & targetType) 
	{targetType_ = targetType;}	

	void setMessage(const std::string & message) {message_ = message;}
	const std::string & message() const {return message_;}

	void setFilePath(const std::string * filename) {filename_ = filename;}
	const std::string * filePath() const {return filename_;}

	void setImgNum(int imageNum) {imageNum_ = imageNum; }
	int imgNum() const {return imageNum_;}

private:
	std::string message_;	
	TargetType targetType_;
	int imageNum_;
	const std::string *filename_;	
};