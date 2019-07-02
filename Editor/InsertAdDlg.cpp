#include "stdafx.h"
#include "InsertAdDlg.h"
#include "EditorGlobal.h"
#include "EditorDocument.h"
#include "AppWebContent.h"
#include "Utils.h"
#include "Command.h"

using namespace std;

InsertAdDlg::InsertAdDlg(EditorDocument * document, QWidget * parent) 
: QDialog(parent)
{
	document_ = document;
	ui.setupUi(this);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint); 

	connect(ui.okButton, SIGNAL(clicked()), this, SLOT(onOk()));
	connect(ui.registerButton, SIGNAL(clicked()), this, SLOT(onRegister()));

	ui.image1Radio->setChecked(true);
	handleRadio(TypeImage1);

	connect(ui.image1Radio, SIGNAL(clicked()), this, SLOT(onImage1Radio()));
	connect(ui.image2Radio, SIGNAL(clicked()), this, SLOT(onImage2Radio()));
	connect(ui.image3Radio, SIGNAL(clicked()), this, SLOT(onImage3Radio()));
	connect(ui.text1Radio, SIGNAL(clicked()), this, SLOT(onText1Radio()));
	connect(ui.text2Radio, SIGNAL(clicked()), this, SLOT(onText2Radio()));
	connect(ui.mobileRadio, SIGNAL(clicked()), this, SLOT(onMobileRadio()));	
}

void InsertAdDlg::onImage1Radio() {handleRadio(TypeImage1);}

void InsertAdDlg::onImage2Radio() {handleRadio(TypeImage2);}

void InsertAdDlg::onImage3Radio() {handleRadio(TypeImage3);}

void InsertAdDlg::onText1Radio() {handleRadio(TypeText1);}

void InsertAdDlg::onText2Radio() {handleRadio(TypeText2);}

void InsertAdDlg::onMobileRadio() {handleRadio(TypeMobile);}

QString InsertAdDlg::typeToStr(InsertAdDlg::Type type) const
{
	QString key;
	switch(type)
	{
	case TypeImage1:
		key = "adHtmlImage1";
		break;
	case TypeImage2:
		key = "adHtmlImage2";
		break;
	case TypeImage3:
		key = "adHtmlImage3";
		break;
	case TypeText1:
		key = "adHtmlText1";
		break;
	case TypeText2:
		key = "adHtmlText2";
		break;
	case TypeMobile:
		key = "adHtmlMobile";
		break;
	}

	return key;
}

QSize InsertAdDlg::typeToDim(Type type) const
{
	QSize size;
	switch(type)
	{
	case TypeImage1:
		size = QSize(120, 600);
		break;
	case TypeImage2:
		size = QSize(293, 95);
		break;
	case TypeImage3:
		size = QSize(240, 240);
		break;
	case TypeText1:
		size = QSize(600, 135);
		break;
	case TypeText2:
		size = QSize(document_->selectedScene()->screenWidth(), 60);
		break;
	case TypeMobile:
		size = QSize(document_->selectedScene()->screenWidth(), 60);
		break;
	}

	return size;
}

void InsertAdDlg::handleRadio(InsertAdDlg::Type type)
{
	QString prevHtml;
	QString key = typeToStr(type);
	type_ = type;	

	prevHtml = EditorGlobal::instance().settingsString(key);
	ui.htmlEdit->setPlainText(prevHtml);
}

void InsertAdDlg::onOk()
{
	QString key = typeToStr(type_);
	QString html = ui.htmlEdit->toPlainText();

	if (html.trimmed().isEmpty())
	{
		QMessageBox::information(this, tr("Error"), tr("Please input HTML code"));
		return;
	}

	EditorGlobal::instance().setSettingsString(key, html);

	int docWidth = document_->document()->width();
	QString widthStr;
	widthStr.sprintf("%d", docWidth);

	html =
	QString(
	"<html>\n"
	"	<head>\n"
	"		<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\" />\n"
	"	</head>\n"
	"	<body topmargin=\"0\" leftmargin=\"0\" marginwidth=\"0\" marginheight=\"0\" ontouchmove=\"event.preventDefault();\">\n"
	) +
	html +
	QString(
	"	</body>\n"
	"</html>\n"
	);

	AppWebContent * content = new AppWebContent();
	content->setMode(AppWebContent::ModeHtml);
	content->setHtml(qStringToStdString(html));

	QSize size = typeToDim(type_);
	content->setWidth(size.width());
	content->setHeight(size.height());
	content->setId("AppAdWebContent");
	
	std::vector<AppObjectSPtr> appObjs;
	appObjs.push_back(AppObjectSPtr(content));

	document_->doCommand(new NewAppObjectsCmd(document_, -1, appObjs));
	

	accept();
}

void InsertAdDlg::onRegister()
{
	QDesktopServices::openUrl(QUrl("http://www.realclick.co.kr/"));
}