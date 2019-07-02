#include "stdafx.h"
#include "GLWidget.h"

#include "Camera.h"
#include "Transform.h"
#include "Document.h"
#include "Scene.h"
#include "EditorDocument.h"
#include "EditorScene.h"
#include "EditorImage.h"

#include "TransformTool.h"
#include "AppObjectTransformTool.h"

#include "Utils.h"
#include "Root.h"
#include "Command.h"
#include "EditorGlobal.h"


#include "Writer.h"
#include "Reader.h"
#include "FileUtils.h"

#include "MainWindow.h"

#include "Image.h"
#include "Global.h"
#include "Model.h"

#include "CameraObject.h"

#include "Xml.h"

extern const char * buildInfoStr;

using namespace std;

GLWidget::GLWidget(MainWindow * mainWindow) 
: QGLWidget(new QGLContext(QGLFormat(/*QGL::SampleBuffers*/)))
{	
	mainWindow_ = mainWindow;
	saveTemporary_ = false;
	shiftPress_ = false;
	ctrlPress_ = false;
	isAltKey_= false;
	isCtrlKey_ = false;
	isShiftKey_ = false;
	isLeftMouseButton_ = false;
	isRightMouseButton_ = false;
	isMiddleMouseButton_ = false;
	isGLWidgetKeyPressed_ = true;
	create();	
}


void GLWidget::initNewFile(const QString & fileName, int width, int height)
{
	fileName_ = fileName;
	width_ = width;
	height_ = height;

	Document * sceneDocument;
	sceneDocument = new Document(width_, height_);
	//set document size information.
	//Don't do it here. Wait until after we have a valid GL context
	
	//set width and height of the first scene to be equal to the document
	//dimensions
	sceneDocument->scenes().front()->setScreenWidth(width_);
	sceneDocument->scenes().front()->setScreenHeight(height_);
	sceneDocument_ = sceneDocument;
	initCommon();

	QFileInfo fileInfo(fileName_);

	QString fn;
	fn = fileInfo.baseName() + tr("~0.tmp");

	fn = stdStringToQString(Global::instance().readDirectory()) + fn;
	Writer(qStringToStdString(fn).c_str(), 0).
		write(sceneDocument_);
}

void GLWidget::initExistingFile(const QString & fileName)
{	
	fileName_ = fileName;

	std::string dir = 
		getDirectory(qStringToStdString(fileName_)).c_str();
	Global::instance().setDirectories(dir, dir, "");
	chdir(dir.c_str());

	Document * sceneDocument;
	Reader(qStringToStdString(fileName_).c_str()).read(sceneDocument);
	//if (getFileExtension(qStringToStdString(fileName_)) == "st")
	//	Reader(qStringToStdString(fileName_).c_str()).read(sceneDocument);
	//else if (getFileExtension(qStringToStdString(fileName_)) == "xml")
	//{
	//	XmlReader xmlReader;
	//	sceneDocument = xmlReader.read(qStringToStdString(fileName_).c_str());	
	//}
	
	width_ = sceneDocument->width();
	height_ = sceneDocument->height();	
	sceneDocument_ = sceneDocument;
	initCommon();

	QFileInfo fileInfo(fileName_);

	QString fn;
	fn = fileInfo.baseName() + tr("~0.tmp");

	fn = stdStringToQString(Global::instance().readDirectory()) + fn;
	Writer(qStringToStdString(fn).c_str(), 0).
		write(sceneDocument_);
}

void GLWidget::create()
{
	document_ = 0;
	isCameraMode_ = false;
	setMouseTracking(true);
	camera_.reset(new Camera);
	cameraInitState_.reset(new Camera);
	curTool_ = 0;
	camModel_ = NULL;

	setAcceptDrops(true);

	installEventFilter(this);
	setAttribute(Qt::WA_OpaquePaintEvent);
	setAttribute(Qt::WA_PaintOnScreen);	

	//setFocusPolicy(Qt::ClickFocus);
	setFocusPolicy(Qt::StrongFocus);

	interval_ = EditorGlobal::instance().tempfileInterval();
	if (interval_ > 0)
		timer_.start(1000 * 60 * interval_, this);
}

void GLWidget::initCommon()
{
	document_ = new EditorDocument(this, sceneDocument_);
	document_->init();
	gl_ = sceneDocument_->renderer();

	transformTool_ = new TransformTool(this);
	tools_.push_back(transformTool_);

	appObjectTransformTool_ = new AppObjectTransformTool(this);
	tools_.push_back(appObjectTransformTool_);

	ssWidth_ = ssHeight_ = 0;
	//ssPbo_ = 0;
}


GLWidget::~GLWidget()
{
	makeCurrent();

	if (timer_.isActive())
		timer_.stop();


	//must make context current to ensure deletion functions reach proper target
	//objects

	delete document_;
	delete sceneDocument_;

	delete camModel_;
	delete jointModel_;
	delete boneModel_;
	delete lightModel_;

	//glDeleteBuffers(1, &ssPbo_);
	foreach(Tool * tool, tools_) delete tool;
}

Vector2 GLWidget::windowToDeviceCoords(const Vector2 & coords) const
{
	return Vector2(2*coords.x/width() - 1, 2*(1 - coords.y/height()) - 1);
}

Vector2 GLWidget::deviceToWindowCoords(const Vector2 & coords) const
{
	return Vector2(width()*(coords.x + 1)/2, height()*(-coords.y + 1)/2);
}


void GLWidget::initializeGL()
{
	makeCurrent();
	GLenum err=glewInit();
	Global::setUseStencilBuffer(GLEW_EXT_packed_depth_stencil);
	//glPolygonMode(GL_FRONT_AND_BACK , GL_LINE);

	glEnable(GL_POLYGON_OFFSET_LINE);
	//glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	glEnable(GL_STENCIL_TEST);
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);
	gl_->init(false);

	camModel_ = initModel(":/data/Resources/camera.stm");
	jointModel_ = initModel(":/data/Resources/joint.stm");
	boneModel_ = initModel(":/data/Resources/bone.stm");
	lightModel_ = initModel(":/data/Resources/directionalLight.stm");
	
	
	BOOST_FOREACH(Tool * tool, tools_) tool->init();

	//set document size information.
	sceneDocument_->setSize(width_, height_, 1, 1, false);
	sceneDocument_->init();

	if (!document_->selectedScene())
	{
		vector<Scene *> selScenes;
		selScenes.push_back(document_->scenes().front().get());
		document_->setSelectedScenes(selScenes);
	}
	Scene * scene = document_->selectedScene();
	*camera_ = *scene->camera();	

	connect(document_, SIGNAL(sceneSelectionChanged()), this, SLOT(onSceneSelectionChanged()));

	setToolMode(Translate);
}

void GLWidget::getCameraScaleFactor(float & sx, float & sy) const
{
	CameraObject * userCam = document_->selectedScene()->userCamera();
	Camera cam = *camera();
	sx = 1.0f;
	sy = 1.0f;
	
	if (userCam)
	{
		*camera_ = *userCam->camera();
		camera_->setAspectRatio(aspectRatio());

		//Have to make adjustments to make sure custom camera fits into screen 
		//outline properly
		Scene * scene = document_->selectedScene();
		float aspect = (float)scene->screenWidth()/scene->screenHeight();
		float windowAspect = (float)width()/height();	
		cam.setAspectRatio(windowAspect);		
		if (aspect > windowAspect) 
		{		
			sy = windowAspect / aspect; 
			sx = sy;
		}
	}
}

Matrix GLWidget::getCameraMatrix() const
{
	float sx, sy;
	getCameraScaleFactor(sx, sy);
	return Matrix::Scale(sx, sy, 1) * camera()->GetMatrix();
}

void GLWidget::paintGL()
{	
	gl_->beginDraw();

	Matrix camMat = getCameraMatrix();

	gl_->setCameraMatrix(camMat);	

	document_->draw();

	gl_->setCameraMatrix(camMat);
	if (curTool_)curTool_->draw();	
	gl_->endDraw();
}

void GLWidget::screenShot(unsigned char * buffer, Scene * scene, int width, int height, bool preserveAspect)
{	

	if (ssWidth_ != width || ssHeight_ != height)
	{
		/*
		glDeleteBuffers(1, &ssPbo_);
		glGenBuffers(1, &ssPbo_);

		glBindBuffer(GL_PIXEL_PACK_BUFFER, ssPbo_);
		glBufferData(GL_PIXEL_PACK_BUFFER, (width+1)*(height+1)*2, 0, GL_STREAM_READ);	
		glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
		*/
		ssWidth_ = width;
		ssHeight_ = height;
	}
	
	Color bgColor = scene->bgColor();
	glClearColor(bgColor.r, bgColor.g, bgColor.b, bgColor.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glViewport(0, 0, ssWidth_, ssHeight_);	
	
	
	Matrix prevCamMatrix = gl_->cameraMatrix();
	
	
	Camera camera = *scene->camera();
	float sx = 1.0f, sy = 1.0f;	
	
	if (preserveAspect)
	{
		float aspect = (float)scene->screenWidth()/scene->screenHeight();
		float ssAspect = (float)ssWidth_/ssHeight_;		
		camera.setAspectRatio(ssAspect);		
		if (aspect > ssAspect) 
		{		
			sy = ssAspect / aspect; 
			sx = sy;
		}		
		
	}

	gl_->setCameraMatrix(Matrix::Scale(sx, sy, 1) * camera.GetMatrix());
	gl_->beginDraw();
	scene->draw(gl_);	
	gl_->endDraw();
/*
	/*
	glBindBuffer(GL_PIXEL_PACK_BUFFER, ssPbo_);
	glReadPixels(0, 0, ssWidth_, ssHeight_, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, 0);

	unsigned char * src = (unsigned char *)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
	if (src)
	{
		memcpy(buffer, src, (ssWidth_+1)*(ssHeight_+1)*2);
		glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
	}

	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
	*/

	glReadPixels(0, 0, ssWidth_, ssHeight_, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, buffer);

	//flip image upside down
	static std::vector<unsigned short> tempRow;
	int stride = ssWidth_;
	if (stride % 2 == 1) ++stride;
	tempRow.resize(stride);
	unsigned short * sbuf = (unsigned short *)buffer;
	
	for (int i = 0; i < ssHeight_ / 2; ++i)
	{
		memcpy(&tempRow[0], &sbuf[stride*i], sizeof(unsigned short) * stride);
		memcpy(&sbuf[stride * i], &sbuf[stride * (ssHeight_ - i - 1)], sizeof(unsigned short) * stride);
		memcpy(&sbuf[stride * (ssHeight_ - i - 1)], &tempRow[0], sizeof(unsigned short) * stride);
	}


	gl_->setCameraMatrix(prevCamMatrix);
	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
}


void GLWidget::screenShot(unsigned char * buffer)
{	
	paintGL();
	glReadPixels(0, 0, width(), height(), GL_RGB, GL_UNSIGNED_SHORT_5_6_5, buffer);
}


void GLWidget::saveAs(const QString & fileName)
{
	string fileNameW = qStringToStdString(fileName);
	QString logFileName = stdStringToQString(
		getDirectory(fileNameW) + "/" + getFileTitle(fileNameW) + ".log");
	
	sceneDocument_->remapDirectories(qStringToStdString(QDir::currentPath()),
		getDirectory(fileNameW));

	sceneDocument_->setTextData(buildInfoStr);
	Writer(qStringToStdString(fileName).c_str(), 0).
			write(sceneDocument_);
	document_->setDirty(false);
	fileName_ = fileName;
	setWindowTitle(fileName_);
}

QString GLWidget::saveTemporary()
{
	boost::uuids::uuid u = EditorGlobal::randomUuid();

	QString fileName = 
		stdStringToQString(boost::lexical_cast<string>(u)) + ".tmp";	

	fileName = stdStringToQString(Global::instance().readDirectory()) + fileName;
	Writer(qStringToStdString(fileName).c_str(), 0).
		write(sceneDocument_);
	
	return fileName;
}

void GLWidget::save()
{	
	string fileNameW = qStringToStdString(fileName_);
	QString logFileName = stdStringToQString(
		getDirectory(fileNameW) + "/" + getFileTitle(fileNameW) + ".log");

	sceneDocument_->setTextData(buildInfoStr);
	Writer(qStringToStdString(fileName_).c_str(), 0).
		write(sceneDocument_);
	document_->setDirty(false);
}



void GLWidget::resizeGL(int width, int height)
{
	glViewport(0, 0, width, height);
	
	//camera_->SetTo2DArea(1024, 768, 60, (double)width/height);
	camera_->setAspectRatio((float)width/height);
	sceneDocument_->setMaskSize(width, height);
}


void GLWidget::keyPressEvent(QKeyEvent * event)
{
	isGLWidgetKeyPressed_ = true;

	QWidget::keyPressEvent(event);

	if(event->key() == Qt::Key_Shift)
	{
		if(toolMode() != Other)
			isShiftKey_ = true;
	}
	if(event->key() == Qt::Key_Alt)
			isAltKey_ = true;
	
	if(event->key() == Qt::Key_Control)
	{	
		if(toolMode() != Other)
			isCtrlKey_ = true;
	}

	bool handled = false;
	if (curTool_) 
	{
		handled = curTool_->keyPressEvent(event);
	}

	if (!handled)
	{
		if ((event->key() == Qt::Key_THORN) && (event->key() == Qt::Key_Control))
			EditorGlobal::instance().setShowGrid(!EditorGlobal::instance().showGrid());
		if (isCtrlKey_ && (event->key() == Qt::Key_0))
		{
			resetCamera();
			update();
		}
		if (isCtrlKey_ && isAltKey_ && (event->key() == Qt::Key_0))
		{
			resetCameraOneToOne();
			update();
	    }
		if (event->key() == Qt::Key_F)
		{
			cameraLookAtSelection();			
		}
		if (isCtrlKey_ && ((event->key() == Qt::Key_Plus) || (event->key() == Qt::Key_Equal)))
		{
			zoomIn();
		}

		else if (isCtrlKey_ && (event->key() == Qt::Key_Minus))
		{
			zoomOut();
		}

		if (isCtrlKey_ && isShiftKey_ && (event->key() == Qt::Key_N))
		{
			onNew();
		}

		if((event->key() == Qt::Key_Delete) && (document_->selectedObjects().size() != 0 || document_->selectedAppObjects().size() !=0))
		{
			if(document_->selectedScene()->userCamera() != document_->selectedObject() || !document_->selectedAppObjects().empty())
				mainWindow_->onDelete();
		}
	}
}

void GLWidget::keyReleaseEvent(QKeyEvent * event)
{
	QWidget::keyReleaseEvent(event);

	if(event->key() == Qt::Key_Shift) isShiftKey_ = false;
	if(event->key() == Qt::Key_Alt)	isAltKey_ = false;
	if(event->key() == Qt::Key_Control)	isCtrlKey_ = false;
}

Vector2 GLWidget::mousePos() const
{
	QPoint cursorPos = mapFromGlobal(QCursor::pos());
	return Vector2(cursorPos.x(), cursorPos.y());
}

void GLWidget::mousePressEvent(QMouseEvent * event)
{
	if(event->button() == Qt::LeftButton)
		isLeftMouseButton_ = true;
	if(event->button() == Qt::RightButton)
		isRightMouseButton_ = true;
	if(event->button() == Qt::MiddleButton)
		isMiddleMouseButton_ = true;

	mouseClickLocation_ = event->posF();

	isCameraMode_ = isAltKey_;
	shiftPress_ = isShiftKey_;
	ctrlPress_ = isCtrlKey_;
	//automatically switch between app / scene object modes based on the type
	//of object that is clicked
	if (!curTool_->hoveringOverGizmo(event))
	{
		Vector2 mousePosDevice = windowToDeviceCoords(
			Vector2(event->posF().x(), event->posF().y()));
		const Camera * cam = camera();
		Ray ray = gl_->unproject(mousePosDevice);	
		Vector3 intPt;
		SceneObject * obj = document_->selectedScene()->intersect(ray, &intPt);
		AppObject * appObj = document_->selectedScene()->intersectAppObject(ray);
		
		MainWindow::ObjectMode toolMode = mainWindow_->toolObjectMode();
		if (appObj)
		{
			if (toolMode != MainWindow::ModeAppObject)
			{
				mainWindow_->setObjectMode(MainWindow::ModeAppObject);
			}
		}
		else
		{
			if (toolMode != MainWindow::ModeSceneObject)
			{
				mainWindow_->setObjectMode(MainWindow::ModeSceneObject);
			}
		}
	}

	if(isCameraMode_)
	{
		CameraObject * userCam = document_->selectedScene()->userCamera();
		if (userCam)
		{
			origUserCamera_ = *userCam->camera();
		}

		isCameraAltKey_ = isAltKey_;
		isCameraShiftKey_ = isShiftKey_;
		isCameraCtrlKey_ = isCtrlKey_;
		*cameraInitState_ = *camera_;
	}

	else
	{	
		if (curTool_) curTool_->mousePressEvent(event);
	}

	//update();
}

#include "Text.h"
#include "Text_2.h"

void GLWidget::mouseDoubleClickEvent(QMouseEvent * event)
{
	QWidget::mouseDoubleClickEvent(event);

	Vector2 mousePosDevice = windowToDeviceCoords(
		Vector2(event->posF().x(), event->posF().y()));

	const Camera * cam = camera();
	Ray ray = gl_->unproject(mousePosDevice);

	Scene * selScene = document_->selectedScene();
	
	Vector3 intPt;
	SceneObject * intObj = selScene->intersect(ray, &intPt);

	if (!intObj) return;
	
	if(!curTool_)
		return;

	curTool_->mouseDoublePressEvent(event);

	if (curTool_->getToolName() != "TextEditTool")
	{
		if(intObj->type() == Text_2().type())
		{
			PropertyPage *pProperty = document_->getPropertyPage(intObj);

			setToolMode(TextEditTool);
			pProperty->onEditObject();
		}
		else if(intObj->type() == Text().type())
		{
			document_->texttoText_2Convert();
		}
	}
	else{
		cout << endl;
	}
}

void GLWidget::mouseReleaseEvent(QMouseEvent * event)
{
	shiftPress_ = false;
	ctrlPress_ = false;

	if (isCameraMode_)
	{
		if (isRightMouseButton_ && isCameraAltKey_)
		{
			//dolly
			float deltaX = event->posF().x() - mouseClickLocation_.x();
			float deltaY = event->posF().y() - mouseClickLocation_.y();
			double delta = deltaX + deltaY;
			//cout << "delta: " << delta << "\n";
			double f = 0.995;
			Scene * scene = document_->selectedScene();
			*camera_ = *cameraInitState_;
			camera_->dollyex(pow(f, delta));

			scene->resizeText(gl_);
		}
	}

	if(event->button() == Qt::LeftButton)
		isLeftMouseButton_ = false;
	if(event->button() == Qt::RightButton)
		isRightMouseButton_ = false;
	if(event->button() == Qt::MiddleButton)
		isMiddleMouseButton_ = false;
	
	if (isCameraMode_)
	{		
		document_->doUserCamChangeCmd(origUserCamera_, *camera_);
		isCameraMode_ = false;
	}
	if (curTool_) curTool_->mouseReleaseEvent(event);
	//update();
}

void GLWidget::mouseMoveEvent(QMouseEvent * event)
{
	curTool_->mouseMoveEvent(event);
		
	if (isCameraMode_)
	{
		if(isLeftMouseButton_ && isCameraAltKey_)
		{
			//orbit
			const float factor = 0.005f;			
			float deltaX = event->posF().x() - mouseClickLocation_.x();
			float deltaY = event->posF().y() - mouseClickLocation_.y();
			*camera_ = *cameraInitState_;
			camera_->orbit(-factor*deltaY, -factor*deltaX);
		}

		else if ((isMiddleMouseButton_ && isCameraAltKey_))
		{
			//pan
			Vector3 intPt, intPt0;
			Vector3 a, b;

			Vector2 mousePosNorm = windowToDeviceCoords(
				Vector2(event->posF().x(), event->posF().y()));			

			Vector2 mousePosNorm0 =	windowToDeviceCoords(
				Vector2(mouseClickLocation_.x(), mouseClickLocation_.y()));			

			Plane lookAtPlane = camera_->lookAtPlane();
					
			gl_->unproject(mousePosNorm, &a, &b);			
			lookAtPlane.intersectLine(&intPt, a, b);
			gl_->unproject(mousePosNorm0, &a, &b);
			lookAtPlane.intersectLine(&intPt0, a, b);

			*camera_ = *cameraInitState_;			
			camera_->pan(intPt0 - intPt);
			Vector3 panVec = intPt0 - intPt;
		}

		else if (isRightMouseButton_ && isCameraAltKey_)
		{
			//dolly
			float deltaX = event->posF().x() - mouseClickLocation_.x();
			float deltaY = event->posF().y() - mouseClickLocation_.y();
			double delta = deltaX + deltaY;
			//cout << "delta: " << delta << "\n";
			double f = 0.995;

			*camera_ = *cameraInitState_;			
			camera_->dolly( pow(f, delta));
		}
		
		CameraObject * userCam = document_->selectedScene()->userCamera();

		if (userCam)
		{
			userCam->setCamera(*camera_);		
		}
	}
	
	else if (curTool_)
	{
		if(isLeftMouseButton_ && !document_->selectedObjects().empty() && (toolMode_ != Other))
		{
			if(document_->selectedObject()->visualAttrib()->isVisible())
			{
				VisualAttrib * attr = document_->selectedObject()->visualAttrib();
				Transform transform = attr->transform();

				Vector3 t = transform.translation();
				Vector3 r = transform.rotation();
				Vector3 s = transform.scaling();
				Vector3 p = transform.pivot();

				if(toolMode_ == Translate)
				{
					QToolTip::showText(event->globalPos(), 
						"X: " + QString::number(t.x, 'f', 4) +
						", Y: " + QString::number(t.y, 'f', 4) + 
						", Z: " + QString::number(t.z, 'f', 4), this, rect());
				}

				else if(toolMode_ == Rotate)
				{
					QToolTip::showText(event->globalPos(),
						"X: " + QString::number(RadToDeg(r.x), 'f', 4) + 
						", Y: " + QString::number(RadToDeg(r.y), 'f', 4) + 
						", Z: " + QString::number(RadToDeg(r.z), 'f', 4), this, rect());
				}

				else if(toolMode_ == Scale)
				{
					QToolTip::showText(event->globalPos(),
						"X: " + QString::number(s.x, 'f', 4) + 
						", Y: " + QString::number(s.y, 'f', 4) + 
						", Z: " + QString::number(s.z, 'f', 4), this, rect());
				}

				else if(toolMode_ == Pivot)
				{
					QToolTip::showText(event->globalPos(),
						"X: " + QString::number(p.x, 'f', 4) + 
						", Y: " + QString::number(p.y, 'f', 4) + 
						", Z: " + QString::number(p.z, 'f', 4), this, rect());
				}
			}
		}
	}
	
	//update();
//	mainWindow_->updateStatusBar(tr(""), event->pos().x(), event->pos().y());
}

void GLWidget::onUndo()
{
	if (isCameraMode_ || curTool_->isBeingUsed()) return;
	document_->undoCommand();
}

void GLWidget::onRedo()
{
	if (isCameraMode_ || curTool_->isBeingUsed()) return;
	document_->redoCommand();
}

bool GLWidget::eventFilter(QObject *obj, QEvent *event)
{
	if (event->type() == QEvent::Timer)
	{
		//update();
	}	
	return QWidget::eventFilter(obj, event);
}

GLWidget::ToolMode GLWidget::toolMode() const
{
	return toolMode_;
}

Tool * GLWidget::tool() const
{
	return curTool_;
}

void GLWidget::setTool(Tool * tool)
{
	if (tool == curTool_) return;
	if (curTool_) 
	{
		curTool_->setActive(false);	
		disconnect(curTool_, SIGNAL(objectChanged()), 0, 0);	
		disconnect(curTool_, SIGNAL(appObjectChanged()), 0, 0);		
	}
	curTool_ = tool;
	connect(curTool_, SIGNAL(objectChanged()), document_, SIGNAL(objectChanged()));
	connect(curTool_, SIGNAL(appObjectChanged()), document_, SIGNAL(appObjectChanged()));
	curTool_->setActive(true);
	
	setCursor(Qt::ArrowCursor);
	
	if(tool->getToolName() == "TextEditTool")
		toolMode_ = TextEditTool;
	else
		toolMode_ = Other;

	emit toolChanged();
}

void GLWidget::appLayerCamera(Camera * camera, Matrix * prevCamMatrix) const
{
	Scene * scene = document_->selectedScene();	
	bool isCustomCamera = scene->userCamera() != NULL;
	*prevCamMatrix = gl_->cameraMatrix();
	if (isCustomCamera)
	{
		float centerX = (float)scene->screenWidth() / 2;
		float centerY = (float)scene->screenHeight() / 2;
		float fovY = 60.0f;
		float aspect = (float) width() / height();
		camera->SetTo2DArea(centerX, centerY, (float)scene->screenWidth(), (float)scene->screenHeight(), fovY, aspect);			
	}
	else
	{
		*camera = *this->camera();
	}
}

Camera GLWidget::appLayerCamera() const
{
	Camera camera;
	Matrix prevMatrix;
	appLayerCamera(&camera, &prevMatrix);
	return camera;
}

void GLWidget::setToolMode(GLWidget::ToolMode mode)
{
	if (curTool_) 
	{
		curTool_->setActive(false);	
		disconnect(curTool_, SIGNAL(objectChanged()), 0, 0);	
		disconnect(curTool_, SIGNAL(appObjectChanged()), 0, 0);		
	}

	if (mode == Translate)
	{
		curTool_ = transformTool_;
		transformTool_->setMode(TransformTool::TranslateMode);
		toolMode_ = mode;		
	}
	else if (mode == Scale)
	{
		curTool_ = transformTool_;
		transformTool_->setMode(TransformTool::ScaleMode);
		toolMode_ = mode;		
	}	
	else if (mode == Pivot)
	{
		curTool_ = transformTool_;
		transformTool_->setMode(TransformTool::PivotMode);
		toolMode_ = mode;		
	}
	else if (mode == Select)
	{
		curTool_ = transformTool_;
		transformTool_->setMode(TransformTool::SelectMode);
		toolMode_ = mode;		
	}
	else if (mode == Rotate)
	{
		curTool_ = transformTool_;
		transformTool_->setMode(TransformTool::RotateMode);
		toolMode_ = mode;
	}
	else if (mode == AppObjectTransform)
	{
		curTool_ = appObjectTransformTool_;
		toolMode_ = mode;
	}
	else if(mode == TextEditTool)
	{
		prevMode_ = toolMode_;
		toolMode_ = mode;
	}

	connect(curTool_, SIGNAL(objectChanged()), document_, SIGNAL(objectChanged()));
	connect(curTool_, SIGNAL(appObjectChanged()), document_, SIGNAL(appObjectChanged()));
	curTool_->setActive(true);

	setCursor(Qt::ArrowCursor);
	//update();
	emit toolChanged();
}

Scene * GLWidget::scene() const
{
	return document_->selectedScene();
}

void GLWidget::nudgeObjects(const Vector3 & nudgeVec)
{
	document_->doCommand(
		new NudgeCmd(
			document_->selectedObjects(), nudgeVec));
}

void GLWidget::dragEnterEvent(QDragEnterEvent * event)
{
	const QMimeData * data = event->mimeData();

	if (data->hasUrls())
         event->acceptProposedAction();
}


void GLWidget::dropEvent(QDropEvent * event)
{
	const QMimeData * data = event->mimeData();

	if (data->hasUrls())
	{
		QList<QUrl> urls = data->urls();
         
		
		vector<SceneObjectSPtr> newObjs;

		BOOST_FOREACH(QUrl url, urls)
		{
			QString localFile = url.toLocalFile();
			if (checkFileExtension(qStringToStdString(localFile), "st") ||
				checkFileExtension(qStringToStdString(localFile), "gif") ||
				checkFileExtension(qStringToStdString(localFile), "pdf") )
			{
				mainWindow_->dropEvent(event);
				return;
			}
		}

		BOOST_FOREACH(QUrl url, urls)
		{
			QString localFile = url.toLocalFile();

			Image * img = new Image();			
			
			string dir = Global::instance().readDirectory();
			img->setFileName(convertToRelativePath(dir, qStringToStdString(localFile)));
			img->init(gl_);

			int trWidth, trHeight;
			Global::instance().getImageDims(img->fileName(), &trWidth, &trHeight);

			img->setId(getFileTitle(qStringToStdString(localFile)));
			ImageSPtr imgSPtr(img);

			if (trWidth > 2048 || trHeight > 2048)
			{
				QMessageBox::information(0, tr("Error"), tr("Image width or height cannot exceed 2048 pixels"));
			}
			else if (trWidth > 0 && trHeight > 0)			
				newObjs.push_back(imgSPtr);
			
			Transform transform = imgSPtr->transform();

			const Camera * cam = camera();
			Vector2 mousePtDevice = windowToDeviceCoords(Vector2(event->pos().x(), event->pos().y()));
			Ray mouseRay = gl_->unproject(mousePtDevice);
			Plane zeroPlane = Plane::fromPointNormal(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f));
			Vector3 intPt;
			zeroPlane.intersect(&intPt, mouseRay);
			intPt.x = roundFloat(intPt.x);
			intPt.y = roundFloat(intPt.y);
			intPt.z = 0.0f;
			transform.setTranslation(intPt);
			imgSPtr->setTransform(transform);
		}

		if (!newObjs.empty())
			document_->doCommand(new NewObjectsCmd(
				document_, document_->selectedScene()->root(), -1, newObjs));
	}
}

void GLWidget::closeEvent(QCloseEvent * event)
{
	if(document_->isDirty())
	{
		QMessageBox::StandardButton reply = 
			QMessageBox::information(this, tr("Message"), 
			tr("Save changes to ") + fileName() + tr("?"),
			QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

		if(reply == QMessageBox::Yes)
		{
			mainWindow_->saveFile();
		}
		else if(reply == QMessageBox::Cancel)
		{
			event->ignore();
			return;
		}
	}

	mainWindow_->subWindowClosing(this);
}


void GLWidget::cameraLookAtSelection()
{	
	//look at intersected point
	Scene * scene = document_->selectedScene();

	QPoint globalPt = QCursor::pos();
	QPoint pt = this->mapFromGlobal(globalPt);
	Vector2 mousePosDevice = windowToDeviceCoords(
		Vector2(pt.x(), pt.y()));
	Ray ray = gl_->unproject(mousePosDevice);	

	Vector3 intPt;
	SceneObject * intObj = scene->intersect(ray, &intPt);
	if (intObj != scene->root())
	{
		CameraObject * userCam = scene->userCamera();
		if (userCam) origUserCamera_ = *userCam->camera();
		
		
		Vector3 lVec = camera_->eye() - camera_->lookAt();
		lVec = lVec.normalize();
		Vector3 newEye = intPt + (camera_->eye() - intPt).magnitude() * lVec;
		camera_->setLookAt(intPt);
		camera_->setEye(newEye);
		if (userCam)
		{				
			userCam->setCamera(*camera_);
			document_->doUserCamChangeCmd(origUserCamera_, *camera_);
		}			
	}

	/*
	//look at center of selected bbox
	Scene * selScene = document_->selectedScene();
	CameraObject * userCam = selScene->userCamera();

	const vector<SceneObject *> & selObjects = document_->selectedObjects();

	if (selObjects.empty()) return;

	BoundingBox selBb;
	BOOST_FOREACH(SceneObject * selObject, selObjects)
	{
		selBb = selBb.unite(selObject->worldExtents());
	}

	Vector3 lookAtPt = selBb.midPt();

	if (userCam)
	{
		Camera origCam = *userCam->camera();		
		userCam->camera()->lookAtPoint(lookAtPt);		
		document_->doUserCamChangeCmd(origCam, *userCam->camera());	
	}

	camera_->lookAtPoint(lookAtPt);
	*/
}

void GLWidget::resetCamera()
{
	Scene * selScene = document_->selectedScene();
	CameraObject * userCam = selScene->userCamera();
	float sw = selScene->screenWidth();
	float sh = selScene->screenHeight();
	float fovY = 60.0f;
	if (userCam)
	{
		Camera origCam = *userCam->camera();
		fovY = origCam.fovY();
		userCam->setTo2dArea(sw/2, sh/2, sw, sh, fovY, sw/sh);
		
		document_->doUserCamChangeCmd(origCam, *userCam->camera());		
	}

	camera_->SetTo2DArea(sw/2, sh/2, sw, sh, fovY, aspectRatio());
	camera_->initDefaultMag();
	document_->selectedScene()->resizeText(document_->renderer());
}

void GLWidget::resetCameraOneToOne()
{
	int windowWidth = size().width();
	int windowHeight = size().height();
	Scene * selScene = document_->selectedScene();
	CameraObject * userCam = selScene->userCamera();
	float sw = selScene->screenWidth();
	float sh = selScene->screenHeight();
	float fovY = 60.0f;
	float pixelWidth = (float)windowWidth;
	float pixelHeight = (float)windowHeight;

	float temp = windowWidth / sw * sh;
	if (temp > windowHeight)
	{
		pixelWidth = (float)(windowHeight / sh * sw);
		pixelHeight = (float)(windowHeight);
	}
	else
	{
		pixelWidth = (float)windowWidth;
		pixelHeight = (float)temp;
	}		

	float centerX, centerY;
	centerX = (float)(windowWidth/2);
	centerY = (float)(windowHeight/2);

	if (sw / 2  < centerX) centerX = sw / 2;
	if (sh / 2 < centerY) centerY = sh / 2;

	if (userCam)
	{
		Camera origCam = *userCam->camera();
		fovY = origCam.fovY();
		userCam->setTo2dArea(
			centerX, centerY,
			pixelWidth, pixelHeight, fovY, (float)sw/sh);
		
		document_->doUserCamChangeCmd(origCam, *userCam->camera());	
	}

	camera_->SetTo2DArea(
		centerX, centerY,
		pixelWidth, pixelHeight, fovY, (float)windowWidth/windowHeight);
	camera_->initDefaultMag();
	document_->selectedScene()->resizeText(document_->renderer());
}

void GLWidget::setCamera(const Camera & camera)
{
	*camera_ = camera;
	camera_->setAspectRatio(aspectRatio());
}

void GLWidget::onSceneSelectionChanged()
{
	Scene * scene = document_->selectedScene();
	
	if (!scene->userCamera())
	{
		resetCamera();
		scene->resizeText(gl_);
	}
}

Model * GLWidget::initModel(const QString & resourceName)
{
	boost::uuids::uuid u = EditorGlobal::randomUuid();
	QString modelFile = QDir::tempPath() + "/" + stdStringToQString(boost::lexical_cast<string>(u)) + ".stm";	
	string modelFileStr = qStringToStdString(modelFile);	
	
	QResource qCamResource(resourceName);	

	FILE * file = fopen(modelFileStr.c_str(), "wb");
	fwrite(qCamResource.data(), 1, qCamResource.size(), file);
	fclose(file);
	
	Model * model = Model::read(getDirectory(modelFileStr).c_str(), modelFileStr.c_str());
	unlink(modelFileStr.c_str());

	model->init(gl_);
	
	return model;
}

void GLWidget::timerEvent(QTimerEvent * event)
{
	QApplication::processEvents();
	if (mainWindow_->curGlWidget() != this)
		return;

	QFileInfo fileInfo(fileName_);

	QString fileName;
	if (saveTemporary_)
		fileName = fileInfo.baseName() + tr("~1.tmp");
	else
		fileName = fileInfo.baseName() + tr("~2.tmp");

	fileName = stdStringToQString(Global::instance().readDirectory()) + fileName;
	Writer(qStringToStdString(fileName).c_str(), 0).
		write(sceneDocument_);

	saveTemporary_ = !saveTemporary_;
	
	//cout << "fileName_: " << fileName.toStdString() << endl;
}

void GLWidget::timerCheck()
{
	if (interval_ != EditorGlobal::instance().tempfileInterval())
	{
		if (timer_.isActive())
			timer_.stop();

		interval_ = EditorGlobal::instance().tempfileInterval();
		if (interval_ > 0)
			timer_.start(1000 * 60 * interval_, this);
	}
}

void GLWidget::zoomIn()
{
	double delta = 20;
	double f = 0.995;
	Scene * scene = document_->selectedScene();
//	camera_->dolly( pow(f, delta));
	camera_->dollyex( pow(f, delta));
	CameraObject * userCam = document_->selectedScene()->userCamera();

	if (userCam)
	{
		userCam->setCamera(*camera_);		
	}
	scene->resizeText(gl_);
}

void GLWidget::zoomOut()
{
	double delta = -20;
	double f = 0.995;
	Scene * scene = document_->selectedScene();
//	camera_->dolly( pow(f, delta));
	camera_->dollyex( pow(f, delta));
	CameraObject * userCam = document_->selectedScene()->userCamera();

	if (userCam)
	{
		userCam->setCamera(*camera_);		
	}
	scene->resizeText(gl_);
}
void GLWidget::onNew()
{
	int width = document_->selectedScene()->screenWidth();
	int height = document_->selectedScene()->screenHeight();
	document_->doCommand(new NewSceneCmd(document_, width, height));
}

std::wstring CreateTextPropertyWithMime(QString txt , TextMimeType type, std::vector<TextProperties_2> &props, std::vector<LineStyle> &lineStyle){
	QTextDocument doc;

	if(type == MIME_HTML)
		doc.setHtml( txt );
	else
		doc.setPlainText(txt);

	QTextBlock currentBlock = doc.firstBlock();

	QString fontDir = QDesktopServices::storageLocation(QDesktopServices::FontsLocation);
	QString fontFileChoice1 = fontDir + "/malgun.ttf";
	QString qText = doc.toPlainText();

	int i = 0;

	int nNextLineCharStartIndex = 0;

	while (currentBlock.isValid()) {

		QTextBlockFormat blockFormat = currentBlock.blockFormat();
		QTextCharFormat charFormat = currentBlock.charFormat();
		QTextBlock::iterator it;
		LineStyle line;
		QString qBlockText = "";

		for (it = currentBlock.begin(); !(it.atEnd()); ++it) {

			QTextFragment currentFragment = it.fragment();

			if (currentFragment.isValid()) {
				// a text fragment also has a char format with font:
				QTextCharFormat fragmentCharFormat = currentFragment.charFormat();
				QFontInfo info(fragmentCharFormat.font());
				QFont realFont(info.family());
				QString fragmentText = currentFragment.text();


				QString fontFile = fontDir + "/" + realFont.rawName() +".ttf";

				bool fontexists = QFile(fontFile).exists();

				if(!fontexists){
					fontFile = fontFileChoice1;
				}

				TextProperties_2 prop;
				prop.index = currentFragment.position();
				prop.font.faceIndex = 0;
				prop.font.fontFile = convertToRelativePath(qStringToStdString(fontFile));

				prop.font.pointSize = fragmentCharFormat.fontPointSize();

				if(prop.font.pointSize <= 0)
					prop.font.pointSize = 16;

				prop.font.bold = fragmentCharFormat.fontWeight() == QFont::Weight::Bold;	
				prop.font.italic = fragmentCharFormat.fontItalic();
				prop.font.shadow = false;
				prop.underline_ = fragmentCharFormat.fontUnderline();
				prop.cancleline_ = fragmentCharFormat.fontOverline();
				//prop.letterwidthratio_ = fragmentCharFormat.fontLetterSpacing();
				QColor qc = fragmentCharFormat.foreground().color();
				prop.color.r = qc.redF();
				prop.color.g = qc.greenF();
				prop.color.b = qc.blueF();

				if(fragmentCharFormat.isImageFormat()){
					// 						
					// 						QTextImageFormat convForm = fragmentCharFormat.toImageFormat();
					// 
					// 						Global::instance().requestObjectLoad(this, qStringToStdString(convForm.name()), LoadRequest::LoadRequestFullImage);
				}

				qBlockText = qBlockText + fragmentText;

				props.push_back(prop);
			}

		}

		line.index_ = i;

		lineStyle.push_back(line);
		currentBlock = currentBlock.next();

		i ++;
	}

	if(qText.length() > 0){
		int nEnter = qText.count("\n");
		int nLine = lineStyle.size();

		if(nEnter < nLine - 1){

			while(nEnter != nLine - 1){
				lineStyle.pop_back();

				nLine --;
			}
		}

		if(nEnter > nLine - 1){
			LineStyle line;

			while(nEnter != nLine - 1){

				//
				line.index_ = nLine/* - 1*/;
				lineStyle.push_back(line);

				nLine ++;
			}
		}

	}	

	return qText.toStdWString();
}

SceneObject *CreateTextObjectWithMime(QString txt , TextMimeType type){

	Text_2 * viewtext = new Text_2();		

	std::vector<TextProperties_2> props;
	std::vector<LineStyle> lineStyle;

	std::wstring wstrText = CreateTextPropertyWithMime(txt, type, props, lineStyle);

	if(props.size() > 0 && lineStyle.size() > 0){
		viewtext->setTextString(wstrText);
		viewtext->setProperties(props);
		viewtext->setLinesStyle(lineStyle);

		return viewtext;
	}else
		delete viewtext;		


	return NULL;
}
