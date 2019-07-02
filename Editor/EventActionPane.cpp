#include "stdafx.h"
#include "EventActionPane.h"
#include "EventListPane.h"
#include "ActionListPane.h"



EventActionPane::EventActionPane(MainWindow * mainWindow)
{	setOrientation(Qt::Vertical);

	//setFrameShape(QFrame::NoFrame);
	eventListPane_ = new EventListPane(mainWindow);
	actionListPane_ = new ActionListPane(mainWindow);

	addWidget(eventListPane_);

	addWidget(actionListPane_);
	setWindowTitle(tr("Event / Action"));
}
void EventActionPane::setDocument(EditorDocument * document)
{
	document_ = document;
	eventListPane_->setDocument(document);
	actionListPane_->setDocument(document);
}