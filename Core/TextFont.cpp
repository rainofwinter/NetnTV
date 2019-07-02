#include "stdafx.h"
#include "TextFont.h"

#include "Reader.h"
#include "Xml.h"
#include "Writer.h"
#include "FileUtils.h"
#include "ScriptProcessor.h"

TextFont TextFont::fromScriptObject(ScriptProcessor * s, JSObject * obj)
{
	TextFont ret;

	getPropertyString(s->jsContext(), obj, "fontFile", &ret.fontFile);
	ret.fontFile = s->getActualFileName(ret.fontFile);
	//getPropertyInt(s->jsContext(), obj, "pointSize", &ret.pointSize);
	getPropertyFloat(s->jsContext(), obj, "pointSize", &ret.pointSize);
	getPropertyInt(s->jsContext(), obj, "faceIndex", &ret.faceIndex);
	getPropertyBool(s->jsContext(), obj, "italic", &ret.italic);
	getPropertyBool(s->jsContext(), obj, "bold", &ret.bold);	
	

	return ret;
}


///////////////////////////////////////////////////////////////////////////////

void TextFont::write(Writer & writer) const
{
	writer.write(fontFile, "fontFile");
	writer.write(pointSize, "pointSize");
	writer.write(faceIndex, "faceIndex");
	writer.write(italic, "italic");
	writer.write(bold, "bold");
	writer.write(shadow, "shadow");
	
}

void TextFont::writeXml(XmlWriter & w) const
{
	w.startTag(xmlTag());
	w.writeTag("FontFile", fontFile);
	w.writeTag("PointSize", pointSize);
	w.writeTag("FaceIndex", faceIndex);
	w.writeTag("Italic", italic);
	w.writeTag("Bold", bold);
	w.writeTag("Shadow", shadow);
	w.endTag();
}

void TextFont::readXml(XmlReader & r, xmlNode * parent)
{
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{
		if (curNode->type != XML_ELEMENT_NODE) continue;
		if (r.getNodeContentIfName(fontFile, curNode, "FontFile"));
		else if (r.getNodeContentIfName(pointSize, curNode, "PointSize"));
		else if (r.getNodeContentIfName(faceIndex, curNode, "FaceIndex"));
		else if (r.getNodeContentIfName(italic, curNode, "Italic"));
		else if (r.getNodeContentIfName(bold, curNode, "Bold"));
		else if (r.getNodeContentIfName(shadow, curNode, "Shadow"));
	}
}


enum OldLineSpacingMode
{
	LineSpacingAuto,
	LineSpacingCustom
};

void TextFont::read(Reader & reader, unsigned char version)
{
	reader.read(fontFile);
	if(version < 3)
	{
		int temp;
		reader.read(temp);

		pointSize = temp;
	}
	else
		reader.read(pointSize);
	reader.read(faceIndex);
	reader.read(italic);
	reader.read(bold);

	if (version == 1)
	{
		OldLineSpacingMode lineSpacingMode;
		int lineSpacing;
		reader.read(lineSpacingMode);
		reader.read(lineSpacing);
	}

	if (version >= 3)
	{
		reader.read(shadow);
	}
}