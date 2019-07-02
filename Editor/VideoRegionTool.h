#pragma once
#include "Tool.h"
#include "MathStuff.h"
#include "Transform.h"
#include "BoundingBox.h"

struct VideoRegion
{
	int x;
	int y;
	int width;
	int height;
};

class VideoObject;

class VideoRegionTool : public Tool
{
	Q_OBJECT
public:
	VideoRegionTool(GLWidget * glWidget);
	~VideoRegionTool();

	void setObject(VideoObject * object);

	virtual void init();
	virtual void uninit();

	virtual void mousePressEvent(QMouseEvent * event);
	virtual void mouseReleaseEvent(QMouseEvent * event);
	virtual void mouseMoveEvent(QMouseEvent * event);
	virtual bool hoveringOverGizmo(QMouseEvent * event);


	virtual void setActive(bool val);
		
	virtual void draw();

	virtual bool isBeingUsed() const;

private slots:

	void onObjectSelectionChanged();

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
	TransformMode transformMode(const Vector3 & mousePos);
	
	TransformMode transformMode_;
	bool transformStarted_;
	bool transformed_;
	Vector3 startMousePt_;
	Matrix transformMatrix_;

	std::vector<Vector3> handlePts_;
	BoundingBox bBox_;

	VideoRegion initRegion_;
	float handlePixelSize_;
	BoundingBox startBBox_;
	std::vector<Vector3> startHandlePts_;

	//void getSelTransformableObjects(std::vector<SceneObject *> * selOjbs);

	/**
	calculate parameters transforms coordinates etc for gizmo display
	*/
	void update();
		
	VideoObject * object_;

};



