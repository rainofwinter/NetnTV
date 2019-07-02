#pragma once
#include <QDialog>
#include "ui_newChannelDlg.h"
#include "Types.h"

///////////////////////////////////////////////////////////////////////////////
class Animation;
class SceneObject;
class AnimationChannel;

class NewChannelDlg : public QDialog
{
	Q_OBJECT
public:
	NewChannelDlg(QWidget * parent, Animation * animation, SceneObject * object);
	AnimationChannelSPtr createRequestedChannel() const;

private:
	Ui::NewChannelDlg ui;
	Animation * animation_;
	SceneObject * object_;
	std::vector<AnimationChannel *> channels_;	
	
};

