#pragma once
#include "ui_TextbookDlg.h"


class TextbookUploadDlg : public QDialog
{
	Q_OBJECT
public:
	TextbookUploadDlg(QWidget * parent);
	~TextbookUploadDlg();

protected slots:
	void onOk();
	void onCancel();

private:
	void parseData();

private:
	Ui::TextbookDlg ui;
};