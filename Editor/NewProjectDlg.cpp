#include "stdafx.h"
#include "NewProjectDlg.h"
#include "EditorGlobal.h"
#include "Utils.h"

NewProjectDlg::NewProjectDlg(QWidget * parent) : QDialog(parent)
{
	ui.setupUi(this);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint); 

	ui.widthEdit->setText("768");
	ui.heightEdit->setText("1024");

	connect(ui.browseButton, SIGNAL(clicked()), this, SLOT(onBrowse()));
	connect(ui.okButton, SIGNAL(clicked()), this, SLOT(onOk()));

	QString directory = EditorGlobal::instance().settings()->
		value("projectLocation").toString();
	ui.locationEdit->setText(directory);
}


void NewProjectDlg::onBrowse()
{
	//QString directory = QFileDialog::getExistingDirectory();
	QString directory = getExistingDirectory();
	if (!directory.isEmpty())
	{
		ui.locationEdit->setText(directory);
	}
}

QString NewProjectDlg::absFileName() const
{
	return absFileName_;
}

void NewProjectDlg::onOk()
{
	#define mkdir mkdir
	QString inputtedDirectory = ui.locationEdit->text();
	QString name = ui.nameEdit->text();

	QString directory = inputtedDirectory;

	if (directory.isEmpty() || name.isEmpty())
	{
		QMessageBox::information(0, tr("Error"), 
			tr("Location and name cannot be empty"));	
		return;
	}

	if (!directory.endsWith('/') && !directory.endsWith('\\'))
		directory += "/";

	if (!QDir().exists(directory))
	{
		QMessageBox::information(0, tr("Error"), tr("Location does not exist"));
		return;
	}

	if (QDir().exists(directory + name))
	{
		QMessageBox::information(
			0, tr("Error"), tr("Directory with specified name already exists"));
		return;
	}

	if (
		!QDir().mkdir(directory + name) ||
		!QDir().mkdir(directory + name + "/Res") ||
		!QDir().mkdir(directory + name + "/Stock")		
		)
	{
		QMessageBox::information(
			0, tr("Error"), tr("Could not create project directory"));
		return;
	}	

	width_ = ui.widthEdit->text().toInt();
	height_ = ui.heightEdit->text().toInt();
	location_ = ui.locationEdit->text();
	name_ = ui.nameEdit->text();

	QDir().setCurrent(directory + name);

	EditorGlobal::instance().settings()->
		setValue("projectLocation", inputtedDirectory);

	
	absFileName_ = QDir().current().absolutePath() + "/" + name + ".st";

	accept();
}