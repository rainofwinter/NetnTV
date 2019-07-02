#pragma once
#include <QDialog>
#include "ui_UrlDlg.h"
#include <EditorVideoObject.h>

////////////////////////////////////////////////////////////////
class Document;
class Scene;
class SceneObject;

class UrlDlg : public QDialog
{
	Q_OBJECT
public:
	UrlDlg(QWidget * parent, Document * document);
	~UrlDlg();

	void setting(std::vector<std::string> sourceName, std::vector<int> sourceMode);
	const std::vector<std::string> & sourceNames() const {return sourceName_;}
	const std::vector<int> & sourceMode() const {return sourceMode_;}

private slots:
	void onOk();
private:
	Ui::Dialog ui;

	Document * document_;

	std::vector<std::string> sourceName_;
	std::vector<int> sourceMode_;

	int defaltCheck, androidCheck, iosCheck, pcCheck;
};
