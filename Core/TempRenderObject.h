#ifndef TempRenderObject_h__
#define TempRenderObject_h__

class GfxRenderer;

class TempRenderObject
{
public:
	virtual ~TempRenderObject() {}
	virtual void init(GfxRenderer * gl, bool firstTime = false) {}
	virtual void uninit() {}

	virtual void draw(GfxRenderer * gl) = 0;
	virtual void start(float time) = 0;
	virtual bool update(float time) = 0;

	virtual bool isStarted() const = 0;
	virtual bool isFinished() const = 0;
private:
};

#endif // TempRenderObject_h__