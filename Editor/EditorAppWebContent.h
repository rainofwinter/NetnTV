#pragma once
#include "EditorAppObject.h"
#include "PropertyPage.h"
#include "AppWebContent.h"

class QWebView;

class EditorAppWebContent : public EditorAppObject
{
public:
	EditorAppWebContent();
	virtual ~EditorAppWebContent();
	virtual EditorAppObject * clone() const {return new EditorAppWebContent(*this);}
	virtual AppObject * createAppObject() const;
	virtual boost::uuids::uuid appObjectType() const;
	virtual void drawObject(GfxRenderer * gl, const EditorDocument * document) const;
	PropertyPage * propertyPage() const;
private:
	mutable QImage * image_;
	mutable QWebView * webView_;
	mutable QPainter * painter_;
	mutable GLuint texture_;
	mutable std::string curUrl_;
	mutable AppWebContent::Mode curMode_;
	mutable std::string curHtml_;
	mutable int curWidth_, curHeight_;
};

///////////////////////////////////////////////////////////////////////////////
class EditorAppWebContentPropertyPage : public PropertyPage
{
	Q_OBJECT
public:
	EditorAppWebContentPropertyPage();
	~EditorAppWebContentPropertyPage();

	virtual void update();
	virtual void onChanged(Property * property);

private slots:
	void onHtml();

private:
	void enabledBasedOnMode();

private:
	CoreAppObjectPropertiesPage * core_;
	VisualAppObjectPropertiesPage * visual_;
	EditProperty * url_;

	ComboBoxProperty * mode_;
	QPushButton * html_;

	QIntValidator dimValidator_;

};