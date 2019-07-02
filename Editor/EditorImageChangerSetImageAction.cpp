#include "stdafx.h"
#include "EditorImageChangerSetImageAction.h"
#include "ImageChangerSetImageAction.h"
#include "Scene.h"
#include "EditorDocument.h"
#include "Document.h"
#include "ImageChanger.h"
#include "Utils.h"

using namespace std;

ActionPropertyPage * EditorImageChangerSetImageAction::propertyPage() const
{
	return new EditorImageChangerSetImagePropertyPage();
}

boost::uuids::uuid EditorImageChangerSetImageAction::actionType() const 
{
	return ImageChangerSetImageAction().type();
}

QString EditorImageChangerSetImageAction::toString(Action * pAction) const
{
	ImageChangerSetImageAction * action = (ImageChangerSetImageAction *)pAction;
	QString typeStr = QString::fromLocal8Bit(action->typeStr());
	
	
	if (action->mode() == ImageChangerSetImageAction::Specific)
	{
		QString str;
		str.sprintf("%d", action->imageIndex());
		return typeStr + " - " + stdStringToQString(action->targetObject()->id()) + " - " + str;
	}
	else
	{
		if (action->direction() == ImageChangerSetImageAction::Previous)
			return typeStr + " - Previous";
		else
			return typeStr + " - Next";
	}
}

///////////////////////////////////////////////////////////////////////////////
EditorImageChangerSetImagePropertyPage::EditorImageChangerSetImagePropertyPage()
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

void EditorImageChangerSetImagePropertyPage::setModeEnabled()
{
	ImageChangerSetImageAction::ChangeMode mode = (ImageChangerSetImageAction::ChangeMode)mode_->value();
	if (mode == ImageChangerSetImageAction::Specific)
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

EditorImageChangerSetImagePropertyPage::~EditorImageChangerSetImagePropertyPage()
{
}

void EditorImageChangerSetImagePropertyPage::setDocument(EditorDocument * document)
{
	ActionPropertyPage::setDocument(document);	

}

void EditorImageChangerSetImagePropertyPage::update()
{
	ImageChangerSetImageAction * action = (ImageChangerSetImageAction *)action_;
	
	QString str;
	str.sprintf("%d", action->imageIndex());
	imageIndex_->setValue(str);

	mode_->setValue((int)action->mode());
	direction_->setValue((int)action->direction());
	setModeEnabled();

}

void EditorImageChangerSetImagePropertyPage::onChanged(Property * property)
{
	PropertyPage::onChanged(property);
	ImageChangerSetImageAction * action = (ImageChangerSetImageAction *)action_;

	int index = imageIndex_->value().toInt();
	action->setImageIndex(index);

	action->setMode((ImageChangerSetImageAction::ChangeMode)mode_->value());
	action->setDirection((ImageChangerSetImageAction::Direction)direction_->value());	

	update();
}

bool EditorImageChangerSetImagePropertyPage::isValid() const
{
	return 
		(mode_->value() == (int)ImageChangerSetImageAction::Specific && !imageIndex_->value().isEmpty()) ||
		(mode_->value() == (int)ImageChangerSetImageAction::Adjacent && direction_->value() >= 0);
}