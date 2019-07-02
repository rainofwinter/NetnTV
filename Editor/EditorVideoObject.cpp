#include "stdafx.h"
#include "EditorDocument.h"
#include "EditorVideoObject.h"
#include "VideoObject.h"
#include "GLWidget.h"
#include "Command.h"
#include "Utils.h"
#include "FileUtils.h"

#include "EditorGlobal.h"
#include "EditorScene.h"

#include "VideoRegionTool.h"
#include "UrlDlg.h"

using namespace std;
EditorVideoObject::EditorVideoObject()
{
}

EditorVideoObject::~EditorVideoObject()
{
}

SceneObject * EditorVideoObject::createSceneObject(EditorDocument * document) const
{	
	VideoObject * videoObject = new VideoObject;
	return videoObject;
}

boost::uuids::uuid EditorVideoObject::sceneObjectType() const
{
	return VideoObject().type();
}

PropertyPage * EditorVideoObject::propertyPage() const
{
	return new EditorVideoObjectPropertyPage;
}

void EditorVideoObject::drawObject(GfxRenderer * gl, const EditorDocument * document) const
{
	return;
}

SceneObject * EditorVideoObject::intersect(
	const EditorDocument * document, const Ray & ray, Vector3 * intPt)
{
	return NULL;	
}


///////////////////////////////////////////////////////////////////////////////
EditorVideoObjectPropertyPage::EditorVideoObjectPropertyPage() :
posValidator_(0, 2048, 0), dimValidator_(0, 2048, 0)
{	
	setGroupBox(false);
	
	core_ = new CorePropertiesPage;
	
	setName(tr("Video"));

	addPropertyPage(core_);
	addSpacing(4);
	
	startGroup(tr("Video"));

	mode_ = new ComboBoxProperty(tr("Mode"));
	mode_->addItem(tr("Local"));
	mode_->addItem(tr("URL"));
	addProperty(mode_);
	addSpacing(4);
	fileName_ = new FileNameProperty(
		tr("filename"), tr("Video (*.mp4 *.avi)"));
	addProperty(fileName_);
	addSpacing(4);
	
	//url_ = new EditProperty(tr("URL"));
	//addProperty(url_);

	//addSpacing(4);
	/////////////////////////////////
	urlPush_ = new QPushButton(tr("Set Url..."));
	connect(urlPush_, SIGNAL(clicked()), this, SLOT(onUrl()));
	addWidget(urlPush_);
	addSpacing(4);

	//////////////////////////////////
	playMode_ = new ComboBoxProperty(tr("Playback"));
	playMode_->addItem(tr("Fullscreen"));
	playMode_->addItem(tr("Region"));
	addProperty(playMode_);
	addSpacing(4);

	interactiveRegionBtn_ = new QPushButton(tr("Set region tool.."));
	interactiveRegionBtn_->setCheckable(true);
	connect(interactiveRegionBtn_, SIGNAL(clicked()), this, SLOT(onInteractiveRegion()));
	addWidget(interactiveRegionBtn_);
	addSpacing(4);
	x_ = new EditProperty(tr("x"), &posValidator_);
	addProperty(x_);
	y_ = new EditProperty(tr("y"), &posValidator_);
	addProperty(y_);
	width_ = new EditProperty(tr("width"), &dimValidator_);
	addProperty(width_);
	height_ = new EditProperty(tr("height"), &dimValidator_);
	addProperty(height_);

	addSpacing(4);
	showControls_ = new BoolProperty(tr("show controls"));
	addProperty(showControls_);

	endGroup();	
}

EditorVideoObjectPropertyPage::~EditorVideoObjectPropertyPage()
{
}

void EditorVideoObjectPropertyPage::setDocument(EditorDocument * document)
{
	if (document_)
	{
		GLWidget * glWidget = document_->glWidget();

		disconnect(glWidget, 0, this, 0);
		
	}

	//this function sets document_ field appropriately
	PropertyPage::setDocument(document);
	
	if (document)
	{
		GLWidget * glWidget = document->glWidget();				

		//the tool might still be active (for example: while in tool, create 
		//a new Video object).
		//Must deactivate it before deleting it.
		if (glWidget->tool() == regionTool_.get()) glWidget->setToolMode(GLWidget::Select);
		regionTool_.reset(new VideoRegionTool(glWidget));

		connect(glWidget, SIGNAL(toolChanged()), this, SLOT(onToolChanged()));	
		onToolChanged();
	}
}

void EditorVideoObjectPropertyPage::onToolChanged()
{
	interactiveRegionBtn_->setChecked(regionTool_.get() == document_->glWidget()->tool());	
}

//////////////////////////////////////////////////////////
void EditorVideoObjectPropertyPage::onUrl()
{
	VideoObject * vidObject = (VideoObject *)document_->selectedObject();
	
	QWidget * dlgParent = topWidget_;
	while (dlgParent->parent()) dlgParent = dlgParent->parentWidget();

	UrlDlg dlg(dlgParent, document_->document());
	sourceName_ = vidObject->sourceNames();
	sourceMode_ = vidObject->sourceMode();

	dlg.setting(sourceName_, sourceMode_);

	if (dlg.exec() ==  QDialog::Accepted)
	{
		sourceName_.clear();
		sourceMode_.clear();
		for (int i = 0 ; i < dlg.sourceNames().size() ; i++)
		{
			sourceName_.push_back(dlg.sourceNames()[i]);
		}
		for (int i = 0 ; i < dlg.sourceMode().size() ; i++)
		{
			sourceMode_.push_back(dlg.sourceMode()[i]);
		}

		document_->doCommand(makeChangePropertyCmd(ChangeObjectProperty,
			vidObject, sourceName_, &VideoObject::sourceNames, &VideoObject::setSourceNames));

		document_->doCommand(makeChangePropertyCmd(ChangeObjectProperty,
			vidObject, sourceMode_, &VideoObject::sourceMode, &VideoObject::setSourceMode));
	}
}
////////////////////////////////////////////////////////////////

void EditorVideoObjectPropertyPage::onInteractiveRegion()
{
	VideoObject * vidObject = (VideoObject *)document_->selectedObject();
	regionTool_->setObject(vidObject);

	if (interactiveRegionBtn_->isChecked())
	{
		document_->glWidget()->setTool(regionTool_.get());
	}
	else
	{
		document_->glWidget()->setToolMode(GLWidget::Select);
	}
}

void EditorVideoObjectPropertyPage::update()
{
	core_->update();
	VideoObject * obj = (VideoObject *)document_->selectedObject();

	bool isLocal = obj->sourceType() == VideoObject::File;
	mode_->setValue((int)!isLocal);
	if (isLocal)
	{
		fileName_->setEnabled(true);
		fileName_->setValue(stdStringToQString(obj->source()));
		//url_->setEnabled(false);
		//url_->setValue("");
		urlPush_->setEnabled(false);
	}
	else
	{
		fileName_->setEnabled(false);
		fileName_->setValue("");
		//url_->setEnabled(true);
		//url_->setValue(stdStringToQString(obj->source()));
		urlPush_->setEnabled(true);
	}

	playMode_->setValue((int)obj->playInRegion());
	if (obj->playInRegion())
	{
		x_->setEnabled(true);
		y_->setEnabled(true);
		width_->setEnabled(true);
		height_->setEnabled(true);
	}
	else
	{
		x_->setEnabled(false);
		y_->setEnabled(false);
		width_->setEnabled(false);
		height_->setEnabled(false);
	}

	x_->setValue(QString::number(obj->x()));
	y_->setValue(QString::number(obj->y()));
	width_->setValue(QString::number(obj->width()));
	height_->setValue(QString::number(obj->height()));

	showControls_->setValue(obj->showPlaybackControls());
}


void EditorVideoObjectPropertyPage::onChanged(Property * property)
{
	VideoObject * obj = (VideoObject *)document_->selectedObject();

	if (property == fileName_)
	{
		std::string fileName = qStringToStdString(fileName_->value());
		fileName = convertToRelativePath(Global::instance().readDirectory(), fileName);
		document_->doCommand(
			makeChangePropertyCmd(ChangeObjectProperty, 
			obj, fileName, &VideoObject::source, &VideoObject::setSource));
	}

	//if (property == url_)
	//{
	//	std::string url = qStringToStdString(url_->value());
	//	document_->doCommand(
	//		makeChangePropertyCmd(ChangeObjectProperty, 
	//		obj, url, &VideoObject::source, &VideoObject::setSource));
	//}

	if (property == mode_)
	{
		VideoObject::SourceType type = (VideoObject::SourceType)mode_->value();
		document_->doCommand(
			makeChangeObjectCmd(obj, type, &VideoObject::sourceType, &VideoObject::setSourceType));
	}

	if (property == playMode_)
	{
		bool playInRegion = (bool)playMode_->value();
		document_->doCommand(
			makeChangeObjectCmd(obj, playInRegion, 
			&VideoObject::playInRegion, &VideoObject::setPlayInRegion));
	}

	if (property == x_)
	{
		int val = x_->value().toInt();
		document_->doCommand(
			makeChangeObjectCmd(obj, val, 
			&VideoObject::x, &VideoObject::setX));
	}

	if (property == y_)
	{
		int val = y_->value().toInt();
		document_->doCommand(
			makeChangeObjectCmd(obj, val, 
			&VideoObject::y, &VideoObject::setY));
	}

	if (property == width_)
	{
		int val = width_->value().toInt();
		document_->doCommand(
			makeChangeObjectCmd(obj, val, 
			&VideoObject::width, &VideoObject::setWidth));
	}

	if (property == height_)
	{
		int val = height_->value().toInt();
		document_->doCommand(
			makeChangeObjectCmd(obj, val, 
			&VideoObject::height, &VideoObject::setHeight));
	}

	if (property == showControls_)
	{
		bool val = showControls_->value();
		document_->doCommand(
			makeChangeObjectCmd(obj, val, 
			&VideoObject::showPlaybackControls, &VideoObject::setShowPlaybackControls));
	}

	update();
}

///////////////////////////////////////////////////////////////////////////////