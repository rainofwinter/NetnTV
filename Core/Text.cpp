#include "stdafx.h"
#include "Text.h"
#include "Mesh.h"
#include "Texture.h"
#include "Material.h"
#include "GfxRenderer.h"
#include "Writer.h"
#include "Xml.h"
#include "Reader.h"
#include "FileUtils.h"
#include "Camera.h"
#include "Global.h"

#include "Document.h"
#include "TextFeatures.h"
#include "Scene.h"
#include "TextHandlerOld.h"

#include "utf8.h"
#include "ScriptProcessor.h"
#include <libxml/HTMLparser.h>

using namespace std;
///////////////////////////////////////////////////////////////////////////////

string char2hex_text( char dec )
{
	char dig1 = (dec&0xF0)>>4;
	char dig2 = (dec&0x0F);
	if ( 0<= dig1 && dig1<= 9) dig1+=48;    //0,48inascii
	if (10<= dig1 && dig1<=15) dig1+=97-10; //a,97inascii
	if ( 0<= dig2 && dig2<= 9) dig2+=48;
	if (10<= dig2 && dig2<=15) dig2+=97-10;
 
	string r;
	r.append( &dig1, 1);
	r.append( &dig2, 1);
	return r;
}

string urlencode_text(const string &c)
{
 
	string escaped="";
	int max = c.length();
	for(int i=0; i<max; i++)
	{
		if ( (48 <= c[i] && c[i] <= 57) ||//0-9
			 (65 <= c[i] && c[i] <= 90) ||//abc...xyz
			 (97 <= c[i] && c[i] <= 122) || //ABC...XYZ
			 (c[i]=='~' || c[i]=='!' || c[i]=='*' || c[i]=='(' || c[i]==')' || c[i]=='\'')
		)
		{
			escaped.append( &c[i], 1);
		}
		else
		{
			escaped.append("%");
			escaped.append( char2hex_text(c[i]) );//converts char 255 to string "ff"
		}
	}
	return escaped;
}

JSClass jsTextClass = InitClass(
	"Text", JSCLASS_HAS_PRIVATE | JSCLASS_MARK_IS_TRACE, JS_PropertyStub, 
	JS_StrictPropertyStub, SceneObject_finalize, SceneObject::gcMark);

JSBool Text_setText(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	Text * thisObj = (Text *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));

	jschar * jsStr;
	if (!JS_ConvertArguments(cx, argc, argv, "W", &jsStr)) return JS_FALSE;
	JS_SET_RVAL(cx, vp, JSVAL_VOID);

		
	thisObj->setTextString(jsCharStrToStdWString(jsStr));
	thisObj->init(thisObj->parentScene()->parentDocument()->renderer());
	return JS_TRUE;
};

JSBool Text_getText(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	Text * thisObj = (Text *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	if (!JS_ConvertArguments(cx, argc, argv, "")) return JS_FALSE;

	string narrowString = "";
	utf8::utf16to8(thisObj->textString().begin(), thisObj->textString().end(), back_inserter(narrowString));
	string str = urlencode_text(narrowString);

	const char * textStr = str.c_str();
	JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(JS_NewStringCopyZ(cx, textStr)));
	return JS_TRUE;
};

JSBool Text_setProperties(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	Text * thisObj = (Text *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	JSObject * jsProperties;
	if (!JS_ConvertArguments(cx, argc, argv, "o", &jsProperties)) return JS_FALSE;
	JS_SET_RVAL(cx, vp, JSVAL_VOID);

	if (!JS_IsArrayObject(cx, jsProperties))
	{
		JS_ReportError(cx, "Text::setProperties - Parameter must be an array.");
		return JS_FALSE;
	}

	jsuint jsLen;
	JS_GetArrayLength(cx, jsProperties, &jsLen);

	std::vector<TextProperties> propertiesList;
	propertiesList.resize(jsLen);

	for (int i = 0; i < (int)jsLen; ++i)
	{
		jsval jsCurVal;
		JS_GetElement(cx, jsProperties, i, &jsCurVal);		
		if (!JSVAL_IS_OBJECT(jsCurVal)) continue;
		JSObject * jsProperties = JSVAL_TO_OBJECT(jsCurVal);

		getPropertyInt(cx, jsProperties, "index", &propertiesList[i].index);
		getPropertyBool(cx, jsProperties, "underline", &propertiesList[i].underline);
		getPropertyString(cx, jsProperties, "link", &propertiesList[i].link);		

		JS_GetProperty(cx, jsProperties, "font", &jsCurVal);
		if (JSVAL_IS_OBJECT(jsCurVal))
		{
			propertiesList[i].font = 
				TextFont::fromScriptObject(s, JSVAL_TO_OBJECT(jsCurVal));
		}

		JS_GetProperty(cx, jsProperties, "color", &jsCurVal);
		if (JSVAL_IS_OBJECT(jsCurVal))
		{
			propertiesList[i].color = 
				Color::fromScriptObject(s, JSVAL_TO_OBJECT(jsCurVal));
		}

	}

	thisObj->setProperties(propertiesList);

	return JS_TRUE;
};


JSBool Text_setBoundaryWidth(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	Text * thisObj = (Text *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	jsdouble jsArg1;
	if (!JS_ConvertArguments(cx, argc, argv, "d", &jsArg1)) return JS_FALSE;
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	thisObj->setBoundaryWidth((float)jsArg1);	
	return JS_TRUE;
};

JSBool Text_setBoundaryHeight(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	Text * thisObj = (Text *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	jsdouble jsArg1;
	if (!JS_ConvertArguments(cx, argc, argv, "d", &jsArg1)) return JS_FALSE;
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	thisObj->setBoundaryHeight((float)jsArg1);	
	return JS_TRUE;
};

JSBool Text_setFontSize(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	Text * thisObj = (Text *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	jsdouble jsArg1;
	if (!JS_ConvertArguments(cx, argc, argv, "d", &jsArg1)) return JS_FALSE;
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	std::vector<TextProperties> props = thisObj->properties();
	if (props.size() > 0)
	{
		TextProperties prop = props.front();
		props.clear();
		prop.font.pointSize = (int)jsArg1;
		props.push_back(prop);
		thisObj->setProperties(props);
		thisObj->init(s->document()->renderer());
	}
	return JS_TRUE;
};

JSBool Text_setColor(JSContext *cx, uintN argc, jsval *vp)
{
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	jsval * argv = JS_ARGV(cx, vp);
	Text * thisObj = (Text *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	JSObject * jsObj;
	if (!JS_ConvertArguments(cx, argc, argv, "o", &jsObj)) return JS_FALSE;	
	Color color = Color::fromScriptObject(s, jsObj);
	thisObj->setColor(color);	
	s->document()->triggerRedraw();
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
};

JSBool Text_getWidth(JSContext *cx, uintN argc, jsval *vp)
{
	JSObject * obj = JS_THIS_OBJECT(cx, vp);
	Text * text = (Text *)JS_GetPrivate(cx, obj);	
	JS_SET_RVAL(cx, vp, DOUBLE_TO_JSVAL(text->width()));
	return JS_TRUE;
}

JSBool Text_getHeight(JSContext *cx, uintN argc, jsval *vp)
{
	JSObject * obj = JS_THIS_OBJECT(cx, vp);
	Text * text = (Text *)JS_GetPrivate(cx, obj);	
	JS_SET_RVAL(cx, vp, DOUBLE_TO_JSVAL(text->height()));
	return JS_TRUE;
}

JSFunctionSpec TextFuncs[] = {
	JS_FS("setText", Text_setText, 1, 0),
	JS_FS("getText", Text_getText, 0, 0),
	JS_FS("setHeight", Text_setProperties, 1, 0),
	JS_FS("getWidth", Text_getWidth, 0, 0),
	JS_FS("getHeight", Text_getHeight, 0, 0),
	JS_FS("setBoundaryWidth", Text_setBoundaryWidth, 1, 0),
	JS_FS("setBoundaryHeight", Text_setBoundaryHeight, 1, 0),
	JS_FS("setProperties", Text_setProperties, 1, 0),
	JS_FS("setFontSize", Text_setFontSize, 1, 0),
	JS_FS("setColor", Text_setColor, 1, 0),
    JS_FS_END
};


///////////////////////////////////////////////////////////////////////////////

void OldTextProperties::read(Reader & reader, unsigned char version)
{
	reader.read(index);
	reader.read(font);
	reader.read(color);
	reader.read(link);
	if (version >= 1)
		reader.read(underline);
	else
		underline = false;

	if (version == 2)
	{
		reader.read(lineSpacingMode);
		reader.read(lineSpacing);
	}
	
}

//xmlNode * OldTextProperties::readXml(XmlReader & r, xmlNode * node)
//{
//	r.getNodeContentIfName(index, node, "Index");
//	font.readXml(r, node);
//	node = node->next;
//	r.getNodeContentIfName(color, node, "Color");
//	r.getNodeContentIfName(link, node, "Link");
//	r.getNodeContentIfName(underline, node, "Underline");
//	int temp = lineSpacingMode;
//	r.getNodeContentIfName(temp, node, "LineSpacingMode");
//	r.getNodeContentIfName(lineSpacing, node, "LineSpacing");
//
//	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
//	{		
//		if (curNode->type != XML_ELEMENT_NODE) continue;		
//		if (r.isNodeName(curNode, "VisualAttrib"))
//		{
//			visualAttrib_.readXml(r, curNode);
//			curNode = curNode->next;
//		}
//		else if (r.getNodeContentIfName(width_, curNode, "Width"));
//		else if (r.getNodeContentIfName(height_, curNode, "Height"));
//		else if (r.getNodeContentIfName(color_, curNode, "Color"));
//	}
//}

TextProperties::TextProperties(const OldTextProperties & rhs)
{
	index = rhs.index;
	font = rhs.font;
	color = rhs.color;
	link = rhs.link;
	underline = rhs.underline;
}

void TextProperties::assign(const TextProperties & rhs, Type types)
{
	if (types & TextProperties::PropertyColor) 
		color = rhs.color;

	if (types & TextProperties::PropertyFont) 
		font = rhs.font;

	if (types & TextProperties::PropertyFontSize) 
		font.pointSize = rhs.font.pointSize;
}

void TextProperties::write(Writer & writer) const
{
	writer.write(index, "index");
	writer.write(font, "font");
	writer.write(color, "color");
	writer.write(link, "link");
	writer.write(underline, "underline");
}

void TextProperties::writeXml(XmlWriter & w) const
{
	w.writeTag("Index", index);
	font.writeXml(w);
	w.writeTag("Color", color);
	w.writeTag("Link", link);
	w.writeTag("Underline", underline);
}

void TextProperties::readXml(XmlReader & r, xmlNode * parent)
{
	//r.getNodeContentIfName(index, node, "Index");
	////r.checkNodeName(node, "TextFont");
	//font.readXml(r, node->children);
	//node = node->next;
	//r.getNodeContentIfName(color, node, "Color");
	//r.getNodeContentIfName(link, node, "Link");
	//r.getNodeContentIfName(underline, node, "Underline");
	
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;
		if (r.getNodeContentIfName(index, curNode, "Index"));
		else if (r.isNodeName(curNode, "TextFont"))
		{
			font.readXml(r, curNode);
			curNode = curNode->next;
		}
		else if (r.getNodeContentIfName(color, curNode, "Color"));
		else if (r.getNodeContentIfName(link, curNode, "Link"));
		else if (r.getNodeContentIfName(underline, curNode, "Underline"));
	}
}

void TextProperties::read(Reader & reader, unsigned char version)
{
	reader.read(index);
	reader.read(font);
	reader.read(color);
	reader.read(link);
	if (version >= 1)
		reader.read(underline);
	else
		underline = false;

	if (version == 2)
	{
		LineSpacingMode lineSpacingMode;
		int lineSpacing;
		reader.read(lineSpacingMode);
		reader.read(lineSpacing);
	}
}
///////////////////////////////////////////////////////////////////////////////

Scene * Memo::getScene() const
{
	/*
	ContainerObjects objs;
	textObj->parentScene()->getContainerObjects(&objs);
	return objs.back().second;
	*/
	return textObj->parentScene();
}

///////////////////////////////////////////////////////////////////////////////
JSObject * Text::createScriptObjectProto(ScriptProcessor * s)
{			
	JSObject * proto = JS_InitClass(
		s->jsContext(), s->jsGlobal(), s->baseSceneObjectProto(), &jsTextClass,
		0, 0, 0, TextFuncs, 0, 0);

	return proto;
}

JSObject * Text::getScriptObject(ScriptProcessor * s)
{
	if (!scriptObject_)
	{
		scriptObject_ = JS_NewObject(
			s->jsContext(), &jsTextClass, s->sceneObjectProto(this), 0);
		JS_SetPrivate(s->jsContext(), scriptObject_, this);

		if (parent_) parent_->getScriptObject(s);
	}

	return scriptObject_;
}

void Text::create()
{
	
	TextProperties properties;
	properties.index = 0;
	properties.font.pointSize = 16;
	properties.font.faceIndex = 0;
	properties.font.bold = 0;
	properties.font.italic = 0;
	properties.underline = false;
	properties.color = Color(0, 0, 0, 1);
	properties_.reserve(1);
	properties_.push_back(properties);

	text_.reserve(4);
	text_.push_back('T');
	text_.push_back('e');
	text_.push_back('x');
	text_.push_back('t');

	boundaryWidth_ = 0;
	boundaryHeight_ = 0;

	textAlign_ = Left;
	letterSpacing_ = 0;

	lineSpacing_ = 0;
	lineSpacingMode_ = LineSpacingAuto;

	canSelectPart_ = false;
	isSelectedPart_ = false;

	charStartIndex_ = 0;
	charEndIndex_ = 0;

	highlights_.clear();
	imgProps_.clear();

	//test
	//ImageProperties imgprop;
	//imgprop.fileName = "C:/ST/f13.png";
	//imgprop.width = 100;
	//imgprop.height = 100;
	//imgprop.index = 2;
	//imgProps_.push_back(imgprop);
}

Text::Text()
{
	create();
	setId("Text");
}

Text::Text(const Text & rhs, ElementMapping * elementMapping)
: SceneObject(rhs, elementMapping)
{
	create();
	text_ = rhs.text_;
	visualAttrib_ = rhs.visualAttrib_;
	properties_ = rhs.properties_;
	boundaryWidth_ = rhs.boundaryWidth_;
	boundaryHeight_ = rhs.boundaryHeight_;
	lineSpacing_ = rhs.lineSpacing_;
	lineSpacingMode_ = rhs.lineSpacingMode_;
	canSelectPart_ = rhs.canSelectPart_;
	isSelectedPart_ = rhs.isSelectedPart_;
	letterSpacing_ = rhs.letterSpacing_;
	textAlign_ = rhs.textAlign_;
	imgProps_ = rhs.imgProps_;
}




Text::~Text()
{
	uninit();
}

void Text::referencedFiles(std::vector<std::string> * refFiles) const
{
	SceneObject::referencedFiles(refFiles);

	BOOST_FOREACH(const TextProperties & props, properties_)
	{
		refFiles->push_back(props.font.fontFile);
	}	
}

int Text::setReferencedFiles(const std::string & baseDirectory,
	const vector<string> & refFiles, int index)
{
	index = SceneObject::setReferencedFiles(baseDirectory, refFiles, index);
	
	BOOST_FOREACH(TextProperties & props, properties_)
	{
		props.font.fontFile = refFiles[index++];
	}	

	return index;
}


void Text::setTextString(const std::wstring & str)
{
	text_ = str;	
}
/*
void Text::processLink(htmlNodePtr node, xmlAttrPtr attr,
	std::string * str, 
	std::vector<TextProperties>::iterator & iter)
{
	int curStrIndex = (int)str->size();
	const xmlChar * url = attr->children->content;
	const xmlChar * link = node->children->content;

	//increment iter to the first prop after start of link
	//region
	for(;iter != postMarkupProperties_.end();++iter)
	{
		TextProperties & props = *iter;		
		if (props.index > curStrIndex) break;
	}					
	
	int linkStartIndex = curStrIndex;

	std::vector<TextProperties>::iterator insertIter = iter;
	--iter;

	TextProperties linkProperties = *iter;
	TextProperties endlinkProperties = linkProperties;

	linkProperties.index = linkStartIndex;	
	linkProperties.underline = true;
	//linkProperties.color = Color(1, 0, 0, 1);
	//linkProperties.font.bold = true;
	//linkProperties.font.pointSize += 4;
	linkProperties.link = (const char *)url;


	//erase current properties element if it is exactly at the start of the 
	//link region. (It's gonna be replaced with the link Properties)
	if ((*iter).index == curStrIndex)
	{
		insertIter = iter = postMarkupProperties_.erase(iter);
	}	
		
	(*str) += (const char *)link;
	curStrIndex = (int)str->size();	

	//increment iter to the first prop after end of link 
	//region
	for(;iter != postMarkupProperties_.end();++iter)
	{
		TextProperties & props = *iter;		
		if (props.index > curStrIndex) break;
	}	

	if (iter != postMarkupProperties_.end()) endlinkProperties = *iter;
	endlinkProperties.index = curStrIndex;

	iter = postMarkupProperties_.insert(insertIter, linkProperties);
	iter = postMarkupProperties_.insert(iter + 1, endlinkProperties);	

	//remove any properties that would have fallen within the 
	//link region
	std::vector<TextProperties>::iterator trailIter = iter + 1;
	while(trailIter != postMarkupProperties_.end())
	{
		TextProperties & props = *trailIter;
		if (props.index <= endlinkProperties.index)
			trailIter = postMarkupProperties_.erase(trailIter);
		else break;
	}
}

void Text::processMarkup(
	std::string * str, 
	std::vector<TextProperties>::iterator & iter, 
	htmlNodePtr element)
{
	for (htmlNodePtr node = element; node != 0; node = node->next)
	{		
		if (node->type == XML_ELEMENT_NODE)
		{
			bool isLink = false;			
			if (!xmlStrcasecmp(node->name, (const xmlChar *)"a"))
			{				
				for (xmlAttrPtr attr = node->properties; attr != 0; attr = attr->next)
				{
					if (!xmlStrcasecmp(attr->name, (const xmlChar *)"HREF"))
					{		
						isLink = true;
						processLink(node, attr, str, iter);
					}
				}

			}

			if (!isLink && node->children)
				processMarkup(str, iter, node->children);
		}
		else if (node->type == XML_TEXT_NODE)
			(*str) += (const char *)node->content;
	}
}
*/
void Text::computeWordWrap(
	std::vector<WordWrapPoint> * wordWrapPoints,
	const std::wstring & str, 						   
	const std::vector<TextProperties> & properties,
	int width, int height)
{
	bool validBoundary = width > 0;

	
	wordWrapPoints->clear();
	int strLen = (int)str.size();
	//first character of current word
	int lastWordBeginning = 0;
	int curWordTotalAdvanceX = 0;

	bool wasWordEnd = false;
	float curX = 0;

	int nextPropsCharIndex = 0;
	int curPropsIndex = -1;
	TextHandlerOld * curTextHandlerOld = 0;


	for (int i = 0; i < strLen; ++i)
	{
		if (i == nextPropsCharIndex)
		{
			++curPropsIndex;
			if (curPropsIndex < (int)properties.size() - 1)
				nextPropsCharIndex = properties[curPropsIndex + 1].index;
			else
				nextPropsCharIndex = -1;
			curTextHandlerOld = properties[curPropsIndex].textHandler;
		}

		const wchar_t & c = str[i];
		bool isNewLine = c == L'\n';
		bool isWhitespace = isNewLine || c == L' ';
		bool isSkip = c == L'\t' || c == L'\r' || c == L'\b';
		
		if (isNewLine)
		{			
			curX = 0;
			continue;
		}
		else if (isSkip) 
		{
			continue;
		}
		const CharData_old * charData = curTextHandlerOld->charData_old(c);			
		short curAdvanceX = 0;
		if (charData) curAdvanceX = charData->advanceX + letterSpacing_;
		curX += curAdvanceX;
		curWordTotalAdvanceX += curAdvanceX;
		
		if (isWhitespace)
		{
			wasWordEnd = true;
			curWordTotalAdvanceX = 0;
		}
		else //non whitespace character
		{			
			if (wasWordEnd)
			{
				lastWordBeginning = i;
				wasWordEnd = false;
			}

			
			if (validBoundary && curX > width)
			{
				WordWrapPoint wordWrapPoint;
							
				if (curWordTotalAdvanceX > width)
				{
					wordWrapPoint.wrapAtIndex = i;
					curX = curAdvanceX;
				}
				else 
				{					
					wordWrapPoint.wrapAtIndex = lastWordBeginning;
					curX = (float)curWordTotalAdvanceX;
				}				
				wordWrapPoints->push_back(wordWrapPoint);
				
			}

		}
	}
}

int Text::toNextLine(TextHandlerOld * handler, const TextProperties & props)
{
	if (lineSpacingMode_ == TextProperties::LineSpacingAuto)
	{
		return (int)handler->toNextLine();//pixelSize();
	}
	else
	{
		return (int)handler->pixelSize() + lineSpacing_;
	}
}

//void Text::setFont(const TextFont & font)
//{
//	properties_.front().font = font;
//}

void Text::computeLines(vector<LineProperties> * lines,
	const std::wstring & str, const std::vector<TextProperties> & properties,
	const std::vector<WordWrapPoint> & wordWrapPoints)
{
	int numWordWrapPoints = (int)wordWrapPoints.size();
	int curWordWrapPoint = 0;

	int strLen = (int)str.size();

	LineProperties curLineProperties;

	TextHandlerOld * curTextHandlerOld = properties[0].textHandler;
	curLineProperties.index = 0;
	curLineProperties.descender = (int)curTextHandlerOld->descender();
	curLineProperties.ascenderHeight = curTextHandlerOld->pixelSize();
	curLineProperties.toNextLine = Text::toNextLine(
			curTextHandlerOld, properties[0]);	

	int nextPropsCharIndex = 0;
	int curPropsIndex = -1;

	for (int i = 0; i <= strLen; ++i)
	{	
		const wchar_t & c = str[i];		

		while(curPropsIndex < (int)properties.size() - 1 &&
			properties[curPropsIndex + 1].index <= i)
		{
			++curPropsIndex;
		}
		
		bool doWrap = false;
		if (curWordWrapPoint < numWordWrapPoints)
		{
			const WordWrapPoint & w = wordWrapPoints[curWordWrapPoint];
			if (i == w.wrapAtIndex)
			{
				doWrap = true;
				++curWordWrapPoint;
			}
		}

		//ignored chars
		if (c == L'\t' || c == L'\r' || c == L'\b') continue;


		curTextHandlerOld = properties[curPropsIndex].textHandler;
		int curToNextLine = Text::toNextLine(
				curTextHandlerOld, properties[curPropsIndex]);		
			
		if (curToNextLine > curLineProperties.toNextLine)
		{
			curLineProperties.descender = (int)curTextHandlerOld->descender();
			curLineProperties.ascenderHeight = curTextHandlerOld->pixelSize();
			curLineProperties.toNextLine = curToNextLine;			
		}

		if (c == L'\n')
		{
			lines->push_back(curLineProperties);
			curLineProperties.toNextLine = 0;
			curLineProperties.index = i + 1;
			continue;	
		}	
		else if (doWrap)
		{
			lines->push_back(curLineProperties);
			curLineProperties.toNextLine = 0;
			curLineProperties.index = i;
			continue;	
		}
		
	}

	lines->push_back(curLineProperties);
}

void Text::init(GfxRenderer * gl, bool firstTime)
{
	init(gl, parentScene_, firstTime);
}

void Text::init(GfxRenderer * gl, Scene * parentScene, bool firstTime)
{
	SceneObject::init(gl, firstTime);

	parentScene_ = parentScene;

/*
	std::string utf8Text;

	utf8::utf16to8(text_.begin(), text_.end(), back_inserter(utf8Text));

	htmlDocPtr doc = htmlParseDoc((unsigned char *)utf8Text.c_str(), "UTF-8");

	utf8Text.clear();
	if (doc)
	{
		htmlNodePtr root = xmlDocGetRootElement(doc);
		if (root)
		{
			postMarkupProperties_ = properties_;
			std::vector<TextProperties>::iterator pitr = 
				postMarkupProperties_.begin();
			processMarkup(&utf8Text, pitr, root);
		}
		xmlFreeDoc(doc);
	}

	noMarkupText_.clear();	
	utf8::utf8to16(utf8Text.begin(), utf8Text.end(), back_inserter(noMarkupText_));
	
	//TODO external users of TextHandlerOld shouldn't have to know about 
	//special internal properties such as the fact that genChars must be
	//performed before genString in a specific manner

	BOOST_FOREACH(TextProperties & props, postMarkupProperties_)
	{			
		props.textHandler = gl->textHandler(props.font);

		props.textHandler->genAnyNewChars(
			postMarkupProperties_, noMarkupText_);
	}

	//word wrap
	vector<WordWrapPoint> wordWrapPoints;
	computeWordWrap(&wordWrapPoints, noMarkupText_,
		postMarkupProperties_, boundaryWidth_, boundaryHeight_);
	
	//compute line properties and separations
	lines_.clear();
	computeLines(&lines_, noMarkupText_, postMarkupProperties_, wordWrapPoints);

	//generate gl render objects

	
	BOOST_FOREACH(TextProperties & props, postMarkupProperties_)
	{
		TextData data;
		data.textHandler = props.textHandler;
		data.color = props.color;
		data.underline = props.underline;

		if (textMap_.find(data) == textMap_.end())
		{
			textMap_[data] = props.textHandler->genString(gl,
				noMarkupText_, postMarkupProperties_, props, lines_,
				boundaryWidth_, boundaryHeight_);
		}			
	}
	*/

	BOOST_FOREACH(TextProperties & props, properties_)
	{
		props.textHandler = gl->textHandlerOld(props.font);
	}

	std::vector<TextProperties> properties;
	properties = properties_;

	BOOST_FOREACH(TextProperties & props, properties)
	{
		//props.font.pointSize *= magnification_;		
		props.textHandler = gl->textHandlerOld(props.font);
		//props.textHandler->setMagnification(magnification_);
		//props.lineSpacing *= magnification_;
		props.textHandler->genAnyNewChars(
			properties, text_);
	}
	
	//word wrap
	vector<WordWrapPoint> wordWrapPoints;
	computeWordWrap(&wordWrapPoints, text_,
		properties, boundaryWidth_, boundaryHeight_);
	
	//compute line properties and separations
	lines_.clear();
	computeLines(&lines_, text_, properties, wordWrapPoints);

	//generate gl render objects

	
	BOOST_FOREACH(TextProperties & props, properties)
	{
		TextData data;
		data.textHandler = props.textHandler;
		data.color = props.color;
		data.underline = props.underline;

		if (textMap_.find(data) == textMap_.end())
		{
			textMap_[data] = props.textHandler->genString(gl,
				text_, properties, props, lines_, imgProps_,
				boundaryWidth_, boundaryHeight_, letterSpacing_, textAlign_);
		}			
	}
}


void Text::uninit()
{	
/*
	BOOST_FOREACH(TextProperties & props, postMarkupProperties_)
	{
		props.textHandler = 0;
	}
*/
	BOOST_FOREACH(TextProperties & props, properties_)
	{
		props.textHandler = 0;
	}

	map<TextData, StringData_old *>::iterator iter = textMap_.begin();
	for (; iter != textMap_.end(); ++iter)
	{
		delete (*iter).second;
	}
	textMap_.clear();
	lines_.clear();

	SceneObject::uninit();
}


void Text::drawObject(GfxRenderer * gl) const
{
	
	Document * doc = parentScene_->parentDocument();
	doc->textFeatures()->registerDrawTextObj(const_cast<Text *>(this));
		
	float fOpacity = totalOpacity();
	if (fOpacity == 0) return;

	if (!highlights_.empty())
	{
		for (int i = 0 ; i < highlights_.size() ; i++)
		{
			drawHighlight(gl, highlights_[i]);
		}
	}

	if (isSelectedPart_)
	{
		Highlight temp;
		temp.startIndex = charStartIndex_;
		temp.endIndex = charEndIndex_;
		temp.color = Color(0.5f, 0.5f, 1.0f, 1.0f);

		drawHighlight(gl, temp);
	}

	
	gl->useTextureProgram();
	gl->setTextureProgramOpacity(fOpacity);	

	bool hasUnderline = false;

	map<TextData, StringData_old *>::const_iterator iter;
	for (iter = textMap_.begin(); iter != textMap_.end(); ++iter)
	{
		const TextData & data = (*iter).first;
		StringData_old * stringData = (*iter).second;

		if (stringData)
		{
			data.textHandler->drawString(
				gl, stringData, data.color, fOpacity, imgProps_);
			
		}
		if (data.underline) hasUnderline = true;
	}	

	if (hasUnderline)
	{
		//draw underlines
		gl->useColorProgram();
		
		for (iter = textMap_.begin(); iter != textMap_.end(); ++iter)
		{
			const TextData & data = (*iter).first;
			StringData_old * stringData = (*iter).second;
			if (!stringData) continue;
			BOOST_FOREACH(TextSpan_old & span, stringData->spans)
			{
				if (data.textHandler->isinit() && data.underline)
				{
					gl->setColorProgramColor(
						data.color.r, data.color.g, data.color.b, data.color.a);
					float underPos = data.textHandler->underlinePosition();
					float underThickness = data.textHandler->underlineThickness();
					gl->drawRect(
						span.x, span.y - underPos - underThickness/2, 
						span.width, underThickness);
					
				}

				
			}		
		}
	}
	if (!memos_.empty() && (memoTex_ && memoTex_->isInit()))
	{
		drawMemoIcon(gl);
	}
}


BoundingBox Text::extents() const
{	
	BoundingBox ret;

	map<TextData, StringData_old *>::const_iterator iter = textMap_.begin();
	for (;iter != textMap_.end(); ++iter)
	{
		StringData_old * stringData = (*iter).second;
		if (!stringData) continue;
		Vector2 size = stringData->size;
		BoundingBox curBb(Vector3(0, 0, 0),
			Vector3(size.x, size.y, 0));

		ret = ret.unite(curBb);	
		
	}

	if (boundaryWidth_ > 0)
	{
		ret.maxPt.x = ret.minPt.x + boundaryWidth_;
	}
	if (boundaryHeight_ > 0)
	{
		ret.maxPt.y = ret.minPt.y + boundaryHeight_;
	}

	return ret;
}


Vector2 Text::processCoords(const Vector2 & globalDevicePos) const
{
	const Camera * camera = parentScene_->camera();
	Ray ray = camera->unproject(globalDevicePos);
	Vector3 out;
	const_cast<Text *>(this)->intersect(&out, ray);
	
	Matrix invTrans = 
		(parentTransform() * visualAttrib_.transformMatrix()).inverse();

	Vector3 invClickPt = invTrans * out;
	return Vector2(invClickPt.x, invClickPt.y);
}

bool Text::lineDataForChar(
	int index, int * lineY, int * olineIndex, 
	int * olineStartIndex, int * olineEndIndex) const
{
	if (index > (int)text_.size() || index < 0) return false;
	int numLines = (int)lines_.size();
	if (numLines == 0) return false;
	
	int lineIndex = -1;
	int curLineY = 0;
	int i = 0;
	for (; i < numLines; ++i)
	{		
		if (lines_[i].index > index) break;		
		lineIndex = i;	
		curLineY += lines_[i].toNextLine;
	}

	curLineY -= lines_[i - 1].toNextLine;

	int lineStartIndex = lines_[lineIndex].index;

	//one past the end index
	int lineEndIndex = (int)text_.size();

	if (lineIndex < numLines - 1)
	{
		lineEndIndex = lines_[lineIndex + 1].index;
	}
	
	*olineIndex = lineIndex;
	*lineY = curLineY;
	*olineStartIndex = lineStartIndex;
	*olineEndIndex = lineEndIndex;

	return true;
}

bool Text::lineDataForLine(int lineIndex, 
	int * olineY, int * olineWidth, int * olineHeight, 
	int * olineStartIndex, int * olineEndIndex) const
{	
	int numLines = (int)lines_.size();
	if (lineIndex >= numLines || lineIndex < 0) return false;	
	int curLineY = 0;

	for (int i = 0; i < lineIndex; ++i) curLineY += lines_[i].toNextLine;

	int lineStartIndex = lines_[lineIndex].index;

	//one past the end index
	int lineEndIndex = (int)text_.size();

	if (lineIndex < numLines - 1)
	{
		lineEndIndex = lines_[lineIndex + 1].index;
	}

	int curPropIndex = startingPropertyIndexForLine(lineIndex);
	int curWidth = 0;
	int nextPropCharIndex = lineStartIndex;
	--curPropIndex;

	for (int i = lineStartIndex; i < lineEndIndex; ++i)
	{
		if (i == nextPropCharIndex)
		{
			++curPropIndex;
			if (curPropIndex < (int)properties_.size() - 1)
				nextPropCharIndex = properties_[curPropIndex + 1].index;
			else
				nextPropCharIndex = -1;
		}

		const wchar_t & c = text_[i];
		const CharData_old * d =
		properties_[curPropIndex].textHandler->charData_old(c);

		if (!d) continue;
		curWidth += d->advanceX + letterSpacing_;		
	}

	curPropIndex = startingPropertyIndexForLine(lineIndex);
	nextPropCharIndex = lineStartIndex;
	--curPropIndex;
	float linelen = 0;
	for (int i = lineStartIndex; i < lineEndIndex; ++i)
	{
		if (i == nextPropCharIndex)
		{
			++curPropIndex;
			if (curPropIndex < (int)properties_.size() - 1)
				nextPropCharIndex = properties_[curPropIndex + 1].index;
			else
				nextPropCharIndex = -1;
		}
		const wchar_t & c = text_[i];
		const CharData_old * d =
		properties_[curPropIndex].textHandler->charData_old(c);

		if (!d) continue;
		linelen += d->advanceX + letterSpacing_;
	}
	if (textAlign_ == Center && boundaryWidth_ > 0)
		*olineWidth = (boundaryWidth_ - linelen)/2 + curWidth;
	else if (textAlign_ == Right && boundaryWidth_ > 0)
		*olineWidth = boundaryWidth_ - linelen + curWidth;
	else	//textAlign_ == Left - default
		*olineWidth = curWidth;	

	*olineY = curLineY;
	*olineHeight = lines_[lineIndex].ascenderHeight - lines_[lineIndex].descender;
	*olineStartIndex = lineStartIndex;
	*olineEndIndex = lineEndIndex;

	return true;
}

bool Text::posAtIndex(int index, Vector2 * posTop, int * posHeight, int * posWidth) const
{
	int curLineY, lineIndex, lineStartIndex, lineEndIndex;
	if (!lineDataForChar(index, &curLineY, &lineIndex, &lineStartIndex, &lineEndIndex)) 
	{
		*posWidth = 0;
		*posHeight = lines_[0].ascenderHeight - lines_[0].descender;
		*posTop = Vector2(0.0f, 0.0f);
		return true;
	}

	int curPropIndex = startingPropertyIndexForLine(lineIndex);
	
	int curWidth = 0;
	int nextPropCharIndex = lineStartIndex;
	--curPropIndex;

	*posWidth = 0;

	for (int i = lineStartIndex; i < lineEndIndex; ++i)
	{
		if (i == nextPropCharIndex)
		{
			++curPropIndex;
			if (curPropIndex < (int)properties_.size() - 1)
				nextPropCharIndex = properties_[curPropIndex + 1].index;
			else
				nextPropCharIndex = -1;
		}

		if (i == index) break;

		const wchar_t & c = text_[i];
		const CharData_old * d =
		properties_[curPropIndex].textHandler->charData_old(c);

		bool isIgnored = c == L'\t' || c == L'\r' || c == L'\b' || c == L'\n';

		if (!d) continue;
		if (i == index)
			*posWidth = d->advanceX + letterSpacing_;
		else if (!isIgnored)
			curWidth += d->advanceX + letterSpacing_;		
	}

	curPropIndex = startingPropertyIndexForLine(lineIndex);
	nextPropCharIndex = lineStartIndex;
	--curPropIndex;
	float linelen = 0;
	for (int i = lineStartIndex; i < lineEndIndex; ++i)
	{
		if (i == nextPropCharIndex)
		{
			++curPropIndex;
			if (curPropIndex < (int)properties_.size() - 1)
				nextPropCharIndex = properties_[curPropIndex + 1].index;
			else
				nextPropCharIndex = -1;
		}
		const wchar_t & c = text_[i];
		const CharData_old * d =
		properties_[curPropIndex].textHandler->charData_old(c);

		bool isIgnored = c == L'\t' || c == L'\r' || c == L'\b' || c == L'\n';

		if (!d) continue;
		if (!isIgnored)
			linelen += d->advanceX + letterSpacing_;
	}
	if (textAlign_ == Center && boundaryWidth_ > 0)
		posTop->x = (boundaryWidth_ - linelen)/2 + (float)curWidth;
	else if (textAlign_ == Right && boundaryWidth_ > 0)
		posTop->x = boundaryWidth_ - linelen + (float)curWidth;
	else	//textAlign_ == Left - default
		posTop->x = (float)curWidth;
	posTop->y = (float)curLineY;

	*posHeight = lines_[lineIndex].ascenderHeight - lines_[lineIndex].descender;

	return true;
}

bool Text::lineDataForPos(
	const Vector2 & localPos, int * olineY, int * olineIndex, 
	int * olineStartIndex, int * olineEndIndex) const
{
	int numLines = (int)lines_.size();

	int curLineY = 0;
	int intLineIndex = 0;
	
	if (localPos.y >= 0)
	{
		for (int i = 0; i < numLines; ++i)
		{
			int lineBottom = curLineY + lines_[i].toNextLine;
			bool isLastLine = i == numLines - 1;
			bool isPosLine = curLineY <= localPos.y && localPos.y < lineBottom;
			if (isLastLine || isPosLine)
			{
				intLineIndex = i;
				break;
			}			
			curLineY += lines_[i].toNextLine;
		}	
	}
	
	int lineStartIndex = lines_[intLineIndex].index;

	//one past the end index
	int lineEndIndex = (int)text_.size();

	if (intLineIndex < numLines - 1)
	{
		lineEndIndex = lines_[intLineIndex + 1].index;
	}

	*olineY = curLineY;
	*olineIndex = intLineIndex;
	*olineStartIndex = lineStartIndex;
	*olineEndIndex = lineEndIndex;


	return true;
}

int Text::startingPropertyIndexForLine(int lineIndex) const
{
	int curPropIndex = 0;
	if (lineIndex < 0 || lineIndex >= (int)lines_.size()) return -1;

	int lineStartIndex = lines_[lineIndex].index;
	while(1)
	{
		if (curPropIndex >= (int)properties_.size() - 1) break;
		if (properties_[curPropIndex + 1].index > lineStartIndex)
			break;
		++curPropIndex;
	}

	return curPropIndex;
}

int Text::indexAtPos(const Vector2 & localPos, int * propertyIndex) const
{

	std::string testStr(text_.begin(), text_.end());

//	Vector2 localPos = processCoords(mousePos);
	int curLineY, lineIndex, lineStartIndex, lineEndIndex;
	if (!lineDataForPos(localPos, &curLineY, &lineIndex, &lineStartIndex, &lineEndIndex)) 
	{
		return false;
	}

	int curPropIndex = startingPropertyIndexForLine(lineIndex);
	*propertyIndex = curPropIndex;

	int curWidth = 0;
	int nextPropCharIndex = lineStartIndex;
	--curPropIndex;
	float linelen = 0;
	for (int i = lineStartIndex; i < lineEndIndex; ++i)
	{
		if (i == nextPropCharIndex)
		{
			++curPropIndex;
			if (curPropIndex < (int)properties_.size() - 1)
				nextPropCharIndex = properties_[curPropIndex + 1].index;
			else
				nextPropCharIndex = -1;
		}
		const wchar_t & c = text_[i];
		const CharData_old * d =
		properties_[curPropIndex].textHandler->charData_old(c);

		bool isIgnored = c == L'\t' || c == L'\r' || c == L'\b' || c == L'\n';

		if (!d) continue;
		if (!isIgnored)
			linelen += d->advanceX + letterSpacing_;
	}


	curPropIndex = startingPropertyIndexForLine(lineIndex);
	nextPropCharIndex = lineStartIndex;
	--curPropIndex;

	int i = lineStartIndex;
	for (; i < lineEndIndex; ++i)
	{
		if (i == nextPropCharIndex)
		{
			++curPropIndex;
			if (curPropIndex < (int)properties_.size() - 1)
				nextPropCharIndex = properties_[curPropIndex + 1].index;
			else
				nextPropCharIndex = -1;
		}

		const wchar_t & c = text_[i];
		bool isNewLine = c == L'\n';
		if (isNewLine) break;
		const CharData_old * d =
		properties_[curPropIndex].textHandler->charData_old(c);

		bool isIgnored = c == L'\t' || c == L'\r' || c == L'\b' || c == L'\n';

		if (!d) continue;
		if (!isIgnored)
			curWidth += d->advanceX + letterSpacing_;
		*propertyIndex = curPropIndex;
		if (textAlign_ == Center && boundaryWidth_ > 0)
		{
			if( (boundaryWidth_ - linelen)/2 + curWidth > localPos.x) break;
		}
		else if (textAlign_ == Right && boundaryWidth_ > 0)
		{
			if( boundaryWidth_ - linelen + curWidth > localPos.x) break;
		}
		else	// textAlign_ == Left - default
		{
			if (curWidth > localPos.x) break;
		}
	}

	

	return i;
}

Vector2 Text::toLocalPos(const Vector2 & pos)
{
	const Camera * cam = parentScene_->camera();
	Ray mouseRay = cam->unproject(parentScene_->screenToDeviceCoords(pos));
	Vector3 out;
	intersect(&out, mouseRay);
	Matrix invMatrix = (parentTransform() * visualAttrib_.transformMatrix()).inverse();
	out = invMatrix * out;

	return Vector2(out.x, out.y);
}

bool Text::pressEvent(const Vector2 & startPos, int pressId)
{
	bool handled = SceneObject::pressEvent(startPos, pressId);

	if (canSelectPart_)
	{
		int startIndex, charIndex, endIndex;

		int propertyIndex;
		charIndex = indexAtPos(toLocalPos(startPos), &propertyIndex);
		selectPart(startIndex, endIndex, charIndex);
		clearTextSelect();
		parentScene_->parentDocument()->textFeatures()->startTextSel(this, startIndex, endIndex);
	}


	if (handled) return true;
	return false;
}

bool Text::releaseEvent(const Vector2 & pos, int pressId)
{
	return true;
}

bool Text::clickEvent(const Vector2 & mousePos, int pressId)
{
	bool handled = SceneObject::clickEvent(mousePos, pressId);
	if (handled)  return true;
	
	int propertyIndex;
	int charIndex;

	charIndex = indexAtPos(mousePos, &propertyIndex);

	if (charIndex >= 0)
	{
		if (!properties_[propertyIndex].link.empty())
		{
			Global::instance().openUrl(
				properties_[propertyIndex].link, false);
		}
	}

	return false;
}


void Text::write(Writer & writer) const
{
	writer.writeParent<SceneObject>(this);
	writer.write(visualAttrib_, "visualAttrib");
	
	writer.write(text_, "text");
	
	writer.write(properties_);
	writer.write(lineSpacingMode_);
	writer.write(lineSpacing_);

	writer.write(boundaryWidth_, "width");
	writer.write(boundaryHeight_, "height");

	writer.write(textAlign_, "textAlign");
	writer.write(letterSpacing_, "letterSpacing");

	writer.write(canSelectPart_, "canSelectPart");
}

void Text::writeXml(XmlWriter & w) const
{
	SceneObject::writeXml(w);
	w.startTag("VisualAttrib");	
	visualAttrib_.writeXml(w);
	w.endTag();

	w.writeTag("TextString", text_);
	w.startTag("Properties");
	for (unsigned int i = 0; i < (unsigned int)properties_.size(); ++i)
	{
		w.startTag("TextProperties");
		properties_[i].writeXml(w);
		w.endTag();
	}
	w.endTag();
	int temp = lineSpacingMode_;
	w.writeTag("LineSpacingMode", temp);
	w.writeTag("LineSpacing", lineSpacing_);

	w.writeTag("Width", boundaryWidth_);
	w.writeTag("Height", boundaryHeight_);
	temp = textAlign_;
	w.writeTag("TextAlign", temp);
	w.writeTag("LetterSpacing", letterSpacing_);
	w.writeTag("SelectPart", canSelectPart_);
}

void Text::readXml(XmlReader & r, xmlNode * parent)
{
	SceneObject::readXml(r, parent);
	std::string str;
	properties_.clear();
	int temp = 0;
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;		
		if (r.isNodeName(curNode, "VisualAttrib"))
		{
			visualAttrib_.readXml(r, curNode);
			curNode = curNode->next;
		}
		else if (r.getNodeContentIfName(str, curNode, "TextString"))
		{
			text_.resize(str.size());
			for ( unsigned int  i=0; i<str.size(); i++)
				text_[i] = static_cast<unsigned char>(str[i]);
		}
		else if (r.isNodeName(curNode, "Properties"))
		{
			xmlNode * propertiesNode = curNode->children;
			//while (r.isNodeName(propertiesNode, "TextProperties"))
			for(; propertiesNode ; propertiesNode = propertiesNode->next)
			{
				if (propertiesNode->type != XML_ELEMENT_NODE) continue;
				TextProperties p;
				p.readXml(r, propertiesNode);
				properties_.push_back(p);
			}
			curNode = curNode->next;
		}
		else if (r.getNodeContentIfName(temp, curNode, "LineSpacingMode")) lineSpacingMode_ = (LineSpacingMode)temp;
		else if (r.getNodeContentIfName(lineSpacing_, curNode, "LineSpacing"));
		else if (r.getNodeContentIfName(boundaryWidth_, curNode, "Width"));
		else if (r.getNodeContentIfName(boundaryHeight_, curNode, "Height"));
		else if (r.getNodeContentIfName(temp, curNode, "TextAlign")) textAlign_ = (TextAlign)temp;
		else if (r.getNodeContentIfName(letterSpacing_, curNode, "LetterSpacing"));
		else if (r.getNodeContentIfName(canSelectPart_, curNode, "SelectPart"));
	}
	////r.checkNodeName(curNode, "Properties");
	//xmlNode * propertiesNode = curNode->children;
	//properties_.clear();
	//while (r.isNodeName(propertiesNode, "TextProperties"))
	//{
	//	TextProperties p;
	//	p.readXml(r, propertiesNode->children);
	//	properties_.push_back(p);
	//	propertiesNode = propertiesNode->next;
	//}
	//curNode = curNode->next;
}


void Text::read(Reader & reader, unsigned char version)
{
	reader.readParent<SceneObject>(this);
	reader.read(visualAttrib_);
	reader.read(text_);
		
	properties_.clear();

	if (version <= 4)
	{
		TextProperties oldProps;
		oldProps.index = 0;
		oldProps.underline = false;

		reader.read(oldProps.font.fontFile);
		
		if (version >= 3)
		{
			reader.read(oldProps.font.faceIndex);
		}
		reader.read(oldProps.font.pointSize);

		if (version >= 4)
		{
			reader.read(oldProps.font.bold);
			reader.read(oldProps.font.italic);
		}

		if (version >= 1)
			reader.read(oldProps.color);

		properties_.push_back(oldProps);
	}
	else if (version <= 7)
	{
		vector<OldTextProperties> oldProperties;
		reader.read(oldProperties);

		lineSpacingMode_ = (LineSpacingMode)oldProperties.front().lineSpacingMode;
		lineSpacing_ = oldProperties.front().lineSpacing;		
		properties_.clear();
		BOOST_FOREACH(OldTextProperties & curOldProp, oldProperties)
			properties_.push_back(TextProperties(curOldProp));
	}
	else
	{
		reader.read(properties_);
		reader.read(lineSpacingMode_);
		reader.read(lineSpacing_);
	}


	if (version >= 2)
	{
		reader.read(boundaryWidth_);
		reader.read(boundaryHeight_);
	}

	if (version >= 6)
	{
		reader.read(textAlign_);
		reader.read(letterSpacing_);
	}

	if (version >= 7)
	{
		reader.read(canSelectPart_);
	}
}


float Text::width() const
{
	return extents().lengthX();	
}

float Text::height() const
{
	return extents().lengthY();	
}

SceneObject * Text::intersect(Vector3 * out, const Ray & ray)
{
	if (!visualAttrib_.isVisible()) return 0;

	Matrix trans = parentTransform() * visualAttrib_.transformMatrix();
		
	BoundingBox bb = extents(); 
	Vector2 size(bb.lengthX(), bb.lengthY());

	Vector3 a = trans * Vector3(0, 0, 0);
	Vector3 b = trans * Vector3(0, size.y, 0);
	Vector3 c = trans * Vector3(size.x, size.y, 0);
	Vector3 d = trans * Vector3(size.x, 0, 0);

	if (triangleIntersectRay(out, a, b, c, ray) ||
		triangleIntersectRay(out, a, c, d, ray))
	{
		return this;
	}
	else return 0;
}

void Text::setLetterSpacing(const int & letterSpacing)
{
	int prevLetterSpacing = letterSpacing_;
	letterSpacing_ = letterSpacing;

	if (prevLetterSpacing != letterSpacing_)
	{
		init(parentScene_->parentDocument()->renderer(), parentScene_, false);
	}
}

const int & Text::letterSpacing() const
{
	return letterSpacing_;
}

bool Text::lineLength(int lineIndex, float * lineLength) const
{
	lineIndex -= 1;
		int numLines = (int)lines_.size();
	if (lineIndex >= numLines || lineIndex < 0) return false;	

	int lineStartIndex = lines_[lineIndex].index;

	//one past the end index
	int lineEndIndex = (int)text_.size();
	
	if (lineIndex < numLines - 1)
	{
		lineEndIndex = lines_[lineIndex + 1].index;
	}

	int curPropIndex = startingPropertyIndexForLine(lineIndex);
	int nextPropCharIndex = lineStartIndex;
	--curPropIndex;
	float linelen = 0;
	for (int i = lineStartIndex; i < lineEndIndex; ++i)
	{
		if (i == nextPropCharIndex)
		{
			++curPropIndex;
			if (curPropIndex < properties_.size() - 1)
				nextPropCharIndex = properties_[curPropIndex + 1].index;
			else
				nextPropCharIndex = -1;
		}
		const wchar_t & c = text_[i];
		const CharData_old * d =
		properties_[curPropIndex].textHandler->charData_old(c);

		bool isIgnored = c == L'\t' || c == L'\r' || c == L'\b' || c == L'\n';

		if (!d) continue;
		if (!isIgnored)
			linelen += d->advanceX + letterSpacing_;
	}

	*lineLength = linelen;

	return true;
}

void Text::selectPart(int charIndex, int charEndIndex)
{
	charStartIndex_ = charIndex;
	charEndIndex_ = charEndIndex;
	
	if (charStartIndex_ != charEndIndex_)
	{
		isSelectedPart_ = true;
	}
	else
		isSelectedPart_ = false;
}

void Text::selectPart(int & startIndex, int & endIndex, int charIndex)
{
	endIndex = charIndex;
	startIndex = charIndex;
	while(startIndex >= 0)
	{
		int prevCharIndex = startIndex - 1;
		if (prevCharIndex < 0) break;
		wchar_t c = text_[prevCharIndex];
		bool ws = c == L' ' || c == L'\t' || c == L'\r' || c == L'\b' || c == L'\n';
		if (ws) break;
		else startIndex = prevCharIndex;
	}

	while(endIndex < text_.size())
	{
		wchar_t c = text_[endIndex];
		bool ws = c == L' ' || c == L'\t' || c == L'\r' || c == L'\b' || c == L'\n';
		if (ws) break;
		else ++endIndex;
	}
	
	
	selectPart(startIndex, endIndex);
}

void Text::addHighlight(const Color & color)
{
	if (!isSelectedPart_) return;
	Highlight temp;
	temp.startIndex = charStartIndex_;
	temp.endIndex = charEndIndex_;
	temp.color = color;

	removeHighlight(charStartIndex_, charEndIndex_);
	highlights_.push_back(temp);
}

void Text::addHighlight(const Highlight & highlight)
{
	removeHighlight(highlight.startIndex, highlight.endIndex);
	highlights_.push_back(highlight);
}

void Text::removeHighlight(unsigned int startIndex, unsigned int endIndex)
{	
	vector<Highlight>::iterator itr;
	for (itr = highlights_.begin(); itr != highlights_.end();)
	{
		Highlight & cur = *itr;


		if (startIndex <= cur.startIndex && endIndex >= cur.endIndex)
		{
			itr = highlights_.erase(itr);
		}
		else if (cur.startIndex <= startIndex && endIndex <= cur.endIndex)
		{
			
			if (cur.startIndex == startIndex)
			{
				cur.startIndex = endIndex;
			}
			else if (cur.endIndex == endIndex)
			{
				cur.endIndex = startIndex;
			}
			else
			{
				Highlight h = cur;
				h.startIndex = endIndex;

				cur.endIndex = startIndex;				
				++itr;								
				itr = highlights_.insert(itr, h);
			}			
			++itr;
		}		
		else
		{
			if (cur.startIndex < startIndex && startIndex < cur.endIndex)
			{
				cur.endIndex = startIndex;
			}
			else if (cur.startIndex < endIndex && endIndex < cur.endIndex)
			{
				cur.startIndex = endIndex;
			}
			++itr;
		}
	}
}

void Text::drawHighlight(GfxRenderer * gl, const Highlight & highlight) const
{
	Color color = highlight.color;
	gl->useColorProgram();
	gl->setColorProgramColor(color.r, color.g, color.b, color.a);
	//gl->setColorProgramColor(0.5f, 0.5f, 1.0f, 1.0f);

	int lineY0, lineIndex0, lineStartIndex0, lineEndIndex0;
	lineDataForChar(highlight.startIndex, &lineY0, &lineIndex0, &lineStartIndex0, &lineEndIndex0);
	//lineDataForChar(charStartIndex_, &lineY0, &lineIndex0, &lineStartIndex0, &lineEndIndex0);
	int lineY1, lineIndex1, lineStartIndex1, lineEndIndex1;
	lineDataForChar(highlight.endIndex, &lineY1, &lineIndex1, &lineStartIndex1, &lineEndIndex1);
	//lineDataForChar(charEndIndex_, &lineY1, &lineIndex1, &lineStartIndex1, &lineEndIndex1);

	Vector2 posTop0; int posHeight0, posWidth0;
	posAtIndex(highlight.startIndex, &posTop0, &posHeight0, &posWidth0);
	//posAtIndex(charStartIndex_, &posTop0, &posHeight0, &posWidth0);
	Vector2 posTop1; int posHeight1, posWidth1;
	posAtIndex(highlight.endIndex, &posTop1, &posHeight1, &posWidth1);
	//posAtIndex(charEndIndex_, &posTop1, &posHeight1, &posWidth1);

	if (lineIndex0 == lineIndex1)
	{
		gl->drawRect(posTop0.x, posTop0.y, posTop1.x - posTop0.x, posHeight0);
	}
	else
	{
		int lineYP, lineWidthP, lineHeightP, lineStartIndexP, lineEndIndexP;
		lineDataForLine(lineIndex0, 
			&lineYP, &lineWidthP, &lineHeightP, &lineStartIndexP, &lineEndIndexP);

		int lineY, lineWidth, lineHeight, lineStartIndex, lineEndIndex;
		float linelen;
		for (int i = lineIndex0 + 1; i <= lineIndex1 + 1; ++i)
		{				
			int drawX = 0;
			int drawY = lineYP;
			int width = lineWidthP;				
			int height = lineHeightP;
			if (textAlign() == 1 && boundaryWidth() > 0)
			{
				if (!lineLength(i, &linelen))
					continue;
				drawX = (boundaryWidth() - linelen) / 2;
				width = linelen;
			}
			else if (textAlign() == 2 && boundaryWidth() > 0)
			{
				if (!lineLength(i, &linelen))
					continue;
				drawX = boundaryWidth() - linelen;
				width = linelen;
			}

			if (i == lineIndex0 + 1) 
			{
				drawX = posTop0.x;
				width = lineWidthP - drawX;
			}
			else if (i == lineIndex1 + 1) 
			{
				width = posTop1.x - drawX;
				//if ( (textObj_->textAlign() == 1 || textObj_->textAlign() == 2) && textObj_->boundaryWidth() > 0)
			}

			if (i < lineIndex1 + 1)
			{
				lineDataForLine(i, 
					&lineY, &lineWidth, &lineHeight, &lineStartIndex, &lineEndIndex);

				if (height > lineY - lineYP) height = lineY - lineYP;

				lineYP = lineY;
				lineWidthP = lineWidth;
				lineHeightP = lineHeight;
				lineStartIndexP = lineStartIndex;
				lineEndIndexP = lineEndIndex;
			}							
			
			gl->drawRect(drawX, drawY, width, height);

		}
	}
}

Vector2 Text::selectedPartPos()
{
	Vector2 posTop; int posHeight, posWidth;
	posAtIndex(charStartIndex_, &posTop, &posHeight, &posWidth);

	Transform t;
	Matrix m = parentTransform(); 
	m *= visualAttrib_.transformMatrix();
	t.setMatrix(m, true);
	posTop.x += t.translation().x;
	posTop.y += t.translation().y;

	return posTop;
}

std::wstring Text::textPartString(unsigned startIndex, unsigned endIndex)
{
	return text_.substr(startIndex, endIndex - startIndex);
}


void Text::search(std::vector<unsigned> & foundIndices, const std::wstring & searchStr) const
{
	if (searchStr.empty()) return;
	wstring::size_type pos = 0;
	foundIndices.clear();
	while (1)
	{
		pos = text_.find(searchStr, pos);
		if (pos == wstring::npos) break;
		foundIndices.push_back((unsigned)pos);
		pos += searchStr.size();
	}
}

//ensure there is a TextProperties at the startIndex, endIndex borders	
void Text::makeBorderProperties(unsigned startIndex, unsigned endIndex)
{
	vector<TextProperties>::iterator itr;
	
	for (itr = properties_.begin(); itr != properties_.end(); ++itr)
	{
		TextProperties * curProps = &*itr;		
		unsigned curPropsEndIndex = text_.size();
		vector<TextProperties>::iterator itrNext = itr + 1;
		if (itrNext != properties_.end()) curPropsEndIndex = (*itrNext).index;
				
		if (curProps->index < startIndex && startIndex < curPropsEndIndex)
		{			
			TextProperties newProps = *curProps;
			itr = properties_.insert(itr, newProps);
			++itr;
			(*itr).index = startIndex;
			curProps = &*itr;
		}

		if (curProps->index < endIndex && endIndex < curPropsEndIndex)
		{
			TextProperties newProps = *curProps;
			itr = properties_.insert(itr, newProps);
			++itr;
			(*itr).index = endIndex;
			curProps = &*itr;
		}
	}

	if (endIndex == text_.size())
	{
		TextProperties newProps = properties_.back();
		newProps.index = endIndex;
		properties_.push_back(newProps);		
	}
}

void Text::applyProperties(const std::vector<TextProperties> & localIndexProperties, unsigned startIndex, unsigned endIndex)
{
	if (localIndexProperties.empty()) return;
	if (endIndex <= startIndex) return;
	
	vector<TextProperties>::iterator itr;

	makeBorderProperties(startIndex, endIndex);
		
	for (itr = properties_.begin(); itr != properties_.end();)
	{
		TextProperties & curProps = *itr;

		if (startIndex <= curProps.index && curProps.index < endIndex)
			itr = properties_.erase(itr);
		else 
			++itr;
	}

	for (itr = properties_.begin(); itr != properties_.end(); ++itr)
	{
		TextProperties & curProps = *itr;
		if (curProps.index > startIndex) break;
	}

	BOOST_FOREACH(TextProperties props, localIndexProperties)
	{
		if (props.index >= endIndex) break;
		props.index += startIndex;
		itr = ++properties_.insert(itr, props);
	}

	mergeProperties();
}


void Text::applyProperties(const TextProperties & newProps, TextProperties::Type types, unsigned charLen)
{
	//check bounds and correct charLen if necessary
	if (newProps.index + charLen > text_.size()) charLen = text_.size() - newProps.index;
	if (charLen == 0) return;	
	if (newProps.index > text_.size() - 1) return;

	
	//compute startIndex and endIndex for newProps
	unsigned startIndex = newProps.index;
	unsigned endIndex = newProps.index + charLen;	

	makeBorderProperties(startIndex, endIndex);

	vector<TextProperties>::iterator itr;

	//Appropriately modify the existing TextProperties that fall within startIndex and endIndex	
	for (itr = properties_.begin(); itr != properties_.end(); ++itr)
	{	
		TextProperties & curProps = *itr;		
		unsigned curPropsEndIndex = text_.size();
		vector<TextProperties>::iterator itrNext = itr + 1;
		if (itrNext != properties_.end()) curPropsEndIndex = (*itrNext).index;

		if (startIndex <= curProps.index && curPropsEndIndex <= endIndex)
		{
			if (types & TextProperties::PropertyColor) 
				curProps.color = newProps.color;

			if (types & TextProperties::PropertyFont) 
				curProps.font = newProps.font;

			if (types & TextProperties::PropertyFontSize) 
				curProps.font.pointSize = newProps.font.pointSize;

			if (types & TextProperties::PropertyFontFileName)
				curProps.font.fontFile = newProps.font.fontFile;
		}
	}

	mergeProperties();
}

void Text::mergeProperties()
{
	vector<TextProperties>::iterator itr;
	for (itr = properties_.begin(); itr != properties_.end();)
	{		
		vector<TextProperties>::iterator itrNext = itr + 1;
		if (itrNext == properties_.end()) break;

		TextProperties & props = *itr;
		TextProperties & propsNext = *itrNext;

		if (props == propsNext)
		{
			itr = properties_.erase(itrNext);
			--itr;
		}
		else
			itr = itrNext;
	}

	//remove extraneous properties
	if (!text_.empty())
	{
		for (itr = properties_.begin(); itr != properties_.end();)
		{
			TextProperties & props = *itr;
			if (props.index >= text_.size())
				itr = properties_.erase(itr);
			else
				++itr;
		}
	}	
}

void Text::getProperties(std::vector<TextProperties> & properties, unsigned int startIndex, unsigned int endIndex, bool localIndices) const
{
	unsigned int propStartIndex, propEndIndex;
	getProperties(propStartIndex, propEndIndex, startIndex, endIndex);

	properties.clear();

	for (unsigned int i = propStartIndex; i <= propEndIndex; ++i) properties.push_back(properties_[i]);		
	
	if (localIndices)
	{
		for (int i = 0; i < (int)properties.size(); ++i)
		{
			if (properties[i].index < startIndex) properties[i].index = 0;
			else properties[i].index -= startIndex;
		}
	}
}

void Text::getProperties(unsigned & propStartIndex, unsigned & propEndIndex, unsigned startIndex, unsigned endIndex) const
{	
	if (endIndex < startIndex) endIndex = startIndex;
	bool isBlock = startIndex < endIndex;

	propStartIndex = UINT_MAX;
	propEndIndex = 0;


	if (isBlock)
	{
		for (unsigned i = 0; i < (unsigned)properties_.size(); ++i)
		{
			unsigned nextPropIndex = (unsigned)text_.size();
			if (i < properties_.size() - 1)
				nextPropIndex = properties_[i + 1].index;

			bool includeProperty = false;

			if (properties_[i].index <= startIndex && startIndex < nextPropIndex)
				includeProperty = true;
			else if (properties_[i].index < endIndex && endIndex <= nextPropIndex)
				includeProperty = true;
			else if (startIndex <= properties_[i].index && nextPropIndex <= endIndex)
				includeProperty = true;		

			if (includeProperty)
			{
				if (propStartIndex > i) propStartIndex = i;
				if (propEndIndex < i) propEndIndex = i;				
			}
		}
	}
	else
	{
		for (unsigned i = 0; i < (unsigned)properties_.size(); ++i)
		{
			unsigned nextPropIndex = (unsigned)text_.size();
			if (i < properties_.size() - 1)
				nextPropIndex = properties_[i + 1].index;

			if (properties_[i].index <= startIndex && startIndex < nextPropIndex)
				propStartIndex = propEndIndex = i;				
		}

		if (propStartIndex == UINT_MAX) propStartIndex = propEndIndex = properties_.size() - 1;
	}
}


void Text::insertText(const std::wstring & text, const std::vector<TextProperties> & props, unsigned index)
{
	unsigned len = text.size();
	
	makeBorderProperties(index, index);
	unsigned prop0, prop1;
	getProperties(prop0, prop1, index, index);

	for (unsigned i = prop0; i < (unsigned)properties_.size(); ++i) properties_[i].index += len;
	
	text_.insert(index, text);

	applyProperties(props, index, index + text.size());
}

void Text::removeText(unsigned startIndex, unsigned endIndex)
{
	if (endIndex <= startIndex) return;	
	unsigned len = endIndex - startIndex;
	
	TextProperties firstProp = properties_.front();

	makeBorderProperties(startIndex, endIndex);
	unsigned prop0, prop1;
	getProperties(prop0, prop1, startIndex, endIndex);
	
	vector<TextProperties>::iterator itr;
	itr = properties_.begin() + prop0;
	for (int i = prop0; i <= prop1; ++i) itr = properties_.erase(itr);
	
	for (; itr != properties_.end(); ++itr)
	{
		TextProperties & props = *itr;
		props.index -= len;
	}

	text_.erase(startIndex, len);

	if (properties_.empty()) properties_.push_back(firstProp);
}

bool Text::asyncLoadUpdate()
{
	bool everythingLoaded = SceneObject::asyncLoadUpdate();
	map<TextData, StringData_old *>::const_iterator iter;
	for (iter = textMap_.begin(); iter != textMap_.end(); ++iter)
	{
		const TextData & data = (*iter).first;
		StringData_old * stringData = (*iter).second;

		if (stringData)
		{
			everythingLoaded &= data.textHandler->asyncLoadUpdate();			
		}
	}	
	return everythingLoaded;	
}

void Text::addImageProperties(ImageProperties prop)
{
	imgProps_.push_back(prop);
}

void Text::setMemos(std::vector<textMemoIndex> memos, Scene* curScene, unsigned char * bits, GfxRenderer *gl, int width, int height)
{
	memos_ = memos;

	if (!memoTex_) memoTex_.reset(new Texture);
	if (!memoTex_->isInit())
	{
		memoTex_->init(gl, width, height, Texture::UncompressedRgba32, bits);
	}

	parentScene_->parentDocument()->triggerRedraw();
}

void Text::addMemo(int startIndex, int endIndex, Scene* curScene)
{
	textMemoIndex newMemo;
	newMemo.startIndex = startIndex;
	newMemo.endIndex = endIndex;
	
	memos_.push_back(newMemo);
}

void Text::deleteMemo(int startIndex, int endIndex)
{
	int index = 0;

	for(int i = 0; i < (int)memos_.size(); i++)
	{
		if(memos_[i].startIndex == startIndex && memos_[i].endIndex == endIndex)
		{
			index = i;
			break;
		}
	}
	
	vector<textMemoIndex>::iterator itr;
	for (itr = memos_.begin(); itr != memos_.end();)
	{
		textMemoIndex & cur = *itr;

		if (startIndex == cur.startIndex && endIndex == cur.endIndex)
		{
			itr = memos_.erase(itr);
			break;
		}
		else
			++itr;
	}
}

void Text::drawMemoIcon(GfxRenderer * gl) const
{
	gl->useTextureProgram();
	gl->setTextureProgramOpacity(1.0f);	

	gl->use(memoTex_.get());

	for(int i = 0; i < (int)memos_.size(); i++)
	{
		int lineY, lineIndex, lineStartIndex, lineEndIndex;
		lineDataForChar(memos_[i].endIndex, &lineY, &lineIndex, &lineStartIndex, &lineEndIndex);
	
		Vector2 posTop; int posHeight, posWidth;
		posAtIndex(memos_[i].startIndex, &posTop, &posHeight, &posWidth);

		Vector3 scale;

		Vector3 curSceneScale = parentScene_->parentDocument()->curScene()->currentScale();

		if(parentScene_ != parentScene_->parentDocument()->curScene())
		{
			SceneObject * containerObj = parentScene_->curContainerObject();

			Vector3 parentSceneScale = parentScene_->currentScale();

			while(containerObj)
			{
				Vector3 cScale = containerObj->parentScene()->currentScale();

				parentSceneScale = Vector3(parentSceneScale.x * cScale.x, parentSceneScale.y * cScale.y, parentSceneScale.z * cScale.z);

				if(containerObj->parentScene() == parentScene_->parentDocument()->curScene())
					break;

				containerObj = containerObj->parentScene()->curContainerObject();
			}

			scale = parentSceneScale;
		}
		else
			scale = curSceneScale;

		gl->drawRect(posTop.x - ((memoW_ / scale.x)/2), posTop.y - ((memoH_ / scale.y)/2), memoW_ / scale.x, memoH_ / scale.y);
	}
}