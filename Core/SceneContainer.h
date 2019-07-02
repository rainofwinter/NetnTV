#pragma once

class SceneContainer
{
public:
	virtual void sceneChangeTo(Scene * scene) = 0;
	virtual void sceneSet(Scene * scene) = 0;
};