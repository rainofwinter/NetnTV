#pragma once
#include <QDialog>
#include "ui_ImageGridDlg.h"

///////////////////////////////////////////////////////////////////////////////
class Document;
class Scene;
class SceneObject;
class GenericList;




class ImageGridDlg : public QDialog
{	

	Q_OBJECT
public:
	ImageGridDlg(
		QWidget * parent, Document * document,
		const std::vector<std::string> & fileNames);
	~ImageGridDlg();

	const std::vector<std::string> & fileNames() const {return fileNames_;}

private slots:
	void onNewFiles();
	void onDeleteFiles();
	void onOk();
	void onFileSelectionChanged();
private:
	void setupList(GenericList ** list, QWidget * container);
private:
	Ui::ImageGridDlg ui;
	GenericList * filesList_;

	Document * document_;
	
	std::vector<std::string> fileNames_;
	
};
///////////////////////////////////////////////////////////////////////////////
