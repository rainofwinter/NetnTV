#pragma once

class Document;
class EditorDocument;

class PdfToTextTemp
{
public:
	PdfToTextTemp(EditorDocument * document, const QString & pdfFilename, QWidget * parent);
	~PdfToTextTemp() {}

	void readXml(const QString & filename, QProgressDialog * progress1);
	QString importPdfWithoutText(const QString & fileName, const float & width, const float & height);
private:
	EditorDocument * document_;
	QString pdfFilename_;
	QString pdf2imgDir_;
	QWidget * parent_;
};