#pragma once
#include "ui_DecompressingDlg.h"

class DecompressingDlg : public QDialog
{
	Q_OBJECT
public:
	enum State
	{
		InProgress,
		Error,
		Success,
		Cancelled
	};
public:
	DecompressingDlg(
		const QString & fileName, const QString & direcotry, 
		QWidget * parent);
	~DecompressingDlg();
	void operator ()();

	State state() const {return state_;}
private slots:
	void onCancel();
private:
	virtual void timerEvent(QTimerEvent * event);
	virtual void showEvent(QShowEvent * event);
	virtual void closeEvent(QCloseEvent * event);
private:
	Ui::DecompressingDlg ui;
	QBasicTimer timer_;

	int progressBarRange_;
	QString fileName_, directory_;

	boost::thread thread_;	
	boost::mutex mutex_;

	State state_;
	bool ended_;
	float progress_;
	
};