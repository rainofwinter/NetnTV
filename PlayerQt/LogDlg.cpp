#include "stdafx.h"
#include "LogDlg.h"
using namespace boost;

LogDlg::LogDlg(QWidget * parent) : QDialog(parent)
{
	ui.setupUi(this);	
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint); 

	ui.logEdit->setReadOnly(true);
	ui.logEdit->setMaximumBlockCount(4192);
	mainThreadId_ = this_thread::get_id();
	timer_.start(10, this);
	
}

void LogDlg::printLogMsg(const char * logMsg)
{
	unique_lock<mutex> lk(mutex_);
	msgsM_.push_back(logMsg);

	if (mainThreadId_ == this_thread::get_id())
	{
		doMsgs();
	}

	lk.unlock();
	
}

void LogDlg::doMsgs()
{
	while (!msgsM_.empty())
	{
		const std::string & str = msgsM_.front();
		if (!this->isVisible())this->show();
		QString msg = QString::fromUtf8(str.c_str());
		ui.logEdit->appendPlainText(msg);	
		msgsM_.pop_front();
	}
}

void LogDlg::timerEvent(QTimerEvent *)
{
	//printf("LogDlg::timerEvent\n");
	unique_lock<mutex> lk(mutex_);
	doMsgs();
	lk.unlock();
}

LogDlg::~LogDlg()
{
	timer_.stop();
	
}
