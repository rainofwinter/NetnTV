#include "stdafx.h"
#include "ImageGrid.h"
#include "Mesh.h"
#include "Texture.h"
#include "Material.h"
#include "GfxRenderer.h"
#include "Writer.h"
#include "Xml.h"
#include "Reader.h"
#include "FileUtils.h"
#include "Image.h"
#include "Scene.h"
#include "Camera.h"
#include "Global.h"
#include "ImageGridImageClickedEvent.h"


using namespace std;

void ImageGrid::create()
{

	thumbWidth_ = 180;
	thumbHeight_ = 127;

	trajRandomness_ = 0.2f;
	trajDepth_ = 1000;

	gap_ = 15;

	width_ = 670;
	height_ = 553;

	duration_ = 1.0f;
	startTime_ = 0.0f;
	
	doAnimation_ = true;
	format_ = Texture::UncompressedRgba32;

	t_ = 0.0f;
}

ImageGrid::ImageGrid()
{
	create();
	setId("ImageGrid");
}

ImageGrid::ImageGrid(const ImageGrid & rhs, ElementMapping * elementMapping)
: SceneObject(rhs, elementMapping)
{
	create();
	fileNames_ = rhs.fileNames_;
	thumbWidth_ = rhs.thumbWidth_;
	thumbHeight_ = rhs.thumbHeight_;
	gap_ = rhs.gap_;
	width_ = rhs.width_;
	height_ = rhs.height_;
	trajDepth_ = rhs.trajDepth_;
	trajRandomness_ = rhs.trajRandomness_;
	duration_ = rhs.duration_;
	visualAttrib_ = rhs.visualAttrib_;
	startTime_ = rhs.startTime_;


}




ImageGrid::~ImageGrid()
{	
	uninit();
}

void ImageGrid::referencedFiles(std::vector<std::string> * refFiles) const
{
	SceneObject::referencedFiles(refFiles);
	for (int i = 0; i < (int)fileNames_.size(); ++i)
	{
		refFiles->push_back(fileNames_[i]);
	}
	
}

int ImageGrid::setReferencedFiles(
	const std::string & baseDirectory,
	const vector<string> & refFiles, int index)
{
	index = SceneObject::setReferencedFiles(baseDirectory, refFiles, index);
	for (int i = 0; i < (int)fileNames_.size(); ++i)
	{
		fileNames_[i] = refFiles[index++];
	}
	
	return index;
}

ImageGrid::ImageData::ImageData()
{
}

ImageGrid::ImageData::~ImageData()
{
	delete image;
}

void ImageGrid::init(GfxRenderer * gl, bool firstTime)
{
	SceneObject::init(gl, firstTime);
	
	int curX = 0;
	int curY = 0;

	trajDepth_ = (float)width_;
	if (height_ > width_) trajDepth_ = (float)height_;

	BOOST_FOREACH(string & fileName, fileNames_)
	{
		if (curY + thumbHeight_ > height_) break;
		ImageData * data = new ImageData;
		data->image = new Image;
		data->image->setFormat(format_);
		data->image->setFileName(fileName);
		data->image->setWidth(thumbWidth_);
		data->image->setHeight(thumbHeight_);
		data->image->init(gl);

		data->tRandMult = 1 + ((rand() % 1000) / 1000.0f) * (1.0f);

		data->startRotX = DegToRad((float)(rand() % 720));
		data->startRotY = DegToRad((float)(rand() % 720));
		data->startRotZ = DegToRad((float)(rand() % 720));

		data->dir = Vector3((float)curX, (float)curY, 0) - 
			Vector3((float)width_, (float)height_, 0);		

		float r;
		data->up = Vector3(0, 0, trajDepth_);
		r = (rand() % 1000) / 1000.0f;
		data->up.x = trajRandomness_*(-trajDepth_/2 + r*trajDepth_);
		r = (rand() % 1000) / 1000.0f;
		data->up.y = trajRandomness_*(-trajDepth_/2 + r*trajDepth_);
		
		images_.push_back(data);

		curX += thumbWidth_ + gap_;
		if (curX + thumbWidth_ > width_)
		{
			curX = 0;
			curY += thumbHeight_ + gap_;
		}
	}

}

void ImageGrid::uninit()
{	
	BOOST_FOREACH(ImageData * data, images_)
	{
		data->image->uninit();
		delete data;
	}
	images_.clear();
	SceneObject::uninit();
}

void ImageGrid::start(float docTime)
{
	t_ = 0;
	if (!doAnimation_) t_ = 1;
	docBaseStartTime_ = Global::currentTime();
}

void ImageGrid::preStart(float docTime)
{
	start(docTime);
}

bool ImageGrid::update(float sceneTime)
{	
	bool needRedraw = SceneObject::update(sceneTime);

	if (t_ < 1)
	{
		float t = (sceneTime - docBaseStartTime_ - startTime_ ) / duration_;	

		if (t < 0) t = 0;
		while (t > 1) t = 1;


		t_ = (t > 1)?1:t;
		needRedraw = true;
	}
	
	return needRedraw;
}

bool ImageGrid::asyncLoadUpdate()
{
	bool everythingLoaded = SceneObject::asyncLoadUpdate();
	
	BOOST_FOREACH(ImageData * data, images_)
	{
		everythingLoaded &= data->image->asyncLoadUpdate();
	}
	return everythingLoaded;	
}


void ImageGrid::drawObject(GfxRenderer * gl) const
{

	float fOpacity = totalOpacity();
	if (fOpacity == 0) return;

	for (int i = 0; i < (int)images_.size(); ++i)
	{
		Image * image = images_[i]->image;
		if (image->isLoaded())
		{
			image->visualAttrib()->setOpacity(fOpacity);
			float sx = (float)thumbWidth_/image->width();
			float sy = (float)thumbHeight_/image->height();
			
			float startX = (float)width_;
			float startY = (float)height_;

			Transform trans = image->transform();
			
			trans.setPivot((float)image->width() / 2, (float)image->height() / 2, 0);			

			float t = t_ * images_[i]->tRandMult;
			if (t > 1) t = 1;
			float y = 1-(t - 0.5f)*(t - 0.5f)/0.25f;			
			
			Vector3 pos = 
				Vector3((float)width_, (float)height_, 0) + 
				t*images_[i]->dir + y*images_[i]->up;
		
			trans.setTranslation(pos.x, pos.y, pos.z);

			float rt = 1 - t;
			trans.setRotation(
				rt*images_[i]->startRotX, 
				rt*images_[i]->startRotY, 
				rt*images_[i]->startRotZ);
			image->visualAttrib()->setOpacity(t);
			image->setTransform(trans);
			image->draw(gl);
		}
	}

}


BoundingBox ImageGrid::extents() const
{
	return BoundingBox(
		Vector3(0, 0, 0), 
		Vector3((float)width_, (float)height_, 0));
}

void ImageGrid::write(Writer & writer) const
{
	writer.writeParent<SceneObject>(this);
	writer.write(visualAttrib_, "visualAttrib");	

	writer.write(fileNames_, "fileNames");
	writer.write(thumbWidth_);
	writer.write(thumbHeight_);
	writer.write(gap_);
	writer.write(width_);
	writer.write(height_);
	writer.write(duration_);
	writer.write(startTime_);
	writer.write(doAnimation_);

	writer.write(format_);
}

void ImageGrid::read(Reader & reader, unsigned char version)
{
	reader.readParent<SceneObject>(this);
	reader.read(visualAttrib_);	

	reader.read(fileNames_);
	reader.read(thumbWidth_);
	reader.read(thumbHeight_);
	reader.read(gap_);
	reader.read(width_);
	reader.read(height_);
	reader.read(duration_);
	reader.read(startTime_);
	if (version >= 2) reader.read(doAnimation_);
	if (version >= 3) reader.read(format_);
}

void ImageGrid::writeXml(XmlWriter & w) const
{
	SceneObject::writeXml(w);
	w.startTag("VisualAttrib");	
	visualAttrib_.writeXml(w);
	w.endTag();
	for (unsigned int i = 0; i < (unsigned int)fileNames_.size(); ++i)
		w.writeTag("FileName", fileNames_[i]);
	w.writeTag("ThumbWidth", thumbWidth_);
	w.writeTag("ThumbHeight", thumbHeight_);
	w.writeTag("Gap", gap_);
	w.writeTag("Width", width_);
	w.writeTag("Height", height_);
	w.writeTag("Duration", duration_);
	w.writeTag("StartTime", startTime_);
}


void ImageGrid::readXml(XmlReader & r, xmlNode * parent)
{
	SceneObject::readXml(r, parent);
	std::string filename;
	
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;
		if (r.isNodeName(curNode, "VisualAttrib"))
		{
			visualAttrib_.readXml(r, curNode);
			curNode = curNode->next;
		}
		else if (r.getNodeContentIfName(filename, curNode, "FileName")) fileNames_.push_back(filename);
		else if (r.getNodeContentIfName(thumbWidth_, curNode, "ThumbWidth"));
		else if (r.getNodeContentIfName(thumbHeight_, curNode, "ThumbHeight"));
		else if (r.getNodeContentIfName(gap_, curNode, "Gap"));
		else if (r.getNodeContentIfName(width_, curNode, "Width"));
		else if (r.getNodeContentIfName(height_, curNode, "Height"));
		else if (r.getNodeContentIfName(duration_, curNode, "Duration"));
		else if (r.getNodeContentIfName(startTime_, curNode, "StartTime"));

	}
}

SceneObject * ImageGrid::intersect(Vector3 * out, const Ray & ray)
{
	if (!visualAttrib_.isVisible()) return 0;
	Matrix trans = parentTransform() * visualAttrib_.transformMatrix();


	Vector3 a = trans * Vector3(0, 0, 0);
	Vector3 b = trans * Vector3(0, (float)height_, 0);
	Vector3 c = trans * Vector3((float)width_, (float)height_, 0);
	Vector3 d = trans * Vector3((float)width_, 0, 0);

	if (triangleIntersectRay(out, a, b, c, ray) ||
		triangleIntersectRay(out, a, c, d, ray))
	{
		return this;
	}
	else return 0;
}

void ImageGrid::processCoords(const Vector2 & pos, 
	Vector2 * localCoords, Ray * mouseRay, Ray * invMouseRay)
{
	Vector3 pt;
	*mouseRay = parentScene_->camera()->unproject(parentScene_->screenToDeviceCoords(pos));
	intersect(&pt, *mouseRay);
	Matrix invTrans = 
		(parentTransform() * visualAttrib_.transformMatrix()).inverse();
	pt = invTrans * pt; 

	localCoords->x = pt.x;
	localCoords->y = pt.y;
	
	Vector3 mouseA = mouseRay->origin;
	Vector3 mouseB = mouseA + mouseRay->dir;
	mouseA = invTrans * mouseA;
	mouseB = invTrans * mouseB;
	*invMouseRay = Ray(mouseA, mouseB - mouseA);
}



bool ImageGrid::clickEvent(const Vector2 & screenCoords, int pressId)
{
	bool handled = SceneObject::clickEvent(screenCoords, pressId);
	Vector2 localPos;
	Ray mouseRay, invMouseRay; 
	
	processCoords(screenCoords, &localPos, &mouseRay, &invMouseRay);

	Vector3 intPt;
	bool intersected = false;
	static ImageGridImageClickedEvent event;

	for (int i = 0; i < (int)images_.size(); ++i)
	{

		intersected = images_[i]->image->intersect(&intPt, invMouseRay) != 0;
		if (intersected)
		{
			event.setImageIndex(i);
			break;
		}
	}

	if (intersected)	
		handled |= handleEvent(&event, Global::currentTime());

	return handled;
}