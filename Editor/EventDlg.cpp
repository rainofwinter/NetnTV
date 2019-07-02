#include "stdafx.h"
#include "EventDlg.h"
#include "Global.h"
#include "EventListener.h"
#include "Event.h"
#include "PaneLabel.h"
#include "GenericList.h"
#include "EditorGlobal.h"
#include "EditorDocument.h"
#include "EditorScene.h"
#include "EditorEvent.h"
#include "PropertyPage.h"

using namespace std;

PropertiesWidget::PropertiesWidget()
{	
	this->setWidgetResizable(true);
	event_ = 0;
	layout_ = 0;
	emptyPage_ = 0;
}

void PropertiesWidget::setPropertyPage(EventPropertyPage * page)
{	
	propertyPage_.reset(page);	
	delete emptyPage_;

	delete layout_;
	layout_ = new QVBoxLayout;
	layout_->setSpacing(0);
	layout_->setMargin(4);
	setLayout(layout_);


	if (page)
	{
		emptyPage_ = 0;
		page->setEvent(event_);
		page->setDocument(document_);		
		page->update();		
		layout_->addWidget(page->widget());
		layout_->addItem(new QSpacerItem(
			0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
		
		connect(page, SIGNAL(changed(Property *)), this, SIGNAL(changed(Property *)));
	}
	else
	{
		QLabel * label = new QLabel(tr("No properties"));
		label->setAlignment(Qt::AlignTop);
		emptyPage_ = label;
		layout_->addWidget(emptyPage_);
		
	}
}

void PropertiesWidget::setEvent(Event * event, EventPropertyPage * propertyPage)
{
	event_ = event;
	setPropertyPage(propertyPage);	
	if (propertyPage_)
	{
		propertyPage_->setEvent(event);
		propertyPage_->update();
	}
}

bool PropertiesWidget::isValid() const
{
	if (!propertyPage_) return true;
	else return propertyPage_->isValid();
}

///////////////////////////////////////////////////////////////////////////////

EventDlg::EventDlg(EditorDocument * document, 
	SceneObject * sceneObject, EventListener * listener, QWidget * parent)
: 
QDialog(parent), document_(document), sceneObject_(sceneObject)
{
	ui.setupUi(this);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint); 

	ui.okButton->setEnabled(false);

	QVBoxLayout * layout = new QVBoxLayout;
	layout->setMargin(0);
	layout->setSpacing(0);
	ui.widget1->setLayout(layout);
	eventsLabel_ = new PaneLabel(tr("Supported Events"));
	layout->addWidget(eventsLabel_);
	eventsList_ = new GenericList;	
	layout->addWidget(eventsList_);

	QVBoxLayout * layout2 = new QVBoxLayout;
	layout2->setMargin(0);
	layout2->setSpacing(0);
	ui.widget2->setLayout(layout2);
	propertiesLabel_ = new PaneLabel(tr("Event Properties"));
	layout2->addWidget(propertiesLabel_);
	properties_ = new PropertiesWidget;
	layout2->addWidget(properties_);
	
	Global::instance().supportedEvents(&events_, sceneObject);

	connect(eventsList_, SIGNAL(selectionChanged()),
		this, SLOT(onCurrentIndexChanged()));

	connect(properties_, SIGNAL(changed(Property *)), 
		this, SLOT(onPropertyChanged(Property *)));

	if (events_.empty())
	{
		eventsList_->setEnabled(false);		
		eventsList_->addTopLevelItem("No available events", 0);
	}
	else
	{		
		BOOST_FOREACH(Event * event, events_)						
			eventsList_->addTopLevelItem(event->typeStr(), event);	
	}

	if (listener)
	{
		int index = 0;
		for (index = 0; index < (int)events_.size(); ++index)
		{		
			if (events_[index]->type() == listener->event()->type())
				break;
		}
		eventsList_->selectIndex(index);
		setEvent(listener->event());		
	}
	else
	{
		eventsList_->selectIndex(0);
	}
}

EventDlg::EventDlg(EditorDocument * document, AppObject * appObject, 
		EventListener * listener, QWidget * parent)
: 
QDialog(parent), document_(document), appObject_(appObject)
{
	ui.setupUi(this);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint); 

	ui.okButton->setEnabled(false);

	QVBoxLayout * layout = new QVBoxLayout;
	layout->setMargin(0);
	layout->setSpacing(0);
	ui.widget1->setLayout(layout);
	eventsLabel_ = new PaneLabel(tr("Supported Events"));
	layout->addWidget(eventsLabel_);
	eventsList_ = new GenericList;	
	layout->addWidget(eventsList_);

	QVBoxLayout * layout2 = new QVBoxLayout;
	layout2->setMargin(0);
	layout2->setSpacing(0);
	ui.widget2->setLayout(layout2);
	propertiesLabel_ = new PaneLabel(tr("Event Properties"));
	layout2->addWidget(propertiesLabel_);
	properties_ = new PropertiesWidget;
	layout2->addWidget(properties_);
	
	Global::instance().supportedEvents(&events_, appObject);

	connect(eventsList_, SIGNAL(selectionChanged()),
		this, SLOT(onCurrentIndexChanged()));

	connect(properties_, SIGNAL(changed(Property *)), 
		this, SLOT(onPropertyChanged(Property *)));

	if (events_.empty())
	{
		eventsList_->setEnabled(false);		
		eventsList_->addTopLevelItem("No available events", 0);
	}
	else
	{		
		BOOST_FOREACH(Event * event, events_)						
			eventsList_->addTopLevelItem(event->typeStr(), event);	
	}

	if (listener)
	{
		int index = 0;
		for (index = 0; index < (int)events_.size(); ++index)
		{		
			if (events_[index]->type() == listener->event()->type())
				break;
		}
		eventsList_->selectIndex(index);
		setEvent(listener->event());		
	}
	else
	{
		eventsList_->selectIndex(0);
	}
}

void EventDlg::onCurrentIndexChanged()
{
	ui.okButton->setEnabled(false);
	int index = eventsList_->selectedIndex();
	if (index < 0) return;	 	
	if (!eventsList_->isEnabled()) return;

	setEvent(events_[index]);
}

void EventDlg::setEvent(Event * event)
{
	EditorScene * edScene = 
		document_->editorScene(document_->selectedScene()).get();

	EditorEvent * edEvent = 
		EditorGlobal::instance().editorEvent(event);

	event_.reset(event->clone());

	EventPropertyPage * evPropPage = 0;
	if (edEvent) evPropPage = edEvent->propertyPage();

	properties_->setEvent(event_.get(), evPropPage);	

	setOkButtonState();
}

void EventDlg::onPropertyChanged(Property * property)
{
	setOkButtonState();
}

void EventDlg::setOkButtonState()
{
	ui.okButton->setEnabled(
		eventsList_->selectedIndex() >= 0 && properties_->isValid());
}

Event * EventDlg::createRequestedEvent() const
{
	int index = eventsList_->selectedIndex();
	if (index < 0) return 0;

	return event_->clone();
}