#pragma once
class Scene;
class SceneObject;
class GfxRenderer;
class SceneReferencer
{
public:
	virtual void setScene(Scene * const & scene) = 0;
	virtual bool isInit() = 0;	
	virtual void init(GfxRenderer * gl, bool firstTime = false) = 0;
	virtual void uninit() = 0;
private:
};