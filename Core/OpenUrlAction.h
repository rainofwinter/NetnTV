#ifndef OpenUrlAction_h__
#define OpenUrlAction_h__

#include "Action.h"

class Animation;

class OpenUrlAction : public Action
{	
public:
	enum Mode
	{
		Regular,
		Mobile		
	};

public:
	virtual boost::uuids::uuid type() const
	{
		boost::uuids::string_generator gen;
		return gen("808808DD-D14C-500B-9775-A17AE68BE307");
	}

	virtual const char * typeStr() const
	{
		return "Open Url";
	}

	OpenUrlAction();
	~OpenUrlAction();
	//default copy constructor / operator =
	virtual Action * clone() const {return new OpenUrlAction(*this);}
	virtual bool remapReferences(const ElementMapping & mapping)
	{return Action::remapReferences(mapping);}

	virtual Action & equals(const Action & rhs);

	virtual bool supportsObject(SceneObject * obj) const
	{
		return obj == 0;
	}

	virtual bool start(SceneObject * object, float time);
	virtual bool hasDuration() const;
	virtual bool isExecuting() const;
	virtual bool update(SceneObject * object, float time);	

	void setUrl(const std::string & url) {url_ = url;}
	const std::string & url() const {return url_;}

	void setMode(const Mode & mode) {mode_ = mode;}
	const Mode & mode() const {return mode_;}

	virtual bool dependsOn(Animation * animation) const {return false;}
	virtual bool dependsOn(Scene * scene) const {return false;}
	virtual bool dependsOn(SceneObject * object) const {return false;}

	virtual unsigned char version() const {return 1;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);

	virtual const char * xmlTag() const {return "OpenURL";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void readXml(XmlReader & r, xmlNode * parent);
private:
	std::string url_;  
	Mode mode_;
};



#endif // OpenUrlAction_h__