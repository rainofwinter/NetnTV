#ifndef VideoPlayer_h__
#define VideoPlayer_h__

class Document;

class VideoPlayer
{
public:
	VideoPlayer() {document_ = 0;}
	virtual ~VideoPlayer() {}

	virtual void activate(int id, const std::string & fileName, 
		bool isUrl, bool showControls, float startTime) = 0;

	virtual void activateRegion(int id, const std::string & sourceName, 
		int x, int y, int width, int height, bool isUrl, 
		bool showControls, float startTime) = 0;

	//TODO account for scale	
	virtual void setRegion(int x, int y, int width, int height) = 0;

	virtual void deactivate() = 0;

	virtual float currentTime() const = 0;

	void setDocument(Document * document) 
	{
		document_ = document;
	}

private:
	Document * document_;
};

#endif // VideoPlayer_h__