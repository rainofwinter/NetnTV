#pragma once
#include "TransformGizmo.h"
#include "MathStuff.h"
#include "Transform.h"

class VisualAttrib;
class Mesh;
class Material;
class TransformTool;

class NullGizmo : public TransformGizmo
{

public:
	NullGizmo(GLWidget * gl, TransformTool * transformTool) : TransformGizmo(gl, transformTool) {}
	~NullGizmo() {}

	virtual void init() {}
	virtual void uninit() {}

	virtual void mousePressEvent(QMouseEvent * event, bool transformableObjClicked) {}
	virtual void mouseReleaseEvent(QMouseEvent * event) {}
	virtual void mouseMoveEvent(QMouseEvent * event) {}

	virtual void draw() {}

	virtual bool intersect(const QPoint & mousePos) const {return false;}

	virtual bool isActive() const {return false;}

};