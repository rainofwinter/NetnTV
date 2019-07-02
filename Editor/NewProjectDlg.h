#pragma once
#include "ui_newProjectDlg.h"
class NewProjectDlg : public QDialog
{
	Q_OBJECT
public:
	NewProjectDlg(QWidget * parent);

	const QString & location() const {return location_;}
	const QString & name() const {return name_;}
	int width() const {return width_;}
	int height() const {return height_;}

	QString absFileName() const;
private slots:
	void onBrowse();
	void onOk();

private:
	Ui::NewProjectDlg ui;

	QString location_;
	QString name_;
	QString absFileName_;
	int width_;
	int height_;
};