#include "stdafx.h"
#include "PageTrackingUrlsDlg.h"
#include <boost/algorithm/string.hpp>
#include "Utils.h"

PageTrackingUrlsDlg::PageTrackingUrlsDlg(QWidget * parent) : QDialog(parent)
{
	ui.setupUi(this);	
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint); 
	connect(ui.okButton, SIGNAL(clicked()), this, SLOT(onOk()));
}

void PageTrackingUrlsDlg::onOk()
{
	url_ = qStringToStdString(ui.urlEdit->text());
	boost::trim(url_);	

	if (ui.urlEdit->text().isEmpty())
	{
		QMessageBox::information(0, tr("Error"), tr("Url cannot be empty"));
		return;
	}

	regexReplace(&url_, "^(.*://|)(.*)$", "http://$2");


	accept();
}