#include "stdafx.h"
#include "EditorOpenUrlAction.h"
#include "OpenUrlAction.h"
#include "Animation.h"
#include "Scene.h"
#include "EditorDocument.h"
#include "Utils.h"

ActionPropertyPage * EditorOpenUrlAction::propertyPage() const
{
	return new EditorOpenUrlActionPropertyPage();
}

boost::uuids::uuid EditorOpenUrlAction::actionType() const 
{
	return OpenUrlAction().type();
}

QString EditorOpenUrlAction::toString(Action * paction) const
{
	OpenUrlAction * action = (OpenUrlAction *)paction;
	QString typeStr = stdStringToQString(action->typeStr());
	return typeStr + " - " + stdStringToQString(action->url());
	
}

///////////////////////////////////////////////////////////////////////////////
EditorOpenUrlActionPropertyPage::EditorOpenUrlActionPropertyPage()
{	
	setGroupBox(false);
	
	url_ = new EditProperty(tr("url"));
	addProperty(url_, PropertyPage::Vertical);

//	addSpacing(4);

//	mode_ = new ComboBoxProperty(tr("mode"));
//	mode_->addItem(tr("Regular"));
//	mode_->addItem(tr("Mobile"));
//	addProperty(mode_, PropertyPage::Vertical);
	
}

EditorOpenUrlActionPropertyPage::~EditorOpenUrlActionPropertyPage()
{
}

void EditorOpenUrlActionPropertyPage::setDocument(EditorDocument * document)
{
	ActionPropertyPage::setDocument(document);		
}

void EditorOpenUrlActionPropertyPage::update()
{
	OpenUrlAction * action = (OpenUrlAction *)action_;
	url_->setValue(stdStringToQString(action->url()));
//	mode_->setValue((int)action->mode());
	
}

void EditorOpenUrlActionPropertyPage::onChanged(Property * property)
{
	PropertyPage::onChanged(property);

	OpenUrlAction * action = (OpenUrlAction *)action_;
	action->setUrl(qStringToStdString(url_->value()));

//	action->setMode((OpenUrlAction::Mode)mode_->value());
	update();
}

bool EditorOpenUrlActionPropertyPage::isValid() const
{
	return !url_->value().isEmpty();// && mode_->value() >= 0;
}