#ifndef EffectAction_h__
#define EffectAction_h__
#include "MathStuff.h"
#include "Transform.h"
#include "Action.h"
#include "Attrib.h"

class Animation;

class EffectAction : public Action
{	
public:
	enum Effect
	{
		FadeIn = 0,
		FadeOut,
		FlipDown,
		SpinHorz,
		SpinVert,
		
	};
public:
	virtual boost::uuids::uuid type() const
	{
		boost::uuids::string_generator gen;
		return gen("1410D0FD-CCCC-5222-9021-014AE68BE107");
	}

	virtual const char * typeStr() const
	{
		return "Effect";
	}

	EffectAction();
	~EffectAction();
	//default copy constructor / operator =
	virtual Action * clone() const {return new EffectAction(*this);}
	virtual bool remapReferences(const ElementMapping & mapping)
	{return Action::remapReferences(mapping);}
	
	virtual Action & equals(const Action & rhs);

	virtual bool supportsObject(SceneObject * obj) const;
	virtual bool needsScenePreStart() const {return true;}

	virtual bool start(SceneObject * object, float time);
	virtual void stop();
	virtual bool hasDuration() const;
	virtual bool isExecuting() const;
	virtual bool update(SceneObject * object, float time);	

	float duration() const {return duration_;}
	void setDuration(float time) {duration_ = time;}

	Effect effect() const {return effect_;}
	void setEffect(const Effect & effect) {effect_ = effect;}

	virtual bool dependsOn(Animation * animation) const {return false;}
	virtual bool dependsOn(Scene * scene) const {return false;}
	virtual bool dependsOn(SceneObject * obj) const;

	virtual unsigned char version() const {return 1;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);

	virtual const char * xmlTag() const {return "Effect";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void readXml(XmlReader & r, xmlNode * parent);
private:
	float duration_;
	Effect effect_;


	float startTime_;
	float endTime_;	
	bool isExecuting_;

	Vector3 axis_;
	Vector3 imgOrigin_;
	VisualAttrib initVisualAttrib_;
};



#endif // EffectAction_h__