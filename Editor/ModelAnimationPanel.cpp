#include "stdafx.h"
#include "ModelAnimationPanel.h"
#include "MainWindow.h"
#include "Model.h"
#include "ModelFile.h"
#include "EditorDocument.h"
#include "GLWidget.h"
#include "ModelAnimation.h"
#include "GenericList.h"

FrameLineEdit::FrameLineEdit()
: validator_(0, INT_MAX, this)
{
	setValidator(&validator_);
}

QSize FrameLineEdit::sizeHint() const
{
	return QSize(0, 0);
}


void addCentralWidget(QWidget * parent, QWidget * widget)
{
	QVBoxLayout * layout = new QVBoxLayout;
	layout->setMargin(0);
	layout->setSpacing(0);
	layout->addWidget(widget);
	parent->setLayout(layout);
}

ModelAnimationPanel::ModelAnimationPanel(MainWindow * mainWindow) : QWidget(mainWindow, Qt::Window)
{
	modelAnimation_.reset(new ModelAnimation);

	ui.setupUi(this);
	this->setWindowTitle(tr("Model Animation"));

	startFrameEdit_ = new FrameLineEdit;
	startFrameEdit_->setReadOnly(true);
	endFrameEdit_ = new FrameLineEdit;
	endFrameEdit_->setReadOnly(true);
	curFrameEdit_ = new FrameLineEdit;

	addCentralWidget(ui.startFrameEditWidget, startFrameEdit_);
	addCentralWidget(ui.endFrameEditWidget, endFrameEdit_);
	addCentralWidget(ui.curFrameEditWidget, curFrameEdit_);

	connect(ui.frameSlider, SIGNAL(valueChanged(int)), SLOT(onSliderValueChanged(int)));
	document_ = 0;

	connect(mainWindow, SIGNAL(documentChanged(EditorDocument *)), this, SLOT(onDocumentChanged(EditorDocument *)));

	GLWidget * glWidget = mainWindow->curGlWidget();
	if (glWidget)
		onDocumentChanged(glWidget->document());

	curModelObj_ = NULL;

	setupRestrictJointsUi();
	setupRestrictMeshesUi();
}

ModelAnimationPanel::~ModelAnimationPanel()
{
}

void ModelAnimationPanel::setupRestrictJointsUi()
{
	QString str;

	QVBoxLayout * layout = new QVBoxLayout();
	ui.restrictJointsWidget->setLayout(layout);

	endJointsList_ = new GenericList;
	connect(endJointsList_, SIGNAL(selectionChanged()), this, SLOT(onEndJointSelectionChanged()));
	

	layout->addWidget(new QLabel(tr("Start joint")));
	startJointEdit_ = new FrameLineEdit;
	connect(startJointEdit_, SIGNAL(editingFinished()), this, SLOT(onStartJointEdit()));
	str.sprintf("%d", modelAnimation_->jointIndex);
	startJointEdit_->setText(str);

	layout->addWidget(startJointEdit_);

	layout->addWidget(new QLabel(tr("End joints")));
	layout->addWidget(endJointsList_);
	
	
	addJointButton_ = new QPushButton(tr("Add"));
	removeJointButton_ = new QPushButton(tr("Remove"));

	connect(addJointButton_, SIGNAL(clicked()), this, SLOT(onAddEndJoint()));
	connect(removeJointButton_, SIGNAL(clicked()), this, SLOT(onRemoveEndJoint()));

	QHBoxLayout * buttonsLayout = new QHBoxLayout;
	buttonsLayout->setMargin(0);
	buttonsLayout->setSpacing(0);
	buttonsLayout->addWidget(addJointButton_);
	buttonsLayout->addWidget(removeJointButton_);
	layout->addLayout(buttonsLayout);

	connect(ui.restrictJointsCheck, SIGNAL(clicked()), this, SLOT(onRestrictJoints()));

	onRestrictJoints();	
	onEndJointSelectionChanged();
}

void ModelAnimationPanel::setupRestrictMeshesUi()
{
	QVBoxLayout * layout = new QVBoxLayout();
	ui.restrictMeshesWidget->setLayout(layout);

	meshList_ = new GenericList;	
	connect(meshList_, SIGNAL(selectionChanged()), this, SLOT(onMeshSelectionChanged()));
	
	layout->addWidget(new QLabel(tr("Animated meshes")));
	layout->addWidget(meshList_);

	addMeshButton_ = new QPushButton(tr("Add"));
	removeMeshButton_ = new QPushButton(tr("Remove"));

	connect(addMeshButton_, SIGNAL(clicked()), this, SLOT(onAddMesh()));
	connect(removeMeshButton_, SIGNAL(clicked()), this, SLOT(onRemoveMesh()));

	QHBoxLayout * buttonsLayout = new QHBoxLayout;
	buttonsLayout->setMargin(0);
	buttonsLayout->setSpacing(0);
	buttonsLayout->addWidget(addMeshButton_);
	buttonsLayout->addWidget(removeMeshButton_);
	layout->addLayout(buttonsLayout);

	connect(ui.restrictMeshesCheck, SIGNAL(clicked()), this, SLOT(onRestrictMeshes()));
	

	onRestrictMeshes();
	onMeshSelectionChanged();

}

void ModelAnimationPanel::onRestrictJoints()
{
	ui.restrictJointsWidget->setEnabled(ui.restrictJointsCheck->isChecked());
	modelAnimation_->restrictJoints = ui.restrictJointsCheck->isChecked();
}


void ModelAnimationPanel::onStartJointEdit()
{
	modelAnimation_->jointIndex = startJointEdit_->text().toInt();
}

void ModelAnimationPanel::onAddEndJoint()
{
	bool ok;
	int jointIndex = QInputDialog::getInteger(this, tr("Joint index"), 
		tr("Joint index"), 0, 0, INT_MAX, 1, &ok);

	if (ok && !modelAnimation_->isExcludedJointIndex(jointIndex))
	{
		QString str;
		str.sprintf("%d", jointIndex);
		endJointsList_->addTopLevelItem(str, NULL);
		syncEndJoints();
	}
	
}

void ModelAnimationPanel::onRemoveEndJoint()
{
	endJointsList_->deleteSelectedItems();
	syncEndJoints();
}

void ModelAnimationPanel::onEndJointSelectionChanged()
{
	removeJointButton_->setEnabled(!endJointsList_->selectedItems().empty());
}

void ModelAnimationPanel::onRestrictMeshes()
{
	ui.restrictMeshesWidget->setEnabled(ui.restrictMeshesCheck->isChecked());
	modelAnimation_->restrictMeshes = ui.restrictMeshesCheck->isChecked();
}

void ModelAnimationPanel::onAddMesh()
{
	bool ok;
	int meshIndex = QInputDialog::getInteger(this, tr("Mesh index"), 
		tr("Mesh index"), 0, 0, INT_MAX, 1, &ok);

	if (ok && !modelAnimation_->isIncludedMeshIndex(meshIndex))
	{
		QString str;
		str.sprintf("%d", meshIndex);
		meshList_->addTopLevelItem(str, NULL);
		syncMeshes();
	}
	
}

void ModelAnimationPanel::onRemoveMesh()
{
	meshList_->deleteSelectedItems();
	syncMeshes();
}

void ModelAnimationPanel::onMeshSelectionChanged()
{
	removeMeshButton_->setEnabled(!meshList_->selectedItems().empty());
}




void ModelAnimationPanel::syncEndJoints()
{
	const std::vector<GenericListItem *> & items = endJointsList_->items();

	std::set<int> jointIndices;

	BOOST_FOREACH(GenericListItem * item, items)
	{
		jointIndices.insert(item->string.toInt());
	}

	modelAnimation_->excludedJoints = jointIndices;
}

void ModelAnimationPanel::syncMeshes()
{
	const std::vector<GenericListItem *> & items = meshList_->items();

	std::set<int> meshIndices;

	BOOST_FOREACH(GenericListItem * item, items)
	{
		meshIndices.insert(item->string.toInt());
	}

	modelAnimation_->meshIndices = meshIndices;
}




void ModelAnimationPanel::onDocumentChanged(EditorDocument * document)
{
	if (document_)
	{
		disconnect(document_, 0, this, 0);
		timer_.stop();
	}

	document_ = document;	

	syncToSelection();

	if (document_)
	{
		connect(document_, SIGNAL(objectSelectionChanged()), this, SLOT(syncToSelection()));		
		timer_.start(300, this);
	}

	
}

void ModelAnimationPanel::onSliderValueChanged(int frame)
{
	QString str;
	str.sprintf("%d", frame);
	curFrameEdit_->setText(str);

	SceneObject * selObj = document_->selectedObject();
	Model * model = ((ModelFile *)selObj)->model();
	//model->setTime((float)frame / model->fps());
	
	model->applyAnimation(*modelAnimation_, (float)frame / model->fps());
}


void ModelAnimationPanel::syncToSelection()
{
	if (!isVisible()) return;
	SceneObject * selObj = NULL;
	
	if (document_) selObj = document_->selectedObject();
	
	if (selObj && selObj->type() == ModelFile().type())
	{
		setEnabled(true);		

		if (selObj->isLoaded() && curModelObj_ != selObj)
		{
			curModelObj_ = (ModelFile *)selObj;

			Model * model = ((ModelFile *)selObj)->model();

			QString str;
			str.sprintf("%d", model->startFrame());
			startFrameEdit_->setText(str);
			str.sprintf("%d", model->endFrame());
			endFrameEdit_->setText(str);
			str.sprintf("%d", ui.frameSlider->value());
			curFrameEdit_->setText(str);

			ui.frameSlider->setMinimum(model->startFrame());
			ui.frameSlider->setMaximum(model->endFrame());	

			onSliderValueChanged(ui.frameSlider->value());
		}
	}
	else if (curModelObj_)
	{		
		curModelObj_ = NULL;

		startFrameEdit_->setText("");
		endFrameEdit_->setText("");
		curFrameEdit_->setText("");
		setEnabled(false);
	}
}

void ModelAnimationPanel::closeEvent(QCloseEvent * event)
{
	emit closed();
}

void ModelAnimationPanel::timerEvent(QTimerEvent * event)
{
	syncToSelection();	
}