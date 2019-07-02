#pragma once
#include "ui_changePasswordDlg.h"
#include <QtNetwork>

class ChangePasswordDlg : public QDialog
{
	Q_OBJECT

public:
	ChangePasswordDlg(QWidget * parent);
	~ChangePasswordDlg();

private slots:
	void initDialog();
	void onSend();
	void replyFinished(QNetworkReply * reply);
	int parsePage(QString replyString);

private:
	Ui::ChangePasswordDlg ui;

	QNetworkAccessManager * network_;

	QString id_;
	QString password_;
	QString newPassword_;
	QString confirmPassword_;

	bool isConnecting_;
};