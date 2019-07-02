#include "stdafx.h"
#include "AnimationPane.h"
#include "AnimationListPane.h"
#include "TimeLinePane.h"
#include "KeyFramePane.h"
#include "ChannelListPane.h"
#include "EditorDocument.h"

///////////////////////////////////////////////////////////////////////////////

AnimationPane::AnimationPane(MainWindow * mainWindow)
{
	setWindowTitle(tr("Animations"));

	animationsPane_ = new AnimationListPane(mainWindow);
	addWidget(animationsPane_);

	channelsPane_ = new ChannelListPane(mainWindow);
	addWidget(channelsPane_);

	timeLinePane_ = new TimeLinePane();
	addWidget(timeLinePane_);

	keyFramePane_ = new KeyFramePane(mainWindow);
	addWidget(keyFramePane_);

	setStretchFactor(0, 0);
	setStretchFactor(1, 0);
	setStretchFactor(2, 1);
	setStretchFactor(3, 0);

	setCollapsible(0, true);
	setCollapsible(1, false);
	setCollapsible(2, false);
	setCollapsible(3, true);

	setHandleWidth(4);

	connect(channelsPane_, SIGNAL(rangeChanged(int, int)), 
		this, SLOT(rangeChangedChannels(int, int)));

	connect(channelsPane_, SIGNAL(valueChanged(int)), 
		this, SLOT(valueChangedChannels(int)));

	connect(timeLinePane_, SIGNAL(valueChanged(int)), 
		this, SLOT(valueChangedTimeline(int)));

}

AnimationPane::~AnimationPane()
{
	delete animationsPane_;
	delete channelsPane_;
	delete timeLinePane_;
	delete keyFramePane_;
}

void AnimationPane::rangeChangedChannels(int min, int max)
{
	timeLinePane_->setVScrollRange(min, max);
}

void AnimationPane::valueChangedChannels(int value)
{
	timeLinePane_->setVScrollValue(value);
}

void AnimationPane::valueChangedTimeline(int value)
{
	channelsPane_->setVScrollValue(value);
}

void AnimationPane::setDocument(EditorDocument * document)
{
	document_ = document;
	if (document_) setEnabled(true);
	else setEnabled(false);

	animationsPane_->setDocument(document);
	channelsPane_->setDocument(document);
	timeLinePane_->setDocument(document);
	keyFramePane_->setDocument(document);
}

