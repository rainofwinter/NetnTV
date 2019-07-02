#include "stdafx.h"
#include "SceneObject.h"
#include "EditorVideoPlayAction.h"
#include "VideoPlayVideoAction.h"
#include "Utils.h"

using namespace std;

ActionPropertyPage * EditorVideoPlayAction::propertyPage() const
{
	return new EditorVideoPlayPropertyPage();
}

boost::uuids::uuid EditorVideoPlayAction::actionType() const 
{
	return VideoPlayVideoAction().type();
}

QString EditorVideoPlayAction::toString(Action * pAction) const
{
	VideoPlayVideoAction * action = (VideoPlayVideoAction *)pAction;

	QString startTimeStr;
	startTimeStr.sprintf("%.3fs", action->startTime());
	QString typeStr =  
		QString::fromLocal8Bit(action->typeStr()) + 
		" - " + 
		stdStringToQString(action->targetObject()->id()) + 
		" : " + 
		startTimeStr;
	return typeStr;
}

///////////////////////////////////////////////////////////////////////////////
EditorVideoPlayPropertyPage::EditorVideoPlayPropertyPage()
	: startTimeValidator_(0.0, 1000000.0, 3, 0)
{	
	setGroupBox(false);

	startTime_ = new EditProperty(tr("start time (seconds)"), &startTimeValidator_);
	addProperty(startTime_, PropertyPage::Vertical);

}

EditorVideoPlayPropertyPage::~EditorVideoPlayPropertyPage()
{
}

void EditorVideoPlayPropertyPage::setDocument(EditorDocument * document)
{
	ActionPropertyPage::setDocument(document);	

}

void EditorVideoPlayPropertyPage::update()
{
	VideoPlayVideoAction * action = (VideoPlayVideoAction *)action_;
	startTime_->setValue(QString::number(action->startTime()));

}

void EditorVideoPlayPropertyPage::onChanged(Property * property)
{
	PropertyPage::onChanged(property);
	VideoPlayVideoAction * action = (VideoPlayVideoAction *)action_;

	if (property == startTime_)
	{
		action->setStartTime(boost::lexical_cast<float>(qStringToStdString(startTime_->value())));
	}

	update();
}

bool EditorVideoPlayPropertyPage::isValid() const
{
	return true;
}