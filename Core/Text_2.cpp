#include "stdafx.h"
#include "Text_2.h"
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
#include "TextHandler.h"

#include "utf8.h"
#include "ScriptProcessor.h"
#include "MathStuff.h"
#include "EventListener.h"
#include "Action.h"

//#include <libxml/HTMLparser.h>

using namespace std;
///////////////////////////////////////////////////////////////////////////////
std::string wstring2string(std::wstring wstr) {
	std::string str(wstr.length(),' ');
	copy(wstr.begin(),wstr.end(),str.begin());
	return str;
}

std::wstring string2wstring(std::string str) {
	std::wstring wstr(str.length(),L' ');
	copy(str.begin(),str.end(),wstr.begin());
	return wstr;
}

std::wstring temp_mbs_to_wcs(std::string const& str, std::locale loc)
{
	typedef std::codecvt<wchar_t, char, std::mbstate_t> codecvt_t;
	codecvt_t const& codecvt = std::use_facet<codecvt_t>(loc);
	std::mbstate_t state = std::mbstate_t();
	std::vector<wchar_t> buf(str.size() + 1);
	char const* in_next = str.c_str();
	wchar_t* out_next = &buf[0];
	std::codecvt_base::result r = codecvt.in(state, 
		str.c_str(), str.c_str() + str.size(), in_next, 
		&buf[0], &buf[0] + buf.size(), out_next);
	if (r == std::codecvt_base::error)
		throw std::runtime_error("can't convert string to wstring");   
	return std::wstring(&buf[0]);
}

std::string temp_wcs_to_mbs(std::wstring const& str, std::locale loc)
{
	typedef std::codecvt<wchar_t, char, std::mbstate_t> codecvt_t;
	codecvt_t const& codecvt = std::use_facet<codecvt_t>(loc);
	std::mbstate_t state = std::mbstate_t();
	std::vector<char> buf((str.size() + 1) * codecvt.max_length());
	wchar_t const* in_next = str.c_str();
	char* out_next = &buf[0];
	std::codecvt_base::result r = codecvt.out(state, 
		str.c_str(), str.c_str() + str.size(), in_next, 
		&buf[0], &buf[0] + buf.size(), out_next);
	if (r == std::codecvt_base::error)
		throw std::runtime_error("can't convert wstring to string");   
	return std::string(&buf[0]);
}
std::string UTF16toUTF8(const std::wstring &wstr){
	std::string utf_;
	utf8::utf16to8(wstr.begin(), wstr.end(), back_inserter(utf_));
	return utf_;
}
std::string UTF16toUTF8(const std::string &str){
	std::string utf_;

	std::locale loc(std::locale(""));
	std::wstring wstr = temp_mbs_to_wcs(str, loc);

	utf8::utf16to8(wstr.begin(), wstr.end(), back_inserter(utf_));

	return utf_;
}

string char2hex_text2( char dec )
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

string urlencode_text2(const string &c)
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
			escaped.append( char2hex_text2(c[i]) );//converts char 255 to string "ff"
		}
	}
	return escaped;
}

JSClass jsText_2Class = InitClass(
	"Text_2", JSCLASS_HAS_PRIVATE | JSCLASS_MARK_IS_TRACE, JS_PropertyStub, 
	JS_StrictPropertyStub, SceneObject_finalize, SceneObject::gcMark);

JSBool Text_2_setText(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	Text_2 * thisObj = (Text_2 *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));

	jschar * jsStr;
	if (!JS_ConvertArguments(cx, argc, argv, "W", &jsStr)) return JS_FALSE;
	JS_SET_RVAL(cx, vp, JSVAL_VOID);

		
	thisObj->setTextString(jsCharStrToStdWString(jsStr));
	thisObj->init(thisObj->parentScene()->parentDocument()->renderer());
	return JS_TRUE;
};

JSBool Text_2_setProperties(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	Text_2 * thisObj = (Text_2 *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);

	JSObject * jsProperties;
	if (!JS_ConvertArguments(cx, argc, argv, "o", &jsProperties)) return JS_FALSE;
	JS_SET_RVAL(cx, vp, JSVAL_VOID);

	if (!JS_IsArrayObject(cx, jsProperties))
	{
		JS_ReportError(cx, "Text_2::setProperties - Parameter must be an array.");
		return JS_FALSE;
	}

	jsuint jsLen;
	JS_GetArrayLength(cx, jsProperties, &jsLen);

	std::vector<TextProperties_2> propertiesList;
	propertiesList.resize(jsLen);

	for (int i = 0; i < (int)jsLen; ++i)
	{
		jsval jsCurVal;
		JS_GetElement(cx, jsProperties, i, &jsCurVal);		
		if (!JSVAL_IS_OBJECT(jsCurVal)) continue;
		JSObject * jsProperties = JSVAL_TO_OBJECT(jsCurVal);

		getPropertyInt(cx, jsProperties, "index", &propertiesList[i].index);
		getPropertyBool(cx, jsProperties, "underline", &propertiesList[i].underline_);
		getPropertyBool(cx, jsProperties, "cancleline", &propertiesList[i].cancleline_);
		getPropertyFloat(cx, jsProperties, "letterwithratio", &propertiesList[i].letterwidthratio_);
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


JSBool Text_2_setBoundaryWidth(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	Text_2 * thisObj = (Text_2 *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	jsdouble jsArg1;
	if (!JS_ConvertArguments(cx, argc, argv, "d", &jsArg1)) return JS_FALSE;
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	thisObj->setBoundaryWidth((float)jsArg1);	
	return JS_TRUE;
};

JSBool Text_2_setBoundaryHeight(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	Text_2 * thisObj = (Text_2 *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	jsdouble jsArg1;
	if (!JS_ConvertArguments(cx, argc, argv, "d", &jsArg1)) return JS_FALSE;
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	thisObj->setBoundaryHeight((float)jsArg1);	
	return JS_TRUE;
};

JSBool Text_2_setColor(JSContext *cx, uintN argc, jsval *vp)
{
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	jsval * argv = JS_ARGV(cx, vp);
	Text_2 * thisObj = (Text_2 *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	JSObject * jsObj;
	if (!JS_ConvertArguments(cx, argc, argv, "o", &jsObj)) return JS_FALSE;	
	Color color = Color::fromScriptObject(s, jsObj);
	thisObj->setColor(color);	
	s->document()->triggerRedraw();
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
};

JSBool Text_2_setFontSize(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	Text_2 * thisObj = (Text_2 *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	jsdouble jsArg1;
	if (!JS_ConvertArguments(cx, argc, argv, "d", &jsArg1)) return JS_FALSE;
	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	std::vector<TextProperties_2> props = thisObj->properties();
	if (props.size() > 0)
	{
		TextProperties_2 prop = props.front();
		props.clear();
		prop.font.pointSize = (int)jsArg1;
		props.push_back(prop);
		thisObj->setProperties(props);
		thisObj->init(s->document()->renderer());
	}
	return JS_TRUE;
};


JSBool Text_2_getText(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	Text_2 * thisObj = (Text_2 *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	if (!JS_ConvertArguments(cx, argc, argv, "")) return JS_FALSE;

	string narrowString = "";
	utf8::utf16to8(thisObj->textString().begin(), thisObj->textString().end(), back_inserter(narrowString));
	string str = urlencode_text2(narrowString);

	const char * textStr = str.c_str();
	JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(JS_NewStringCopyZ(cx, textStr)));
	return JS_TRUE;
};

JSBool Text_2_getWidth(JSContext *cx, uintN argc, jsval *vp)
{
	JSObject * obj = JS_THIS_OBJECT(cx, vp);
	Text_2 * text = (Text_2 *)JS_GetPrivate(cx, obj);	
	JS_SET_RVAL(cx, vp, DOUBLE_TO_JSVAL(text->width()));
	return JS_TRUE;
}

JSBool Text_2_getHeight(JSContext *cx, uintN argc, jsval *vp)
{
	JSObject * obj = JS_THIS_OBJECT(cx, vp);
	Text_2 * text = (Text_2 *)JS_GetPrivate(cx, obj);	
	JS_SET_RVAL(cx, vp, DOUBLE_TO_JSVAL(text->height()));
	return JS_TRUE;
}

JSBool Text_2_getWordText(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	JSObject * obj = JS_THIS_OBJECT(cx, vp);
	Text_2 * text = (Text_2 *)JS_GetPrivate(cx, obj);	

	jsdouble jsX, jsY;
	if (!JS_ConvertArguments(cx, argc, argv, "dd", &jsX, &jsY)) 
		return JS_FALSE;
	JS_SET_RVAL(cx, vp, JSVAL_VOID);

	if (!jsX)
	{
		JS_ReportError(cx, "parameter not a valid x");
		return JS_FALSE;
	}

	if (!jsY)
	{
		JS_ReportError(cx, "parameter not a valid y");
		return JS_FALSE;
	}
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	WordInfo rv = text->getPosWordInfo((float)jsX, (float)jsY);	

	string narrowString = "";
	utf8::utf16to8(rv.text.begin(), rv.text.end(), back_inserter(narrowString));
	string str = urlencode_text2(narrowString);
	JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(JS_NewStringCopyZ(cx, str.c_str())));
	return JS_TRUE;
}

JSBool Text_2_getWordLeftPos(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	JSObject * obj = JS_THIS_OBJECT(cx, vp);
	Text_2 * text = (Text_2 *)JS_GetPrivate(cx, obj);	

	jsdouble jsX, jsY;
	if (!JS_ConvertArguments(cx, argc, argv, "dd", &jsX, &jsY)) 
		return JS_FALSE;
	JS_SET_RVAL(cx, vp, JSVAL_VOID);

	if (!jsX)
	{
		JS_ReportError(cx, "parameter not a valid x");
		return JS_FALSE;
	}

	if (!jsY)
	{
		JS_ReportError(cx, "parameter not a valid y");
		return JS_FALSE;
	}
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	WordInfo rv = text->getPosWordInfo((float)jsX, (float)jsY);	

	float leftPos = rv.left;
	JS_SET_RVAL(cx, vp, DOUBLE_TO_JSVAL(leftPos));
	return JS_TRUE;
}

JSBool Text_2_getWordRightPos(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	JSObject * obj = JS_THIS_OBJECT(cx, vp);
	Text_2 * text = (Text_2 *)JS_GetPrivate(cx, obj);	

	jsdouble jsX, jsY;
	if (!JS_ConvertArguments(cx, argc, argv, "dd", &jsX, &jsY)) 
		return JS_FALSE;
	JS_SET_RVAL(cx, vp, JSVAL_VOID);

	if (!jsX)
	{
		JS_ReportError(cx, "parameter not a valid x");
		return JS_FALSE;
	}

	if (!jsY)
	{
		JS_ReportError(cx, "parameter not a valid y");
		return JS_FALSE;
	}
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	WordInfo rv = text->getPosWordInfo((float)jsX, (float)jsY);	

	float right = rv.right;
	JS_SET_RVAL(cx, vp, DOUBLE_TO_JSVAL(right));
	return JS_TRUE;
}

JSBool Text_2_getWordTopPos(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	JSObject * obj = JS_THIS_OBJECT(cx, vp);
	Text_2 * text = (Text_2 *)JS_GetPrivate(cx, obj);	

	jsdouble jsX, jsY;
	if (!JS_ConvertArguments(cx, argc, argv, "dd", &jsX, &jsY)) 
		return JS_FALSE;
	JS_SET_RVAL(cx, vp, JSVAL_VOID);

	if (!jsX)
	{
		JS_ReportError(cx, "parameter not a valid x");
		return JS_FALSE;
	}

	if (!jsY)
	{
		JS_ReportError(cx, "parameter not a valid y");
		return JS_FALSE;
	}
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	WordInfo rv = text->getPosWordInfo((float)jsX, (float)jsY);	

	float TopPos = rv.top;
	JS_SET_RVAL(cx, vp, DOUBLE_TO_JSVAL(TopPos));
	return JS_TRUE;
}

JSBool Text_2_getWordBottomPos(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	JSObject * obj = JS_THIS_OBJECT(cx, vp);
	Text_2 * text = (Text_2 *)JS_GetPrivate(cx, obj);	

	jsdouble jsX, jsY;
	if (!JS_ConvertArguments(cx, argc, argv, "dd", &jsX, &jsY)) 
		return JS_FALSE;
	JS_SET_RVAL(cx, vp, JSVAL_VOID);

	if (!jsX)
	{
		JS_ReportError(cx, "parameter not a valid x");
		return JS_FALSE;
	}

	if (!jsY)
	{
		JS_ReportError(cx, "parameter not a valid y");
		return JS_FALSE;
	}
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	WordInfo rv = text->getPosWordInfo((float)jsX, (float)jsY);	

	float bottomPos = rv.bottom;
	JS_SET_RVAL(cx, vp, DOUBLE_TO_JSVAL(bottomPos));
	return JS_TRUE;
}

JSFunctionSpec Text_2Funcs[] = {
	JS_FS("setText", Text_2_setText, 1, 0),
	JS_FS("getText", Text_2_getText, 0, 0),
	JS_FS("setHeight", Text_2_setProperties, 1, 0),
	JS_FS("setBoundaryWidth", Text_2_setBoundaryWidth, 1, 0),
	JS_FS("setBoundaryHeight", Text_2_setBoundaryHeight, 1, 0),
	JS_FS("setProperties", Text_2_setProperties, 1, 0),
	JS_FS("setFontSize", Text_2_setFontSize, 1, 0),
	JS_FS("setColor", Text_2_setColor, 1, 0),
	JS_FS("getWidth", Text_2_getWidth, 0, 0),
	JS_FS("getHeight", Text_2_getHeight, 0, 0),
	JS_FS("getWordText", Text_2_getWordText, 2, 0),
	JS_FS("getWordLeftPos", Text_2_getWordLeftPos, 2, 0),
	JS_FS("getWordRightPos", Text_2_getWordRightPos, 2, 0),
	JS_FS("getWordTopPos", Text_2_getWordTopPos, 2, 0),
	JS_FS("getWordBottomPos", Text_2_getWordBottomPos, 2, 0),
	JS_FS_END
};



///////////////////////////////////////////////////////////////////////////////

TextProperties_2::TextProperties_2(const TextProperties_2 & rhs , bool bCopyObjectProp){
	index = rhs.index;
	font = rhs.font;
	color = rhs.color;
	link = rhs.link;
	underline_ = rhs.underline_;
	cancleline_ = rhs.cancleline_;
	letterwidthratio_ = rhs.letterwidthratio_;
	bObject = false;
}

void TextProperties_2::assign(const TextProperties_2 & rhs, Type types)
{
	if (types & TextProperties_2::PropertyColor) 
		color = rhs.color;

	if (types & TextProperties_2::PropertyFont) 
		font = rhs.font;

	if (types & TextProperties_2::PropertyFontSize) 
		font.pointSize = rhs.font.pointSize;
}

TextProperties_2::~TextProperties_2(){
}

void TextProperties_2::SetObjectProperties(ObjectProperties *pSrcobject){

	ObjectProperties *pTargetObjectProperty = NULL;
	
	if(pSrcobject->property_type == IMAGE_OBJECT){
		imageObject_.fileName = ((ImageObjectProperties *)pSrcobject)->fileName;
		pTargetObjectProperty = (ObjectProperties *)&imageObject_;
	}

	pTargetObjectProperty->property_type = pSrcobject->property_type;
	pTargetObjectProperty->bMakeTexture = false;


	bObject = true;
	object_type = pSrcobject->property_type;
	
}


void TextProperties_2::write(Writer & writer) const
{
	writer.write(index, "index");
	writer.write(font, "font");
	writer.write(color, "color");
	writer.write(link, "link");
	writer.write(underline_, "underline");
	writer.write(cancleline_, "cancleline");
	writer.write(letterwidthratio_, "letterwidthratio");
	writer.write(bObject, "isObject");

	if(bObject){

		writer.write(object_type, "Object Property Type");

		if(object_type == IMAGE_OBJECT){
			writer.write(imageObject_.fileName);
			writer.write(imageObject_.width);
			writer.write(imageObject_.height);
		}
	}
}

void TextProperties_2::writeXml(XmlWriter & w) const
{
	w.writeTag("Index", index);
	font.writeXml(w);
	w.writeTag("Color", color);
	w.writeTag("Link", link);
	w.writeTag("Underline", underline_);
	w.writeTag("Cancleline", cancleline_);
	w.writeTag("LetterwithRatio", letterwidthratio_);
	
	if(bObject){

		w.startTag("Object");

		if(object_type == IMAGE_OBJECT){
			w.writeTag("ObjectType", "Image");
			std::string fileName = UTF16toUTF8(imageObject_.fileName);
			w.writeTag("FileName", fileName);
			w.writeTag("Width", imageObject_.width);
			w.writeTag("Height", imageObject_.height);
		}else
			w.writeTag("ObjectType", "Unknown");
		
		w.endTag();
	}
}

void TextProperties_2::readXml(XmlReader & r, xmlNode * parent)
{
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
		else if (r.getNodeContentIfName(underline_, curNode, "Underline"));
		else if (r.getNodeContentIfName(cancleline_, curNode, "Cancleline"));
		else if (r.getNodeContentIfName(letterwidthratio_, curNode, "LetterwithRatio"));
		
		//TODO Add Object part (TextProperties_2::readXml)

	}
}

void TextProperties_2::read(Reader & reader, unsigned char version)
{
	reader.read(index);
	reader.read(font);
	reader.read(color);
	reader.read(link);
	reader.read(underline_);
	reader.read(cancleline_);
	reader.read(letterwidthratio_);
	reader.read(bObject);
	if(bObject){
		reader.read(object_type);

		if(object_type == IMAGE_OBJECT){
			imageObject_.property_type = object_type;
			reader.read(imageObject_.fileName);
			reader.read(imageObject_.width);
			reader.read(imageObject_.height);
		}
	}
}
///////////////////////////////////////////////////////////////////////////////
JSObject * Text_2::createScriptObjectProto(ScriptProcessor * s)
{			
	JSObject * proto = JS_InitClass(
		s->jsContext(), s->jsGlobal(), s->baseSceneObjectProto(), &jsText_2Class,
		0, 0, 0, Text_2Funcs, 0, 0);

	return proto;
}

JSObject * Text_2::getScriptObject(ScriptProcessor * s)
{
	if (!scriptObject_)
	{
		scriptObject_ = JS_NewObject(
			s->jsContext(), &jsText_2Class, s->sceneObjectProto(this), 0);
		JS_SetPrivate(s->jsContext(), scriptObject_, this);

		if (parent_) parent_->getScriptObject(s);
	}

	return scriptObject_;
}

void Text_2::create()
{
	
	TextProperties_2 properties;
	properties.index = 0;
	properties.font.pointSize = 16;
	properties.font.faceIndex = 0;
	properties.font.bold = 0;
	properties.font.italic = 0;
	properties.underline_ = false;
	properties.cancleline_ = false;
	properties.letterwidthratio_ = 100.0f;
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

	letterSpacing_ = 0;

	lineSpacing_ = 0;
	lineSpacingMode_ = LineSpacingAuto;

	canSelectPart_ = true;
	isSelectedPart_ = false;

	charStartIndex_ = 0;
	charEndIndex_ = 0;

	highlights_.clear();

	LineStyle lineStyle;

	linesStyle_.push_back(lineStyle);

	direction_ = DRight;

	dw_ = 1024;
	dh_ = 768;
}

const std::string Text_2::strBaseFontFilePath = "batang.ttc";

Text_2::Text_2()
{
	create();
	setId("TextObject");

}


Text_2::Text_2(const Text_2 & rhs, ElementMapping * elementMapping)
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
	linesStyle_ = rhs.linesStyle_;
	direction_ = rhs.direction_;
}


Text_2::~Text_2()
{
	uninit();
}

Texture * Text_2::addImageTexture(std::string imageName){

	if (imageTextureMap_.find(imageName) == imageTextureMap_.end())
	{
		Texture *pTexture = new Texture();
		imageTextureMap_[imageName] = pTexture;
	}			

	return imageTextureMap_[imageName];
}

void Text_2::referencedFiles(std::vector<std::string> * refFiles) const
{
	SceneObject::referencedFiles(refFiles);

	BOOST_FOREACH(const TextProperties_2 & props, properties_)
	{
		refFiles->push_back(props.font.fontFile);
		TextProperties_2 *prop = (TextProperties_2 *)&props;
		if (prop->isObjectProperty())
		{
			const ObjectProperties *p= prop->GetObjectProperty();
			if (p->property_type == IMAGE_OBJECT){
				ImageObjectProperties *p1 = (ImageObjectProperties *)p;

				refFiles->push_back(p1->fileName);
			}
		}
	}	
}

int Text_2::setReferencedFiles(const std::string & baseDirectory,
	const vector<string> & refFiles, int index)
{
	index = SceneObject::setReferencedFiles(baseDirectory, refFiles, index);
	
	BOOST_FOREACH(TextProperties_2 & props, properties_)
	{
		props.font.fontFile = refFiles[index++];
		if (props.isObjectProperty())
		{
			const ObjectProperties *p= props.GetObjectProperty();

			if (p->property_type == IMAGE_OBJECT){
				ImageObjectProperties *p1 = (ImageObjectProperties *)p;

				p1->fileName = refFiles[index++];
			}
		}
	}	

	return index;
}


void Text_2::setTextString(const std::wstring & str)
{
	text_ = str;	
}

void Text_2::setTextString(const std::string & str)
{
	text_ = string2wstring(str);
}

WordInfo Text_2::getPosWordInfo(float x, float y){
	WordInfo rv;

	rv.hasWord = false; //true일 때만 유효한 값

	int startIndex, endIndex;

	int propertyIndex;
	Vector2 startPos;
	startPos.x = x;
	startPos.y = y;

	startIndex = indexAtPos(toLocalPos(startPos), &propertyIndex);

	if(startIndex >= 0 && startIndex < text_.size()){
		wchar_t c = text_[startIndex];
		bool ws = c == L' ' || c == L'\t' || c == L'\r' || c == L'\b' || c == L'\n';

		if(ws)
			return rv;
	}

	endIndex = startIndex;
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

	if(startIndex != endIndex)
		rv.hasWord = true;

	if(rv.hasWord){

		Vector2 posStart, posEnd;
		float posHeight, posWidth;

		posAtIndex(startIndex, &posStart, &posHeight, &posWidth);	
		Scene * scene = parentScene();
		ContainerObjects containerObjs;
		scene->getContainerObjects(&containerObjs);

		Vector2 ret = parentTransform() * visualAttrib()->transformMatrix() * Vector3(0.0f, 0.0f, 0.0f);

		for (int i = 0; i < containerObjs.size(); ++i)
		{
			SceneObject * container = containerObjs[i].first;
			if (container) 
			{
				ret += container->parentTransform() * container->visualAttrib()->transformMatrix() * Vector3(0.0f, 0.0f, 0.0f);
			}
		}

		posAtIndex(endIndex, &posEnd, &posHeight, &posWidth);	

		rv.left = ret.x + posStart.x;
		rv.top = ret.y + posStart.y;

		rv.right = ret.x + posEnd.x - posStart.y + posWidth; 

		rv.bottom = ret.y +  posHeight;

		rv.text = textPartString(startIndex, endIndex);

		//printf("left : %f, right : %f, top :%f, bottom : %f\n", rv.left, rv.right, rv.top, rv.bottom);

	}

	

	return rv;
}
void Text_2::computeWordWrap(
	std::vector<WordWrapPoint_2> * wordWrapPoints,
	const std::wstring & str, 						   
	const std::vector<TextProperties_2> & properties, const std::vector<LineStyle> & linesStyle,
	int width, int height)
{
	bool validBoundary = width > 0;
	wordWrapPoints->clear();
	int strLen = (int)str.size();
	//first character of current word
	int lastWordBeginning = 0;
	float curWordTotalAdvanceX = 0.0;

	bool wasWordEnd = false;
	float curX = 0;

	int nextPropsCharIndex = 0;
	int curPropsIndex = -1;
	int curLineStyleIndex = -1;
	int nextLineStyleIndex = 0;
	
	TextProperties_2 *pCurTextProp = (TextProperties_2 *)&properties[0];
	TextHandler * curTextHandler = properties[0].textHandler;
	LineStyle *pCurLineStyle = (LineStyle *) &linesStyle[0];

	WordWrapPoint_2 wordWrapPoint;
	float nWhiteSpaceW = 0;

	wordWrapPoint.wrapAtIndex = 0;
	wordWrapPoint.wrapWidth = pCurLineStyle->startPosX;
	wordWrapPoint.toNextLine = Text_2::toNextLine(curTextHandler, properties[0]);	
	wordWrapPoint.descender = curTextHandler->descender();
	wordWrapPoint.nStartx = pCurLineStyle->startPosX;

	curX = pCurLineStyle->startPosX;

	if(pCurTextProp->isObjectProperty()){
		wordWrapPoint.ascenderHeight = pCurTextProp->GetObjectProperty()->height;
	}else{
		wordWrapPoint.ascenderHeight = curTextHandler->pixelSize();
	}

	for (int i = 0; i < strLen; ++i)
	{
		if (i == nextPropsCharIndex)
		{
			++curPropsIndex;
			if (curPropsIndex < (int)properties.size() - 1)
				nextPropsCharIndex = properties[curPropsIndex + 1].index;
			else
				nextPropsCharIndex = -1;

			curTextHandler = properties[curPropsIndex].textHandler;

			pCurTextProp = (TextProperties_2 *)&properties[curPropsIndex];
			
			int ascenderHeight = 0;

			if(pCurTextProp->isObjectProperty()){
				ascenderHeight = pCurTextProp->GetObjectProperty()->height;
			}else{
				ascenderHeight = curTextHandler->pixelSize();
			}

			if(wordWrapPoint.ascenderHeight < ascenderHeight)
				wordWrapPoint.ascenderHeight = ascenderHeight;

		}

		if (i == nextLineStyleIndex)
		{
			++curLineStyleIndex;

			if (curLineStyleIndex < linesStyle.size() - 1)
				nextLineStyleIndex = linesStyle[curLineStyleIndex + 1].charIndex;
			else
				nextLineStyleIndex = -1;

			pCurLineStyle = (LineStyle *) &linesStyle[curLineStyleIndex];

			wordWrapPoint.wrapAtIndex = i;
			wordWrapPoint.descender = curTextHandler->descender();
			wordWrapPoint.nStartx = pCurLineStyle->startPosX;
			wordWrapPoint.toNextLine = 0;

			curX = pCurLineStyle->startPosX;
			curWordTotalAdvanceX = pCurLineStyle->startPosX;

			lastWordBeginning = i;
		
		}

		const wchar_t & c = str[i];
		bool isNewLine = c == L'\n';
		bool isWhitespace = isNewLine || c == L' ';
		bool isSkip = c == L'\r' || c == L'\b';
		bool isTap = c == L'\t';
		
		float curToNextLine = Text_2::toNextLine(curTextHandler, properties[curPropsIndex]);	

		if (curToNextLine > wordWrapPoint.toNextLine)
		{
			wordWrapPoint.descender = curTextHandler->descender();

			if(pCurTextProp->isObjectProperty()){
				wordWrapPoint.ascenderHeight = pCurTextProp->GetObjectProperty()->height;
			}else{
				wordWrapPoint.ascenderHeight = curTextHandler->pixelSize();
			}
			wordWrapPoint.toNextLine = curToNextLine;			
		}

		if (isSkip) 
		{
			continue;
		}
		
		const CharData * charData = curTextHandler->charData(c);	

		float curAdvanceX = 0;

		TextProperties_2 *curProp = (TextProperties_2 *)&properties_[curPropsIndex];

		if(charData && curProp->isObjectProperty()){
			CharData *pObjChar = (CharData *)charData;
			pObjChar->advanceX = curProp->GetObjectProperty()->width;
		}else if(charData && isTap){

			int nTabCount = 0;
			double nModule = 0;
			CharData *pObjChar = (CharData *)charData;

			nTabCount = (curX) / TAB_SIZE;
			nModule = fmod((double)curX ,TAB_SIZE);

			if(nModule >= 0.0)
				nTabCount ++;

			pObjChar->width = (nTabCount * TAB_SIZE) - (curX);
			pObjChar->advanceX = charData->width;

		}

		if (charData && !isNewLine){ 

			if(isTap || curProp->isObjectProperty())
			{
				curAdvanceX = charData->advanceX + letterSpacing_;
			}
			else
			{
				curAdvanceX = (charData->advanceX * curProp->letterwidthratio_ / 100.0f) + letterSpacing_;
			}
		}else
			curAdvanceX = 0.0;

		curX += curAdvanceX;
		curWordTotalAdvanceX += curAdvanceX;

		if (isNewLine)
		{
			wordWrapPoint.wrapWidth = curX + ENTER_SIZE;
			wordWrapPoints->push_back(wordWrapPoint);

			wordWrapPoint.wrapAtIndex = i + 1;

			continue;
		}

		if (isWhitespace)
		{
			wasWordEnd = true;
			curWordTotalAdvanceX = 0;
			nWhiteSpaceW += curAdvanceX;
		}
		else //non whitespace character
		{			
			if (wasWordEnd)
			{
				lastWordBeginning = i;
				wasWordEnd = false;
			}

			
			if (validBoundary && curX > width && wordWrapPoint.wrapAtIndex != i)
			{
				float spaceW = 0;
				int prev = 1;

				if (curWordTotalAdvanceX + pCurLineStyle->startPosX > width)
				{
					//wordWrapPoint.wrapWidth = curX -  (float)curAdvanceX - nWhiteSpaceW;
					if(i < strLen && str[i + 1] != L' ')
						wordWrapPoint.wrapWidth = curX - (float)curAdvanceX - nWhiteSpaceW;
					else
					{
						while(str[lastWordBeginning - prev] == L' ')
						{
							const CharData * spacedata = curTextHandler->charData(str[lastWordBeginning - prev]);

							spaceW += (spacedata->advanceX * curProp->letterwidthratio_ / 100.0f) + letterSpacing_;
							prev++;
						}
						wordWrapPoint.wrapWidth = curX - (float)curAdvanceX - spaceW;// - nWhiteSpaceW;
					}
				}
				else 
				{					
					//wordWrapPoint.wrapWidth = curX - (float)curWordTotalAdvanceX - nWhiteSpaceW;
					if(i < strLen && str[i + 1] != L' ')
					{
						while(str[lastWordBeginning - prev] == L' ')
						{
							const CharData * spacedata = curTextHandler->charData(str[lastWordBeginning - prev]);

							spaceW += (spacedata->advanceX * curProp->letterwidthratio_ / 100.0f) + letterSpacing_;
							prev++;
						}

						wordWrapPoint.wrapWidth = curX - (float)curWordTotalAdvanceX - spaceW;// - nWhiteSpaceW;

					}
					else
						wordWrapPoint.wrapWidth = curX - (float)curWordTotalAdvanceX;// - nWhiteSpaceW;
				}

				wordWrapPoints->push_back(wordWrapPoint);

				if (curWordTotalAdvanceX + pCurLineStyle->startPosX > width)
				{
					wordWrapPoint.wrapAtIndex = i;
					curX = pCurLineStyle->startPosX + curAdvanceX;
				}
				else 
				{					
					wordWrapPoint.wrapAtIndex = lastWordBeginning;
					curX = pCurLineStyle->startPosX + curWordTotalAdvanceX;
				}
			}

			nWhiteSpaceW = 0;

		}
	}

	wordWrapPoint.wrapWidth = curX;
	wordWrapPoints->push_back(wordWrapPoint);
}

float Text_2::toNextLine(TextHandler * handler, const TextProperties_2 & props)
{
	TextProperties_2 *pCurTextProp = (TextProperties_2 *)&props;

	if (lineSpacingMode_ == TextProperties_2::LineSpacingAuto)
	{
		if(pCurTextProp->isObjectProperty())
		{
			float temp = handler->toNextLine() - handler->pixelSize();

			return (pCurTextProp->GetObjectProperty()->height + temp);
		}

		return handler->toNextLine() ;//pixelSize();
	}
	else
	{
		if(pCurTextProp->isObjectProperty())
		{
			float temp = handler->toNextLine() - handler->pixelSize();
			
			return pCurTextProp->GetObjectProperty()->height + temp + lineSpacing_;
		}

		return handler->pixelSize() + lineSpacing_ - handler->descender();
	}
}


void Text_2::init(GfxRenderer * gl, bool firstTime)
{
	init(gl, parentScene_, firstTime);
}

void Text_2::init(GfxRenderer * gl, Scene * parentScene, bool firstTime)
{
	SceneObject::init(gl, firstTime);

	parentScene_ = parentScene;

	if (properties_.size() == 0)
		return;

	std::vector<TextProperties_2> properties;
	std::vector<LineStyle> linesStyle;

	BOOST_FOREACH(TextProperties_2 & props, properties_)
	{
		if(props.isObjectProperty()){

			const ObjectProperties *pObjectProp = props.GetObjectProperty();

			if(pObjectProp->property_type == IMAGE_OBJECT){
				ImageObjectProperties *pTargetProp = (ImageObjectProperties *)pObjectProp;
				int w = 0, h = 0;

				pTargetProp->x = 0;
				pTargetProp->y = 0;
#ifndef ANDROID
				Global::instance().getImageDims(pTargetProp->fileName, &w, &h);
				pTargetProp->width = w;
				pTargetProp->height = h;
#endif				
				pTargetProp->texture = addImageTexture(pTargetProp->fileName);
				pTargetProp->texture->init(gl, pTargetProp->fileName, Texture::UncompressedRgba32);
			}
		}
		
		TextFont targetfont = props.font;
		targetfont.setTextureScaleSize(Global::instance().curSceneMagnification());
		props.textHandler = gl->textHandler(targetfont);
	}

	
	ReconstructLinStyle(gl);
	
	//Check Line error

	int nCurTextPos = 0;
	int nCurLineIndex = 0;

	while(nCurTextPos - text_.length()){

		if(linesStyle_.size() <= nCurLineIndex){
			LineStyle tempLineStyle(&linesStyle_[nCurLineIndex - 1]);

			linesStyle_.push_back(tempLineStyle);
		}


		LineStyle &curLineStyle = linesStyle_[nCurLineIndex];

		

		if(curLineStyle.index_ == nCurLineIndex && curLineStyle.charIndex == nCurTextPos){
			

			nCurTextPos = text_.find('\n', nCurTextPos);

			if(nCurTextPos == -1)
				break;


			nCurTextPos ++;
			nCurLineIndex ++;
			

			continue;

		}

		curLineStyle.index_ = nCurLineIndex;
		curLineStyle.charIndex = nCurTextPos + 1;

		nCurTextPos = text_.find('\n', nCurTextPos);


		if(nCurTextPos == -1)
			break;


		nCurTextPos ++;
		nCurLineIndex ++;

		
	}
	
	properties = properties_;
	linesStyle = linesStyle_;

	BOOST_FOREACH(TextProperties_2 & props, properties_)
	{
		TextFont targetfont = props.font;
		targetfont.setTextureScaleSize(Global::instance().curSceneMagnification());
		props.textHandler = gl->textHandler(targetfont);

		if(!props.textHandler->isinit())
			return;

		props.textHandler->genAnyNewChars(properties, text_);
	}

	
	
	//word wrap
	wordWrapPoints_.clear();

	computeWordWrap(&wordWrapPoints_, text_,
		properties, linesStyle, boundaryWidth_, boundaryHeight_);
	
	nDisplayW_ = 0.0;
	nDisplayH_ = 0.0;

	BOOST_FOREACH(WordWrapPoint_2 & wrap, wordWrapPoints_)
	{
		if(wrap.wrapWidth > nDisplayW_)
			nDisplayW_ = wrap.wrapWidth;

		if(boundaryHeight_ == 0){
			
			nDisplayH_ += (wrap.toNextLine);
		}
		
	}

	if(boundaryHeight_ > 0)
		nDisplayH_ = boundaryHeight_;

	if(boundaryWidth_ > 0)
		nDisplayW_ = boundaryWidth_;


	BOOST_FOREACH(TextProperties_2 & props, properties_)
	{
		TextData data;
		data.textHandler = props.textHandler;
		data.color = props.color;
		data.underline = props.underline_;
		data.cancleline = props.cancleline_;
		data.shadow = props.font.shadow;
		data.letterwidthratio = props.letterwidthratio_;

		if (textMap_.find(data) == textMap_.end())
		{
			textMap_[data] = props.textHandler->genString(gl,
				text_, direction_, properties_, props, wordWrapPoints_, linesStyle_, nDisplayW_, nDisplayH_, letterSpacing_);
		}			
	}

	std::vector<TextSpan> spans;
	
	map<TextData, StringData *>::const_iterator iter;

	for (iter = textMap_.begin(); iter != textMap_.end(); ++iter)
	{
		const TextData & data = (*iter).first;
		StringData * stringData = (*iter).second;

		std::copy(stringData->spans.begin(), stringData->spans.end(), std::back_inserter(spans));

	}	

	std::sort(spans.begin(), spans.end());
	underlineInfos_.clear();
	canclelineInfo_.clear();

	bool enable_push_cancle = false;
	bool enable_push_under = false;

	int curLineIdx = -1;
	bool newLine = false;

	UnderlineInfo underline;
	CanclelineInfo cancleline;
	LineDrawRectInfo underRectInfo;
	LineDrawRectInfo cancleRectInfo;
	underline.avglineY = 0;
	underline.avglineboder = 0;
	

	BOOST_FOREACH(const TextSpan &span, spans){
		
		if(curLineIdx + 1 == span.lineIndex){
			newLine = true;
			curLineIdx = span.lineIndex;
		}else{
			newLine = false;;
		}

		if(!span.texture->isinit() && enable_push_under)
		{
			enable_push_under = false;
			underline.avglineY = underline.avglineY/underline.spancount;
			underline.avglineboder = ceil(underline.avglineboder/underline.spancount);
			underlineInfos_.push_back(underline);
			underline.rectInfos.clear();
		}

		if(!span.texture->isinit())
			continue;
		

		if((!span.underline && enable_push_under) || (newLine && enable_push_under)){

			enable_push_under = false;
			underline.avglineY = underline.avglineY/underline.spancount;
			underline.avglineboder = ceil(underline.avglineboder/underline.spancount);
			underlineInfos_.push_back(underline);
			underline.rectInfos.clear();

		}

		if(span.underline && !enable_push_under){

			enable_push_under = true;
			underline.avglineY = 0;
			underline.avglineboder = 0;
			underline.spancount = 0;
		}

		if(span.cancleline && !enable_push_cancle){

			enable_push_cancle = true;
			cancleline.avglineY = 0;
			cancleline.avglineboder = 0;
			cancleline.spancount = 0;
		}

		if(enable_push_under){

			underline.avglineY += (span.texture->underlinePosition() + span.texture->underlineThickness());
			underline.avglineboder += span.texture->underlineThickness();
			underline.spancount ++;


			underRectInfo.color = span.color;
			underRectInfo.x = span.x;
			underRectInfo.y = span.y;
			underRectInfo.width = span.width;

			underline.rectInfos.push_back(underRectInfo);
		}
		
		if(enable_push_cancle){

			cancleline.rectInfos.clear();


			cancleline.avglineY = span.texture->underlinePosition() + (span.texture->toNextLine() + span.texture->descender()) / 2;
			cancleline.avglineboder = span.texture->underlineThickness();
			cancleline.spancount ++;


			cancleRectInfo.color = span.color;
			cancleRectInfo.x = span.x;
			cancleRectInfo.y = span.y;
			cancleRectInfo.width = span.width;

			cancleline.rectInfos.push_back(cancleRectInfo);

			canclelineInfo_.push_back(cancleline);

			enable_push_cancle = false;
		}

		
	}

	if(enable_push_under){
		underline.avglineY = underline.avglineY/underline.spancount;
		underline.avglineboder = ceil(underline.avglineboder/underline.spancount);
		underlineInfos_.push_back(underline);
		underline.rectInfos.clear();
	}
}


void Text_2::uninit()
{	
	BOOST_FOREACH(TextProperties_2 & props, properties_)
	{
		props.textHandler = 0;
	}

	map<TextData, StringData *>::iterator iter = textMap_.begin();
	for (; iter != textMap_.end(); ++iter)
	{
		delete (*iter).second;
	}
	textMap_.clear();

	map<std::string, Texture *>::iterator iter2 = imageTextureMap_.begin();
	for (; iter2 != imageTextureMap_.end(); ++iter2)
	{
		delete (*iter2).second;
	}
	imageTextureMap_.clear();

	wordWrapPoints_.clear();

	SceneObject::uninit();
}


void Text_2::drawObject(GfxRenderer * gl) const
{
	if (!parentScene_) return;

	Document * doc = parentScene_->parentDocument();
	doc->textFeatures()->registerDrawTextObj(const_cast<Text_2 *>(this));
		
	float fOpacity = totalOpacity();
	if (fOpacity == 0) return;

	if (!highlights_.empty())
	{
		for (int i = 0 ; i < highlights_.size() ; i++)
		{
			drawHighlight(gl, highlights_[i]);
		}
	}

	
	gl->useTextureProgram();
	gl->setTextureProgramOpacity(fOpacity);	

	std::vector<TextProperties_2> properties;
	properties = properties_;

	BOOST_FOREACH(TextProperties_2 & props, properties){

		if(props.isObjectProperty()){
			const ObjectProperties *pTargetProp = props.GetObjectProperty();

			if(pTargetProp->texture->isLoaded()){
				gl->use(pTargetProp->texture);
				gl->drawRect(pTargetProp->x, pTargetProp->y, pTargetProp->width,pTargetProp->height);
			}

		}
	}

	bool hasUnderline = false;
	bool hasCancleline = false;
	
	
	
	map<TextData, StringData *>::const_iterator iter;
	
	for (iter = textMap_.begin(); iter != textMap_.end(); ++iter)
	{
		const TextData & data = (*iter).first;
		StringData * stringData = (*iter).second;

		if (stringData)
		{
			data.textHandler->drawString(
				gl, stringData, data.color, fOpacity);
			
		}		
	}	

	if(underlineInfos_.size() > 0 || canclelineInfo_.size() > 0){
		gl->useColorProgram();

		BOOST_FOREACH(const UnderlineInfo &curUnder, underlineInfos_){

			BOOST_FOREACH(const LineDrawRectInfo &curUnderRect, curUnder.rectInfos){
				gl->setColorProgramColor(curUnderRect.color.r, curUnderRect.color.g, curUnderRect.color.b, curUnderRect.color.a);
				
				gl->drawRect(
					curUnderRect.x, curUnderRect.y - curUnder.avglineY, curUnderRect.width, curUnder.avglineboder);
			}
		}

		BOOST_FOREACH(const CanclelineInfo &curCancle, canclelineInfo_){

			BOOST_FOREACH(const LineDrawRectInfo &curCancleRect, curCancle.rectInfos){
				gl->setColorProgramColor(curCancleRect.color.r, curCancleRect.color.g, curCancleRect.color.b, curCancleRect.color.a);

				gl->drawRect(
					curCancleRect.x, curCancleRect.y - curCancle.avglineY, 	curCancleRect.width, curCancle.avglineboder);
			}

		}
	}	

	if (isSelectedPart_)
	{
		Highlight temp;
		temp.startIndex = charStartIndex_;
		temp.endIndex = charEndIndex_;
		temp.color = Color(235.0 / 255.0, 157.0 / 255.0, 54.0 / 255.0, 0.3f);
	
		drawHighlight(gl, temp);
	}

	if (!memos_.empty() && (memoTex_ && memoTex_->isInit()))
	{
		drawMemoIcon(gl);
	}
}


BoundingBox Text_2::extents() const
{	
	BoundingBox ret;


	BoundingBox curBb(Vector3(0, 0, 0),	Vector3(nDisplayW_, nDisplayH_, 0));

	ret = ret.unite(curBb);	

	return ret;
}


Vector2 Text_2::processCoords(const Vector2 & globalDevicePos) const
{
	const Camera * camera = parentScene_->camera();
	Ray ray = camera->unproject(globalDevicePos);
	Vector3 out;
	const_cast<Text_2 *>(this)->intersect(&out, ray);
	
	Matrix invTrans = 
		(parentTransform() * visualAttrib_.transformMatrix()).inverse();

	Vector3 invClickPt = invTrans * out;
	return Vector2(invClickPt.x, invClickPt.y);
}

bool Text_2::lineDataForChar(
	int index, float * lineY, int * olineIndex, 
	int * olineStartIndex, int * olineEndIndex) const
{
	if (index > (int)text_.size() || index < 0) return false;
	int numLines = (int)wordWrapPoints_.size();
	
	if (numLines == 0) 
		return false;
	
	int lineIndex = -1;
	float curLineY = 0;
	int i = 0;
	for (; i < numLines; ++i)
	{		
		if (wordWrapPoints_[i].wrapAtIndex > index) 
			break;

		lineIndex = i;	
		curLineY += wordWrapPoints_[i].toNextLine;
	}

	if(lineIndex == 0)
		curLineY = 0;
	else
		curLineY += ( - wordWrapPoints_[i -1].toNextLine - wordWrapPoints_[i -1].descender);

	

	int lineStartIndex = wordWrapPoints_[lineIndex].wrapAtIndex;

	//one past the end index
	int lineEndIndex = (int)text_.size();

	if (lineIndex < numLines - 1)
	{
		lineEndIndex = wordWrapPoints_[lineIndex + 1].wrapAtIndex;
	}
	
	*olineIndex = lineIndex;
	*lineY = curLineY;
	*olineStartIndex = lineStartIndex;
	*olineEndIndex = lineEndIndex;

	return true;
}

bool Text_2::lineDataForLine(int lineIndex, 
	float * olineY, float * olineWidth, float * olineHeight, 
	int * olineStartIndex, int * olineEndIndex) const
{	
	int numLines = (int)wordWrapPoints_.size();
	
	if (lineIndex >= numLines || lineIndex < 0) 
		return false;	

	float curLineY = 0;
	
	int i = 0;

	for (i = 0; i < lineIndex; ++i) 
		curLineY += wordWrapPoints_[i].toNextLine;

	//curLineY += (wordWrapPoints_[i - 1].descender);

	int lineStartIndex = wordWrapPoints_[lineIndex].wrapAtIndex;

	//one past the end index
	int lineEndIndex = (int)text_.size();

	if (lineIndex < numLines - 1)
	{
		lineEndIndex = wordWrapPoints_[lineIndex + 1].wrapAtIndex;
	}

	//if(lineIndex == 0)
		*olineY = curLineY;
// 	else
// 		*olineY = curLineY - wordWrapPoints_[lineIndex].descender;

	*olineWidth = wordWrapPoints_[lineIndex].wrapWidth;
	*olineHeight = wordWrapPoints_[lineIndex].toNextLine;
	*olineStartIndex = lineStartIndex;
	*olineEndIndex = lineEndIndex;

	return true;
}

bool Text_2::posAtIndex(int index, Vector2 * posTop, float * posHeight, float * posWidth) const
{
	float curLineY;
	int lineIndex, lineStartIndex, lineEndIndex;
	if (!lineDataForChar(index, &curLineY, &lineIndex, &lineStartIndex, &lineEndIndex)) 
	{
		*posWidth = 0;
		*posHeight = wordWrapPoints_[0].toNextLine;
		*posTop = Vector2(0.0f, 0.0f);
		return true;
	}

	int curPropIndex = startingPropertyIndexForLine(lineIndex);
	
	float curWidth = 0;
	int nextPropCharIndex = lineStartIndex;
	--curPropIndex;

	const LineStyle lineStyle = getLineStyleByCharIndex(lineStartIndex);
	const WordWrapPoint_2 curLine = getLineByIndex(lineIndex);
	
	curWidth = lineStyle.startPosX;

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
		const CharData * d =
		properties_[curPropIndex].textHandler->charData(c);

		bool isIgnored = c == L'\r' || c == L'\b' || c == L'\n';
		bool isTap = c == L'\t';

		if (!d) continue;

		TextProperties_2 *curProp = (TextProperties_2 *)&properties_[curPropIndex];

		if(curProp->isObjectProperty()){
			CharData *pObjChar = (CharData *)d;
			pObjChar->advanceX = curProp->GetObjectProperty()->width;
		}

		if(isTap){

			int nTabCount = 0;
			double nModule = 0;
			CharData *pObjChar = (CharData *)d;

			nTabCount = (curWidth) / TAB_SIZE;
			nModule = fmod((double)curWidth ,TAB_SIZE);

			if(nModule >= 0.0)
				nTabCount ++;

			pObjChar->width = (nTabCount * TAB_SIZE) - curWidth;
			pObjChar->advanceX = d->width;

		}

		float linelen = curLine.wrapWidth;//텍스트 라인 길이
		int justifyLineEndIndex = lineEndIndex;

		float justifySpacing = (nDisplayW_ - linelen) / (justifyLineEndIndex - lineStartIndex - 1);

		int linestyleEndIndex = text_.size();

		if(lineStyle.index_ != linesStyle_[linesStyle_.size()-1].index_)
			linestyleEndIndex = linesStyle_[lineStyle.index_+1].charIndex;

		bool JustifyCheck = lineStyle.textAlign_ == LineStyle::Justify_Left;

		if (i == index){
			if(isTap || curProp->isObjectProperty())
			{
				if(JustifyCheck && lineEndIndex != linestyleEndIndex)
				{
					*posWidth = d->advanceX + letterSpacing_ + justifySpacing;
				}
				else
					*posWidth = d->advanceX + letterSpacing_;
			}
			else
			{
				if(JustifyCheck && lineEndIndex != linestyleEndIndex)
				{
					*posWidth = d->advanceX * curProp->letterwidthratio_ / 100.0f + letterSpacing_ + justifySpacing;
				}
				else
					*posWidth = d->advanceX * curProp->letterwidthratio_ / 100.0f + letterSpacing_;
			}
		}
		else if (!isIgnored){
			if(isTap || curProp->isObjectProperty())
			{
				if(JustifyCheck && lineEndIndex != linestyleEndIndex)
				{
					curWidth += d->advanceX + letterSpacing_ + justifySpacing;
				}
				else
					curWidth += d->advanceX + letterSpacing_;
			}
			else
			{
				if(JustifyCheck && lineEndIndex != linestyleEndIndex)
				{
					curWidth += d->advanceX * curProp->letterwidthratio_ / 100.0f + letterSpacing_ + justifySpacing;
				}
				else
					curWidth += d->advanceX * curProp->letterwidthratio_ / 100.0f + letterSpacing_;
			}
		}
	}


	if(lineStyle.textAlign_ == LineStyle::Center)
	{
		if(direction_ == DRight){
			if(lineStyle.index_ == linesStyle_.size() - 1)
				posTop->x = (nDisplayW_ - curLine.wrapWidth)/2 + curWidth;
			else 
				posTop->x = (nDisplayW_ - curLine.wrapWidth + ENTER_SIZE)/2 + curWidth;
		}else{
			if(lineStyle.index_ == linesStyle_.size() - 1)
				posTop->x = (nDisplayW_ + curLine.wrapWidth)/2 - curWidth;
			else 
				posTop->x = (nDisplayW_ + curLine.wrapWidth - ENTER_SIZE)/2 - curWidth;
		}
		

	}
	else if (lineStyle.textAlign_ == LineStyle::Right)
	{
		if(direction_ == DRight){
			if(lineStyle.index_ == linesStyle_.size() - 1)
				posTop->x = nDisplayW_ - curLine.wrapWidth + curWidth;
			else 
				posTop->x = nDisplayW_ - curLine.wrapWidth + ENTER_SIZE + curWidth;
		}else{
				posTop->x = nDisplayW_ - curWidth;
		}

	}
	else	// textAlign_ == Left - default
	{

		if(direction_ == DRight){
			posTop->x = curWidth;
		}else{
			if(lineStyle.index_ == linesStyle_.size() - 1)
				posTop->x = curLine.wrapWidth - curWidth;
			else 
				posTop->x = curLine.wrapWidth - ENTER_SIZE - curWidth;
		}

		
	}
	
	if(lineIndex == 0)
		posTop->y = curLineY;
	else
		posTop->y = curLineY + wordWrapPoints_[lineIndex].descender;

	*posHeight = wordWrapPoints_[lineIndex].toNextLine;

	return true;
}

bool Text_2::lineDataForPos(
	const Vector2 & localPos, int * olineY, int * olineIndex, 
	int * olineStartIndex, int * olineEndIndex) const
{
	int numLines = (int)wordWrapPoints_.size();

	int curLineY = 0;
	int lineIndex = 0;
	
	if (localPos.y >= 0)
	{
		for (int i = 0; i < numLines; ++i)
		{
			int lineBottom = curLineY + wordWrapPoints_[i].toNextLine;
			bool isLastLine = i == numLines - 1;
			bool isPosLine = curLineY <= localPos.y && localPos.y < lineBottom;
			if (isLastLine || isPosLine)
			{
				lineIndex = i;
				break;
			}			
			curLineY += wordWrapPoints_[i].toNextLine;
		}	
	}
	
	int lineStartIndex = 0;
	if (wordWrapPoints_.size())
		lineStartIndex = wordWrapPoints_[lineIndex].wrapAtIndex;		

	//one past the end index
	int lineEndIndex = (int)text_.size();

	if (lineIndex < numLines - 1)
	{
		lineEndIndex = wordWrapPoints_[lineIndex + 1].wrapAtIndex;
	}

	if(lineIndex == 0)
		*olineY = curLineY;
	else
		*olineY = curLineY + wordWrapPoints_[lineIndex].descender;
	
	*olineIndex = lineIndex;
	*olineStartIndex = lineStartIndex;
	*olineEndIndex = lineEndIndex;


	return true;
}

int Text_2::startingPropertyIndexForLine(int lineIndex) const
{
	int curPropIndex = 0;
	if (lineIndex < 0 || lineIndex >= (int)wordWrapPoints_.size()) return -1;

	int lineStartIndex = wordWrapPoints_[lineIndex].wrapAtIndex;
	while(1)
	{
		if (curPropIndex >= (int)properties_.size() - 1) break;
		if (properties_[curPropIndex + 1].index > lineStartIndex)
			break;
		++curPropIndex;
	}

	return curPropIndex;
}

int Text_2::indexAtPos(const Vector2 & localPos, int * propertyIndex) const
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

	float curWidth = 0;
	float prevCurWidth = 0.0;
	int nextPropCharIndex = lineStartIndex;

	const WordWrapPoint_2 curLine = wordWrapPoints_[lineIndex];
	
	--curPropIndex;

	const LineStyle lineStyle = getLineStyleByCharIndex(lineStartIndex);
	
	float linelen = curLine.wrapWidth;
	
	int i = lineStartIndex;

	curWidth = curLine.nStartx;
	float baseX = 0.0;

	if(lineStyle.textAlign_ == LineStyle::Center){
		baseX = nDisplayW_ / 2 - curLine.wrapWidth / 2;
	}else if(lineStyle.textAlign_ == LineStyle::Right){
		baseX = nDisplayW_ - curLine.wrapWidth;
	}
	
	int justifyLineEndIndex = lineEndIndex;
	
	float justifySpacing = (nDisplayW_ - linelen) / (justifyLineEndIndex - lineStartIndex - 1);

	int linestyleEndIndex = text_.size();

	if(lineStyle.index_ != linesStyle_[linesStyle_.size()-1].index_)
		linestyleEndIndex = linesStyle_[lineStyle.index_+1].charIndex;

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
		const CharData * d =
		properties_[curPropIndex].textHandler->charData(c);

		bool isIgnored = c == L'\r' || c == L'\b' || c == L'\n';
		bool isTap = c == L'\t';

		if (!d) continue;

		TextProperties_2 *curProp = (TextProperties_2 *)&properties_[curPropIndex];

		if(curProp->isObjectProperty()){
			CharData *pObjChar = (CharData *)d;
			pObjChar->advanceX = curProp->GetObjectProperty()->width;
		}

		if(isTap){

			int nTabCount = 0;
			double nModule = 0;
			CharData *pObjChar = (CharData *)d;

			nTabCount = (curWidth) / TAB_SIZE;
			nModule = fmod((double)curWidth ,TAB_SIZE);

			if(nModule >= 0.0)
				nTabCount ++;

			pObjChar->width = (nTabCount * TAB_SIZE) - curWidth;
			pObjChar->advanceX = d->width;

		}

		prevCurWidth = curWidth;

		bool JustifyCheck = lineStyle.textAlign_ == LineStyle::Justify_Left;

		if (!isIgnored){
			if(isTap || curProp->isObjectProperty())
			{
				if(JustifyCheck && lineEndIndex != linestyleEndIndex)
				{
					curWidth += (d->advanceX + letterSpacing_ + justifySpacing);
				}
				else 
					curWidth += (d->advanceX + letterSpacing_);
			
			}
			else
			{
				if(JustifyCheck && lineEndIndex != linestyleEndIndex)
				{
					curWidth += (d->advanceX * curProp->letterwidthratio_ / 100.0f + letterSpacing_ + justifySpacing);
				}
				else 
					curWidth += (d->advanceX * curProp->letterwidthratio_ / 100.0f + letterSpacing_);

			}
		}

		*propertyIndex = curPropIndex;

		float nEnterSize = ENTER_SIZE;

		if(lineStyle.index_ == linesStyle_.size() - 1)
			nEnterSize = 0;

		if(lineStyle.textAlign_ == LineStyle::Center)
		{
			if(direction_ == DRight){
				if( (nDisplayW_ - (linelen - nEnterSize))/2 + curWidth > localPos.x) {

					if(abs((nDisplayW_ - (linelen - nEnterSize))/2 + prevCurWidth - localPos.x) > abs((nDisplayW_ - (linelen - nEnterSize))/2 + curWidth - localPos.x))
						i ++;

					break;
				}
			}else{
				if( (nDisplayW_ - linelen)/2 + curWidth > nDisplayW_ - localPos.x) break;
			}


		}
		else if (lineStyle.textAlign_ == LineStyle::Right)
		{
			if(direction_ == DRight){
				if(nDisplayW_ - linelen + curWidth > localPos.x) break;
			}else{
				if(curWidth > nDisplayW_ -localPos.x) break;
			}

		}
		else	// textAlign_ == Left - default
		{

			if(direction_ == DRight){
				if (curWidth - nEnterSize > localPos.x){

					if(abs(prevCurWidth - nEnterSize - localPos.x) > abs(curWidth - nEnterSize - localPos.x))
						i ++;

					break;
				}
			}else{
				if (curWidth > nDisplayW_ - localPos.x){
					break;
				}
			}
		}
	}

	

	return i;
}

Vector2 Text_2::toLocalPos(const Vector2 & pos)
{
	const Camera * cam = parentScene_->camera();
	Ray mouseRay = cam->unproject(parentScene_->screenToDeviceCoords(pos));
	Vector3 out;
	intersect(&out, mouseRay);
	Matrix invMatrix = (parentTransform() * visualAttrib_.transformMatrix()).inverse();
	out = invMatrix * out;

	return Vector2(out.x, out.y);
}

bool Text_2::pressEvent(const Vector2 & startPos, int pressId)
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

bool Text_2::clickEvent(const Vector2 & mousePos, int pressId)
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


void Text_2::write(Writer & writer) const
{
	writer.writeParent<SceneObject>(this);
	writer.write(visualAttrib_, "visualAttrib");
	
	writer.write(text_, "text");

	writer.write(properties_);

	writer.write(lineSpacingMode_);
	writer.write(lineSpacing_);

	writer.write(boundaryWidth_, "width");
	writer.write(boundaryHeight_, "height");

	writer.write(letterSpacing_, "letterSpacing");

	writer.write(canSelectPart_, "canSelectPart");

	writer.write(direction_ , "TextDirection");

	writer.write(linesStyle_.size(), "Line Style Length");

	BOOST_FOREACH(const LineStyle &curLineStyle, linesStyle_)
	{
		writer.write(curLineStyle.index_, "Line Style index");
		writer.write(curLineStyle.tabIndex_, "Line Style Tab index");
		
		writer.write(curLineStyle.textAlign_, "Line Align type");

		writer.write(curLineStyle.padding_.left, "Line Style Padding left");
		writer.write(curLineStyle.padding_.top, "Line Style Padding top");
		writer.write(curLineStyle.padding_.right, "Line Style Padding right");
		writer.write(curLineStyle.padding_.bottom, "Line Style Padding bottom");

		
		writer.write(curLineStyle.lineIncreasement_.type, "Line Style type");
		if(curLineStyle.lineIncreasement_.type != NONE){
			writer.write(curLineStyle.lineIncreasement_.index, "Line Increasement index");
			writer.write(curLineStyle.lineIncreasement_.depth, "Line Increasement depth");
		}
	}
}

void Text_2::writeXml(XmlWriter & w) const
{
	SceneObject::writeXml(w);
	w.startTag("VisualAttrib");	
	visualAttrib_.writeXml(w);
	w.endTag();

	std::string utf_;
	utf8::utf16to8(text_.begin(), text_.end(), back_inserter(utf_));
	w.writeTag("TextString", utf_);
	w.startTag("Properties");
	for (unsigned int i = 0; i < (unsigned int)properties_.size(); ++i)
	{
		w.startTag("TextProperties_2");
		properties_[i].writeXml(w);
		w.endTag();
	}
	w.endTag();
	int temp = lineSpacingMode_;
	w.writeTag("LineSpacingMode", temp);
	w.writeTag("LineSpacing", lineSpacing_);

	w.writeTag("Width", boundaryWidth_);
	w.writeTag("Height", boundaryHeight_);
	w.writeTag("LetterSpacing", letterSpacing_);
	w.writeTag("SelectPart", canSelectPart_);
	w.writeTag("TextDirection", (int)direction_);

	w.startTag("LineStyles");

	BOOST_FOREACH(const LineStyle &curLineStyle, linesStyle_){
		std::stringstream lineAttr;
		std::stringstream paddingAttr;
		
		lineAttr.str("");
		lineAttr << "index='" << curLineStyle.index_<< "' ";
		lineAttr << "tabIndex='" << curLineStyle.tabIndex_<< "' ";

		if(curLineStyle.lineIncreasement_.type == NONE)
			lineAttr << "lineType='" << "NORMAL" << "' ";
		else
			lineAttr << "lineType='" << "INCREASEMENT"<< "' ";
		
		if(curLineStyle.textAlign_ == LineStyle::Left)
			lineAttr << "lineAlign='" << "Left"<< "' ";
		else if(curLineStyle.textAlign_ == LineStyle::Center)
			lineAttr << "lineAlign='" << "Center"<< "' ";
		else if(curLineStyle.textAlign_ == LineStyle::Right)
			lineAttr << "lineAlign='" << "Right"<< "' ";
		else if(curLineStyle.textAlign_ == LineStyle::Justify_Left)
			lineAttr << "lineAlign='" << "Justify_Left"<< "' ";
		w.startTag("Line",lineAttr.str());


		paddingAttr.str("");
		paddingAttr << curLineStyle.padding_.left << "," << curLineStyle.padding_.top << "," << curLineStyle.padding_.right << "," << curLineStyle.padding_.bottom;
		
		w.writeTag("Padding",paddingAttr.str());


		if(curLineStyle.lineIncreasement_.type != NONE){

			w.startTag("LineIncreasementStyle");		
		
			w.writeTag("index", curLineStyle.lineIncreasement_.index);
			w.writeTag("depth", curLineStyle.lineIncreasement_.depth);

			wchar_t wc = curLineStyle.lineIncreasement_.type;
			std::wstring wstr(&wc , 1);
			std::string str = UTF16toUTF8(wstr);
			w.writeTag("type", str);

			w.endTag();
		}
		w.endTag();
	}

	w.endTag();
}

void Text_2::readXml(XmlReader & r, xmlNode * parent)
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
			//while (r.isNodeName(propertiesNode, "TextProperties_2"))
			for(; propertiesNode ; propertiesNode = propertiesNode->next)
			{
				if (propertiesNode->type != XML_ELEMENT_NODE) continue;
				TextProperties_2 p;
				p.readXml(r, propertiesNode);
				properties_.push_back(p);
			}
			curNode = curNode->next;
		}
		else if (r.getNodeContentIfName(temp, curNode, "LineSpacingMode")) lineSpacingMode_ = (LineSpacingMode)temp;
		else if (r.getNodeContentIfName(lineSpacing_, curNode, "LineSpacing"));
		else if (r.getNodeContentIfName(boundaryWidth_, curNode, "Width"));
		else if (r.getNodeContentIfName(boundaryHeight_, curNode, "Height"));
		else if (r.getNodeContentIfName(letterSpacing_, curNode, "LetterSpacing"));
		else if (r.getNodeContentIfName(canSelectPart_, curNode, "SelectPart"));
		else if (r.getNodeContentIfName(temp, curNode, "TextDirection")){
			direction_ = (TextDirection)temp;
		}
		else if (r.isNodeName(curNode, "LineStyles"))
		{
			xmlNode * lineNode = curNode->children;
			linesStyle_.clear();
			//while (r.isNodeName(propertiesNode, "TextProperties_2"))
			for(; lineNode ; lineNode = lineNode->next)
			{
				if (lineNode->type != XML_ELEMENT_NODE) continue;

				LineStyle line;
				std::string strLineAlign;
				std::string strLineType;
				r.getNodeAttribute(strLineAlign, lineNode, "lineAlign");
				r.getNodeAttribute(strLineType, lineNode, "lineType");
				r.getNodeAttribute(line.index_, lineNode, "index");
				r.getNodeAttribute(line.tabIndex_, lineNode, "tabIndex");

				if(strLineAlign == "Left")
					line.textAlign_ == LineStyle::Left;
				else if(strLineAlign == "Right")
					line.textAlign_ == LineStyle::Right;
				else if(strLineAlign == "Center")
					line.textAlign_ == LineStyle::Center;
				else if(strLineAlign == "Justify_Left")
					line.textAlign_ == LineStyle::Justify_Left;


				if(strLineType == "NORMAL")
					line.lineIncreasement_.type = NONE;
				else{

					xmlNode * lineIncreasementStyleNode = lineNode->children;

					for(; lineIncreasementStyleNode ; lineIncreasementStyleNode = lineIncreasementStyleNode->next){

						if (lineIncreasementStyleNode->type != XML_ELEMENT_NODE) continue;


					}
				}

				// 작업중

				line.padding_.bottom = 0;
				line.padding_.top = 0;
				line.padding_.left = 0;
				line.padding_.right = 0;
				line.letterSpacing_ = 0;

				linesStyle_.push_back(line);
				
				
			}
			curNode = curNode->next;
		}
	}
}


void Text_2::read(Reader & reader, unsigned char version)
{
	reader.readParent<SceneObject>(this);
	reader.read(visualAttrib_);
	reader.read(text_);
		
	properties_.clear();
	reader.read(properties_);
	reader.read(lineSpacingMode_);
	reader.read(lineSpacing_);
	
	reader.read(boundaryWidth_);
	reader.read(boundaryHeight_);
	reader.read(letterSpacing_);
	reader.read(canSelectPart_);
	
	reader.read(direction_);
	
	int lineNumber = 0;
	reader.read(lineNumber);

	linesStyle_.clear();
	for(int i = 0; i < lineNumber; i ++){
		LineStyle curLineStyle;

		reader.read(curLineStyle.index_);
		reader.read(curLineStyle.tabIndex_);

		reader.read(curLineStyle.textAlign_);

		reader.read(curLineStyle.padding_.left);
		reader.read(curLineStyle.padding_.top);
		reader.read(curLineStyle.padding_.right);
		reader.read(curLineStyle.padding_.bottom);

		reader.read(curLineStyle.lineIncreasement_.type);

		if(curLineStyle.lineIncreasement_.type != NONE){
			reader.read(curLineStyle.lineIncreasement_.index);
			reader.read(curLineStyle.lineIncreasement_.depth);
		}

		linesStyle_.push_back(curLineStyle);
	}
}

SceneObject * Text_2::intersect(Vector3 * out, const Ray & ray)
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

void Text_2::setLetterSpacing(const float & letterSpacing)
{
	float prevLetterSpacing = letterSpacing_;
	letterSpacing_ = letterSpacing;
	
	if (isInit() && prevLetterSpacing != letterSpacing_)
	{
		init(parentScene_->parentDocument()->renderer(), parentScene_, false);
	}
}

const float & Text_2::letterSpacing() const
{
	return letterSpacing_;
}

bool Text_2::lineLength(int lineIndex, float * lineLength) const
{
	lineIndex -= 1;
		int numLines = (int)wordWrapPoints_.size();
	if (lineIndex >= numLines || lineIndex < 0) return false;	

	*lineLength = wordWrapPoints_[lineIndex].wrapWidth;

	return true;

	int lineStartIndex = wordWrapPoints_[lineIndex].wrapAtIndex;

	const LineStyle lineStyle = getLineStyleByCharIndex(lineStartIndex);

	//one past the end index
	int lineEndIndex = (int)text_.size();
	
	if (lineIndex < numLines - 1)
	{
		lineEndIndex = wordWrapPoints_[lineIndex + 1].wrapAtIndex;
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
		const CharData * d =
		properties_[curPropIndex].textHandler->charData(c);

		bool isIgnored = c == L'\r' || c == L'\b' || c == L'\n';

		if (!d) continue;

		TextProperties_2 *curProp = (TextProperties_2 *)&properties_[curPropIndex];

		if(curProp->isObjectProperty()){
			CharData *pObjChar = (CharData *)d;
			pObjChar->advanceX = curProp->GetObjectProperty()->width;
		}

		if(c == L'\t'){

			int nTabCount = 0;
			double nModule = 0;
			CharData *pObjChar = (CharData *)d;

			nTabCount = (linelen / TAB_SIZE);
			nModule = fmod((double)linelen ,TAB_SIZE);

			if(nModule >= 0.0)
				nTabCount ++;

			pObjChar->width = (nTabCount * TAB_SIZE) - linelen;
			pObjChar->advanceX = d->width;

		}

		float linelenW = wordWrapPoints_[lineIndex].wrapWidth;//텍스트 라인 길이
		
		int justifyLineEndIndex = lineEndIndex;

		/*if(lineEndIndex != text_.size())
			justifyLineEndIndex -= 1;*/

		float justifySpacing = (nDisplayW_ - linelenW) / (justifyLineEndIndex - lineStartIndex - 1);
		

		int linestyleEndIndex = text_.size();

		if(lineStyle.index_ != linesStyle_[linesStyle_.size()-1].index_)
			linestyleEndIndex = linesStyle_[lineStyle.index_+1].charIndex;

		bool JustifyCheck = lineStyle.textAlign_ == LineStyle::Justify_Left;

		if (!isIgnored)
		{
			if(JustifyCheck && lineEndIndex != linestyleEndIndex)
			{
				linelen += d->advanceX + letterSpacing_ + justifySpacing;
			}
			else
				linelen += d->advanceX + letterSpacing_;
		}
	}

	*lineLength = linelen;

	return true;
}

void Text_2::selectPart(int charIndex, int charEndIndex)
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

void Text_2::selectPart(int & startIndex, int & endIndex, int charIndex)
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

void Text_2::addHighlight(const Color & color)
{
	if (!isSelectedPart_) return;
	Highlight temp;
	temp.startIndex = charStartIndex_;
	temp.endIndex = charEndIndex_;
	temp.color = color;

	removeHighlight(charStartIndex_, charEndIndex_);
	highlights_.push_back(temp);
}

void Text_2::addHighlight(const Highlight & highlight)
{
	removeHighlight(highlight.startIndex, highlight.endIndex);
	highlights_.push_back(highlight);
}

void Text_2::removeHighlight(unsigned int startIndex, unsigned int endIndex)
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

void Text_2::drawHighlight(GfxRenderer * gl, const Highlight & highlight) const
{
	Color color = highlight.color;
	gl->useColorProgram();
	gl->setColorProgramColor(color.r, color.g, color.b, color.a);
	//gl->setColorProgramColor(0.5f, 0.5f, 1.0f, 1.0f);

	float lineY0, lineY1;

	int lineIndex0, lineStartIndex0, lineEndIndex0;
	lineDataForChar(highlight.startIndex, &lineY0, &lineIndex0, &lineStartIndex0, &lineEndIndex0);
	//lineDataForChar(charStartIndex_, &lineY0, &lineIndex0, &lineStartIndex0, &lineEndIndex0);
	int lineIndex1, lineStartIndex1, lineEndIndex1;
	lineDataForChar(highlight.endIndex, &lineY1, &lineIndex1, &lineStartIndex1, &lineEndIndex1);
	//lineDataForChar(charEndIndex_, &lineY1, &lineIndex1, &lineStartIndex1, &lineEndIndex1);

	Vector2 posTop0; float posHeight0, posWidth0;
	posAtIndex(highlight.startIndex, &posTop0, &posHeight0, &posWidth0);
	//posAtIndex(charStartIndex_, &posTop0, &posHeight0, &posWidth0);
	Vector2 posTop1; float posHeight1, posWidth1;
	posAtIndex(highlight.endIndex, &posTop1, &posHeight1, &posWidth1);
	//posAtIndex(charEndIndex_, &posTop1, &posHeight1, &posWidth1);

	if (lineIndex0 == lineIndex1)
	{
		gl->drawRect(posTop0.x, posTop0.y, posTop1.x - posTop0.x, posHeight0);
	}
	else
	{
		int lineStartIndexP, lineEndIndexP;
		float lineYP, lineWidthP, lineHeightP;

		float linelen = 0.0;


		float drawX = 0.0;
		float drawY = 0.0;
		float width = 0.0;				
		float height = 0.0;

		for (int i = lineIndex0 ; i <= lineIndex1; ++i){
			float moveX = 0.0;
			const WordWrapPoint_2 line = getLineByIndex(i);
			const LineStyle lineStyle = getLineStyleByCharIndex(line.wrapAtIndex);

			lineDataForLine(i, &lineYP, &lineWidthP, &lineHeightP, &lineStartIndexP, &lineEndIndexP);

			if(textDirection() == Text_2::DRight)
            {
				drawX = line.nStartx;
#ifdef IOS
                sleep(0.0001);
#endif
            }
			else
				drawX = nDisplayW_ - line.nStartx;

			drawY = lineYP;
			height = lineHeightP;

			if(textDirection() == Text_2::DRight){
				if(lineStyle.textAlign_ == LineStyle::Center){
					moveX = (float)(nDisplayW_ / 2.0 - (float)lineWidthP / 2.0);
				}else if(lineStyle.textAlign_ == LineStyle::Right){
					moveX =  (float)(nDisplayW_ - (float)lineWidthP + ENTER_SIZE);
				}
			}else{
				if(lineStyle.textAlign_ == LineStyle::Center){
					moveX = - ((float)(nDisplayW_ / 2.0 - (float)lineWidthP / 2.0));
				}else if(lineStyle.textAlign_ == LineStyle::Right){
					moveX = - ENTER_SIZE;

				}
			}

			drawX += moveX;

			float justifySpacing = 0; 

			if(lineStyle.textAlign_ == LineStyle::Justify_Left)
				justifySpacing = ENTER_SIZE;

			if(i == lineIndex0){
				drawX = posTop0.x;
			}

			if(i == lineIndex0){
				if(textDirection() == Text_2::DRight)
				{
					width = moveX + lineWidthP - posTop0.x + justifySpacing;
					if(lineStyle.textAlign_ == LineStyle::Justify_Left)
					{
						const LineStyle nextlineStyle = getLineStyleByCharIndex(lineEndIndexP+1);
							
						if(nextlineStyle.index_ == lineStyle.index_)
							width += nDisplayW_ - lineWidthP;
					}
				}
				else
					width =  - posTop0.x;


			}
			else if(i == lineIndex1){
				width = posTop1.x - drawX;

			}else{
				if(textDirection() == Text_2::DRight)
				{
					width = (float)lineWidthP - line.nStartx + justifySpacing;

					if(lineStyle.textAlign_ == LineStyle::Justify_Left)
					{
						const LineStyle nextlineStyle = getLineStyleByCharIndex(lineEndIndexP+1);
							
						if(nextlineStyle.index_ == lineStyle.index_)
							width += nDisplayW_ - lineWidthP;
					}
				}
				else
				{
					drawX -= ENTER_SIZE;
					width = -((float)lineWidthP - line.nStartx - ENTER_SIZE);
				}

			}



			gl->drawRect(drawX, drawY, width, height);

		}
	}
}

Vector2 Text_2::selectedPartPos()
{
	Vector2 posTop; float posHeight, posWidth;
	posAtIndex(charStartIndex_, &posTop, &posHeight, &posWidth);

	Transform t;
	Matrix m = parentTransform(); 
	m *= visualAttrib_.transformMatrix();
	t.setMatrix(m, true);
	posTop.x += t.translation().x;
	posTop.y += t.translation().y;

	return posTop;
}

std::wstring Text_2::textPartString(unsigned startIndex, unsigned endIndex)
{
	return text_.substr(startIndex, endIndex - startIndex);
}


void Text_2::search(std::vector<unsigned> & foundIndices, const std::wstring & searchStr) const
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

//ensure there is a TextProperties_2 at the startIndex, endIndex borders	
void Text_2::makeBorderProperties(unsigned startIndex, unsigned endIndex)
{
	vector<TextProperties_2>::iterator itr;
	
	for (itr = properties_.begin(); itr != properties_.end(); ++itr)
	{
		TextProperties_2 * curProps = &*itr;		
		unsigned curPropsEndIndex = text_.size();
		vector<TextProperties_2>::iterator itrNext = itr + 1;
		if (itrNext != properties_.end()) curPropsEndIndex = (*itrNext).index;
				
		if (curProps->index < startIndex && startIndex < curPropsEndIndex)
		{			
			TextProperties_2 newProps = *curProps;
			itr = properties_.insert(itr, newProps);
			++itr;
			(*itr).index = startIndex;
			curProps = &*itr;
		}

		if (curProps->index < endIndex && endIndex < curPropsEndIndex)
		{
			TextProperties_2 newProps = *curProps;
			itr = properties_.insert(itr, newProps);
			++itr;
			(*itr).index = endIndex;
			curProps = &*itr;
		}
	}

	if (endIndex == text_.size())
	{
		if(text_.size() == 0 || text_.at(endIndex - 1) == IMAGE_OBJECT_STRING){
			TextProperties_2 newProps(properties_.back(), false);
			newProps.index = endIndex;
			properties_.push_back(newProps);
		}else{
			TextProperties_2 newProps = properties_.back();
			newProps.index = endIndex;
			properties_.push_back(newProps);
		}
	}
}

void Text_2::applyProperties(const std::vector<TextProperties_2> & localIndexProperties, unsigned startIndex, unsigned endIndex)
{
	if (localIndexProperties.empty()) return;
	if (endIndex <= startIndex) return;
	
	vector<TextProperties_2>::iterator itr;

	makeBorderProperties(startIndex, endIndex);
		
	for (itr = properties_.begin(); itr != properties_.end();)
	{
		TextProperties_2 & curProps = *itr;

		if (startIndex <= curProps.index && curProps.index < endIndex)
			itr = properties_.erase(itr);
		else 
			++itr;
	}

	for (itr = properties_.begin(); itr != properties_.end(); ++itr)
	{
		TextProperties_2 & curProps = *itr;
		if (curProps.index > startIndex) break;
	}

	BOOST_FOREACH(TextProperties_2 props, localIndexProperties)
	{
		if (props.index >= endIndex) break;
		props.index += startIndex;
		itr = ++properties_.insert(itr, props);
	}

	mergeProperties();
}


void Text_2::applyProperties(const TextProperties_2 & newProps, TextProperties_2::Type types, unsigned charLen)
{
	//check bounds and correct charLen if necessary
	if (newProps.index + charLen > text_.size()) charLen = text_.size() - newProps.index;
	if (charLen == 0) return;	
	if (newProps.index > text_.size() - 1) return;

	
	//compute startIndex and endIndex for newProps
	unsigned startIndex = newProps.index;
	unsigned endIndex = newProps.index + charLen;	

	makeBorderProperties(startIndex, endIndex);

	vector<TextProperties_2>::iterator itr;

	//Appropriately modify the existing TextProperties_2 that fall within startIndex and endIndex	
	for (itr = properties_.begin(); itr != properties_.end(); ++itr)
	{	
		TextProperties_2 & curProps = *itr;		
		unsigned curPropsEndIndex = text_.size();
		vector<TextProperties_2>::iterator itrNext = itr + 1;
		if (itrNext != properties_.end()) curPropsEndIndex = (*itrNext).index;

		if (startIndex <= curProps.index && curPropsEndIndex <= endIndex)
		{
			if (types & TextProperties_2::PropertyColor) 
				curProps.color = newProps.color;

			if (types & TextProperties_2::PropertyUnderLine)
				curProps.underline_ = newProps.underline_;

			if (types & TextProperties_2::PropertyCancleLine)
				curProps.cancleline_ = newProps.cancleline_;

			if (types & TextProperties_2::PropertyLetterWidthRatio)
				curProps.letterwidthratio_ = newProps.letterwidthratio_;

			if (types & TextProperties_2::PropertyFont) 
				curProps.font = newProps.font;

			if (types & TextProperties_2::PropertyFontSize) 
				curProps.font.pointSize = newProps.font.pointSize;

			if (types & TextProperties_2::PropertyFontFileName)
				curProps.font.fontFile = newProps.font.fontFile;

			if (types & TextProperties_2::PropertyFontShandow)
				curProps.font.shadow = newProps.font.shadow;

			
		}
	}

	mergeProperties();
}




void Text_2::mergeProperties()
{
	vector<TextProperties_2>::iterator itr;
	for (itr = properties_.begin(); itr != properties_.end();)
	{		
		vector<TextProperties_2>::iterator itrNext = itr + 1;
		if (itrNext == properties_.end()) break;

		TextProperties_2 & props = *itr;
		TextProperties_2 & propsNext = *itrNext;

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
			TextProperties_2 & props = *itr;
			if (props.index >= text_.size())
				itr = properties_.erase(itr);
			else
				++itr;
		}
	}	
}

void Text_2::getProperties(std::vector<TextProperties_2> & properties, unsigned int startIndex, unsigned int endIndex, bool localIndices) const
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

void Text_2::getProperties(unsigned & propStartIndex, unsigned & propEndIndex, unsigned startIndex, unsigned endIndex) const
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


void Text_2::insertText(const std::wstring & text, const std::vector<TextProperties_2> & props, unsigned index)
{
	unsigned len = text.size();
	
	makeBorderProperties(index, index);
	unsigned prop0, prop1;
	getProperties(prop0, prop1, index, index);

	for (unsigned i = prop0; i < (unsigned)properties_.size(); ++i) 
		properties_[i].index += len;
	
	text_.insert(index, text);

	applyProperties(props, index, index + text.size());
}

void Text_2::removeText(unsigned startIndex, unsigned endIndex)
{
	if (endIndex <= startIndex) return;	
	unsigned len = endIndex - startIndex;
	
	TextProperties_2 firstProp = properties_.front();

	makeBorderProperties(startIndex, endIndex);
	unsigned prop0, prop1;
	getProperties(prop0, prop1, startIndex, endIndex);
	
	vector<TextProperties_2>::iterator itr;
	itr = properties_.begin() + prop0;
	for (int i = prop0; i <= prop1; ++i) itr = properties_.erase(itr);
	
	for (; itr != properties_.end(); ++itr)
	{
		TextProperties_2 & props = *itr;
		props.index -= len;
	}

	text_.erase(startIndex, len);

	if (properties_.empty()) {
		firstProp.ResetObjectProperties();
		properties_.push_back(firstProp);
	}
}

void Text_2::addLineStyle(int position){
	LineStyle *lineStyle = &linesStyle_[position];

	LineStyle addlineStyle(lineStyle);

	addLineStyle(addlineStyle, position);
}

void Text_2::delLineStyle(int  nStart , int nEnd){

	int i = 0;
	bool bFind = false;

	std::vector<LineStyle>::iterator itr;

	itr = linesStyle_.begin();

	BOOST_FOREACH(const LineStyle &lineStyle, linesStyle_){

		if(i == nStart){

			bFind = true;

			for(i = nStart; i <=nEnd; i ++){
				
				itr = linesStyle_.erase(itr);
			}

			break;
		}

		i ++;
		itr ++;
	}

	i = 0;

	BOOST_FOREACH(LineStyle &lineStyle, linesStyle_){

		if(i >= nStart){

			lineStyle.index_ = i;
		}

		i ++;

	}

}

void Text_2:: addLineStyle(LineStyle &lineStyle, int position){

	int i = 0;
	bool bFind = false;
	
	std::vector<LineStyle>::iterator itr;

	itr = linesStyle_.begin();

	BOOST_FOREACH(const LineStyle &curLineStyle, linesStyle_){

		if(i == position){

			bFind = true;
			itr++;

			linesStyle_.insert(itr, lineStyle);

			break;
		}

		i ++;
		itr ++;
	}

	i = 0;

	BOOST_FOREACH(LineStyle &curLineStyle, linesStyle_){

		if(i > position){

			curLineStyle.index_ = i;
		}

		i ++;
	
	}

}

void Text_2::addLineStyle(const std::vector<LineStyle> &linestyles, int position){

	int i = 0;
	bool bFind = false;

	std::vector<LineStyle>::iterator itr;

	itr = linesStyle_.begin();

	BOOST_FOREACH(const LineStyle &curLineStyle, linesStyle_){

		if(i == position){

			bFind = true;
			itr++;

			linesStyle_.insert(itr, linestyles.begin(), linestyles.end());

			break;
		}

		i ++;
		itr ++;
	}

	i = 0;

	BOOST_FOREACH(LineStyle &curLineStyle, linesStyle_){

		if(i > position){

			curLineStyle.index_  = i;
		}

		i ++;

	}
}

void Text_2::applyLineStyle(const LineStyle &linestyle, LineStyle::Type types, unsigned int lineLen){


	if(types == LineStyle::LineStyleIncleasement){
	
		BOOST_FOREACH(LineStyle &curLineStyle, linesStyle_){

			if(linestyle.index_ <= curLineStyle.index_)
				curLineStyle.lineIncreasement_.type = linestyle.lineIncreasement_.type;



			if(linestyle.index_ + lineLen  == curLineStyle.index_)
				break;
		}
	}else if(types == LineStyle::LineStyleAlign){

		BOOST_FOREACH(LineStyle &curLineStyle, linesStyle_){

			if(linestyle.index_ <= curLineStyle.index_)
				curLineStyle.textAlign_ = linestyle.textAlign_;



			if(linestyle.index_ + lineLen  == curLineStyle.index_)
				break;
		}

	}else if(types == LineStyle::LineStyleTap){

		BOOST_FOREACH(LineStyle &curLineStyle, linesStyle_){

			if(linestyle.index_ <= curLineStyle.index_){

				curLineStyle.tabIndex_ += linestyle.tabIndex_;

				if(curLineStyle.tabIndex_ < 0)
					curLineStyle.tabIndex_ = 0;
			}

			if(linestyle.index_ + lineLen  == curLineStyle.index_)
				break;
		}
	}
	/*else if(types == LineStyle::LineStyleLetterSpacing){

		BOOST_FOREACH(LineStyle &curLineStyle, linesStyle_){

			if(linestyle.index_ <= curLineStyle.index_)
			{
				curLineStyle.letterSpacing_ = linestyle.letterSpacing_;

			}
			if(linestyle.index_ + lineLen  == curLineStyle.index_)
				break;
		}

	}*/
}

void Text_2::getLinesStyle(std::vector<LineStyle> & properties, unsigned int startIndex, unsigned int endIndex, bool localIndices) const{
	
	properties.clear();

	for (unsigned int i = startIndex; i <= endIndex; ++i) 
		properties.push_back(linesStyle_[i]);		

	if (localIndices)
	{
		for (int i = 0; i < (int)properties.size(); ++i)
		{
			if (properties[i].index_ < startIndex) properties[i].index_ = 0;
			else properties[i].index_ -= startIndex;
		}
	}
}

const LineStyle &Text_2::getLineStyleByCharIndex(int index) const{

	int curLine = 0;

	for(int i = 0; i < (int)text_.length(); i ++){

		const wchar_t & c = text_[i];

		if(i == index)
			break;

		if(c == L'\n'){
			curLine ++;
		}

		
	}
	if(linesStyle_.size() <= curLine)
		curLine = linesStyle_.size() - 1;
	return linesStyle_[curLine];


}

const WordWrapPoint_2 &Text_2::getLineByIndex(int index) const{
	if(index < 0)
		index = 0;

	if(index > wordWrapPoints_.size() -1)
		index = wordWrapPoints_.size() -1;

	return wordWrapPoints_[index];
}

int Text_2::getFirstCharIndexByLine(int lineNum){

	int curLine = 0;

	if(lineNum == 0)
		return 0;

	for(int i = 0; i < (int)text_.length(); i ++){

		const wchar_t & c = text_[i];

		if(c == L'\n'){
			curLine ++;
		}

		if(lineNum == curLine){

			return i + 1;
		}

	}

	return text_.length() - 1;
		

}

void Text_2::ReconstructLinStyle(GfxRenderer * gl){

	int curLineIndex = 0;
	int curIncreasementIndex = 0;
	int curRootIncreasementIndex = 0;
	bool bWriteIncreasement = false;

	LineIncreasementType curLineIncreaseType = NONE;

	BOOST_FOREACH(LineStyle & curLineStyle, linesStyle_)
	{
		curLineStyle.charIndex = getFirstCharIndexByLine(curLineStyle.index_);
		curLineStyle.startPosX = 0;
		curLineIndex = curLineStyle.index_;

		bWriteIncreasement = false;

		if(curLineStyle.lineIncreasement_.type == NONE){
			int nTapLevelWidth = curLineStyle.tabIndex_ * TAB_LEVEL_SIZE;

			curLineStyle.startPosX = nTapLevelWidth;
		}else if(curLineStyle.lineIncreasement_.type != NONE){

			int nCharIndex = getFirstCharIndexByLine(curLineIndex);

			if(curLineIncreaseType != curLineStyle.lineIncreasement_.type){
				curIncreasementIndex = 0;
			}

			curLineIncreaseType = curLineStyle.lineIncreasement_.type;

			curLineStyle.lineIncreasement_.index = curIncreasementIndex;

			if(text_.length() > nCharIndex && text_.at(nCharIndex) != L'\n'){
				curIncreasementIndex ++;
				bWriteIncreasement = true;
			}else{
				curLineStyle.lineIncreasement_.wstrIndex = L"";
			}

			if(bWriteIncreasement){
			
				std::vector<TextProperties_2> lineTextProps;

				getProperties(lineTextProps, curLineStyle.charIndex, curLineStyle.charIndex, true);

				curLineStyle.lineIncreasement_.prop = lineTextProps.front();
				curLineStyle.lineIncreasement_.prop.ResetObjectProperties();
				
				TextFont targetfont = curLineStyle.lineIncreasement_.prop.font;
				targetfont.setTextureScaleSize(Global::instance().curSceneMagnification());
				
				curLineStyle.lineIncreasement_.prop.textHandler = gl->textHandler(targetfont);

				lineTextProps.push_back(curLineStyle.lineIncreasement_.prop);

				if(curLineStyle.lineIncreasement_.type == NUMBER){

					wstringstream convert;
					convert << curLineStyle.lineIncreasement_.index + 1;
					curLineStyle.lineIncreasement_.wstrIndex = convert.str();
					curLineStyle.lineIncreasement_.wstrIndex.append(L".");
				}
				if(curLineStyle.lineIncreasement_.type == LOWERER_CASE_ALPHABET || 
					curLineStyle.lineIncreasement_.type == UPPER_CASE_ALPHABET){
						int stringCode = curLineStyle.lineIncreasement_.type + curLineStyle.lineIncreasement_.index;
						curLineStyle.lineIncreasement_.wstrIndex = curLineStyle.lineIncreasement_.type + curLineStyle.lineIncreasement_.index;
						curLineStyle.lineIncreasement_.wstrIndex.append(L".");
				}else if(curLineStyle.lineIncreasement_.type == CIRCLE_NUMBER || curLineStyle.lineIncreasement_.type == CIRCLE_ALPHABET 
					|| curLineStyle.lineIncreasement_.type == BRACKET_NUMBER || curLineStyle.lineIncreasement_.type == BRACKET_ALPHABET){

						curLineStyle.lineIncreasement_.wstrIndex = curLineStyle.lineIncreasement_.type + curLineStyle.lineIncreasement_.index;

				}else if(curLineStyle.lineIncreasement_.type == EXCLAMATION_POINT || curLineStyle.lineIncreasement_.type == RECTANGLE 
					|| curLineStyle.lineIncreasement_.type == TRIAGLE_1 || curLineStyle.lineIncreasement_.type == TRIAGLE_2 || curLineStyle.lineIncreasement_.type == STAR){

						curLineStyle.lineIncreasement_.wstrIndex = curLineStyle.lineIncreasement_.type;
				}

				curLineStyle.lineIncreasement_.prop.textHandler->genAnyNewChars(lineTextProps, curLineStyle.lineIncreasement_.wstrIndex);

			}

			int nIncleaseWidth = 0;
			

			BOOST_FOREACH (const wchar_t &c , curLineStyle.lineIncreasement_.wstrIndex){
				const CharData *pChar = curLineStyle.lineIncreasement_.prop.textHandler->charData(c);

				nIncleaseWidth += pChar->advanceX;

			}

			int nCount = 0;
			double nModule = 0;

			nCount = (nIncleaseWidth) / MAX_INCREASEMENT_SIZE;
			nModule = fmod((double)nIncleaseWidth ,MAX_INCREASEMENT_SIZE);

			if(nModule >= 0.0)
				nCount ++;

			curLineStyle.startPosX = (curLineStyle.tabIndex_ * MAX_INCREASEMENT_SIZE) + (nCount * MAX_INCREASEMENT_SIZE);

		}
	}
}

bool Text_2::asyncLoadUpdate()
{
	bool everythingLoaded = SceneObject::asyncLoadUpdate();

	BOOST_FOREACH(TextProperties_2 & props, properties_)
	{
		if(props.isObjectProperty()){

			const ObjectProperties *pObjectProp = props.GetObjectProperty();
			
			pObjectProp->texture->asyncLoadUpdate();

			everythingLoaded &= pObjectProp->texture->isLoaded();
		}
	}

	handleLoadedEventDispatch(everythingLoaded);

	return everythingLoaded;
}

int LineStyle::getLineIncreasementIndex(LineIncreasementType &type){
	
	switch(type){
		case NONE:
			return 0;
		break;
		case NUMBER:
			return 1;
		break;
		case LOWERER_CASE_ALPHABET:
			return 2;
		break;
		case UPPER_CASE_ALPHABET:
			return 3;
		break;
		case CIRCLE_NUMBER:
			return 4;
		break;
		case CIRCLE_ALPHABET:
			return 5;
		break;
		case BRACKET_NUMBER:
			return 6;
		break;
		case BRACKET_ALPHABET:
			return 7;
		break;
		case EXCLAMATION_POINT:
			return 8;
			break;
		case RECTANGLE:
			return 9;
			break;
		case TRIAGLE_1:
			return 10;
			break;
		case TRIAGLE_2:
			return 11;
			break;
		case STAR:
			return 12;
			break;
		default:
			return -1;
		break;

	}

	return -1;
}

const float &Text_2::getRenderStringHeight()
{
	std::map<TextData, StringData *>::iterator iter;
	float maxHeight = 0;
	for (iter = textMap_.begin() ; iter != textMap_.end() ; iter++)
	{
		float height = iter->second->size.y;
		maxHeight = max(maxHeight, height);
	}
	return maxHeight;
}

Vector2 Text_2::selectWidth(int start, int end)
{
	float lineY0, lineY1;

	int lineIndex0, lineStartIndex0, lineEndIndex0;
	lineDataForChar(start, &lineY0, &lineIndex0, &lineStartIndex0, &lineEndIndex0);
	int lineIndex1, lineStartIndex1, lineEndIndex1;
	lineDataForChar(end, &lineY1, &lineIndex1, &lineStartIndex1, &lineEndIndex1);

	Vector2 posTop0; float posHeight0, posWidth0;
	posAtIndex(start, &posTop0, &posHeight0, &posWidth0);
	Vector2 posTop1; float posHeight1, posWidth1;
	posAtIndex(end, &posTop1, &posHeight1, &posWidth1);

	if (lineIndex0 == lineIndex1)
	{
		return Vector2(posTop1.x - posTop0.x, posHeight0);
	}
	else
	{
		int lineStartIndexP, lineEndIndexP;
		float lineYP, lineWidthP, lineHeightP;

		float linelen = 0.0;
		float drawX = 0.0;
		float drawY = 0.0;
		float width = 0.0;				
		float height = 0.0;

		for (int i = lineIndex0 ; i <= lineIndex1; ++i){
			float moveX = 0.0;
			const WordWrapPoint_2 line = getLineByIndex(i);
			const LineStyle lineStyle = getLineStyleByCharIndex(line.wrapAtIndex);

			lineDataForLine(i, &lineYP, &lineWidthP, &lineHeightP, &lineStartIndexP, &lineEndIndexP);

			drawX = line.nStartx;
			drawY = lineYP;
			height = lineHeightP;

			if(lineStyle.textAlign_ == LineStyle::Center){
				moveX = (float)(nDisplayW_ / 2.0 - (float)lineWidthP / 2.0);
			}else if(lineStyle.textAlign_ == LineStyle::Right){
				moveX =  (float)(nDisplayW_ - (float)lineWidthP + ENTER_SIZE);
			}

			drawX += moveX;

			float justifySpacing = 0; 

			if(lineStyle.textAlign_ == LineStyle::Justify_Left)
				justifySpacing = ENTER_SIZE;

			if(i == lineIndex0){
				drawX = posTop0.x;
			}

			if(i == lineIndex0)
			{
				width = moveX + lineWidthP - posTop0.x + justifySpacing;
				if(lineStyle.textAlign_ == LineStyle::Justify_Left)
				{
					const LineStyle nextlineStyle = getLineStyleByCharIndex(lineEndIndexP+1);
							
					if(nextlineStyle.index_ == lineStyle.index_)
						width += nDisplayW_ - lineWidthP;
				}
			}
			else if(i == lineIndex1){
				width = posTop1.x - drawX;

			}
			else
			{
				width = (float)lineWidthP - line.nStartx + justifySpacing;

				if(lineStyle.textAlign_ == LineStyle::Justify_Left)
				{
					const LineStyle nextlineStyle = getLineStyleByCharIndex(lineEndIndexP+1);
					
					if(nextlineStyle.index_ == lineStyle.index_)
						width += nDisplayW_ - lineWidthP;
				}
			}
			return Vector2(posTop1.x - drawX, height * (lineIndex1-lineIndex0 +1));
		}
	}
}

void Text_2::setMemos(std::vector<textMemoIndex> memos, Scene* curScene, unsigned char * bits, GfxRenderer *gl, int width, int height)
{
	memos_ = memos;

	if (!memoTex_) memoTex_.reset(new Texture);
	if (!memoTex_->isInit())
	{
		memoTex_->init(gl, width, height, Texture::UncompressedRgba32, bits);
	}

	parentScene_->parentDocument()->triggerRedraw();
}

void Text_2::addMemo(int startIndex, int endIndex, Scene* curScene)
{
	textMemoIndex newMemo;
	newMemo.startIndex = startIndex;
	newMemo.endIndex = endIndex;
	
	memos_.push_back(newMemo);
}

void Text_2::deleteMemo(int startIndex, int endIndex)
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

void Text_2::drawMemoIcon(GfxRenderer * gl) const
{
	gl->useTextureProgram();
	gl->setTextureProgramOpacity(1.0f);	

	gl->use(memoTex_.get());

	for(int i = 0; i < (int)memos_.size(); i++)
	{
		float lineY;
		
		int lineIndex, lineStartIndex, lineEndIndex;
		lineDataForChar(memos_[i].endIndex, &lineY, &lineIndex, &lineStartIndex, &lineEndIndex);
	
		Vector2 posTop; float posHeight, posWidth;
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

void Text_2::convertText_2(Text* orgText)
{
	setId(orgText->id());
	visualAttrib_.setTransform(orgText->visualAttrib()->transform());
	visualAttrib_.setOpacity(orgText->visualAttrib()->opacity());
	visualAttrib_.setVisible(orgText->visualAttrib()->isVisible());

	eventListeners_.clear();
		
	BOOST_FOREACH(EventListenerSPtr eventListener, orgText->eventListeners())	
	{
		eventListeners_.push_back(EventListenerSPtr(
			new EventListener(*eventListener.get())));
	}

	BOOST_FOREACH(EventListenerSPtr eventListener, eventListeners_)	
	{
		BOOST_FOREACH(ActionSPtr action, eventListener->actions())
		{
			if(action->targetObject() == orgText)
				action->setTargetObject(this);
		}
	}

	text_ = orgText->textString();
		
	properties_.clear();

	BOOST_FOREACH(TextProperties prob, orgText->properties())
	{
		TextProperties_2 convertprob;
		convertprob.index = prob.index;

		convertprob.font = prob.font;
		convertprob.color = prob.color;
		convertprob.link = prob.link;
		convertprob.underline_ = prob.underline;
		convertprob.cancleline_ = false;
		convertprob.letterwidthratio_ = 100.0f;

		properties_.push_back(convertprob);
	}

	int temp = orgText->lineSpacingMode();
	lineSpacingMode_ = (LineSpacingMode)temp;
	lineSpacing_ = orgText->lineSpacing();
	

	boundaryWidth_ = orgText->boundaryWidth();
	boundaryHeight_ = orgText->boundaryHeight();

	letterSpacing_ = orgText->letterSpacing();
	canSelectPart_ = orgText->canSelectPart();
	
	LineStyle curLineStyle;
	int i = 0;
	linesStyle_.clear();
	BOOST_FOREACH(const wchar_t &c, text_)
	{
		if(i == 0 || c == L'\r' || c == L'\b' || c == L'\n')
		{
			curLineStyle.index_ = i;
			curLineStyle.textAlign_ = (LineStyle::TextAlign)orgText->textAlign();
			linesStyle_.push_back(curLineStyle);
			i++;
		}
	}
	if(linesStyle_.empty())
	{
		curLineStyle.index_ = i;
		curLineStyle.textAlign_ = (LineStyle::TextAlign)orgText->textAlign();
		linesStyle_.push_back(curLineStyle);
	}
}