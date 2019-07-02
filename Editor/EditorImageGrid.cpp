#include "stdafx.h"
#include "EditorDocument.h"
#include "EditorImageGrid.h"
#include "ImageGrid.h"
#include "GLWidget.h"
#include "Command.h"
#include "Utils.h"
#include "ImageGridDlg.h"
#include "MaskObject.h"

using namespace std;
QString EditorImageGrid::lastDir_;
EditorImageGrid::EditorImageGrid()
{
}

EditorImageGrid::~EditorImageGrid()
{
}

void EditorImageGrid::init()
{
	ImageGrid * grid = (ImageGrid *)sceneObject_;
	sceneObject_->update(FLT_MAX);
}

SceneObject * EditorImageGrid::createSceneObject(EditorDocument * document) const
{	
	ImageGrid * img = new ImageGrid();
	img->update(FLT_MAX);
	return img;
}

boost::uuids::uuid EditorImageGrid::sceneObjectType() const
{
	return ImageGrid().type();
}

PropertyPage * EditorImageGrid::propertyPage() const
{
	return new EditorImageGridPropertyPage;
}

void EditorImageGrid::drawObject(GfxRenderer * gl, const EditorDocument * document) const
{
	
	glPolygonMode(GL_FRONT_AND_BACK , GL_FILL);
	gl->useTextureProgram();
	
	SceneObject * parent = sceneObject_->parent();
	
	bool check = true;
	while(parent)
	{
		check = !(parent->type() == MaskObject().type());

		if(!check) break;
		parent = parent->parent();
	}

	if (/*isThisAndAncestorsVisible() && */sceneObject_->isThisAndAncestorsVisible() && check)
	{
		sceneObject_->drawObject(gl);
	}

	if (document->isObjectSelected(sceneObject_) || 
		document->isObjectAncestorSelected(sceneObject_))
	{		
		gl->useColorProgram();
		gl->setColorProgramColor(0, 1, 0, 1);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glPolygonOffset(-1, 0);
		ImageGrid * image = (ImageGrid *)sceneObject_;
		GLfloat vertices[] = {
			0, 0, 0, 
			static_cast<GLfloat>(image->width()), 0, 0,
			static_cast<GLfloat>(image->width()), static_cast<GLfloat>(image->height()), 0,
			0, static_cast<GLfloat>(image->height()), 0
		};
		
		gl->enableVertexAttribArrayPosition();
		gl->bindArrayBuffer(0);
		gl->vertexAttribPositionPointer(0, (char *)vertices);	
		gl->applyCurrentShaderMatrix();
		glDrawArrays(GL_QUADS, 0, 4);
		glPolygonOffset(0, 0);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	}
}


///////////////////////////////////////////////////////////////////////////////
EditorImageGridPropertyPage::EditorImageGridPropertyPage() :
dimValidator_(0, 9999, 0),
timeValidator_(0, 9999, 4, 0)
{
	
	setGroupBox(false);
	
	core_ = new CorePropertiesPage;
	visual_ = new VisualPropertiesPage;
	
	setName(tr("ImageGrid"));

	addPropertyPage(core_);
	addSpacing(4);
	addPropertyPage(visual_);
	addSpacing(4);
	
	startGroup(tr("ImageGrid"));	

	filesButton_ = new QPushButton(tr("Files..."));
	connect(filesButton_, SIGNAL(clicked()), this, SLOT(onFiles()));
	addWidget(filesButton_);

	addSpacing(4);

	width_ = new EditProperty(tr("width"), &dimValidator_);
	addProperty(width_);
	height_ = new EditProperty(tr("height"), &dimValidator_);
	addProperty(height_);

	addSpacing(4);
	thumbWidth_ = new EditProperty(tr("thumb width"), &dimValidator_);
	addProperty(thumbWidth_);
	thumbHeight_ = new EditProperty(tr("thumb height"), &dimValidator_);
	addProperty(thumbHeight_);
	gap_ = new EditProperty(tr("gap"), &dimValidator_);
	addProperty(gap_);
	
	addSpacing(4);
	startGroup(tr("Animation"));
	doAnimation_ = new BoolProperty(tr("play animation"));	
	addProperty(doAnimation_, PropertyPage::Vertical);
	addSpacing(4);
	duration_ = new EditProperty(tr("duration"), &timeValidator_);
	addProperty(duration_);
	startTime_ = new EditProperty(tr("start time"), &timeValidator_);
	addProperty(startTime_);
	endGroup();

	endGroup();	
	
}

EditorImageGridPropertyPage::~EditorImageGridPropertyPage()
{

}

void EditorImageGridPropertyPage::onFiles()
{
	ImageGrid * obj = 
			(ImageGrid *)document_->selectedObject();

	QWidget * dlgParent = topWidget_;
	while (dlgParent->parent()) dlgParent = dlgParent->parentWidget();
	ImageGridDlg dlg(dlgParent, document_->document(), obj->fileNames());
	
	if (dlg.exec() == QDialog::Accepted)
	{
		document_->doCommand(
			makeChangeObjectCmd(obj, dlg.fileNames(), &ImageGrid::fileNames,
			&ImageGrid::setFileNames));	
	}
}

void EditorImageGridPropertyPage::update()
{
	core_->update();
	visual_->update();
	ImageGrid * obj = (ImageGrid *)document_->selectedObject();


	QString str;

	str.sprintf("%.4f", obj->startTime());
	startTime_->setValue(str);
	str.sprintf("%.4f", obj->duration());
	duration_->setValue(str);

	
	str.sprintf("%d", obj->thumbWidth());
	thumbWidth_->setValue(str);
	str.sprintf("%d", obj->thumbHeight());
	thumbHeight_->setValue(str);
	str.sprintf("%d", obj->gap());
	gap_->setValue(str);

	str.sprintf("%d", obj->width());
	width_->setValue(str);
	str.sprintf("%d", obj->height());
	height_->setValue(str);

	doAnimation_->setValue(obj->doAnimation());
	enableAnimationFields(obj->doAnimation());
}

void EditorImageGridPropertyPage::enableAnimationFields(bool enable)
{
	duration_->setEnabled(enable);
	startTime_->setEnabled(enable);
}

void EditorImageGridPropertyPage::onChanged(Property * property)
{
	ImageGrid * obj = (ImageGrid *)document_->selectedObject();
	
	if (property == thumbWidth_)
	{
		int val = thumbWidth_->value().toInt();
		document_->doCommand(makeChangeObjectCmd(
			obj, val, &ImageGrid::thumbWidth, &ImageGrid::setThumbWidth));
	}
	if (property == thumbHeight_)
	{
		int val = thumbHeight_->value().toInt();
		document_->doCommand(makeChangeObjectCmd(
			obj, val, &ImageGrid::thumbHeight, &ImageGrid::setThumbHeight));
	}
	if (property == gap_)
	{
		int val = gap_->value().toInt();
		document_->doCommand(makeChangeObjectCmd(
			obj, val, &ImageGrid::gap, &ImageGrid::setGap));
	}
	if (property == width_)
	{
		int val = width_->value().toInt();
		document_->doCommand(makeChangeObjectCmd(
			obj, val, &ImageGrid::width, &ImageGrid::setWidth));
	}
	if (property == height_)
	{
		int val = height_->value().toInt();
		document_->doCommand(makeChangeObjectCmd(
			obj, val, &ImageGrid::height, &ImageGrid::setHeight));
	}
	if (property == doAnimation_)
	{
		bool val = doAnimation_->value();
		document_->doCommand(makeChangeObjectCmd(
			obj, val, &ImageGrid::doAnimation, &ImageGrid::setDoAnimation));
	}
	if (property == duration_)
	{
		float val = duration_->value().toFloat();
		enableAnimationFields(val);
		document_->doCommand(makeChangeObjectCmd(
			obj, val, &ImageGrid::duration, &ImageGrid::setDuration));
	}
	if (property == startTime_)
	{
		float val = startTime_->value().toFloat();
		document_->doCommand(makeChangeObjectCmd(
			obj, val, &ImageGrid::startTime, &ImageGrid::setStartTime));
	}
}
