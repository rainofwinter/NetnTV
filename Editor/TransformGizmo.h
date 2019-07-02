#pragma once
#include "MathStuff.h"

class Transform;
class GLWidget;
class TransformTool;

class TransformGizmo : public QObject
{		
public:
	TransformGizmo(GLWidget * glWidget, TransformTool * transformTool);

	virtual ~TransformGizmo() {}
	virtual void init() {}
	virtual void uninit() {}	
	virtual void mousePressEvent(QMouseEvent * event, bool transformableObjClicked) {}
	virtual void mouseMoveEvent(QMouseEvent * event) {}
	virtual void mouseReleaseEvent(QMouseEvent * event) {}
	virtual void mouseDoublePressEvent(QMouseEvent * event) {}
	virtual void draw() {}
	virtual void setTransformAttrib(Transform * transform)
	{
		transform_ = transform;
	}
	Transform * transformAttrib() const {return transform_;}
	virtual void setParentTransform(const Matrix & parentTransform)
	{
		parentTransform_ = parentTransform;
	}
	
	virtual bool isActive() const {return false;}

	virtual bool intersect(const QPoint & mousePos) const {return false;}
	
protected:
	void transformStarted();
	void transformEnded();
	void transformChanged();
	
protected:
	GLWidget * glWidget_;
	GfxRenderer * gl_;
	Transform * transform_;
	Matrix parentTransform_;

private:
	TransformTool * transformTool_;
};