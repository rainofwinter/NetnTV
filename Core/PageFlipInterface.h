#pragma once

class Texture;

enum TransitionDirection
{
	TransitionDirectionNone,
	TransitionDirectionLeft,
	TransitionDirectionRight,
	TransitionDirectionUp,
	TransitionDirectionDown
};


class PageFlipInterface
{
public:
	virtual void setPageDims(float w, float h) {}
	virtual ~PageFlipInterface() {}

	virtual void setTexture(Texture * texture) = 0;
	virtual void setGlTexture(GLuint tetxure) = 0;

	virtual void init(GfxRenderer * gl) = 0;
	virtual void uninit() = 0;
	virtual void draw(GfxRenderer * gl) = 0;
	virtual bool update() = 0;

	virtual GLuint texture() const = 0;

	virtual void pressEvent(const Vector2 & startPos, TransitionDirection pageFlipDir) = 0;
	virtual void releaseEvent(const Vector2 & pos) = 0;
	virtual void moveEvent(const Vector2 & pos) = 0;
	
	virtual bool doPageChange() const = 0;

	virtual void animate(TransitionDirection dir) = 0;
	virtual void reset() = 0;

	virtual TransitionDirection curTransitionDirection() const = 0;
private:
};
