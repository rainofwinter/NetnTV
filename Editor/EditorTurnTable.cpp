#include "stdafx.h"
#include "EditorDocument.h"
#include "EditorTurnTable.h"
#include "TurnTable.h"
#include "GLWidget.h"
#include "Command.h"
#include "Utils.h"
#include "MaskObject.h"

using namespace std;
QString EditorTurnTable::lastDir_;
EditorTurnTable::EditorTurnTable()
{
}

EditorTurnTable::~EditorTurnTable()
{
}

SceneObject * EditorTurnTable::createSceneObject(EditorDocument * document) const
{
	QStringList fileNames = getOpenFileNames(
		QObject::tr("Insert TurnTable images"), lastDir_, 
		QObject::tr("Images (*.png *.jpg *.jpeg)"));

	if (!fileNames.empty())
	{
		vector<string> stdFileNames;
		BOOST_FOREACH(QString str, fileNames)
			stdFileNames.push_back(convertToRelativePath(qStringToStdString(str)));

		TurnTable * obj = new TurnTable();
		obj->setImgFileNames(stdFileNames);

		return obj;
	}
	else
		return 0;
}

boost::uuids::uuid EditorTurnTable::sceneObjectType() const
{
	return TurnTable().type();
}

PropertyPage * EditorTurnTable::propertyPage() const
{
	return new EditorTurnTablePropertyPage;
}

void EditorTurnTable::drawObject(GfxRenderer * gl, const EditorDocument * document) const
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
		sceneObject_->drawObject(gl);

	if (document->isObjectSelected(sceneObject_) || 
		document->isObjectAncestorSelected(sceneObject_))
	{		
		gl->useColorProgram();
		gl->setColorProgramColor(0, 1, 0, 1);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glPolygonOffset(-1, 0);
		TurnTable * image = (TurnTable *)sceneObject_;
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
EditorTurnTablePropertyPage::EditorTurnTablePropertyPage() :
validator_(-100, 100, 4, 0)
{	
	setGroupBox(false);
	
	core_ = new CorePropertiesPage;
	visual_ = new VisualPropertiesPage;
	
	setName(tr("TurnTable"));
	/*
	AddLabel(tr("TurnTable"));
	AddSpacing(4);*/
	addPropertyPage(core_);
	addSpacing(4);
	addPropertyPage(visual_);
	addSpacing(4);

	speed_ = new EditProperty(tr("speed"), &validator_);
	addProperty(speed_);

	/*
	
	startGroup(tr("TurnTable"));	
	endGroup();
	*/
}

EditorTurnTablePropertyPage::~EditorTurnTablePropertyPage()
{
}

void EditorTurnTablePropertyPage::update()
{
	core_->update();
	visual_->update();
	TurnTable * obj = (TurnTable *)document_->selectedObject();

	QString str;
	str.sprintf("%.4f", obj->speed());
	speed_->setValue(str);

/*
	fileName_->setValue(QString::fromLocal8Bit(image->fileName()));*/
}

void EditorTurnTablePropertyPage::onChanged(Property * property)
{
	TurnTable * obj = (TurnTable *)document_->selectedObject();
	
	if (property == speed_)
	{
		float speed = speed_->value().toFloat();		
		document_->doCommand(makeChangePropertyCmd(
			ChangeObjectProperty, obj, speed, 
			&TurnTable::speed, 
			&TurnTable::setSpeed));
	}
}