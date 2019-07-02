#include "stdafx.h"
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
#include "TextEditObject.h"

#include "Document.h"
#include "Scene.h"
#include "TextHandler.h"

#include "utf8.h"
#include "ScriptProcessor.h"

using namespace std;
///////////////////////////////////////////////////////////////////////////////

string char2hex( char dec )
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

string urlencode(const string &c)
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
            escaped.append( char2hex(c[i]) );//converts char 255 to string "ff"
        }
    }
    return escaped;
}


JSClass jsTextEditObjectClass = InitClass(
	"TextEditObject", JSCLASS_HAS_PRIVATE | JSCLASS_MARK_IS_TRACE, JS_PropertyStub, 
	JS_StrictPropertyStub, SceneObject_finalize, SceneObject::gcMark);

JSBool TextEditObject_getText(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	TextEditObject * thisObj = (TextEditObject *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	ScriptProcessor * s = (ScriptProcessor *)JS_GetContextPrivate(cx);
	if (!JS_ConvertArguments(cx, argc, argv, "")) return JS_FALSE;

	string narrowString = "";
	utf8::utf16to8(thisObj->textString().begin(), thisObj->textString().end(), back_inserter(narrowString));
	string str = urlencode(narrowString);
	
	const char * textStr = str.c_str();
	JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(JS_NewStringCopyZ(cx, textStr)));
	return JS_TRUE;
};

JSBool TextEditObject_getWidth(JSContext *cx, uintN argc, jsval *vp)
{	
	TextEditObject * thisObj = (TextEditObject *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	JS_SET_RVAL(cx, vp, DOUBLE_TO_JSVAL((jsdouble)thisObj->width()));
	return JS_TRUE;
};

JSBool TextEditObject_getHeight(JSContext *cx, uintN argc, jsval *vp)
{
	TextEditObject * thisObj = (TextEditObject *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));
	JS_SET_RVAL(cx, vp, DOUBLE_TO_JSVAL((jsdouble)thisObj->height()));
	return JS_TRUE;
};


JSBool TextEditObject_setText(JSContext *cx, uintN argc, jsval *vp)
{
	jsval * argv = JS_ARGV(cx, vp);
	TextEditObject * thisObj = (TextEditObject *)JS_GetPrivate(cx, JS_THIS_OBJECT(cx, vp));

	jschar * jsStr;
	if (!JS_ConvertArguments(cx, argc, argv, "W", &jsStr)) return JS_FALSE;
	JS_SET_RVAL(cx, vp, JSVAL_VOID);

	thisObj->setTextString(jsCharStrToStdWString(jsStr));
	thisObj->inputTextDraw();

	return JS_TRUE;
};

JSFunctionSpec TextEditObjectFuncs[] = {
	JS_FS("setText", TextEditObject_setText, 1, 0),
	JS_FS("getText", TextEditObject_getText, 0, 0),
	JS_FS("getWidth", TextEditObject_getWidth, 0, 0),
	JS_FS("getHeight", TextEditObject_getHeight, 0, 0),
    JS_FS_END
};

///////////////////////////////////////////////////////////////////////////////
JSObject * TextEditObject::createScriptObjectProto(ScriptProcessor * s)
{			
	JSObject * proto = JS_InitClass(
		s->jsContext(), s->jsGlobal(), s->baseSceneObjectProto(), &jsTextEditObjectClass,
		0, 0, 0, TextEditObjectFuncs, 0, 0);

	return proto;
}

JSObject * TextEditObject::getScriptObject(ScriptProcessor * s)
{
	if (!scriptObject_)
	{
		scriptObject_ = JS_NewObject(
			s->jsContext(), &jsTextEditObjectClass, s->sceneObjectProto(this), 0);
		JS_SetPrivate(s->jsContext(), scriptObject_, this);

		if (parent_) parent_->getScriptObject(s);
	}

	return scriptObject_;
}

void TextEditObject::create()
{
	text_ = new Text_2;
	text_->setTextString(L"");
	boxColor_ = Color(0,0,0,1);

	boundaryWidth_ = 100;
	boundaryHeight_ = 50;
}

TextEditObject::TextEditObject()
{
	create();
	setId("TextEditObject");
}

TextEditObject::TextEditObject(const TextEditObject & rhs, ElementMapping * elementMapping)
: SceneObject(rhs, elementMapping)
{
	create();
	//text_ = rhs.text_;
	visualAttrib_ = rhs.visualAttrib_;
	properties_ = rhs.properties_;
	text_->setTextString(rhs.textString());
	text_->setProperties(rhs.properties());
}

TextEditObject::~TextEditObject()
{
	uninit();
	delete text_;
}

void TextEditObject::referencedFiles(std::vector<std::string> * refFiles) const
{
	SceneObject::referencedFiles(refFiles);

	text_->referencedFiles(refFiles);
}

int TextEditObject::setReferencedFiles(const std::string & baseDirectory,
	const vector<string> & refFiles, int index)
{
	index = SceneObject::setReferencedFiles(baseDirectory, refFiles, index);

	index = text_->setReferencedFiles(baseDirectory, refFiles, index);

	return index;
}

void TextEditObject::setTextString(const std::wstring & str)
{
	text_->setTextString(str);
	/*if (parentScene())
	{
		text_->init(parentScene()->parentDocument()->renderer());
		parentScene()->parentDocument()->triggerRedraw();
	}*/
}

const std::wstring & TextEditObject::textString() const
{
	return text_->textString();
}

const TextFont & TextEditObject::font() const
{
	return text_->font();
}

void TextEditObject::setFont(const TextFont & font)
{
	std::vector<TextProperties_2> props = text_->properties();
	props.front().font = font;
	text_->setProperties(props);
}
const std::vector<TextProperties_2> & TextEditObject::properties() const
{
	return text_->properties();
}
void TextEditObject::setProperties(const std::vector<TextProperties_2> & properties)
{
	properties_ = properties;
	text_->setProperties(properties);
}

const Color & TextEditObject::color() const
{
	return text_->color();
}
void TextEditObject::setColor(const Color & color)
{
	text_->setColor(color);
}

void TextEditObject::init(GfxRenderer * gl, bool firstTime)
{
	init(gl, parentScene_, firstTime);
}

void TextEditObject::init(GfxRenderer * gl, Scene * parentScene, bool firstTime)
{
	SceneObject::init(gl, firstTime);

	parentScene_ = parentScene;

	text_->init(gl, parentScene, firstTime);
}

void TextEditObject::uninit()
{
	text_->uninit();

	SceneObject::uninit();
}

void TextEditObject::drawObject(GfxRenderer * gl) const
{	

	float fOpacity = totalOpacity();
	if (fOpacity == 0) return;
	//gl->useColorProgram();
	//gl->setColorProgramColor(boxColor_.r, boxColor_.g, boxColor_.b, boxColor_.a);
	//gl->drawBox(extents().minPt, extents().maxPt);

	gl->useTextureProgram();
	gl->setTextureProgramOpacity(fOpacity);	

	text_->drawObject(gl);
	
	
}

BoundingBox TextEditObject::extents() const
{	
	BoundingBox ret;
	ret.maxPt.x = boundaryWidth_;
	ret.maxPt.y = boundaryHeight_;

	return ret;
}

bool TextEditObject::clickEvent(const Vector2 & mousePos, int pressId)
{
	bool handled = SceneObject::clickEvent(mousePos, pressId);
	if (handled) return true;

	Global::instance().handledTextEditObj(this);

	return false;
}

void TextEditObject::write(Writer & writer) const
{
	writer.writeParent<SceneObject>(this);
	writer.write(visualAttrib_, "visualAttrib");

	writer.write(text_->properties());
	writer.write(boxColor_);

	writer.write(boundaryWidth_, "width");
	writer.write(boundaryHeight_, "height");	
}

void TextEditObject::read(Reader & reader, unsigned char version)
{
	reader.readParent<SceneObject>(this);
	reader.read(visualAttrib_);

	if (version < 1)
	{
		std::vector<TextProperties> temp_properties;
		reader.read(temp_properties);

		properties_.clear();

		BOOST_FOREACH(TextProperties prob, temp_properties)
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
	}
	else
		reader.read(properties_);

	reader.read(boxColor_);

	reader.read(boundaryWidth_);
	reader.read(boundaryHeight_);

	text_->setProperties(properties_);
}


void TextEditObject::writeXml(XmlWriter & w) const
{
	SceneObject::writeXml(w);
	w.startTag("VisualAttrib");	
	visualAttrib_.writeXml(w);
	w.endTag();

	w.startTag("Properties_2");
	for (unsigned int i = 0; i < (unsigned int)text_->properties().size(); ++i)
	{
		w.startTag("TextProperties_2");
		text_->properties()[i].writeXml(w);
		w.endTag();
	}
	w.endTag();

	w.writeTag("Color", boxColor_);
	w.writeTag("Width", boundaryWidth_);
	w.writeTag("Height", boundaryHeight_);
}

void TextEditObject::readXml(XmlReader & r, xmlNode * parent)
{
	SceneObject::readXml(r, parent);

	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;
		if (r.isNodeName(curNode, "VisualAttrib"))
		{
			visualAttrib_.readXml(r, curNode);
			curNode = curNode->next;
		}
		else if (r.isNodeName(curNode, "Properties"))
		{
			xmlNode * propertiesNode = curNode->children;
			//while (r.isNodeName(propertiesNode, "TextProperties"))
			for(; propertiesNode ; propertiesNode = propertiesNode->next)
			{
				if (propertiesNode->type != XML_ELEMENT_NODE) continue;
				TextProperties temp;
				temp.readXml(r, propertiesNode);

				TextProperties_2 p;

				p.index = temp.index;
				p.font = temp.font;
				p.color = temp.color;
				p.link = temp.link;
				p.underline_ = temp.underline;
				p.cancleline_ = false;
				p.letterwidthratio_ = 100.0f;
				
				properties_.push_back(p);
			}
			text_->setProperties(properties_);
			curNode = curNode->next;
		}

		else if (r.isNodeName(curNode, "Properties_2"))
		{
			xmlNode * propertiesNode = curNode->children;
			for(; propertiesNode ; propertiesNode = propertiesNode->next)
			{
				if (propertiesNode->type != XML_ELEMENT_NODE) continue;
				TextProperties_2 p;
				p.readXml(r, propertiesNode);
				properties_.push_back(p);
			}
			text_->setProperties(properties_);
			curNode = curNode->next;
		}

		else if (r.getNodeContentIfName(boxColor_, curNode, "Color"));
		else if (r.getNodeContentIfName(boundaryWidth_, curNode, "Width"));
		else if (r.getNodeContentIfName(boundaryHeight_, curNode, "Height"));
	}
}

float TextEditObject::width() const
{
	return extents().lengthX();	
}

float TextEditObject::height() const
{
	return extents().lengthY();	
}

float TextEditObject::posX() const
{
	Matrix m = parentTransform() * transform().computeMatrix();
	Transform t;
	t.setMatrix(m, true);
	return t.translation().x;
}

float TextEditObject::posY() const
{
	Matrix m = parentTransform() * transform().computeMatrix();
	Transform t;
	t.setMatrix(m, true);
	return t.translation().y;
}

SceneObject * TextEditObject::intersect(Vector3 * out, const Ray & ray)
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


void TextEditObject::setBoundaryWidth(const float & width)
{
	boundaryWidth_ = width;
	//text_->setBoundaryWidth(boundaryWidth_);
}

void TextEditObject::setBoundaryHeight(const float & height)
{
	boundaryHeight_ = height;
	//text_->setBoundaryHeight(boundaryHeight_);
}

void TextEditObject::inputTextDraw()
{
	if (parentScene())
	{
		text_->init(parentScene()->parentDocument()->renderer());
		parentScene()->parentDocument()->triggerRedraw();
	}
}