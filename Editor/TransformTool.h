#pragma once
#include "Tool.h"
#include "MathStuff.h"
#include "Transform.h"

class TransformGizmo;
class TranslateGizmo;
class RotateGizmo;
class ScaleGizmo;
class PivotGizmo;
class NullGizmo;

class SceneObject;
class TransformTool : public Tool
{
	friend class TransformGizmo;
	Q_OBJECT
public:
	enum Mode
	{
		TranslateMode,
		ScaleMode,
		PivotMode,
		SelectMode,
		RotateMode,
	};
public:
	TransformTool(GLWidget * gl);
	~TransformTool();

	virtual void init();
	virtual void uninit();
	virtual bool keyPressEvent(QKeyEvent * event){return false;}
	virtual void keyReleaseEvent(QKeyEvent * event){}
	virtual void mousePressEvent(QMouseEvent * event);
	virtual void mouseReleaseEvent(QMouseEvent * event);
	virtual void mouseMoveEvent(QMouseEvent * event);
	virtual bool hoveringOverGizmo(QMouseEvent * event);
	virtual void mouseDoublePressEvent(QMouseEvent * event);

	virtual void setActive(bool val);
		
	virtual void draw();

	void setMode(Mode mode);

	virtual bool isBeingUsed() const;
	
private slots:
	void onObjectChanged();
	void initTransformObjs();

private:
	void onTransformStarted();
	void onTransformChanged();
	void onTransformEnded();	
	
	

	void getSelTransformableObjects(std::vector<SceneObject *> * selOjbs);

	/**
	calculate parameters transforms coordinates etc for gizmo display
	*/
	void update();
		
private:
	Transform initTransform_;
	Transform transform_;
	TransformGizmo * gizmo_;
	std::vector<SceneObject *> selObjs_;
	std::map<SceneObject *, Transform> initTransforms_;


	TranslateGizmo * translateGizmo_;
	RotateGizmo * rotateGizmo_;
	ScaleGizmo * scaleGizmo_;
	PivotGizmo * pivotGizmo_;
	NullGizmo * nullGizmo_;

	bool isTransforming_;

	bool isShiftKey_;
	bool isCtrlKey_;
};