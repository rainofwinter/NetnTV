#pragma once

#include <QGLWidget>
#include "GfxRenderer.h"
#include "MathStuff.h"
#include "Transform.h"
#include "Types.h"
#include "Camera.h"
#include "Text.h"
#include "Text_2.h"

class Image;
class Camera;
class Document;
class Scene;
class SceneObject;
class EditorScene;
class EditorDocument;
class EditorImage;
class Tool;
class TransformTool;
class AppObjectTransformTool;
class MainWindow;

enum TextMimeType{
	MIME_HTML ,MIME_PLAINTEXT
};

std::wstring CreateTextPropertyWithMime(QString txt , TextMimeType type, std::vector<TextProperties_2> &pros, std::vector<LineStyle> &lineStyle);

SceneObject *CreateTextObjectWithMime(QString txt , TextMimeType type);

class GLWidget : public QGLWidget
{
	friend class MainWindow;

	Q_OBJECT
public:
	enum ToolMode
	{
		Other,
		Select,
		Translate,
		Scale, 
		Rotate,
		Pivot,
		AppObjectTransform,
		TextEditTool
	};

public:
	
	GLWidget(MainWindow * mainWindow);
	void initExistingFile(const QString & fileName);	
	void initNewFile(const QString & fileName, int width, int height);
	
	~GLWidget();

	Tool * tool() const;

	ToolMode toolMode() const;
	void setToolMode(ToolMode mode);

	void setTool(Tool * tool);

	Scene * scene() const;
	EditorDocument * document() const {return document_;}

	Vector2 windowToDeviceCoords(const Vector2 & coords) const;
	Vector2 deviceToWindowCoords(const Vector2 & coords) const;
	Vector2 deviceToWindowCoords(const Vector3 & coords) const
	{return deviceToWindowCoords(Vector2(coords.x, coords.y));}
	Vector2 mousePos() const;
	
	
	/**
	If you want the proper camera matrix it's better to call this function 
	instead of calling camera() and then getting the matrix directly.

	That's because some scaling / modifications may have been applied afterward
	to the matrix.

	Note: this is not the *current* renderer camera matrix
	*/
	Matrix getCameraMatrix() const;
	const Camera * camera() const {return camera_.get();}


	float aspectRatio() const {return (float)width()/height();}
	
	GfxRenderer * renderer() const {return gl_;}
	
	void onUndo();	
	void onRedo();	

	void saveAs(const QString & fileName);
	QString saveTemporary();
	void save();
	bool hasBeenSaved() const {return !fileName_.isEmpty();}
	const QString & fileName() const {return fileName_;}
	void nudgeObjects(const Vector3 & nudgeVec);

	void screenShot(unsigned char * buffer, Scene * scene, int width, int height, bool preserveAspect);
	void screenShot(unsigned char * buffer);

	Model * camModel() const {return camModel_;}
	Model * jointModel() const {return jointModel_;}
	Model * boneModel() const {return boneModel_;}
	Model * lightModel() const {return lightModel_;}

	bool shiftPress() const {return shiftPress_;}
	bool ctrlPress() const {return ctrlPress_;}
	void resetCamera();
	void resetCameraOneToOne();
	void cameraLookAtSelection();

	void setCamera(const Camera & camera);


	void appLayerCamera(Camera * camera, Matrix * prevCamMatrix) const;
	Camera appLayerCamera() const;

	MainWindow * mainWindow() const {return mainWindow_;}

	void timerCheck();
	void zoomIn();
	void zoomOut();
	void onNew();

	ToolMode prevMode(){ return prevMode_; }

signals:
	void toolChanged();
	void closing(QWidget *);
	
private:

	void getCameraScaleFactor(float & sx, float & sy) const;

	void create();
	void initCommon();

	virtual void initializeGL();
	virtual void paintGL();
	virtual void resizeGL(int width, int height);

	virtual void keyPressEvent(QKeyEvent *);
	virtual void keyReleaseEvent(QKeyEvent *);
	virtual void mousePressEvent(QMouseEvent *);
	virtual void mouseDoubleClickEvent(QMouseEvent *);
	virtual void mouseReleaseEvent(QMouseEvent *);
	virtual void mouseMoveEvent(QMouseEvent *);
	virtual bool eventFilter(QObject *, QEvent *);

	virtual void dragEnterEvent(QDragEnterEvent * event);
	virtual void dropEvent(QDropEvent * event);

	virtual void closeEvent(QCloseEvent * event);
	virtual void timerEvent(QTimerEvent *);

	Model * initModel(const QString & resourceName);

private slots:

	void onSceneSelectionChanged();

private:

	GfxRenderer * gl_;
	CameraSPtr camera_;

	/**
	backup camera state when user first starts a camera manipulation mouse drag
	session
	*/
	CameraSPtr cameraInitState_;

	QPointF mouseClickLocation_;

	/*
	GLuint texture_;	
	GLuint vertBuffer_, indexBuffer_, texCoordBuffer_, colorBuffer_;
	*/

	EditorDocument * document_;
	Document * sceneDocument_;

	Tool * curTool_;
	std::vector<Tool *> tools_;
	TransformTool * transformTool_;
	AppObjectTransformTool * appObjectTransformTool_;

	bool isCameraMode_;	
	bool isCameraAltKey_;
	bool isCameraShiftKey_;
	bool isCameraCtrlKey_;	
	Camera origUserCamera_;

	MainWindow * mainWindow_;


	QString fileName_;
	int width_;
	int height_;


	//--------------------------------------------------------------------------
	ToolMode toolMode_, prevMode_;

	//GLuint ssPbo_;
	int ssWidth_, ssHeight_;

	Model * camModel_;
	Model * jointModel_;
	Model * boneModel_;
	Model * lightModel_;

	QBasicTimer timer_;
	bool saveTemporary_;
	int interval_;
	
	bool shiftPress_;
	bool ctrlPress_;

	bool isLeftMouseButton_;
	bool isRightMouseButton_;
	bool isMiddleMouseButton_;

	bool isShiftKey_;
	bool isAltKey_;
	bool isCtrlKey_;

	bool isGLWidgetKeyPressed_;
};

