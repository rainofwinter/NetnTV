#pragma once
#include "ui_publishDlg.h"

enum PublishTextureCompress
{
	PublishTextureCompressNone,
	PublishTextureCompressEtc1,
	PublishTextureCompressPvr
};

class Document;

class PublishDlg : public QDialog
{
	Q_OBJECT
public:
	PublishDlg(Document * document, QWidget * parent);

	QString directory() const {return directory_;}
	QString file() const {return file_;}
		
	PublishTextureCompress textureCompress() const;

	bool isCatalogDocument() const;

private slots:
	void onZipBrowse();
	void onOk();
	
private:
	Ui::PublishDlg ui;

	QString directory_;
	QString file_;

};