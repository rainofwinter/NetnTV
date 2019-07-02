#pragma once
#include "ui_publishingDlg.h"
#include "Texture.h"
#include "PublishDlg.h"

class Document;
class Image;
class SceneObject;

class PublishingDlg : public QDialog
{
	Q_OBJECT
public:
	PublishingDlg(Document * document, 
		const QString & publishDir, const QString & publishFile,
		bool isCatalogDocument, QWidget * parent);

	///publish thread
	void operator ()();

private slots:
	void onOk();
	void onCancel();

private:

	void publish(const QString & dir);

	virtual void showEvent(QShowEvent * event);
	virtual void closeEvent(QCloseEvent * event);
	virtual void timerEvent(QTimerEvent * event);

	Texture::Format doReduceBitDepth(unsigned char * bits, int width, int height);

	void determine16BitImages(Document * document, std::set<std::string> * imgs);
	void determine16BitImages(
		SceneObject * parent, std::set<std::string> * imgs);

	void determineTexCompressImages(Document * document, std::set<std::string> * imgs);
	void determineTexCompressImages(
		SceneObject * parent, std::set<std::string> * imgs);

	void changeTo16BitFormat(
		Document * document, std::map<std::string, Texture::Format> * imgFormats);
	void changeTo16BitFormat(
		SceneObject * parent, std::map<std::string, Texture::Format> * imgFormats);

	void changeToOrigFormat(Document * document);
	void changeToOrigFormat(SceneObject * parent);

	void copyRefFile(std::vector<std::string> * copiedFiles, const std::string & absFileName, const std::string & destAbsFileName);

	bool exchangeXmltext(std::vector<std::string> & fileNames, std::map<std::string, std::vector<std::string>> & orignames, std::map<std::string, std::vector<std::string>> & newtexts);

private:
	Ui::PublishingDlg ui;

	Document * document_;
	QString publishDir_;
	QString publishFile_;
	std::vector<std::string> refFiles_;
	bool isCatalogDocument_;

	boost::thread thread_;
	
	boost::mutex mutex_;
	QString displayMsg_;
	int progress_;
	int progressMax_;
	bool cancel_;
	bool complete_;

	QString errorMsg_;
	QBasicTimer timer_;
	

	std::vector<std::string> xmlitems_;
	std::string curXml_;
	int xmlcnt_;
	std::vector<std::string> xmlfiles_;
	std::map<std::string, std::vector<std::string>> origNames_;
	std::map<std::string, std::vector<std::string>> destNames_;
};