#include "stdafx.h"
#include "NewChannelDlg.h"
#include "Animation.h"
#include "AnimationChannel.h"
#include "SceneObject.h"
#include "Global.h"

using namespace std;

NewChannelDlg::NewChannelDlg(QWidget * parent, 
	Animation * animation, SceneObject * object)
: QDialog(parent)
{
	ui.setupUi(this);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint); 

	animation_ = animation;
	object_ = object;

	const vector<AnimationChannelSPtr> * curChannels = animation->channels(object);
	std::vector<AnimationChannel *> allChannels_;
	Global::instance().supportedAnimationChannels(&allChannels_, object);
	foreach(AnimationChannel * channel, allChannels_)
	{
		bool addChannel = true;
		if (curChannels)
		{
			foreach(AnimationChannelSPtr curChannel, *curChannels)
			{
				if (curChannel->type() == channel->type())
					addChannel = false;
			}
		}

		if (addChannel)
		{
			ui.comboBox->addItem(QString::fromLocal8Bit(channel->name()));
			channels_.push_back(channel);
		}
	}	

	if (channels_.empty()) 
	{
		ui.okButton->setEnabled(false);
		ui.comboBox->addItem(tr("No available unused attributes"));
		ui.comboBox->setEnabled(false);
	}
}

AnimationChannelSPtr NewChannelDlg::createRequestedChannel() const
{	
	int index = ui.comboBox->currentIndex();
	AnimationChannel * newChannel = channels_[index]->clone();
	newChannel->setObject(object_);
	return AnimationChannelSPtr(newChannel);
}
