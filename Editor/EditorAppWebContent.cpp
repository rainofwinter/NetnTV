#include "stdafx.h"
#include "EditorDocument.h"
#include "EditorAppWebContent.h"
#include "AppWebContent.h"
#include "GLWidget.h"
#include "Command.h"
#include "Utils.h"
#include "Texture.h"
#include "FileUtils.h"
#include "AppWebContentHtmlDlg.h"
#include "Global.h"

#include <QtWebKit/QWebView>

using namespace std;
EditorAppWebContent::EditorAppWebContent()
{
	texture_ = 0;

	image_ = 0;
	webView_ = 0;
	painter_ = 0;	
	curWidth_ = -1;
	curHeight_ = -1;
}

EditorAppWebContent::~EditorAppWebContent()
{
	if (texture_ != 0) glDeleteTextures(1, &texture_);

	delete webView_;
	delete painter_;
	delete image_;
	
}

AppObject * EditorAppWebContent::createAppObject() const
{	
	AppWebContent * webContent = new AppWebContent();
	webContent->init();
	return webContent;
}

boost::uuids::uuid EditorAppWebContent::appObjectType() const
{
	return AppWebContent().type();
}

PropertyPage * EditorAppWebContent::propertyPage() const
{
	return new EditorAppWebContentPropertyPage;
}

void EditorAppWebContent::drawObject(GfxRenderer * gl, const EditorDocument * document) const
{
	
	AppWebContent * appWebContent = (AppWebContent *)appObject_;
	glPolygonMode(GL_FRONT_AND_BACK , GL_FILL);
	
	gl->useTextureProgram();

	if (!webView_)
	{
		webView_ = new QWebView;				
	}

	if (curMode_ != appWebContent->mode())
	{
		curMode_ = appWebContent->mode();
		if (curMode_ == AppWebContent::ModeUrl)
			webView_->setUrl(QUrl(""));
		else 
			webView_->setHtml("");
	}
	
	if (curMode_ == AppWebContent::ModeUrl)
	{
	if (curUrl_ != appWebContent->url())
	{
		curUrl_ = appWebContent->url();
		webView_->setUrl(QUrl(curUrl_.c_str()));
	}
	}
	else
	{
		if (curHtml_ != appWebContent->html())
		{
			curHtml_ = appWebContent->html();
			webView_->setHtml(QString::fromUtf8(curHtml_.c_str()));
		}
	}
	

	if (curWidth_ != appObject_->width() || curHeight_ != appObject_->height())
	{
		if (texture_ != 0) glDeleteTextures(1, &texture_);	
		delete painter_;
		delete image_;

		texture_ = 0;
		painter_ = 0;
		image_ = 0;

		curWidth_ = appObject_->width();
		curHeight_ = appObject_->height();
		webView_->resize(curWidth_, curHeight_);
	}

	if (!texture_)
	{
		image_ = new QImage(
			appObject_->width(), appObject_->height(), QImage::Format_RGB888);		
		glGenTextures(1, &texture_);
		glBindTexture(GL_TEXTURE_2D, texture_);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 
			image_->width(), image_->height(), 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

		painter_ = new QPainter(image_);
	}
	else 
	{
		glBindTexture(GL_TEXTURE_2D, texture_);
	}


	webView_->render(painter_);

	const unsigned char * bits = image_->bits();
	int width = image_->width();
	int height = image_->height();
		
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 
		width, height, GL_RGB, GL_UNSIGNED_BYTE, image_->bits());

	gl->setTextureProgramOpacity(appWebContent->opacity());
	
	if (appWebContent->visible())
	{
		gl->drawRect(appWebContent->x(), appWebContent->y(), appWebContent->width(), appWebContent->height());
	}

	
	if (document->isAppObjectSelected(appObject_))
	{		
		gl->useColorProgram();
		gl->setColorProgramColor(1, 0, 0, 1);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glPolygonOffset(-1, 0);
		AppWebContent * obj = (AppWebContent *)appObject_;
		int x = obj->x();
		int y = obj->y();
		GLfloat vertices[] = {
			static_cast<GLfloat>(x), static_cast<GLfloat>(y), 0, 
			static_cast<GLfloat>(x + obj->width()), static_cast<GLfloat>(y), 0,
			static_cast<GLfloat>(x + obj->width()), static_cast<GLfloat>(y + obj->height()), 0,
			static_cast<GLfloat>(x), static_cast<GLfloat>(y + obj->height()), 0
		};
		
		gl->enableVertexAttribArrayPosition();
		gl->bindArrayBuffer(0);
		gl->vertexAttribPositionPointer(0, (char *)vertices);
		gl->applyCurrentShaderMatrix();
		glDrawArrays(GL_QUADS, 0, 4);
		glPolygonOffset(0, 0);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	}

}


///////////////////////////////////////////////////////////////////////////////
EditorAppWebContentPropertyPage::EditorAppWebContentPropertyPage() :
dimValidator_(0, 2048, 0)
{
	
	setGroupBox(false);
	
	core_ = new CoreAppObjectPropertiesPage;
	visual_ = new VisualAppObjectPropertiesPage;
	
	setName(tr("AppWebContent"));
	addPropertyPage(core_);
	addSpacing(4);
	addPropertyPage(visual_);
	addSpacing(4);
	
	startGroup(tr("AppWebContent"));
	mode_ = new ComboBoxProperty(tr("mode"));
	mode_->addItem(tr("url"));
	mode_->addItem(tr("html"));
	addProperty(mode_);
	addSpacing(4);
	
	addSpacing(4);
	url_ = new EditProperty(tr("URL"));
	addProperty(url_);

	addSpacing(4);
	html_ = new QPushButton(tr("html"));
	connect(html_, SIGNAL(clicked()), this, SLOT(onHtml()));
	addWidget(html_);

	endGroup();
	
}

EditorAppWebContentPropertyPage::~EditorAppWebContentPropertyPage()
{
}

void EditorAppWebContentPropertyPage::update()
{
	core_->update();
	visual_->update();
	AppWebContent * obj = (AppWebContent *)document_->selectedAppObject();

	mode_->setValue((int)obj->mode());
	enabledBasedOnMode();

	url_->setValue(stdStringToQString(obj->url()));
}

void EditorAppWebContentPropertyPage::enabledBasedOnMode()
{
	AppWebContent::Mode mode = (AppWebContent::Mode)mode_->value();
	switch (mode)
	{
	case AppWebContent::ModeUrl:
		url_->setEnabled(true);
		html_->setEnabled(false);
		break;
	case AppWebContent::ModeHtml:
		url_->setEnabled(false);
		html_->setEnabled(true);
		break;
	}
}

void EditorAppWebContentPropertyPage::onChanged(Property * property)
{
	AppObject * obj = document_->selectedAppObject();
	AppWebContent * webContent = (AppWebContent *)obj;
	if (property == mode_)
	{
		AppWebContent::Mode mode = (AppWebContent::Mode)mode_->value();
		document_->doCommand(makeChangePropertyCmd(ChangeAppObjectProperty,
			webContent, mode, &AppWebContent::mode, &AppWebContent::setMode));
		enabledBasedOnMode();
	}

	//TODO
	if (property == url_)
	{
		string url = qStringToStdString(url_->value());

		/*
		bool filePrefix = regexMatch(url, "^\\s*file://.*$");
		if (filePrefix)
		{
			regexReplace(&url, "^\\s*(file://)(.*)$", "$2");
		}
		regexReplace(&url, "^\\s*(.*://|)(.*)$", "http://$2");
		*/
		
		document_->doCommand(makeChangePropertyCmd(ChangeAppObjectProperty,
			webContent, url, &AppWebContent::url, &AppWebContent::setUrl));
	}

	update();
}

void EditorAppWebContentPropertyPage::onHtml()
{
	AppObject * obj = document_->selectedAppObject();
	AppWebContent * webContent = (AppWebContent *)obj;

	QWidget * dlgParent = topWidget_;
	while (dlgParent->parent()) dlgParent = dlgParent->parentWidget();

	AppWebContentHtmlDlg dlg(document_, webContent, dlgParent);

	dlg.exec();

}