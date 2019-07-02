#include "stdafx.h"
#include "Global.h"
#include "FileUtils.h"
#include "Utils.h"
#include "EditorGlobal.h"
#include "TextbookUpload.h"

using namespace std;


TextbookUploadDlg::TextbookUploadDlg(QWidget * parent)
{
	
/*	connect(ui.okButton, SIGNAL(clicked()), this, SLOT(onOk()));
	connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(onCancel()));*/	
}

TextbookUploadDlg::~TextbookUploadDlg()
{
}

void TextbookUploadDlg::onOk()
{
	accept();
}

void TextbookUploadDlg::onCancel()
{
	reject();
}

void TextbookUploadDlg::parseData()
{
}