#include "StdAfx.h"
#include "ButtonObjectDlg.h"
#include "Document.h"
#include "Scene.h"
#include "SceneObject.h"
#include "Global.h"
#include "EditorGlobal.h"
#include "EditorObject.h"
#include "Utils.h"

using namespace std;

ButtonObjectDlg::ButtonObjectDlg(
	QWidget * parent, Document * document,
	const vector<string> & files) : QDialog(parent)
{
	ui.setupUi(this);

	ui.lineEdit->setReadOnly(true);
	ui.lineEdit_2->setReadOnly(true);

	ui.okButton->setEnabled(false);
	connect(ui.okButton, SIGNAL(clicked()), this, SLOT(onOk()));
	connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	connect(ui.pressButton, SIGNAL(clicked()), this, SLOT(onPressImg()));
	connect(ui.releaseButton, SIGNAL(clicked()), this, SLOT(onReleaseImg()));
}

ButtonObjectDlg::~ButtonObjectDlg()
{
}

void ButtonObjectDlg::onPressImg()
{
	QStringList fileNames = getOpenFileNames(QObject::tr("Images"), QString(), 
		QObject::tr("Images (*.png *.jpg *.jpeg)"));
	BOOST_FOREACH(QString file, fileNames)
	{
		pressFileName_.clear();
		ui.lineEdit->setText(convertToRelativePath(file));
		pressFileName_.push_back(qStringToStdString(ui.lineEdit->text()));
	}
	if (!ui.lineEdit->text().isEmpty() && !ui.lineEdit_2->text().isEmpty())
	{
		ui.okButton->setEnabled(true);
	}
}

void ButtonObjectDlg::onReleaseImg()
{
	QStringList fileNames = getOpenFileNames(QObject::tr("Images"), QString(), 
		QObject::tr("Images (*.png *.jpg *.jpeg)"));
	BOOST_FOREACH(QString file, fileNames)
	{
		releaseFileName_.clear();
		ui.lineEdit_2->setText(convertToRelativePath(file));
		releaseFileName_.push_back(qStringToStdString(ui.lineEdit_2->text()));
	}
	if (!ui.lineEdit->text().isEmpty() && !ui.lineEdit_2->text().isEmpty())
	{
		ui.okButton->setEnabled(true);
	}
}

void ButtonObjectDlg::onOk()
{
	if (!ui.lineEdit->text().isEmpty() && !ui.lineEdit_2->text().isEmpty())
	{
		fileNames_.clear();
		fileNames_.push_back(pressFileName_[0]);
		fileNames_.push_back(releaseFileName_[0]);
		accept();
	}
}
