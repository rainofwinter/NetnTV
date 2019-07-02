#include "stdafx.h"
#include "EditorShareAction.h"
#include "ShareAction.h"

//
//#include "Scene.h"
#include "EditorDocument.h"
#include "Utils.h"
#include "SceneObject.h"
#include "Image.h"


ActionPropertyPage * EditorShareAction::propertyPage() const
{
	return new EditorShareActionPropertyPage();
}

boost::uuids::uuid EditorShareAction::actionType() const 
{
	return ShareAction().type();
}

QString EditorShareAction::toString(Action * paction) const
{
	ShareAction * action = (ShareAction *)paction;
	QString typeStr = QString::fromLocal8Bit(action->typeStr());
		
	if(action->targetType() == ShareAction::TextTarget)
		return typeStr + " - " + "Text";
	
	else if(action->targetType() == ShareAction::UrlTarget)
		return typeStr + " - " + "URL";	
	/*
	else if(action->targetType() == ShareAction::ImageTarget)
		return typeStr + " - " + "Image";
	*/
}

//QString EditorShareAction::toData(Action * paction) const
//{
//	ShareAction * action = (ShareAction *)paction;
//	return " - " + stdStringToQString(action->message());
//}

///////////////////////////////////////////////////////////////////////////////
EditorShareActionPropertyPage::EditorShareActionPropertyPage()
{	
	setGroupBox(false);

	targetType_ = new ComboBoxProperty(tr("Shared Type"));
	targetType_->addItem(tr("Text"));	
	//targetType_->addItem(tr("URL"));
	//targetType_->addItem(tr("Image"));
	addProperty(targetType_, PropertyPage::Vertical);			

	addSpacing(4);
		
	msg_ = new EditTextProperty(tr("Message"));
	msg_->widget()->setFixedHeight(150);		//max height 200
	addProperty(msg_, PropertyPage::Vertical);	

	
	addSpacing(4);
		
	imgPath_ = new ComboBoxProperty(tr("Images"));
	imgPath_->addItem(tr(""));
	//addProperty(imgPath_, PropertyPage::Vertical);
	
	imgcnt_ = 0;	
}

EditorShareActionPropertyPage::~EditorShareActionPropertyPage()
{
}


void EditorShareActionPropertyPage::setDocument(EditorDocument * document)
{
	ActionPropertyPage::setDocument(document);		
}

void EditorShareActionPropertyPage::update()
{
	ShareAction * action = (ShareAction *)action_;	

	ShareAction::TargetType type = action->targetType();	
	targetType_->setValue((int)type);
	
	imgcnt_ = 0;
	imgPath_->clearItems();
	filePath_.clear();
	BOOST_FOREACH(const SceneSPtr & curScene, document_->document()->scenes())
	{						
		std::vector<SceneObject *> objs;
		curScene->getSceneObjectsByID(Image().type(), &objs);
		
		BOOST_FOREACH(SceneObject * scObj, objs)
		{						
			std::string imgid = scObj->id();
			QString str = QString::fromUtf8(imgid.c_str());			
			imgPath_->addItem(str);			

			Image* img = (Image*)scObj;
						
			filePath_[imgcnt_] = &(img->fileName());
			imgcnt_++;				
		}		
	}
	int imageNumber = action->imgNum();
	imgPath_->setValue(imageNumber);
	
	msg_->setValue(stdStringToQString(action->message()));	

	//reset Share action data
	std::map<int,  const std::string *>::iterator FindIter = filePath_.find( imgPath_->value() );		
	if( FindIter != filePath_.end() )
	{	
		action->setMessage(*(FindIter->second));
		action->setFilePath(FindIter->second);
	}		
}

void EditorShareActionPropertyPage::onChanged(Property * property)
{	PropertyPage::onChanged(property);

	ShareAction * action = (ShareAction *)action_;	

	if (property == targetType_)
	{
		ShareAction::TargetType type = 
			(ShareAction::TargetType)targetType_->value();		
		action->setTargetType(type);

		if(type == ShareAction::ImageTarget)
			imgPath_->setEnabled(true);
		else
			imgPath_->setEnabled(false);
	}
	if (property == msg_)
	{
		action->setMessage(qStringToStdString(msg_->value()));
	}	

	if (property == imgPath_)
	{			
		std::map<int, const std::string *>::iterator FindIter = filePath_.find( imgPath_->value() );
		
		if( FindIter != filePath_.end() )
		{
			imgPath_->setValue(FindIter->first);
			action->setImgNum(FindIter->first);
			action->setMessage(*(FindIter->second));
			action->setFilePath(FindIter->second);
		}		
	}	
	update();
}

bool EditorShareActionPropertyPage::isValid() const
{
	ShareAction::TargetType type = 
		(ShareAction::TargetType)targetType_->value();

	bool validTarget = type == ShareAction::TextTarget;// || ShareAction::UrlTarget || ShareAction::ImageTarget;

	return !msg_->value().isEmpty() && validTarget;
}