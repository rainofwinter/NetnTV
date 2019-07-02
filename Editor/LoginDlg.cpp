#include "stdafx.h"
#include "LoginDlg.h"

#ifdef Q_WS_WIN
#include <IPHlpApi.h>
#endif

LoginDlg::LoginDlg(QWidget * parent) : QDialog(parent), settings_("Net&TV", "Studio4UX")
{
	ui.setupUi(this);
	setWindowTitle("Login");
	setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
	setFixedSize(width(), height());

	ui.pwLineEdit->setEchoMode(QLineEdit::Password);
	ui.pwLineEdit->setInputMethodHints(
		Qt::ImhHiddenText | Qt::ImhNoPredictiveText | Qt::ImhNoAutoUppercase);

	isCheckTrialState_ = false;
	isNetworkConnected_ = false;
	isRememberId_ = false;
	isRememberPw_ = false;
	isDialogShow_ = false;
	isLoging_ = false;
	isLogoutRequesting_ = false;

	network_ = new QNetworkAccessManager(this);

	connect(ui.idLineEdit, SIGNAL(ui.idLineEdit->returnPressed()), this, SLOT(onLogin()));
	connect(ui.pwLineEdit, SIGNAL(ui.pwLineEdit->returnPressed()), this, SLOT(onLogin()));
	connect(ui.loginButton, SIGNAL(clicked()), this, SLOT(onLogin()));
	connect(ui.rememberIDCheckBox, SIGNAL(toggled(bool)), this, SLOT(rememberIdChecked(bool)));
	connect(network_, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));

	initDialog();
}

LoginDlg::~LoginDlg()
{
	delete network_;
}

void LoginDlg::onLogin()
{
	setLogMode(LogMode::ModeLogin);
	idQString_ = ui.idLineEdit->text();
	pwQString_ = ui.pwLineEdit->text();
	encodedPassword_ = QByteArray(QCryptographicHash::hash(pwQString_.toStdString().c_str(), QCryptographicHash::Md5)).toHex();
	macAddress_ = getMacAddress();

	isRememberId_ = ui.rememberIDCheckBox->isChecked();
	isRememberPw_ = ui.rememberPWCheckBox->isChecked();

	std::string loginStr = "data[user_id]=" + idQString_.toStdString() + 
		"&data[user_pw]=" + encodedPassword_.toStdString() + 
		"&data[local_id]=" + macAddress_.toStdString();

	QByteArray loginData(loginStr.c_str());
	QNetworkRequest request(QUrl("http://login.netntv.net/login/login.php"));
	network_->post(request, loginData);
	isLoging_ = true;
	ui.loginButton->setEnabled(false);
}

void LoginDlg::onLogout()
{
	if(!isLogoutRequesting_)
	{
		setLogMode(LogMode::ModeLogout);
		std::string logoutStr = "data[user_id]=" + idQString_.toStdString() + 
			"&data[local_id]=" + macAddress_.toStdString();

		QByteArray logoutData(logoutStr.c_str());
		QNetworkRequest request(QUrl("http://login.netntv.net/login/logout.php"));
		network_->post(request, logoutData);
		isLogoutRequesting_ = true;
		QTimer::singleShot(2000, this, SLOT(illegalQuit()));
	}
}

void LoginDlg::illegalQuit()
{
	if(isLogoutRequesting_)
	{
		isNetworkConnected_ = false;
		isCheckTrialState_ = false;
		isLoging_ = false;
		isLogoutRequesting_ = false;
		ui.loginButton->setEnabled(true);
	}
}

void LoginDlg::replyFinished(QNetworkReply * reply)
{
	if(reply->error() == QNetworkReply::NoError) isNetworkConnected_ = true;
	else
	{
		isNetworkConnected_ = false;
		isCheckTrialState_ = false;
		QMessageBox::critical(0, "Studio4UX", "Network is not connected!");
	}

	QByteArray replyByte = reply->readAll();
	QString replyString = QString::fromUtf8(replyByte.constData());

	if(logMode_ == ModeLogin)
	{
		int loginReturn = parsePage(replyString);
		if(loginReturn == 200)
		{
			currentTime_ = QDateTime::currentDateTime();

			int limitSec = currentTime_.msecsTo(endTime_) / 1000;
			int limitDay = (limitSec / 86400);
			std::ostringstream s;
			s << limitDay;
			const std::string limitDayStr(s.str());

			if(limitDay <= 30)
			{
				QMessageBox msgBox;
				msgBox.setText(QString::fromStdString(idQString_.toStdString() + " is available " + limitDayStr + "days."));
				msgBox.setWindowTitle("Studio4UX");
				int ret = msgBox.exec();
				isCheckTrialState_ = true;
				accept();
			}
			else
			{
				QMessageBox msgBox;
				msgBox.setText("Login Success.");
				msgBox.setWindowTitle("Success!");
				int ret = msgBox.exec();
				isCheckTrialState_ = true;
				accept();
			}
			saveSettings();
		}
		else if(loginReturn == 201)
		{
			QMessageBox msgBox;
			msgBox.setText(QString::fromStdString(idQString_.toStdString() + " is don't exist."));
			msgBox.setWindowTitle("Failed!");
			int ret = msgBox.exec();
			isCheckTrialState_ = false;
		}
		else if(loginReturn == 202)
		{
			QMessageBox msgBox;
			msgBox.setText("Password mismatch.");
			msgBox.setWindowTitle("Failed!");
			int ret = msgBox.exec();
			isCheckTrialState_ = false;
		}
		else if(loginReturn == 203)
		{
			QMessageBox msgBox;
			msgBox.setText("Account expired.");
			msgBox.setWindowTitle("Failed!");
			int ret = msgBox.exec();
			isCheckTrialState_ = false;
		}
		else if(loginReturn == 204)
		{
			QMessageBox msgBox;
			msgBox.setText(QString::fromStdString(idQString_.toStdString() + " using other computer."));
			msgBox.setWindowTitle("Failed!");
			int ret = msgBox.exec();
			isCheckTrialState_ = false;
		}
	}

	else if(logMode_ == ModeLogout)
	{
		if(replyString.toInt())
			isCheckTrialState_ = false;
	}
	isLoging_ = false;
	isLogoutRequesting_ = false;
	ui.loginButton->setEnabled(true);
}

QString LoginDlg::getMacAddress()
{
	QString macAddress = "??:??:??:??:??:??";
#ifdef Q_WS_WIN
	PIP_ADAPTER_INFO pInfo = NULL;

	unsigned long len = 0;
	unsigned long nError;

	if(pInfo != NULL) delete pInfo;

	nError = GetAdaptersInfo(pInfo, &len);
	if(nError != 0)
	{
		pInfo = (PIP_ADAPTER_INFO)malloc(len);
		nError = GetAdaptersInfo(pInfo, &len);
	}

	if(nError == 0)
		macAddress.sprintf("%02X:%02X:%02X:%02X:%02X:%02X", 
		pInfo->Address[0], pInfo->Address[1], pInfo->Address[2], pInfo->Address[3], pInfo->Address[4], pInfo->Address[5]);

	delete pInfo;
#endif
	return macAddress;
}

void LoginDlg::closeEvent(QCloseEvent * event)
{
	emit dialogClosed();
}

void LoginDlg::saveSettings()
{
	settings_.setValue("rememberId", isRememberId_);
	settings_.setValue("rememberPw", isRememberPw_);
	
	if(isRememberId_) settings_.setValue("recentLoginID", idQString_);
	else settings_.setValue("recentLoginID", "");

	if(isRememberPw_) settings_.setValue("recentLoginPW", pwQString_);
	else settings_.setValue("recentLoginPW", "");

	settings_.setValue("lastUserMacAddress", macAddress_);
}

void LoginDlg::initDialog()
{
	ui.rememberIDCheckBox->setChecked(false);
	ui.rememberPWCheckBox->setChecked(false);

	ui.idLineEdit->setText("");
	ui.pwLineEdit->setText("");

	isRememberId_ = false;
	isRememberPw_ = false;
	isRememberId_ = settings_.value("rememberId").toBool();
	isRememberPw_ = settings_.value("rememberPw").toBool();

	if(isRememberId_)
	{
		ui.rememberIDCheckBox->setChecked(true);
		ui.idLineEdit->setText(settings_.value("recentLoginID").toString());
	}
	if(isRememberPw_)
	{
		ui.rememberPWCheckBox->setChecked(true);
		ui.pwLineEdit->setText(settings_.value("recentLoginPW").toString());
	}

	if(ui.idLineEdit->text() != "")
		ui.pwLineEdit->setFocus();
	else
		ui.idLineEdit->setFocus();
}

int LoginDlg::parsePage(QString replyString)
{
	if(!isNetworkConnected_ || replyString == "") return 0;

	QXmlStreamReader xml(replyString);

	int loginReturn = 0;
	
	while(!xml.atEnd() && !xml.hasError())
	{
		QXmlStreamReader::TokenType token = xml.readNext();

		if(token == QXmlStreamReader::StartDocument)
			continue;

		if(token == QXmlStreamReader::StartElement)
		{
			if(xml.name() == "form")
				continue;
			if(xml.name() == "login")
			{
				token = xml.readNext();
				if(token == QXmlStreamReader::Characters)
					loginReturn = xml.text().toString().toInt();
			}
			if(xml.name() == "time")
			{
				token = xml.readNext();
				if(token == QXmlStreamReader::Characters)
				{
					QString timeStamp = xml.text().toString();
					endTime_.setTime_t(timeStamp.toUInt());
				}
			}
		}
	}
	return loginReturn;
}

void LoginDlg::rememberIdChecked(bool checked)
{
	if(!checked) ui.rememberPWCheckBox->setChecked(false);
	ui.rememberPWCheckBox->setEnabled(checked);
}