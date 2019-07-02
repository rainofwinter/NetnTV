#include "stdafx.h"
#include "Panorama.h"
#include "Mesh.h"
#include "Texture.h"
#include "Material.h"
#include "GfxRenderer.h"
#include "Writer.h"
#include "Xml.h"
#include "Reader.h"
#include "FileUtils.h"
#include "Scene.h"
#include "Camera.h"
#include "TextureRenderer.h"
#include "MeshPart.h"

using namespace std;

void Panorama::create()
{
	for (int i = 0; i < 6; ++i)
	{
		textures_[i] = new Texture;
		materials_[i] = new Material;
		materials_[i]->SetDiffuseMap(textures_[i]);
	}
	
	width_ = 512;
	height_ = 512;

	/*
	fileNames_[0] = "C:\\ST4\\panorama\\Res\\back.jpg";
	fileNames_[1] = "C:\\ST4\\panorama\\Res\\down.jpg";
	fileNames_[2] = "C:\\ST4\\panorama\\Res\\front.jpg";
	fileNames_[3] = "C:\\ST4\\panorama\\Res\\left.jpg";
	fileNames_[4] = "C:\\ST4\\panorama\\Res\\right.jpg";
	fileNames_[5] = "C:\\ST4\\panorama\\Res\\top.jpg";
	*/
	fileNames_[0] = "";
	fileNames_[1] = "";
	fileNames_[2] = "";
	fileNames_[3] = "";
	fileNames_[4] = "";
	fileNames_[5] = "";

	Vector3 verts[] =
	{
		//back
		Vector3(1, -1, -1),
		Vector3(1, 1, -1),
		Vector3(-1, 1, -1),
		Vector3(-1, -1, -1),
		
		
		//down
		Vector3(-1, 1, 1),
		Vector3(-1, 1, -1),
		Vector3(1, 1, -1),
		Vector3(1, 1, 1),

		//front
		Vector3(-1, -1, 1),
		Vector3(-1, 1, 1),
		Vector3(1, 1, 1),
		Vector3(1, -1, 1),

		//left
		Vector3(-1, -1, -1),
		Vector3(-1, 1, -1),
		Vector3(-1, 1, 1),
		Vector3(-1, -1, 1),

		//right
		Vector3(1, -1, 1),
		Vector3(1, 1, 1),
		Vector3(1, 1, -1),
		Vector3(1, -1, -1),

		//top				
		Vector3(-1, -1, -1),
		Vector3(-1, -1, 1),
		Vector3(1, -1, 1),
		Vector3(1, -1, -1)
	};

	Vector2 texCoords[] = 
	{
		Vector2(0, 0),
		Vector2(0, 1),
		Vector2(1, 1),
		Vector2(1, 0),		

		Vector2(0, 0),
		Vector2(0, 1),
		Vector2(1, 1),
		Vector2(1, 0),	

		Vector2(0, 0),
		Vector2(0, 1),
		Vector2(1, 1),
		Vector2(1, 0),	

		Vector2(0, 0),
		Vector2(0, 1),
		Vector2(1, 1),
		Vector2(1, 0),	

		Vector2(0, 0),
		Vector2(0, 1),
		Vector2(1, 1),
		Vector2(1, 0),	

		Vector2(0, 0),
		Vector2(0, 1),
		Vector2(1, 1),
		Vector2(1, 0)
	};

	std::vector<VertexData> vertexDataList;
	int numVerts = 24;

	for(int i = 0; i < numVerts; ++i)
	{
		VertexData vertexData;
		vertexData.position = verts[i];
		vertexData.texCoords = texCoords[i];
		vertexDataList.push_back(vertexData);
	}

	mesh_ = new Mesh;
	mesh_->setVertices(vertexDataList);

	mesh_->setTransformMatrix(
		Matrix::Scale(100, 100, 100));

	unsigned short indices[] = {0, 1, 2, 0, 2, 3};
	for (int i = 0; i < 6; ++i)
	{		
		mesh_->addMeshPart(
			vector<unsigned short>(indices, indices + 6),
			materials_[i]);

		for (int j = 0; j < 6; ++j) indices[j] += 4;
	}

	
	

	textureRenderer_ = new TextureRenderer;
	texture_ = new Texture;
	
	pressed_ = false;
	startedMoving_ = false;
	redrawTriggered_ = false;
}

Panorama::Panorama()
{
	create();
	setId("Panorama");
}

Panorama::Panorama(const Panorama & rhs, ElementMapping * elementMapping)
: SceneObject(rhs, elementMapping)
{
	create();
	for (int i = 0; i < 6; ++i)
	{
		fileNames_[i] = rhs.fileNames_[i];
	}	
	visualAttrib_ = rhs.visualAttrib_;
	width_ = rhs.width_;
	height_ = rhs.height_;
}




Panorama::~Panorama()
{
	uninit();
	delete mesh_;
	for (int i = 0; i < 6; ++i)
	{
		delete textures_[i];	
		delete materials_[i];
	}
	delete textureRenderer_;
	delete texture_;
}

void Panorama::referencedFiles(std::vector<std::string> * refFiles) const
{
	SceneObject::referencedFiles(refFiles);
	for (int i = 0; i < 6; ++i)
	{
		refFiles->push_back(fileNames_[i]);
	}
	
}

int Panorama::setReferencedFiles(const std::string & baseDirectory,
	const vector<string> & refFiles, int index)
{
	index = SceneObject::setReferencedFiles(baseDirectory, refFiles, index);
	for (int i = 0; i < 6; ++i)
	{
		fileNames_[i] = refFiles[index++];
	}
	
	return index;
}


void Panorama::init(GfxRenderer * gl, bool firstTime)
{
	SceneObject::init(gl, firstTime);
	for (int i = 0; i < 6; ++i)
	{
		textures_[i]->init(gl, fileNames_[i], Texture::UncompressedRgba32);
		materials_[i]->init(gl);
	}
	mesh_->init(gl);

	camXAngle_ = 0;
	camYAngle_ = 0;

	texture_->init(gl, (int)ceil(width_), (int)ceil(height_), Texture::UncompressedRgb16, 0);

	camera_.SetTo2DArea(width_/2, height_/2, width_, height_, 60, width_/height_);
	textureRenderer_->init(gl, texture_->width(), texture_->height(), false);
	textureRenderer_->setTexture(texture_);

	camera_.setLookAt(Vector3(0, 0, 1));
	camera_.setEye(Vector3(0, 0, 0));
	camera_.setFovY(90);
	camera_.setAspectRatio(width_/height_);
}

void Panorama::uninit()
{	
	mesh_->uninit();
	for (int i = 0; i < 6; ++i)
	{
		textures_[i]->uninit();
		materials_[i]->uninit();
	}

	
	textureRenderer_->uninit();
	texture_->uninit();
	SceneObject::uninit();
}

bool Panorama::pressEvent(const Vector2 & startPos, int pressId)
{
	pressed_ = true;
	startedMoving_ = false;
	curPressId_ = pressId;

	invTransMatrix_ = 
		(parentTransform() * visualAttrib_.transformMatrix()).inverse();
	
	pressCamXAngle_ = camXAngle_;
	pressCamYAngle_ = camYAngle_;

	return true;
}

bool Panorama::releaseEvent(const Vector2 & pos, int pressId)
{
	if (pressId != curPressId_) return false;
	pressed_ = false;
	
	return true;
}

bool Panorama::moveEvent(const Vector2 & pos, int pressId)
{
	if (pressId != curPressId_) return false;

	if (!startedMoving_)
	{
		startedMoving_ = true;
		pressPos_ = localCoords(pos, invTransMatrix_);
	}
	Vector2 movePos = localCoords(pos, invTransMatrix_);

	float xDelta = movePos.x - pressPos_.x;
	float yDelta = movePos.y - pressPos_.y;
	
	
	camXAngle_ = pressCamXAngle_ + 0.004f*xDelta;
	camYAngle_ = pressCamYAngle_ + 0.004f*yDelta;
	if (camYAngle_ > M_PI_2) camYAngle_ = (float)M_PI_2;
	if (camYAngle_ < -M_PI_2) camYAngle_ = -(float)M_PI_2;


	Matrix rotMat = 
		Matrix::Rotate(camXAngle_, 0, 1, 0) * 
		Matrix::Rotate(camYAngle_, -1, 0, 0);
	Vector3 lookAt = rotMat * Vector3(0, 0, 1);
	Vector3 up = rotMat * Vector3(0, -1, 0);
	
	camera_.setLookAt(lookAt);
	camera_.setUp(up);

	redrawTriggered_ = true;
	return true;
}


bool Panorama::update(float sceneTime)
{	
	bool needRedraw = SceneObject::update(sceneTime);

	if (redrawTriggered_)
	{
		needRedraw = true;
		redrawTriggered_ = false;
	}
	
	for (int i = 0; i < 6; ++i)
	{
		needRedraw |= ! textures_[i]->isLoaded();
	}
	
	return needRedraw;
}

bool Panorama::asyncLoadUpdate()
{
	bool everythingLoaded = true;
	for (int i = 0; i < 6; ++i)
	{
		textures_[i]->asyncLoadUpdate();
		everythingLoaded &= textures_[i]->isLoaded();
	}

	return everythingLoaded;	
}

bool Panorama::isLoaded() const
{
	bool isLoaded = true;
	for (int i = 0; i < 6; ++i)
	{
		isLoaded &= textures_[i]->isLoaded();
	}

	return isLoaded;	
}

void Panorama::drawObject(GfxRenderer * gl) const
{
	float fOpacity = totalOpacity();
	if (fOpacity == 0) return;	
	
	gl->useTextureProgram();
	gl->setTextureProgramOpacity(1);


	
	textureRenderer_->beginRenderToTexture(gl);	
	
	//glDisable(GL_STENCIL_TEST);	
	Matrix prevCamMatrix = gl->cameraMatrix();		
	gl->setCameraMatrix(camera_.GetMatrix());
	gl->pushMatrix();
	gl->loadMatrix(Matrix::Identity());

	const vector<MeshPart *> & meshParts = mesh_->meshParts();
	for (int i = 0; i < (int)meshParts.size(); ++i)
	{
		gl->use(meshParts[i]->material()->diffuseMap());
		mesh_->drawBare(gl, i);		
	}

	gl->popMatrix();		
	gl->setCameraMatrix(prevCamMatrix);	

	//glEnable(GL_STENCIL_TEST);
	
	textureRenderer_->endRenderToTexture(gl);


	gl->setTextureProgramOpacity(fOpacity);	
	gl->use(texture_);
	gl->drawRect(0, height_, width_, -height_);
	

	/*
	glDisable(GL_STENCIL_TEST);	
	Matrix prevCamMatrix = gl->cameraMatrix();		
	gl->setCameraMatrix(camera_.GetMatrix());
	gl->pushMatrix();
	gl->loadMatrix(Matrix::Identity());
	
	const vector<MeshPart *> & meshParts = mesh_->meshParts();
	for (int i = 0; i < (int)meshParts.size(); ++i)
	{
		gl->use(meshParts[i]->material()->diffuseMap());
		mesh_->drawBare(gl, i);		
	}

	gl->popMatrix();		
	gl->setCameraMatrix(prevCamMatrix);	

	glEnable(GL_STENCIL_TEST);
	*/

}


BoundingBox Panorama::extents() const
{
	return BoundingBox(
		Vector3(0, 0, 0), 
		Vector3(width_, height_, 0));
}

void Panorama::write(Writer & writer) const
{
	writer.writeParent<SceneObject>(this);
	writer.write(visualAttrib_, "visualAttrib");

	writer.write(width_, "width");
	writer.write(height_, "height");

	for (int i = 0; i < 6; ++i)
	{
		writer.write(fileNames_[i], "fileName");
	}	
	
	
}

void Panorama::read(Reader & reader, unsigned char version)
{
	reader.readParent<SceneObject>(this);
	reader.read(visualAttrib_);

	reader.read(width_);
	reader.read(height_);

	for (int i = 0; i < 6; ++i)
	{
		reader.read(fileNames_[i]);
	}	
}

void Panorama::writeXml(XmlWriter & w) const
{
	SceneObject::writeXml(w);
	w.startTag("VisualAttrib");	
	visualAttrib_.writeXml(w);
	w.endTag();
	w.writeTag("Width", width_);
	w.writeTag("Height", height_);
	for (int i = 0; i < 6; ++i)
		w.writeTag("FileName", fileNames_[i]);
}

void Panorama::readXml(XmlReader & r, xmlNode * parent)
{
	SceneObject::readXml(r, parent);
	std::string filename;
	int i = 0;
	
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;
		if (r.isNodeName(curNode, "VisualAttrib"))
		{
			visualAttrib_.readXml(r, curNode);
			curNode = curNode->next;
		}
		else if (r.getNodeContentIfName(width_, curNode, "Width"));
		else if (r.getNodeContentIfName(height_, curNode, "Height"));
		else if (r.getNodeContentIfName(filename, curNode, "FileName")) fileNames_[i++] = filename;
	}
}

const float & Panorama::width() const
{
	return width_;
}

const float & Panorama::height() const
{
	return height_;
}

SceneObject * Panorama::intersect(Vector3 * out, const Ray & ray)
{
	if (intersectRect(out, ray)) return this;
	else return 0;
}

Vector2 Panorama::localCoords(
	const Vector2 & pos, const Matrix & invTransMatrix) const
{
	Ray mouseRay = parentScene_->camera()->unproject(
		parentScene_->screenToDeviceCoords(pos));
	Vector3 pt;
	intersectRect(&pt, mouseRay);	
	pt = invTransMatrix * pt;
	return Vector2(pt.x, pt.y);
}

bool Panorama::intersectRect(Vector3 * out, const Ray & ray) const
{
	Matrix trans = parentTransform() * visualAttrib_.transformMatrix();
	
	Vector3 a = trans * Vector3(0, 0, 0);
	Vector3 b = trans * Vector3(0, height(), 0);
	Vector3 c = trans * Vector3(width(), height(), 0);
	Vector3 d = trans * Vector3(width(), 0, 0);

	if (triangleIntersectRay(out, a, b, c, ray) ||
		triangleIntersectRay(out, a, c, d, ray))
	{
		return true;
	}
	else return false;
}