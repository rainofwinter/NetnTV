#pragma once
#include "ui_serialDlg.h"
#include "Activation.h"

class Scene;
class SerialDlg : public QDialog
{
	Q_OBJECT
public:
	SerialDlg(const QString & activationNumber, QWidget * parent);
	~SerialDlg();

	const TrialState & trialState() const {return trialState_;}
protected slots:	
	void onOk();
	void onActivationKeyBrowse();
private:
	Ui::SerialDlg ui;

	TrialState trialState_;
};