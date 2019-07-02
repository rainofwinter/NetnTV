#ifndef PlayVideoAction_h__
#define PlayVideoAction_h__

#include "Action.h"
#include "SourceType.h"


class Animation;

class PlayVideoAction : public Action
{	
public:
	virtual boost::uuids::uuid type() const
	{
		boost::uuids::string_generator gen;
		return gen("8223104D-D14C-500B-9775-A17AE68BE307");
	}

	virtual const char * typeStr() const
	{
		return "Play video";
	}

	PlayVideoAction();
	~PlayVideoAction();
	//default copy constructor / operator =
	virtual Action * clone() const {return new PlayVideoAction(*this);}
	virtual bool remapReferences(const ElementMapping & mapping)
	{return Action::remapReferences(mapping);}
	
	virtual void referencedFiles(std::vector<std::string> * refFiles) const;
	virtual int setReferencedFiles(const std::string & baseDirectory,
		const std::vector<std::string> & refFiles, int index);

	virtual Action & equals(const Action & rhs);

	virtual bool supportsObject(SceneObject * obj) const
	{
		return obj == 0;
	}

	virtual bool start(SceneObject * object, float time);
	virtual bool hasDuration() const;
	virtual bool isExecuting() const;
	virtual bool update(SceneObject * object, float time);	

	void setVideo(const std::string & source) {source_ = source;}
	const std::string & video() const {return source_;}

	virtual bool dependsOn(Animation * animation) const {return false;}
	virtual bool dependsOn(Scene * scene) const {return false;}
	virtual bool dependsOn(SceneObject * obj) const {return false;}

	virtual unsigned char version() const {return 4;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);
	virtual const char * xmlTag() const {return "PlayVideo";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void readXml(XmlReader & r, xmlNode * parent);

	int x() const {return x_;}
	void setX(float x) {x_ = x;}

	int y() const {return y_;}
	void setY(float y) {y_ = y;}
	
	int width() const {return width_;}
	void setWidth(int width) {width_ = width;}

	int height() const {return height_;}
	void setHeight(int height) {height_ = height;}

	bool isPlayInRegion() const {return playInRegion_;}
	void setPlayInRegion(bool val) {playInRegion_ = val;}

	const SourceType & sourceType() const {return sourceType_;}
	void setSourceType(const SourceType & sourceType) 
	{
		sourceType_ = sourceType;
	}

	void setShowPlaybackControls(const bool & val) 
	{
		showPlaybackControls_ = val;
	}
	
	const bool & showPlaybackControls() const
	{
		return showPlaybackControls_;
	}

	const float & startTime() const {return startTime_;}
	void setStartTime(const float & startTime) {startTime_ = startTime;}

private:
	std::string source_;

	SourceType sourceType_;


	bool playInRegion_;
	bool showPlaybackControls_;
	int x_;
	int y_;
	int width_;
	int height_;

	float startTime_;

	bool isPlaying_;
};



#endif // PlayVideoAction_h__