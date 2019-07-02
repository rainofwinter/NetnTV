#pragma once
#include "ui_modelAnimationPanel.h"

class MainWindow;
class EditorDocument;
class ModelFile;
class ModelAnimation;
class GenericList;

class FrameLineEdit : public QLineEdit
{
public:
	FrameLineEdit();
	virtual QSize sizeHint() const;
private:
	QIntValidator validator_;
};


class ModelAnimationPanel : public QWidget
{
	Q_OBJECT
public:
	ModelAnimationPanel(MainWindow * mainWindow);
	~ModelAnimationPanel();

	

private slots:
	void syncToSelection();
	void onSliderValueChanged(int value);
	void onDocumentChanged(EditorDocument * document);

	void onRestrictJoints();
	void onStartJointEdit();
	void onAddEndJoint();
	void onRemoveEndJoint();
	void onEndJointSelectionChanged();

	void onRestrictMeshes();
	void onAddMesh();
	void onRemoveMesh();
	void onMeshSelectionChanged();
	

signals:
	void closed();

private:
	void setupRestrictJointsUi();
	void setupRestrictMeshesUi();
	void syncEndJoints();
	void syncMeshes();

	virtual void closeEvent(QCloseEvent * event);
	virtual void timerEvent(QTimerEvent * event);


private:

	Ui::ModelAnimationPanel ui;
	EditorDocument * document_;
	QBasicTimer timer_;
	QLineEdit * startFrameEdit_, * endFrameEdit_, * curFrameEdit_;
	ModelFile * curModelObj_;


	
	QPushButton * addJointButton_, * removeJointButton_;
	QPushButton * addMeshButton_, * removeMeshButton_;
	GenericList * meshList_;
	GenericList * endJointsList_;

	FrameLineEdit * startJointEdit_;

	boost::scoped_ptr<ModelAnimation> modelAnimation_;
};