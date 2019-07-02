#include "stdafx.h"
#include "EditorImageSetFileAction.h"
#include "ImageSetFileAction.h"
#include "Animation.h"
#include "Scene.h"
#include "EditorDocument.h"
#include "Utils.h"

ActionPropertyPage * EditorImageSetFileAction::propertyPage() const
{
	return new EditorImageSetFileActionPropertyPage();
}

boost::uuids::uuid EditorImageSetFileAction::actionType() const 
{
	return ImageSetFileAction().type();
}

QString EditorImageSetFileAction::toString(Action * paction) const
{
	ImageSetFileAction * action = (ImageSetFileAction *)paction;
	QString typeStr = QString::fromLocal8Bit(action->typeStr());
	return typeStr + " - " + stdStringToQString(action->fileName());
	
}

///////////////////////////////////////////////////////////////////////////////
EditorImageSetFileActionPropertyPage::EditorImageSetFileActionPropertyPage()
{	
	setGroupBox(false);	
	
	fileName_ = new FileNameProperty(
		tr("filename"), tr("Images (*.png *.jpg *.jpeg)"));
	addProperty(fileName_, PropertyPage::Vertical);
	
}

EditorImageSetFileActionPropertyPage::~EditorImageSetFileActionPropertyPage()
{
}

void EditorImageSetFileActionPropertyPage::setDocument(EditorDocument * document)
{
	ActionPropertyPage::setDocument(document);		
}

void EditorImageSetFileActionPropertyPage::update()
{
	ImageSetFileAction * action = (ImageSetFileAction *)action_;

	fileName_->setValue(stdStringToQString(action->fileName()));
	

}

void EditorImageSetFileActionPropertyPage::onChanged(Property * property)
{
	PropertyPage::onChanged(property);

	ImageSetFileAction * action = (ImageSetFileAction *)action_;
	action->setFileName(qStringToStdString(fileName_->value()));	

	update();
}

bool EditorImageSetFileActionPropertyPage::isValid() const
{
	return !fileName_->value().isEmpty();
}