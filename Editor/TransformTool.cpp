#include "stdafx.h"
#include "TransformGizmo.h"
#include "TransformTool.h"
#include "GLWidget.h"
#include "Attrib.h"
#include "Camera.h"
#include "Mesh.h"
#include "Material.h"
#include "EditorDocument.h"
#include "EditorObject.h"
#include "Command.h"
#include "Scene.h"

#include "Utils.h"

#include "TranslateGizmo.h"
#include "RotateGizmo.h"
#include "ScaleGizmo.h"
#include "PivotGizmo.h"
#include "NullGizmo.h"

#include "EditorScene.h"

using namespace std;

TransformTool::TransformTool(GLWidget * gl) : Tool(gl)
{	
	translateGizmo_ = new TranslateGizmo(gl, this);
	rotateGizmo_ = new RotateGizmo(gl, this);
	scaleGizmo_ = new ScaleGizmo(gl, this);
	pivotGizmo_ = new PivotGizmo(gl, this);
	nullGizmo_ = new NullGizmo(gl, this);
	gizmo_ = nullGizmo_;

	isTransforming_ = false;

	isShiftKey_ = false;
	isCtrlKey_ = false;

	toolName_ = "TransformTool";
}

TransformTool::~TransformTool()
{	
	delete translateGizmo_;
	delete scaleGizmo_;
	delete rotateGizmo_;
	delete pivotGizmo_;
	delete nullGizmo_;
}

void TransformTool::setMode(Mode mode)
{	
	switch(mode)
	{
	case TranslateMode:
		gizmo_ = translateGizmo_;
		break;
	case ScaleMode:
		gizmo_ = scaleGizmo_;
		break;
	case PivotMode:
		gizmo_ = pivotGizmo_;
		break;
	case SelectMode:
		gizmo_ = nullGizmo_;
		break;                   
	case RotateMode:
		gizmo_ = rotateGizmo_;
		break;
	}
}


void TransformTool::init()
{	
	rotateGizmo_->init();
	scaleGizmo_->init();
	pivotGizmo_->init();
	translateGizmo_->init();
	nullGizmo_->init();
}

void TransformTool::uninit()
{
	rotateGizmo_->uninit();
	scaleGizmo_->uninit();
	pivotGizmo_->uninit();
	translateGizmo_->uninit();
	nullGizmo_->uninit();
}

bool TransformTool::hoveringOverGizmo(QMouseEvent * event)
{
	return gizmo_->intersect(event->pos());
}

void TransformTool::mousePressEvent(QMouseEvent * event)
{
	isShiftKey_ = glWidget_->shiftPress();
	isCtrlKey_ = glWidget_->ctrlPress();
	bool clickedGizmo = gizmo_->intersect(event->pos());
	
	Vector2 mousePosDevice = glWidget_->windowToDeviceCoords(
		Vector2(event->posF().x(), event->posF().y()));

	Ray ray = glWidget_->renderer()->unproject(mousePosDevice);

	vector<SceneObject *> hoverObjs;
	//document_->selectedScene()->intersectAll(&hoverObjs, ray);

	Scene * selScene = document_->selectedScene();
	EditorScene * selEdScene = document_->editorScene(selScene).get();	
	selEdScene->intersectAll(document_, ray, &hoverObjs);
	
	SceneObject * hoverObj = 0;
	EditorObject * edHoverObj = 0;
	BOOST_FOREACH(SceneObject * obj, hoverObjs)
	{
		EditorObject * edObj = document_->editorObject(obj).get();
		bool visible = obj ->isThisAndAncestorsVisible();
		//bool uiVisible = !edObj || edObj->isThisAndAncestorsVisible();
		if (visible && /*uiVisible && */!obj->group())
		{
			hoverObj = obj;
			edHoverObj = edObj;
		}
	}
	
	if (clickedGizmo)
	{
		gizmo_->mousePressEvent(event, hoverObj != 0);
	}
	else
	{
		bool unlocked = !edHoverObj || edHoverObj->isThisAndAncestorsUnlocked();
		if (hoverObj && unlocked)
		{
			if (isShiftKey_)			
				document_->setObjectSelected(hoverObj, true);
			else if (isCtrlKey_)
				document_->setObjectSelected(hoverObj, 
					!document_->isObjectSelected(hoverObj));
			else			
				document_->selectObject(hoverObj);	
			
			gizmo_->mousePressEvent(event, hoverObj != 0);
		}
		else if (!hoverObj)
		{
			/*
			Bug fix: kill focus to trigger editingFinished signal before selection is 
			cleared. Otherwise, a property page might try to apply changes to an empty
			selection.
			*/
			glWidget_->setFocus();
			document_->deselectAll();
		}
	}
}


void TransformTool::mouseReleaseEvent(QMouseEvent * event)
{
	gizmo_->mouseReleaseEvent(event);	
}

void TransformTool::mouseMoveEvent(QMouseEvent * event)
{
	gizmo_->mouseMoveEvent(event);
}
void TransformTool::mouseDoublePressEvent(QMouseEvent * event)
{	
	gizmo_->mouseDoublePressEvent(event);	
}
void TransformTool::setActive(bool val)
{
	Tool::setActive(val);

	//TODO sort out connections (when to disconnect, when to connect etc)

	document_->disconnect(this);

	if (val)
	{		
		connect(document_, SIGNAL(objectSelectionChanged()), 
			this, SLOT(initTransformObjs()));

		//need to update parent transforms after reparenting
		connect(document_, SIGNAL(objectListChanged()), 
			this, SLOT(initTransformObjs()));

		//when object transform changes externally (user type-in or animation)
		//then the gizmo might need to be changed accordingly
		connect(document_, SIGNAL(objectChanged()),
			this, SLOT(onObjectChanged()));

		//same issue as above
		connect(document_, SIGNAL(timeLineTimeChanged()),
			this, SLOT(onObjectChanged()));

		connect(document_, SIGNAL(timeLineTimeChanging()),
			this, SLOT(onObjectChanged()));

		initTransformObjs();
	}
	
}


void TransformTool::draw()
{
	gizmo_->draw();	
	
}

void TransformTool::initTransformObjs()
{	
	static TransformGizmo * sPrevGizmo = NULL;

	vector<SceneObject *> prevSelObjs = selObjs_;	
	getSelTransformableObjects(&selObjs_);
	bool selChanged = prevSelObjs != selObjs_;

	bool gizmoChanged = gizmo_ != sPrevGizmo;
	sPrevGizmo = gizmo_;
	
	if (selObjs_.size() == 1)
	{
		SceneObject * selObj = selObjs_.front();
		initTransform_ = transform_ = selObj->transform();
		gizmo_->setTransformAttrib(&transform_);
		gizmo_->setParentTransform(selObj->parentTransform());
	}
	else if (selObjs_.size() > 1)
	{
		BoundingBox bbox;
		BOOST_FOREACH(SceneObject * selObj, selObjs_)		
			bbox = bbox.unite(selObj->worldExtents());		
		
		Vector3 prevPivot = transform_.globalPivot();
		transform_ = Transform();		
		if (selChanged)		
			transform_.setPivot(bbox.midPt());
		else
			transform_.setPivot(prevPivot);

		initTransform_ = transform_;
		gizmo_->setTransformAttrib(&transform_);
		gizmo_->setParentTransform(Matrix::Identity());
	}
	else
	{
		gizmo_->setTransformAttrib(0);
		gizmo_->setParentTransform(Matrix::Identity());
	}

	initTransforms_.clear();
	BOOST_FOREACH(SceneObject * selObj, selObjs_)
	{
		initTransforms_[selObj] = selObj->transform();
	}

}

void TransformTool::onTransformStarted()
{
	isTransforming_ = true;
	initTransformObjs();
}

void TransformTool::onTransformChanged()
{
	if (selObjs_.size() == 1)
	{
		SceneObject * selObj = selObjs_.front();
		selObj->setTransform(transform_);				
	}
	else if (selObjs_.size() > 1)
	{		
		BOOST_FOREACH(SceneObject * selObj, selObjs_)
		{
			Transform transform = initTransforms_[selObj];
			Matrix parentTransform = selObj->parentTransform();

			transform.concatenate(
				parentTransform.inverse() * 
				transform_.computeMatrix() * 
				parentTransform, true);

			selObj->setTransform(transform);
		}
	}	

	emit objectChanged();
}

void TransformTool::onTransformEnded()
{
	vector<Transform> transforms;
	vector<Transform> initTransforms;

	if (!(initTransform_ == transform_))
	{

		if (selObjs_.size() == 1)
		{
			SceneObject * selObj = selObjs_.front();

			initTransforms.push_back(initTransforms_[selObj]);
			transforms.push_back(selObj->transform());			
			
			document_->doCommand(new GizmoTransformCmd(
				selObjs_, initTransforms, transforms,
				&transform_, initTransform_, transform_));			
		}
		else if (selObjs_.size() > 1)
		{
			BOOST_FOREACH(SceneObject * selObj, selObjs_)
			{
				initTransforms.push_back(initTransforms_[selObj]);
				transforms.push_back(selObj->transform());			
			}

			document_->doCommand(new GizmoTransformCmd(
				selObjs_, initTransforms, transforms,
				&transform_, initTransform_, transform_));
		}
	}	

	isTransforming_ = false;
	
}

void TransformTool::getSelTransformableObjects(vector<SceneObject *> * selObjs)
{
	selObjs->clear();
	*selObjs = document_->selectedObjects();
	vector<SceneObject *>::iterator iter;

	for (iter = selObjs->begin(); iter != selObjs->end();)
	{
		if (!(*iter)->visualAttrib())
		{
			iter = selObjs->erase(iter);
		}
		else
			++iter;
	}
}

void TransformTool::onObjectChanged()
{
	if (isTransforming_) return;

	initTransformObjs();
}

bool TransformTool::isBeingUsed() const
{
	return isTransforming_;
}