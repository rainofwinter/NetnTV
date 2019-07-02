#pragma once
#include "SceneObject.h"
#include "Attrib.h"
#include "Color.h"
#include "Text_2.h"
#include "Text.h"

class TextEditObject : public SceneObject
{
public:
	virtual JSObject * createScriptObjectProto(ScriptProcessor * s);
	virtual JSObject * getScriptObject(ScriptProcessor * s);

	virtual const char * typeStr() const 
	{
		return "TextEditObject";
	}
	typedef SceneObject base_type;
	virtual boost::uuids::uuid type() const 
	{
		static boost::uuids::uuid uuid = 
			sUuidGen_("B1520BD8-DC18-4725-B349-78D770C9748A");
		return uuid;
	}
	virtual SceneObject * clone(ElementMapping * elementMapping) const 
	{return new TextEditObject(*this, elementMapping);}

	virtual void remapReferences(const ElementMapping & mapping) 
	{SceneObject::remapReferences(mapping);}

	virtual void referencedFiles(std::vector<std::string> * refFiles) const;
	virtual int setReferencedFiles(const std::string & baseDirectory,
		const std::vector<std::string> & refFiles, int index);

	TextEditObject();
	TextEditObject(const TextEditObject & rhs, ElementMapping * elementMapping);

	~TextEditObject();

	void setTextString(const std::wstring & text);
	const std::wstring & textString() const;

	const int & lineSpacing() const {return text_->lineSpacing();}
	void setLineSpacing(const int & lineSpacing) { text_->setLineSpacing(lineSpacing);}

	const Text_2::LineSpacingMode & lineSpacingMode() const {return text_->lineSpacingMode();}

	void setLineSpacingMode(const Text_2::LineSpacingMode & lineSpacingMode) 
	{
		text_->setLineSpacingMode(lineSpacingMode);
	}

	const float & boundaryWidth() const {return boundaryWidth_;}
	void setBoundaryWidth(const float & width);

	const float & boundaryHeight() const {return boundaryHeight_;}
	void setBoundaryHeight(const float & height);

	virtual void drawObject(GfxRenderer * gl) const;

	virtual void init(GfxRenderer * gl, bool firstTime = false);

	void init(GfxRenderer * gl, Scene * parentScene, bool firstTime);
	virtual void uninit();
	virtual VisualAttrib * visualAttrib() {return &visualAttrib_;}

	virtual BoundingBox extents() const;

	virtual bool clickEvent(const Vector2 & mousePos, int pressId);

	virtual unsigned char version() const {return 1;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);
	virtual const char * xmlTag() const {return "TextEdit";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void readXml(XmlReader & r, xmlNode * node);

	float width() const;
	float height() const;
	float posX() const;
	float posY() const;

	const TextFont & font() const;
	void setFont(const TextFont & font);

	const std::vector<TextProperties_2> & properties() const;
	void setProperties(const std::vector<TextProperties_2> & properties);

	const Color & color() const;			// font color
	void setColor(const Color & color);		// font color

	virtual SceneObject * intersect(Vector3 * intPt, const Ray & ray);
	void inputTextDraw();
private:
	TextEditObject(const TextEditObject & rhs);
	TextEditObject & operator = (const TextEditObject & rhs);
	void create();

private:
	Text_2* text_;

	VisualAttrib visualAttrib_;

	std::vector<TextProperties_2> properties_;

	Color boxColor_;

	float boundaryWidth_;
	float boundaryHeight_;	
};