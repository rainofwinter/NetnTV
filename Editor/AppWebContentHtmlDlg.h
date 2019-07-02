#pragma once
#include <QDialog>
#include "ui_AppWebContentHtmlDlg.h"

///////////////////////////////////////////////////////////////////////////////
class EditorDocument;
class AppWebContent;

class AppWebContentHtmlDlg : public QDialog
{
	Q_OBJECT
public:
	AppWebContentHtmlDlg(EditorDocument * document, AppWebContent * obj, QWidget * parent);

private slots:
	
	void onOk();

private:
	Ui::AppWebContentHtmlDlg ui;
	std::string initialHtml_;
	AppWebContent * obj_;
	EditorDocument * document_;
	
};

