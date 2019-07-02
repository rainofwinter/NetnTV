#include "stdafx.h"
#include "UpdateQuestionDlg.h"
#include "EditorGlobal.h"
#include "Utils.h"

UpdateQuestionDlg::UpdateQuestionDlg(QWidget * parent) :
QDialog(parent)
{
	ui.setupUi(this);	
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
	isUpdateNow = false;

	connect(ui.okButton, SIGNAL(clicked()), this, SLOT(onOk()));
}


UpdateQuestionDlg::~UpdateQuestionDlg()
{
	isContinuePopUpMessage_ = ui.checkContinue->isChecked();
	EditorGlobal::instance().settings()->setValue("dontPopUpUpdateMessage", isContinuePopUpMessage_);
}

void UpdateQuestionDlg::onOk()
{
	isUpdateNow = true;
	updateEditor();
}