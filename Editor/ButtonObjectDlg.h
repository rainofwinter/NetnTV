#pragma once
#include <QDialog>
#include "ui_buttonImage.h"
/////////////////////////////////////////////////////////////

class Document;
class Scene;
class SceneObject;

class ButtonObjectDlg : public QDialog
{
	Q_OBJECT
public:
	ButtonObjectDlg(
		QWidget * parent, Document * document,const std::vector<std::string> & fileNames);
	~ButtonObjectDlg();

	const std::vector<std::string> & pressFileName() const {return pressFileName_;}
	const std::vector<std::string> & releaseFileName() const {return releaseFileName_;}
	const std::vector<std::string> & fileNames() const {return fileNames_;}
	
private slots:
	void onPressImg();
	void onReleaseImg();
	void onOk();
private:
	Ui::ButtonObject ui;

	Document * document_;
	
	std::vector<std::string> pressFileName_, releaseFileName_, fileNames_;
};