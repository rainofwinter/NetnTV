#include "stdafx.h"
#include "TextDlg.h"
#include "Scene.h"
#include "Text_2.h"

TextDlg::TextDlg(Text_2 * text, QWidget * parent) 
: 
QDialog(parent)
{
	ui.setupUi(this);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint); 

	ui.textEdit->setPlainText(QString::fromStdWString(text->textString()));

	connect(ui.okButton, SIGNAL(clicked()), this, SLOT(onOk()));
}

void TextDlg::onOk()
	{
	textString_ = ui.textEdit->toPlainText().toStdWString();
	accept();
}

