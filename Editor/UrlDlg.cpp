#include "StdAfx.h"
#include "UrlDlg.h"
#include "Document.h"
#include "Scene.h"
#include "SceneObject.h"
#include "Global.h"
#include "EditorGlobal.h"
#include "EditorObject.h"
#include "Utils.h"

#include "EditorVideoObject.h"

using namespace std;

UrlDlg::UrlDlg(QWidget * parent, Document * document)
{
	ui.setupUi(this);

	ui.comboBox->addItem(tr("<default>"));
	ui.comboBox->addItem(tr("Android"));
	ui.comboBox->addItem(tr("Ios"));
	ui.comboBox->addItem(tr("PC"));

//	ui.comboBox->setCurrentIndex(0);
//	ui.comboBox->setEnabled(false);

	ui.comboBox1->addItem(tr("<default>"));
	ui.comboBox1->addItem(tr("Android"));
	ui.comboBox1->addItem(tr("Ios"));
	ui.comboBox1->addItem(tr("PC"));

//	ui.comboBox1->setCurrentIndex(1);
//	ui.comboBox1->setEnabled(false);

	ui.comboBox2->addItem(tr("<default>"));
	ui.comboBox2->addItem(tr("Android"));
	ui.comboBox2->addItem(tr("Ios"));
	ui.comboBox2->addItem(tr("PC"));

//	ui.comboBox2->setCurrentIndex(2);
//	ui.comboBox2->setEnabled(false);

	ui.comboBox3->addItem(tr("<default>"));
	ui.comboBox3->addItem(tr("Android"));
	ui.comboBox3->addItem(tr("Ios"));
	ui.comboBox3->addItem(tr("PC"));

//	ui.comboBox3->setCurrentIndex(3);
//	ui.comboBox3->setEnabled(false);
	
	connect(ui.okButton, SIGNAL(clicked()), this, SLOT(onOk()));
	connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}

UrlDlg::~UrlDlg()
{
}

void UrlDlg::onOk() 
{
	defaltCheck = 0;
	androidCheck = 0;
	iosCheck = 0;
	pcCheck = 0;

	if (!ui.lineEdit->text().isEmpty())
	{
		sourceName_.push_back(qStringToStdString(ui.lineEdit->text()));
		sourceMode_.push_back(ui.comboBox->currentIndex());
	}
	if(!ui.lineEdit1->text().isEmpty())
	{
		sourceName_.push_back(qStringToStdString(ui.lineEdit1->text()));
		sourceMode_.push_back(ui.comboBox1->currentIndex());
	}
	if(!ui.lineEdit2->text().isEmpty())
	{
		sourceName_.push_back(qStringToStdString(ui.lineEdit2->text()));
		sourceMode_.push_back(ui.comboBox2->currentIndex());
	}
	if(!ui.lineEdit3->text().isEmpty())
	{
		sourceName_.push_back(qStringToStdString(ui.lineEdit3->text()));
		sourceMode_.push_back(ui.comboBox3->currentIndex());
	}

	for(int i = 0; i < sourceMode_.size(); i++)
	{
		if (sourceMode_[i] == 0)
		{
			defaltCheck++;
		}else if (sourceMode_[i] == 1){
			androidCheck++;
		}else if(sourceMode_[i] == 2){
			iosCheck++;
		}else if(sourceMode_[i] == 3){
			pcCheck++;
		}
	}
	if (defaltCheck > 1 || androidCheck > 1 || iosCheck > 1 || pcCheck > 1)
	{
		sourceName_.clear();
		sourceMode_.clear();
		QMessageBox::information(this, tr("Error"), tr("The same item is not available."));
	}else {
		accept();
	}
}

void UrlDlg::setting(std::vector<std::string> sourceName, std::vector<int> sourceMode)
{
	for (int i = 0; i < sourceName.size() ; i++)
	{
		if(i == 0)
		{
			QString str = stdStringToQString(sourceName[i]);
			ui.lineEdit->setText(str);				
		}else if(i == 1){
			QString str = stdStringToQString(sourceName[i]);
			ui.lineEdit1->setText(str);	
		}else if(i == 2){
			QString str = stdStringToQString(sourceName[i]);
			ui.lineEdit2->setText(str);	
		}else if(i == 3){
			QString str = stdStringToQString(sourceName[i]);
			ui.lineEdit3->setText(str);	
		}
	}
	for (int i = 0; i < sourceMode.size() ; i++)
	{
		if(i == 0)
		{
			ui.comboBox->setCurrentIndex(sourceMode[i]);
		}else if(i == 1){
			ui.comboBox1->setCurrentIndex(sourceMode[i]);
		}else if(i == 2){
			ui.comboBox2->setCurrentIndex(sourceMode[i]);
		}else if(i == 3){
			ui.comboBox3->setCurrentIndex(sourceMode[i]);
		}
	}
}


