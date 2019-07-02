#pragma once
#include "ui_pageTrackingUrlsDlg.h"

class PageTrackingUrlsDlg : public QDialog
{
	Q_OBJECT
public:
	PageTrackingUrlsDlg(QWidget * parent);

	std::string url() const {return url_;}

private slots:
	void onOk();

private:
	Ui::PageTrackingUrlsDlg ui;
	std::string url_;
};