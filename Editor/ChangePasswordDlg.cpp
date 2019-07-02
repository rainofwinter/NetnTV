#include "stdafx.h"
#include "ChangePasswordDlg.h"

ChangePasswordDlg::ChangePasswordDlg(QWidget * parent) : QDialog(parent)
{
	ui.setupUi(this);
	setWindowTitle("Change Account Password");
	setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
	setFixedSize(width(), height());

	ui.currentPWLineEdit->setEchoMode(QLineEdit::Password);
	ui.currentPWLineEdit->setInputMethodHints(Qt::ImhHiddenText | Qt::ImhNoPredictiveText | Qt::ImhNoAutoUppercase);
	ui.newPWLineEdit->setEchoMode(QLineEdit::Password);
	ui.newPWLineEdit->setInputMethodHints(Qt::ImhHiddenText | Qt::ImhNoPredictiveText | Qt::ImhNoAutoUppercase);
	ui.confirmPWLineEdit->setEchoMode(QLineEdit::Password);
	ui.confirmPWLineEdit->setInputMethodHints(Qt::ImhHiddenText | Qt::ImhNoPredictiveText | Qt::ImhNoAutoUppercase);

	network_ = new QNetworkAccessManager(this);

	connect(ui.sendButton, SIGNAL(clicked()), this, SLOT(onSend()));
	connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(ui.idLineEdit, SIGNAL(ui.idLineEdit->returnPressed()), this, SLOT(onSend()));
	connect(ui.currentPWLineEdit, SIGNAL(ui.currentPWLineEdit->returnPressed()), this, SLOT(onSend()));
	connect(ui.newPWLineEdit, SIGNAL(ui.newPWLineEdit->returnPressed()), this, SLOT(onSend()));
	connect(ui.confirmPWLineEdit, SIGNAL(ui.confirmPWLineEdit->returnPressed()), this, SLOT(onSend()));
	connect(network_, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));

	initDialog();
}

ChangePasswordDlg::~ChangePasswordDlg()
{
	delete network_;
}

void ChangePasswordDlg::initDialog()
{
	ui.idLineEdit->setText("");
	ui.currentPWLineEdit->setText("");
	ui.newPWLineEdit->setText("");
	ui.confirmPWLineEdit->setText("");

	ui.idLineEdit->setFocus();
}

void ChangePasswordDlg::onSend()
{
	isConnecting_ = true;
	ui.sendButton->setEnabled(false);
	id_ = ui.idLineEdit->text();
	password_ = QByteArray(QCryptographicHash::hash(ui.currentPWLineEdit->text().toStdString().c_str(), QCryptographicHash::Md5)).toHex();
	newPassword_ = QByteArray(QCryptographicHash::hash(ui.newPWLineEdit->text().toStdString().c_str(), QCryptographicHash::Md5)).toHex();
	confirmPassword_ = QByteArray(QCryptographicHash::hash(ui.confirmPWLineEdit->text().toStdString().c_str(), QCryptographicHash::Md5)).toHex();

	if(newPassword_ != confirmPassword_)
	{
		QMessageBox msgBox;
		msgBox.setText("New password mismatch confirm password");
		int ret = msgBox.exec();
		ui.sendButton->setEnabled(true);
	}
	else
	{
		std::string networkArguments = "data[user_id]=" + id_.toStdString() + "&data[user_pw]=" + 
			password_.toStdString() + "&data[npw]=" + newPassword_.toStdString();
		QByteArray byteArguments(networkArguments.c_str());
		QNetworkRequest request(QUrl("http://login.netntv.net/login/modify_pw.php"));
		network_->post(request, byteArguments);
	}
}

void ChangePasswordDlg::replyFinished(QNetworkReply * reply)
{
	if(reply->error() != QNetworkReply::NoError)
	{
		QMessageBox::critical(0, "Studio4UX", "Network is not connected!");
		accept();
	}

	QByteArray replyByte = reply->readAll();
	QString replyString = QString::fromUtf8(replyByte.constData());

	int resultChange = parsePage(replyString);

	if(resultChange == 200)
	{
		QMessageBox msgBox;
		msgBox.setText("Changed password.");
		int ret = msgBox.exec();
		accept();
	}
	else if(resultChange == 201)
	{
		QMessageBox msgBox;
		msgBox.setText(QString::fromStdString(ui.idLineEdit->text().toStdString() + " is don't exist."));
		int ret = msgBox.exec();
	}
	else if(resultChange == 202)
	{
		QMessageBox msgBox;
		msgBox.setText("Error!");
		int ret = msgBox.exec();
	}

	isConnecting_ = false;
	ui.sendButton->setEnabled(true);
}

int ChangePasswordDlg::parsePage(QString replyString)
{
	QXmlStreamReader xml(replyString);

	int resultChange;

	while(!xml.atEnd() && !xml.hasError())
	{
		QXmlStreamReader::TokenType token = xml.readNext();
		if(token == QXmlStreamReader::StartDocument)
			continue;

		if(token == QXmlStreamReader::StartElement)
		{
			if(xml.name() == "form")
				continue;

			if(xml.name() == "change")
			{
				token = xml.readNext();
				if(token == QXmlStreamReader::Characters)
					resultChange = xml.text().toString().toInt();
			}
		}
	}
	return resultChange;
}