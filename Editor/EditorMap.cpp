#include "stdafx.h"
#include "EditorDocument.h"
#include "EditorMap.h"
#include "GLWidget.h"
#include "Command.h"
#include "Utils.h"
#include "FileUtils.h"
#include "Reader.h"
#include "Map.h"

using namespace std;
QString EditorMap::lastDir_;

EditorMap::EditorMap()
{
}

EditorMap::~EditorMap()
{
}

SceneObject * EditorMap::createSceneObject(EditorDocument * document) const
{		
	QString fileName = getOpenFileName(QObject::tr("Insert Map"), lastDir_, 
		QObject::tr("ST Model (*.stm)"));


	if (!fileName.isEmpty())
	{
		lastDir_ = QFileInfo(fileName).path();
		Map * map;
		map = new Map;
		map->setFileName(convertToRelativePath(qStringToStdString(fileName)));

		return map;
	}
	else
		return 0;
}

boost::uuids::uuid EditorMap::sceneObjectType() const
{
	return Map().type();
}

PropertyPage * EditorMap::propertyPage() const
{
	return new EditorMapPropertyPage;
}

void EditorMap::drawObject(GfxRenderer * gl, const EditorDocument * document) const
{
	glPolygonMode(GL_FRONT_AND_BACK , GL_FILL);
	gl->useTextureProgram();
	
	if (/*isThisAndAncestorsVisible() && */sceneObject_->isThisAndAncestorsVisible())
		sceneObject_->drawObject(gl);

	if (document->isObjectSelected(sceneObject_) || 
		document->isObjectAncestorSelected(sceneObject_))
	{		
		
		gl->useColorProgram();
		gl->setColorProgramColor(0, 1, 0, 1);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glPolygonOffset(-1, 0);
		
		Map * obj = (Map *)sceneObject_;
		BoundingBox bbox = obj->extents();
		
		GLfloat vertices[] = {
			static_cast<GLfloat>(bbox.minPt.x), static_cast<GLfloat>(bbox.minPt.y), static_cast<GLfloat>(bbox.minPt.z), 
			static_cast<GLfloat>(bbox.minPt.x), static_cast<GLfloat>(bbox.maxPt.y), static_cast<GLfloat>(bbox.minPt.z), 
			static_cast<GLfloat>(bbox.maxPt.x), static_cast<GLfloat>(bbox.maxPt.y), static_cast<GLfloat>(bbox.minPt.z), 
			static_cast<GLfloat>(bbox.maxPt.x), static_cast<GLfloat>(bbox.minPt.y), static_cast<GLfloat>(bbox.minPt.z),

			static_cast<GLfloat>(bbox.minPt.x), static_cast<GLfloat>(bbox.minPt.y), static_cast<GLfloat>(bbox.maxPt.z), 
			static_cast<GLfloat>(bbox.minPt.x), static_cast<GLfloat>(bbox.maxPt.y), static_cast<GLfloat>(bbox.maxPt.z), 
			static_cast<GLfloat>(bbox.maxPt.x), static_cast<GLfloat>(bbox.maxPt.y), static_cast<GLfloat>(bbox.maxPt.z), 
			static_cast<GLfloat>(bbox.maxPt.x), static_cast<GLfloat>(bbox.minPt.y), static_cast<GLfloat>(bbox.maxPt.z),

			static_cast<GLfloat>(bbox.minPt.x), static_cast<GLfloat>(bbox.minPt.y), static_cast<GLfloat>(bbox.minPt.z), 
			static_cast<GLfloat>(bbox.minPt.x), static_cast<GLfloat>(bbox.maxPt.y), static_cast<GLfloat>(bbox.minPt.z), 
			static_cast<GLfloat>(bbox.minPt.x), static_cast<GLfloat>(bbox.maxPt.y), static_cast<GLfloat>(bbox.maxPt.z), 
			static_cast<GLfloat>(bbox.minPt.x), static_cast<GLfloat>(bbox.minPt.y), static_cast<GLfloat>(bbox.maxPt.z),

			static_cast<GLfloat>(bbox.maxPt.x), static_cast<GLfloat>(bbox.minPt.y), static_cast<GLfloat>(bbox.minPt.z), 
			static_cast<GLfloat>(bbox.maxPt.x), static_cast<GLfloat>(bbox.maxPt.y), static_cast<GLfloat>(bbox.minPt.z), 
			static_cast<GLfloat>(bbox.maxPt.x), static_cast<GLfloat>(bbox.maxPt.y), static_cast<GLfloat>(bbox.maxPt.z), 
			static_cast<GLfloat>(bbox.maxPt.x), static_cast<GLfloat>(bbox.minPt.y), static_cast<GLfloat>(bbox.maxPt.z),

			static_cast<GLfloat>(bbox.minPt.x), static_cast<GLfloat>(bbox.minPt.y), static_cast<GLfloat>(bbox.minPt.z), 
			static_cast<GLfloat>(bbox.minPt.x), static_cast<GLfloat>(bbox.minPt.y), static_cast<GLfloat>(bbox.maxPt.z), 
			static_cast<GLfloat>(bbox.maxPt.x), static_cast<GLfloat>(bbox.minPt.y), static_cast<GLfloat>(bbox.maxPt.z), 
			static_cast<GLfloat>(bbox.maxPt.x), static_cast<GLfloat>(bbox.minPt.y), static_cast<GLfloat>(bbox.minPt.z),

			static_cast<GLfloat>(bbox.minPt.x), static_cast<GLfloat>(bbox.maxPt.y), static_cast<GLfloat>(bbox.minPt.z), 
			static_cast<GLfloat>(bbox.minPt.x), static_cast<GLfloat>(bbox.maxPt.y), static_cast<GLfloat>(bbox.maxPt.z), 
			static_cast<GLfloat>(bbox.maxPt.x), static_cast<GLfloat>(bbox.maxPt.y), static_cast<GLfloat>(bbox.maxPt.z), 
			static_cast<GLfloat>(bbox.maxPt.x), static_cast<GLfloat>(bbox.maxPt.y), static_cast<GLfloat>(bbox.minPt.z)
		};
		
		GLint prevDepthFunc;
		glGetIntegerv(GL_DEPTH_FUNC, &prevDepthFunc);
		glDepthFunc(GL_LEQUAL);
		gl->enableVertexAttribArrayPosition();
		gl->bindArrayBuffer(0);
		gl->vertexAttribPositionPointer(0, (char *)vertices);
		gl->applyCurrentShaderMatrix();
		glDrawArrays(GL_QUADS, 0, 6*4);
		glPolygonOffset(0, 0);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}


///////////////////////////////////////////////////////////////////////////////

EditorMapPropertyPage::EditorMapPropertyPage()
{
	setGroupBox(false);
	
	core_ = new CorePropertiesPage;
	visual_ = new VisualPropertiesPage;
	
	setName(tr("Map"));
	addPropertyPage(core_);
	addSpacing(4);
	addPropertyPage(visual_);	

	addSpacing(4);

	startGroup(tr("Map"));

	meshFileName_ = new FileNameProperty(
		tr("Mesh"), tr("ST Model (*.stm)"));
	addProperty(meshFileName_);

	addSpacing(4);

	pathMeshFileName_ = new FileNameProperty(
		tr("Path mesh"), tr("ST Model (*.stm)"));
	addProperty(pathMeshFileName_);
	
	endGroup();
}

EditorMapPropertyPage::~EditorMapPropertyPage()
{
}

void EditorMapPropertyPage::setDocument(EditorDocument * document)
{

	//this function sets document_ field appropriately
	PropertyPage::setDocument(document);
	
}

void EditorMapPropertyPage::update()
{
	Map * obj = (Map *)document_->selectedObject();
	core_->update();
	visual_->update();

	meshFileName_->setValue(stdStringToQString(obj->fileName()));
	pathMeshFileName_->setValue(stdStringToQString(obj->pathMeshFileName()));
}

void EditorMapPropertyPage::onChanged(Property * property)
{
	Map * obj = (Map *)document_->selectedObject();
	if (property == meshFileName_)
	{
		string fileName = qStringToStdString(meshFileName_->value());
		document_->doCommand(new ChangeMapFileNameCmd(
			document_, obj, fileName, obj->pathMeshFileName()));
	}

	if (property == pathMeshFileName_)
	{
		string fileName = qStringToStdString(pathMeshFileName_->value());
		document_->doCommand(new ChangeMapFileNameCmd(
			document_, obj, obj->fileName(), fileName));
	}

	update();
}

