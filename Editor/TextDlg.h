#pragma once
#include "ui_textDlg.h"

class Scene;
class Text_2;

class TextDlg : public QDialog
{
	Q_OBJECT
public:
	TextDlg(Text_2 * text, QWidget * parent);
	
	const std::wstring & textString() const {return textString_;}
private slots:
	void onOk();
private:
	Ui::TextDlg ui;
	std::wstring textString_;

};