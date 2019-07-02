#pragma once
#include "ui_MultiChoiceQuestionDlg.h"

class CustomObject;

class MultiChoiceQuestionDlg : public QDialog
{
	Q_OBJECT
public:
	MultiChoiceQuestionDlg(QWidget * parent, CustomObject * obj);
	const QString & xml() const {return xml_;}
private:
	void getDataFromObj(CustomObject * obj);
	void refreshComboBox();
private slots:
	void onAdd();
	void onDelete();
	void onOk();
	void onFontBrowse();
	void onUncheckedBrowse();
	void onCheckedBrowse();
	void onCheckAnsBrowse();
	void onChoicesSelChanged();

	void onCorrectBrowse();
	void onIncorrectBrowse();

private:
	Ui::MultiChoiceQuestionDlg ui;
	QString xml_;
};