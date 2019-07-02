#pragma once
#include <QDialog>
#include "ui_InsertAdDlg.h"

///////////////////////////////////////////////////////////////////////////////
class EditorDocument;

class InsertAdDlg : public QDialog
{
	Q_OBJECT
public:
	enum Type
	{
		TypeImage1,
		TypeImage2,
		TypeImage3,
		TypeText1,
		TypeText2,
		TypeMobile
	};
public:
	InsertAdDlg(EditorDocument * document, QWidget * parent);

private slots:

	void onImage1Radio();
	void onImage2Radio();
	void onImage3Radio();
	void onText1Radio();
	void onText2Radio();
	void onMobileRadio();
	
	void onOk();
	void onRegister();

private:
	QString typeToStr(Type type) const;
	QSize typeToDim(Type type) const;
	void handleRadio(Type type);
private:
	Ui::InsertAdDlg ui;

	Type type_;

	EditorDocument * document_;
	
};

