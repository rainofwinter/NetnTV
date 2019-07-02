#include "stdafx.h"
#include "PublishDlg.h"
#include "EditorGlobal.h"
#include "Document.h"
#include "Utils.h"
#include "FileUtils.h"



PublishDlg::PublishDlg(Document * document, QWidget * parent) : QDialog(parent)
{
	ui.setupUi(this);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint); 
	/*
	ui.texCompressComboBox->addItem(tr("None (All platforms)"));
	ui.texCompressComboBox->addItem(tr("ETC1 (Android)"));
	ui.texCompressComboBox->addItem(tr("PVR (IOS)"));
	*/
	connect(ui.zipBrowse, SIGNAL(clicked()), this, SLOT(onZipBrowse()));
	connect(ui.okButton, SIGNAL(clicked()), this, SLOT(onOk()));

}

void PublishDlg::onZipBrowse()
{
	QString zipFile = 
		getSaveFileNameNoOverwrite(tr("Publish file"), QString(), tr("Zip files (*.zip)"), "zip");
	ui.zipEdit->setText(zipFile);
}

PublishTextureCompress PublishDlg::textureCompress() const
{
	return PublishTextureCompressNone;
	//return (PublishTextureCompress)ui.texCompressComboBox->currentIndex();
}

bool PublishDlg::isCatalogDocument() const
{
	return ui.publishCatalogCheckBox->isChecked();
}

void PublishDlg::onOk()
{
	bool zipEmpty = ui.zipEdit->text().isEmpty();
	if (zipEmpty)
	{
		QMessageBox::information(0, tr("Error"), 
			tr("Zip file field cannot be empty"));
		return;
	}	

	file_ = ui.zipEdit->text();


	if (!boost::iequals(getFileExtension(qStringToStdString(file_)), "zip"))
	{
		file_ += ".zip";
	}

	std::string homeDir = qStringToStdString(QDir::homePath());
	std::string contentDataSubDir = "Studio4UX_contentData";
	std::string decompressDir = homeDir + "/" + contentDataSubDir + "/";

	std::string curDir = getCurDir();
	_chdir(homeDir.c_str());
	_mkdir(contentDataSubDir.c_str());
	_chdir(curDir.c_str());

	directory_ = stdStringToQString(decompressDir);


	accept();
}

