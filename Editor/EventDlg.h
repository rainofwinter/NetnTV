#pragma once
#include <QDialog>
#include "ui_EventDlg.h"
#include "Types.h"


///////////////////////////////////////////////////////////////////////////////
class Property;
class EditorDocument;
class EventPropertyPage;

class PropertiesWidget : public QScrollArea
{
	Q_OBJECT
public:
	PropertiesWidget();
	virtual QSize sizeHint() const {return QSize(150, 150);}
	void setEvent(Event * event, EventPropertyPage * propertyPage);	
	void setDocument(EditorDocument * document) {document_ = document;}
	bool isValid() const;

signals:
	void changed(Property *);
private:
	void setPropertyPage(EventPropertyPage * page);
private:
	boost::scoped_ptr<EventPropertyPage> propertyPage_;

	Event * event_;
	EditorDocument * document_;
	QVBoxLayout * layout_;

	QWidget * emptyPage_;
};

///////////////////////////////////////////////////////////////////////////////
class GenericList;
class EditorDocument;
class Animation;
class Property;
class SceneObject;
class AppObject;
class AnimationChannel;
class PaneLabel;


class EventDlg : public QDialog
{
	Q_OBJECT
public:
	EventDlg(EditorDocument * document, SceneObject * sceneObject, 
		EventListener * listener, QWidget * parent);

	EventDlg(EditorDocument * document, AppObject * appObject, 
		EventListener * listener, QWidget * parent);

	Event * createRequestedEvent() const;

	void setOkButtonState();

private slots:
	void onCurrentIndexChanged();
	void onPropertyChanged(Property * property);

private:
	void setEvent(Event * event);
private:
	Ui::EventDlg ui;
	EditorDocument * document_;

	SceneObject * sceneObject_;
	AppObject * appObject_;
	std::vector<Event *> events_;

	GenericList * eventsList_;
	PaneLabel * eventsLabel_;

	PropertiesWidget * properties_;
	PaneLabel * propertiesLabel_;

	boost::scoped_ptr<Event> event_;	
};

