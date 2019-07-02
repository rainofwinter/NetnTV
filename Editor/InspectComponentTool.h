#pragma once
#include "Tool.h"
#include "MathStuff.h"
#include "Transform.h"
#include "ui_InspectComponentWindow.h"

class SceneObject;
class ModelFile;
class MainWindow;
class Joint;
class Mesh;

///////////////////////////////////////////////////////////////////////////////

class InspectComponentWindow : public QWidget
{
	Q_OBJECT
public:
	InspectComponentWindow(MainWindow * mainWindow);
	void setText(const QString & text);
signals:
	void closed();
private slots:
	void onDone();
private:
	virtual void closeEvent(QCloseEvent *);
private:
	Ui::InspectComponentWindow ui;
};

///////////////////////////////////////////////////////////////////////////////

class InspectComponentTool : public Tool
{
	Q_OBJECT
public:
	InspectComponentTool(GLWidget * gl);
	~InspectComponentTool();

	virtual void init();
	virtual void uninit();

	virtual void mousePressEvent(QMouseEvent * event);
	virtual void mouseReleaseEvent(QMouseEvent * event);
	virtual void mouseMoveEvent(QMouseEvent * event);

	virtual void setActive(bool val);
		
	virtual void draw();

	virtual bool isBeingUsed() const;

	void setModel(ModelFile * modelObj);
private:
	void syncTextToSelection();
	void endTool();
private slots:

	void onWindowClosed();
	void onLostObjSelection();
		
private:
	ModelFile * modelObj_;
	Model * model_;
	InspectComponentWindow * inspectWnd_;

	int selJoint_;	
	int selMesh_;

	bool disableWindowClosedHandling_;
};


