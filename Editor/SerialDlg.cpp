#include "stdafx.h"
#include "serialDlg.h"
#include "EditorGlobal.h"
#include "Utils.h"

SerialDlg::SerialDlg(const QString & activationNumber, QWidget * parent) :
QDialog(parent)
{
	ui.setupUi(this);	
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint); 	

	ui.activationNumberEdit->setText(activationNumber);

	connect(ui.okButton, SIGNAL(clicked()), this, SLOT(onOk()));
	connect(
		ui.activationKeyBrowse, SIGNAL(clicked()), 
		this, SLOT(onActivationKeyBrowse()));
}



SerialDlg::~SerialDlg()
{

}

void SerialDlg::onOk()
{
	std::string fileName = ui.activationKeyEdit->text().toLocal8Bit().data();
	bool ret = trialState_.readFile(fileName);
	if (!ret)
	{
		QMessageBox::information(this, "Error", "Could not read Key file");
		return;
	}

	accept();
}

void SerialDlg::onActivationKeyBrowse()
{

	QString fileName = getOpenFileName(QObject::tr("Open Key File"), "", 
		QObject::tr("Keys (*.akey)"));

	if (fileName.isEmpty()) return;
	ui.activationKeyEdit->setText(fileName);
}

