#pragma once
#include "ui_updateQuestionDlg.h"
#include "Activation.h"


class UpdateQuestionDlg : public QDialog
{
	Q_OBJECT
public:
	UpdateQuestionDlg(QWidget * parent);
	~UpdateQuestionDlg();

	bool isUpdateNow;
protected slots:	
	void onOk();
private:
	Ui::UpdateQuestionDlg ui;
	bool isContinuePopUpMessage_;
};