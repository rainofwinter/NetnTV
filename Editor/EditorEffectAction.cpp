#include "stdafx.h"
#include "EditorEffectAction.h"
#include "EffectAction.h"
#include "SceneObject.h"
#include "Utils.h"

ActionPropertyPage * EditorEffectAction::propertyPage() const
{
	return new EditorEffectActionPropertyPage();
}

boost::uuids::uuid EditorEffectAction::actionType() const 
{
	return EffectAction().type();
}

QString EditorEffectAction::toString(Action * action) const
{
	EffectAction * effectAction = (EffectAction *)action;
	QString typeStr = QString::fromLocal8Bit(effectAction->typeStr());
	QString effectStr;
	switch(effectAction->effect())
	{
	case EffectAction::FadeIn: effectStr = "Fade in"; break;
	case EffectAction::FadeOut: effectStr = "Fade out"; break;
	case EffectAction::FlipDown: effectStr = "Flip down"; break;	
	case EffectAction::SpinHorz: effectStr = "Spin horizontally"; break;
	case EffectAction::SpinVert: effectStr = "Spin vertically"; break;
	}
	return typeStr + " - " + stdStringToQString(action->targetObject()->id()) + " - " + effectStr;
	
}

///////////////////////////////////////////////////////////////////////////////
EditorEffectActionPropertyPage::EditorEffectActionPropertyPage() :
validator_(0, 99999, 4, 0)
{	
	setGroupBox(false);
	
	effect_ = new ComboBoxProperty(tr("effect"));
	addProperty(effect_, PropertyPage::Vertical);
	effect_->addItem(tr("Fade in"));
	effect_->addItem(tr("Fade out"));
	effect_->addItem(tr("Flip down"));
	effect_->addItem(tr("Spin horizontally"));
	effect_->addItem(tr("Spin vertically"));
	duration_ = new EditProperty(tr("duration"));
	addProperty(duration_, PropertyPage::Vertical);
	
}

EditorEffectActionPropertyPage::~EditorEffectActionPropertyPage()
{
}

void EditorEffectActionPropertyPage::setDocument(EditorDocument * document)
{
	ActionPropertyPage::setDocument(document);	
}

void EditorEffectActionPropertyPage::update()
{
	EffectAction * action = (EffectAction *)action_;
	QString str;
	str.sprintf("%.4f", action->duration());
	duration_->setValue(str);

	effect_->setValue((int)action->effect());
	
	
}

void EditorEffectActionPropertyPage::onChanged(Property * property)
{
	PropertyPage::onChanged(property);
	EffectAction * action = (EffectAction *)action_;	

	action->setDuration(duration_->value().toFloat());

	action->setEffect((EffectAction::Effect)effect_->value());

	update();
}


bool EditorEffectActionPropertyPage::isValid() const
{
	return true;
}