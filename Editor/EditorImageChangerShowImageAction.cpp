#include "stdafx.h"
#include "EditorImageChangerShowImageAction.h"
#include "ImageChangerShowImageAction.h"
#include "Scene.h"
#include "EditorDocument.h"
#include "Document.h"
#include "ImageChanger.h"
#include "Utils.h"

using namespace std;

ActionPropertyPage * EditorImageChangerShowImageAction::propertyPage() const
{
	return new EditorImageChangerShowImagePropertyPage();
}

boost::uuids::uuid EditorImageChangerShowImageAction::actionType() const 
{
	return ImageChangerShowImageAction().type();
}

QString EditorImageChangerShowImageAction::toString(Action * pAction) const
{
	ImageChangerShowImageAction * action = (ImageChangerShowImageAction *)pAction;
	QString typeStr = QString::fromLocal8Bit(action->typeStr());
	

	if (action->mode() == ImageChangerShowImageAction::Specific)
	{
		QString str;
	str.sprintf("%d", action->imageIndex());
	return typeStr + " - " + stdStringToQString(action->targetObject()->id()) + " - " + str;
	}
	else
	{
		if (action->direction() == ImageChangerShowImageAction::Previous)
			return typeStr + " - Previous";
		else
			return typeStr + " - Next";
	}
	
}

///////////////////////////////////////////////////////////////////////////////
EditorImageChangerShowImagePropertyPage::EditorImageChangerShowImagePropertyPage()
: validator_(0, 9999, 0)
{	
	setGroupBox(false);
	
	mode_ = new ComboBoxProperty(tr("mode"));
	addProperty(mode_);
	mode_->addItem(tr("Specific"));
	mode_->addItem(tr("Adjacent"));

	addSpacing(4);	
	imageIndex_ = new EditProperty(tr("Image index"), &validator_);
	addProperty(imageIndex_, PropertyPage::Vertical);	

	addSpacing(4);
	direction_ = new ComboBoxProperty(tr("direction"));
	addProperty(direction_, PropertyPage::Vertical);
	direction_->addItem(tr("Previous"));
	direction_->addItem(tr("Next"));

	setModeEnabled();
	
}

void EditorImageChangerShowImagePropertyPage::setModeEnabled()
{
	ImageChangerShowImageAction::ChangeMode mode = (ImageChangerShowImageAction::ChangeMode)mode_->value();
	if (mode == ImageChangerShowImageAction::Specific)
	{
		imageIndex_->setEnabled(true);
		direction_->setEnabled(false);
	}
	else
	{
		imageIndex_->setEnabled(false);
		direction_->setEnabled(true);
	}
}

EditorImageChangerShowImagePropertyPage::~EditorImageChangerShowImagePropertyPage()
{
}

void EditorImageChangerShowImagePropertyPage::setDocument(EditorDocument * document)
{
	ActionPropertyPage::setDocument(document);	

}

void EditorImageChangerShowImagePropertyPage::update()
{
	ImageChangerShowImageAction * action = (ImageChangerShowImageAction *)action_;
	
	QString str;
	str.sprintf("%d", action->imageIndex());
	imageIndex_->setValue(str);

	mode_->setValue((int)action->mode());
	direction_->setValue((int)action->direction());
	setModeEnabled();
}

void EditorImageChangerShowImagePropertyPage::onChanged(Property * property)
{
	PropertyPage::onChanged(property);
	ImageChangerShowImageAction * action = (ImageChangerShowImageAction *)action_;

	int index = imageIndex_->value().toInt();
	action->setImageIndex(index);

	action->setMode((ImageChangerShowImageAction::ChangeMode)mode_->value());
	action->setDirection((ImageChangerShowImageAction::Direction)direction_->value());	

	update();
}

bool EditorImageChangerShowImagePropertyPage::isValid() const
{
	return 
		(mode_->value() == (int)ImageChangerShowImageAction::Specific && !imageIndex_->value().isEmpty()) ||
		(mode_->value() == (int)ImageChangerShowImageAction::Adjacent && direction_->value() >= 0);
}