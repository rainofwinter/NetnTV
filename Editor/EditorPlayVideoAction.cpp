#include "stdafx.h"
#include "EditorPlayVideoAction.h"
#include "PlayVideoAction.h"
#include "Animation.h"
#include "Scene.h"
#include "EditorDocument.h"
#include "Utils.h"

ActionPropertyPage * EditorPlayVideoAction::propertyPage() const
{
	return new EditorPlayVideoActionPropertyPage();
}

boost::uuids::uuid EditorPlayVideoAction::actionType() const 
{
	return PlayVideoAction().type();
}

QString EditorPlayVideoAction::toString(Action * paction) const
{
	PlayVideoAction * action = (PlayVideoAction *)paction;
	QString typeStr = QString::fromLocal8Bit(action->typeStr());
	QString startTimeStr;
	startTimeStr.sprintf("%.3fs", action->startTime());
	return typeStr + " - " + stdStringToQString(action->video()) + " : " + startTimeStr;
	
}

///////////////////////////////////////////////////////////////////////////////

EditorPlayVideoActionPropertyPage::EditorPlayVideoActionPropertyPage() :
validator_(0, 9999, 0), startTimeValidator_(0.0, 1000000.0, 3, 0)
{	
	setGroupBox(false);	
	
	

	startGroup(tr("source"));
	videoType_ = new ComboBoxProperty(tr("type"));
	videoType_->addItem(tr("file"));
	videoType_->addItem(tr("url"));
	addProperty(videoType_);
	addSpacing(4);

	videoFileName_ = new FileNameProperty(
		tr("filename"), 
		tr("Videos (*.mp4 *.avi)")
		);
	addProperty(videoFileName_, PropertyPage::Vertical);
	url_ = new EditProperty(tr("url"));
	addSpacing(4);
	addProperty(url_, PropertyPage::Vertical);
	endGroup();

	
	addSpacing(4);
	displayMode_ = new ComboBoxProperty(tr("mode"));
	displayMode_->addItem(tr("fullscreen"));
	displayMode_->addItem(tr("region"));
	addProperty(displayMode_, PropertyPage::Vertical);

	addSpacing(4);
	startTime_ = new EditProperty(tr("start time (seconds)"), &startTimeValidator_);
	addProperty(startTime_, PropertyPage::Vertical);

	addSpacing(4);
	showPlaybackControls_ = new BoolProperty(tr("show playback controls"));
	addProperty(showPlaybackControls_, PropertyPage::Vertical);

	addSpacing(4);
	startGroup("Region");

	addSpacing(4);
	x_ = new EditProperty("x", &validator_);
	addProperty(x_);

	y_ = new EditProperty("y", &validator_);
	addProperty(y_);
	
	width_ = new EditProperty("width", &validator_);
	addProperty(width_);

	height_ = new EditProperty("height", &validator_);
	addProperty(height_);
	endGroup();
	
}

EditorPlayVideoActionPropertyPage::~EditorPlayVideoActionPropertyPage()
{
}

void EditorPlayVideoActionPropertyPage::setDocument(EditorDocument * document)
{
	ActionPropertyPage::setDocument(document);		
}

void EditorPlayVideoActionPropertyPage::update()
{
	PlayVideoAction * action = (PlayVideoAction *)action_;

	QString str;
	str.sprintf("%d", action->x());
	x_->setValue(str);

	str.sprintf("%d", action->y());
	y_->setValue(str);

	str.sprintf("%d", action->width());
	width_->setValue(str);

	str.sprintf("%d", action->height());
	height_->setValue(str);

	SourceType type = action->sourceType();
	setSourceFieldsEnabled(type);
	if (type == SourceFile)
		videoFileName_->setValue(stdStringToQString(action->video()));
	else
		url_->setValue(stdStringToQString(action->video()));

	videoType_->setValue((int)type);

	
	setRegionFieldsEnabled(action->isPlayInRegion());
	int val = 0;
	if (action->isPlayInRegion()) val = 1;
	displayMode_->setValue(val);

	showPlaybackControls_->setValue(action->showPlaybackControls());
	
	startTime_->setValue(QString::number(action->startTime()));
}

void EditorPlayVideoActionPropertyPage::onChanged(Property * property)
{
	PropertyPage::onChanged(property);

	PlayVideoAction * action = (PlayVideoAction *)action_;
	

	if (property == displayMode_)
	{
		bool enabled = displayMode_->value() == 1;
		setRegionFieldsEnabled(enabled);
		action->setPlayInRegion(enabled);
	}

	if (property == showPlaybackControls_)
	{
		bool val = showPlaybackControls_->value();
		action->setShowPlaybackControls(val);
	}

	if (property == videoType_)
	{
		SourceType val = (SourceType)videoType_->value();
		setSourceFieldsEnabled(val);
		action->setSourceType(val);
		action->setVideo("");
	}

	if (property == videoFileName_)
	{
		action->setVideo(qStringToStdString(videoFileName_->value()));
	}

	if (property == url_)
	{
		action->setVideo(qStringToStdString(url_->value()));
	}

	if (property == x_)
	{
		int val = x_->value().toInt();
		action->setX(val);
	}

	if (property == y_)
	{
		int val = y_->value().toInt();
		action->setY(val);
	}

	if (property == width_)
	{
		int val = width_->value().toInt();
		action->setWidth(val);
	}

	if (property == height_)
	{
		int val = height_->value().toInt();
		action->setHeight(val);
	}

	if (property == startTime_)
	{
		action->setStartTime(boost::lexical_cast<float>(qStringToStdString(startTime_->value())));
	}

	update();
}

void EditorPlayVideoActionPropertyPage::setRegionFieldsEnabled(bool enabled)
{
	x_->setEnabled(enabled);
	y_->setEnabled(enabled);
	width_->setEnabled(enabled);
	height_->setEnabled(enabled);
}

void EditorPlayVideoActionPropertyPage::setSourceFieldsEnabled(SourceType sourceType)
{
	if (sourceType == SourceFile)
	{
		videoFileName_->setEnabled(true);
		url_->setEnabled(false);
	}
	else
	{
		videoFileName_->setEnabled(false);
		url_->setEnabled(true);
	}
		

}

bool EditorPlayVideoActionPropertyPage::isValid() const
{

	bool empty = true;
	if ((SourceType)videoType_->value() == SourceFile)
	{
		empty = videoFileName_->value().isEmpty();
	}
	else
	{
		empty = url_->value().isEmpty();
	}
	return !empty;
}