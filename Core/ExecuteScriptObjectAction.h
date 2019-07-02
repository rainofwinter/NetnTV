#ifndef ExecuteScriptObjectAction_h__
#define ExecuteScriptObjectAction_h__

#include "Action.h"

class Animation;

class ExecuteScriptObjectAction : public Action
{	
public:
	enum TargetType
	{
		SceneTarget,
		ExternalTarget
	};


	virtual boost::uuids::uuid type() const
	{
		boost::uuids::string_generator gen;
		return gen("8283087D-ADBC-500B-9775-A17AE68BE307");
	}

	virtual const char * typeStr() const
	{
		return "Broadcast message";
	}

	ExecuteScriptObjectAction();
	~ExecuteScriptObjectAction();
	//default copy constructor / operator =
	virtual Action * clone() const {return new ExecuteScriptObjectAction(*this);}
	virtual bool remapReferences(const ElementMapping & mapping);

	virtual Action & equals(const Action & rhs);

	virtual bool supportsObject(SceneObject * obj) const
	{
		return obj == 0;
	}

	const TargetType & targetType() const 
	{return targetType_;}

	void setTargetType(const TargetType & targetType) 
	{targetType_ = targetType;}

	virtual bool start(SceneObject * object, float time);
	virtual bool hasDuration() const;
	virtual bool isExecuting() const;
	virtual bool update(SceneObject * object, float time);	

	void setMessage(const std::string & message) {message_ = message;}
	const std::string & message() const {return message_;}

	void setScene(Scene * scene) {scene_ = scene;}
	Scene * scene() const {return scene_;}

	virtual bool dependsOn(Animation * animation) const {return false;}
	virtual bool dependsOn(Scene * scene) const;
	virtual bool dependsOn(SceneObject * obj) const {return false;}

	virtual unsigned char version() const {return 2;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);

	virtual const char * xmlTag() const {return "ExecuteScriptObject";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void readXml(XmlReader & r, xmlNode * parent);
private:
	std::string message_;
	Scene * scene_;
	TargetType targetType_;
};



#endif // ExecuteScriptObjectAction_h__