#pragma once
#include "ui_LogDlg.h"

class LogDlg : public QDialog
{
public:
	LogDlg(QWidget * parent = 0);
	~LogDlg();
	/**
	Can potentially be called from different thread
	*/
	void printLogMsg(const char * logMsg);
	void operator ()();
private:
	virtual void timerEvent(QTimerEvent *);
private:
	QBasicTimer timer_;
	boost::thread thread_;
	bool endThread_;
	boost::thread::id mainThreadId_;

	boost::mutex mutex_;
	/**
	must be called from main thread and with mutex_ locked
	*/
	void doMsgs();
	std::deque<std::string> msgsM_;


	Ui::LogDlg ui;	
};