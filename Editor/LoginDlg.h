#pragma once
#include "ui_LoginDlg.h"
#include <QtNetwork>

class LoginDlg : public QDialog
{
	Q_OBJECT

public:
	enum LogMode
	{
		ModeLogin,
		ModeLogout
	};

public:
	LoginDlg(QWidget * parent);
	~LoginDlg();

	bool checkTrialState() { return isCheckTrialState_; }

	QString loginId() { return idQString_; }
	QString macAddress() { return macAddress_; }
	QString limitDay() { return limitDay_; }

	LogMode logMode() { return logMode_; }
	void initDialog();

public:
	bool isDialogShow_;

signals:
	void dialogClosed();

public slots:
	void onLogin();
	void onLogout();

private slots:
	void replyFinished(QNetworkReply * reply);
	void rememberIdChecked(bool);
	void illegalQuit();
	int parsePage(QString replyString);

private:
	QString getMacAddress();
	void setLogMode(LogMode modeLog){ logMode_ = modeLog; }
	void saveSettings();

protected:
	virtual void closeEvent(QCloseEvent * event);

public:
	QNetworkAccessManager * network_;

private:
	Ui::LoginDlg ui;

	QSettings settings_;

	QString idQString_;
	QString pwQString_;
	QString encodedPassword_;
	QString macAddress_;
	QString limitDay_;

	bool isCheckTrialState_;
	bool isRememberId_;
	bool isRememberPw_;
	bool isNetworkConnected_;
	bool isLoging_;
	bool isLogoutRequesting_;

	LogMode logMode_;

	QDateTime currentTime_;
	QDateTime endTime_;
	QDateTime leftTime_;
};