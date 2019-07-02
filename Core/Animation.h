#ifndef Animation_h__
#define Animation_h__

#include "Exception.h"
#include "Types.h"
class Scene;
class SceneObject;
class AnimationChannel;
class Animation;
class Writer;
class XmlWriter;
class XmlReader;
class Reader;
class ElementMapping;
class ScriptProcessor;

class Animation
{
	friend class Scene;
public:	

	static JSObject * createScriptObjectProto(ScriptProcessor * s);
	JSObject * getScriptObject(ScriptProcessor * s);
	
	JSObject * createJsonScriptObject(ScriptProcessor * s) const;
	void fromJsonScriptObject(ScriptProcessor * s, JSObject * obj);

	Animation();
	Animation(const Animation & rhs, ElementMapping * elementMapping);
	~Animation();

	/**
	@return true if obj is being animated by this animation
	*/
	bool hasObject(SceneObject * obj) const;

	bool update(float time);
	void setRepeat(const bool & val) {repeat_ = val;}
	const bool & repeat() const {return repeat_;}

	void remapReferences(const ElementMapping & elementMapping);
	
	ErrorCode addChannel(const AnimationChannelSPtr & channel);
	const std::vector<AnimationChannelSPtr> * channels(SceneObject * obj) const;
	
	void setChannels(
		SceneObject * obj, const std::vector<AnimationChannelSPtr> & channels);

	ErrorCode deleteChannel(AnimationChannel * channel);

	void objects(std::vector<SceneObject *> * objects) const;
	
	const std::string & name() const {return name_;}
	void setName(const std::string & name) {name_ = name;}

	float duration() const;

	void insertBefore(
		const AnimationChannelSPtr & newObj, AnimationChannel * refObj);
	

	AnimationChannelSPtr findChannel(AnimationChannel * channel) const;

	unsigned char version() const {return 0;}
	void write(Writer & writer) const;
	void writeXml(XmlWriter & w) const;
	void read(Reader & reader, unsigned char);
	void readXml(XmlReader & r, xmlNode * parent);

	void setParentScene(Scene * parentScene) {parentScene_ = parentScene;}
	Scene * parentScene() const {return parentScene_;}

	float lastUpdateTime() const {return lastUpdateTime_;}

private:	
	std::vector<AnimationChannelSPtr> * channelsToModify(SceneObject * obj);

	Animation(const Animation & rhs);
	Animation & operator = (const Animation & rhs);

private:
	struct ObjAnimChannels
	{
		SceneObject * object;
		std::vector<AnimationChannelSPtr> channels;

		unsigned char version() const {return 0;}
		void write(Writer & writer) const;
		void writeXml(XmlWriter & w) const;
		void read(Reader & reader, unsigned char);
		void readXml(XmlReader & r, xmlNode * parent);
	};

private:
	std::vector<ObjAnimChannels> objChannels_;	
	bool repeat_;
	std::string name_;
	Scene * parentScene_;
	float lastUpdateTime_;
	JSObject * scriptObject_;
};

#endif // Animation_h__

