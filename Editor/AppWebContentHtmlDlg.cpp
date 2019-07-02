#include "stdafx.h"
#include "AppWebContentHtmlDlg.h"
#include "EditorGlobal.h"
#include "EditorDocument.h"
#include "AppWebContent.h"
#include "Utils.h"
#include "Command.h"

using namespace std;

AppWebContentHtmlDlg::AppWebContentHtmlDlg(EditorDocument * document, 
	AppWebContent * obj, QWidget * parent) 
: QDialog(parent)
{
	document_ = document;
	obj_ = obj;
	ui.setupUi(this);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint); 

	connect(ui.okButton, SIGNAL(clicked()), this, SLOT(onOk()));

	initialHtml_ = obj_->html();
	ui.htmlEdit->setPlainText(QString::fromUtf8(initialHtml_.c_str()));
	
}


void AppWebContentHtmlDlg::onOk()
{

	QByteArray utf8Chars = ui.htmlEdit->toPlainText().toUtf8();
	std::string htmlUtf8(utf8Chars.begin(), utf8Chars.end());

	//std::string html = qStringToStdString(ui.htmlEdit->toPlainText());

	if (htmlUtf8 != initialHtml_)
	{
		document_->doCommand(makeChangePropertyCmd(ChangeAppObjectProperty,
			obj_, htmlUtf8, &AppWebContent::html, &AppWebContent::setHtml));
	}

	accept();
}