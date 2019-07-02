#pragma once
#include "Tool.h"
#include "MathStuff.h"
#include "Transform.h"
#include "BoundingBox.h"
#include "AppObject.h"


struct InitTransformData
{
	//Transform initTransform;
	AppObjectTransform initTransform;
	AppObject * appObject;
};

class AppObject;

class AppObjectTransformTool : public Tool
{
	Q_OBJECT
public:
	AppObjectTransformTool(GLWidget * glWidget);
	~AppObjectTransformTool();

	virtual void init();
	virtual void uninit();

	virtual bool keyPressEvent(QKeyEvent * event);
	virtual void keyReleaseEvent(QKeyEvent * event);
	virtual void mousePressEvent(QMouseEvent * event);
	virtual void mouseReleaseEvent(QMouseEvent * event);
	virtual void mouseMoveEvent(QMouseEvent * event);
	virtual bool hoveringOverGizmo(QMouseEvent * event);


	virtual void setActive(bool val);
		
	virtual void draw();

	virtual bool isBeingUsed() const;

private:
	enum TransformMode
	{
		ResizeNW,
		ResizeW,
		ResizeSW,		
		ResizeS,		
		ResizeSE,
		ResizeE,		
		ResizeNE,
		ResizeN,
		Translate,
		None
	};

private:
	void setCursor(const Vector3 & mousePos);
	void startTransform(const Vector3 & mousePos);
	void computeBBoxAndHandlePts();
	void update();
	TransformMode transformMode(const Vector3 & mousePos);
	
	TransformMode transformMode_;
	bool transformStarted_;
	bool transformed_;
	Vector3 startMousePt_;	
	BoundingBox startBBox_;
	std::vector<Vector3> startHandlePts_;
	Matrix transformMatrix_;

	std::vector<Vector3> handlePts_;
	BoundingBox bBox_;

	std::vector<InitTransformData> initTransformData_;

	bool isShiftKey_;
	bool isCtrlKey_;

	//void getSelTransformableObjects(std::vector<SceneObject *> * selOjbs);

	/**
	calculate parameters transforms coordinates etc for gizmo display
	*/
};



