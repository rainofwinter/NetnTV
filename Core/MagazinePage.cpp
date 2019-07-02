#include "stdafx.h"
#include "MagazinePage.h"
#include "Writer.h"
#include "Reader.h"
#include "FileUtils.h"
#include "Scene.h"
#include "Xml.h"
#include "Document.h"
#include "Global.h"
using namespace std;

void MagazinePage::write(Writer & writer) const
{
	writer.write(scene_, "scene");

	writer.write(thumbnailFile_, "thumbnail");
}

void MagazinePage::read(Reader & reader, unsigned char version)
{
	reader.read(scene_);
	if (version == 0)
	{
		string tempStr;
		reader.read(tempStr);
		thumbnailFile_ = string(tempStr.begin(), tempStr.end());
	}
	else
		reader.read(thumbnailFile_);
}

void MagazinePage::writeXml(XmlWriter & w) const
{
	w.writeTag("Scene", scene_);
	w.writeTag("Thumbnail", convertToRelativePath(Global::instance().readDirectory(), thumbnailFile_));
}

void MagazinePage::readXml(XmlReader & r, xmlNode * parent)
{
	unsigned val;
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;
		if (r.getNodeContentIfName(val, curNode, "Scene"))
		{
			scene_ = r.idMapping().getScene(val);
		}
		else if (r.getNodeContentIfName(thumbnailFile_, curNode, "Thumbnail"));
	}
}