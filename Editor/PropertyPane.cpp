#include "stdafx.h"
#include "PropertyPane.h"
#include "EditorDocument.h"
#include "EditorScene.h"
#include "EditorObject.h"
#include "PropertyPage.h"
#include "Scene.h"
#include "SceneObject.h"
#include "MainWindow.h"

using namespace std;

PropertyPane::PropertyPane()
{
	ignoreModeChanged_ = false;
	document_ = 0;
	curPropertyPage_ = 0;
	curPageWidget_ = 0;
	setWidgetResizable(true);
	widget_ = new QWidget;
	layout_ = new QVBoxLayout;	
	layout_->setSpacing(0);
	layout_->setMargin(4);
	widget_->setLayout(layout_);
	setWidget(widget_);

	setWindowTitle(tr("Properties"));

	layout_->addItem(new QSpacerItem(
		0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));

	//setFrameShape(QFrame::NoFrame);

	mode_ = ModeSceneObject;
}

PropertyPane::~PropertyPane()
{
	//prevent curPageWidget_ from being automatically deleted by qt
	//it will be deleted in the PropertyPage destructor
	if (curPageWidget_) curPageWidget_->setParent(0);
}


void PropertyPane::setObjectMode(PropertyPane::ObjectMode mode)
{	
	mode_ = mode;

	//trigger a property page update
	if (mode == ModeSceneObject)
		objectSelectionChanged();
	else
		appObjectSelectionChanged();
}

void PropertyPane::closeEvent(QCloseEvent * event)
{
	QScrollArea::closeEvent(event);
}

void PropertyPane::setDocument(EditorDocument * document)
{
	if (document_) document_->disconnect(this);
	
	document_ = document;

	if (document_)
	{
		connect(document_, SIGNAL(objectSelectionChanged()),
			this, SLOT(objectSelectionChanged()));		

		connect(document_, SIGNAL(appObjectSelectionChanged()),
			this, SLOT(appObjectSelectionChanged()));	

		objectSelectionChanged();
		appObjectSelectionChanged();

		connect(document_, SIGNAL(objectChanged()), 
			this, SLOT(updateCurPage()));

		connect(document_, SIGNAL(appObjectChanged()), 
			this, SLOT(updateCurPage()));

		connect(document_, SIGNAL(sceneChanged()),
			this, SLOT(updateCurPage()));

		connect(document_, SIGNAL(timeLineTimeChanged()), 
			this, SLOT(updateCurPage()));

		setEnabled(true);
	}
	else
	{
		setPage(0);
		setEnabled(false);
	}
}

void PropertyPane::setView(GLWidget * widget)
{
	view_ = widget;
}

QSize PropertyPane::sizeHint() const
{
	return QSize(200, 50);
}

void PropertyPane::objectSelectionChanged()
{
	//object selection has changed, need to display the appropriate property 
	//page
	
	if (mode_ == ModeAppObject) return;
	
	SceneObject * obj = document_->selectedObject();
	if (!obj) obj = (SceneObject *)document_->selectedScene()->root();			
	setPage(document_->getPropertyPage(obj));	
	
	if (curPropertyPage_) curPropertyPage_->update();	
}

void PropertyPane::appObjectSelectionChanged()
{
	if (mode_ == ModeSceneObject) return;
	

	AppObject * obj = document_->selectedAppObject();
	if (obj)
	{
		setPage(document_->getPropertyPage(obj));
	}
	else
	{
		SceneObject * rootObj = 
			(SceneObject *)document_->selectedScene()->root();
		setPage(document_->getPropertyPage(rootObj));
	}
	
	if (curPropertyPage_) curPropertyPage_->update();
}

void PropertyPane::updateCurPage()
{
	if (curPropertyPage_) curPropertyPage_->update();
}

void PropertyPane::setPage(PropertyPage * page)
{
	if (curPropertyPage_)
	{			
		/*
		setDocument(NULL) must come after hide().
		hide() can trigger an onChanged signal. In response to this onChanged 
		signal, the current property page may use the current document 
		reference to modify the currently selected object.
		If the current document reference is NULL, this will lead to a crash.
		*/
		layout_->removeWidget(curPageWidget_);
		curPageWidget_->hide();
		curPageWidget_->setParent(0);
		curPropertyPage_->setDocument(NULL);
		curPropertyPage_ = 0;
		curPageWidget_ = 0;
	}

	curPropertyPage_ = page;
	if (!page) return;

	curPropertyPage_->setDocument(document_);

	curPageWidget_ = page->widget();	
	layout_->insertWidget(0, curPageWidget_);
	curPageWidget_->show();
	curPropertyPage_->setView(view_);
	//curPropertyPage_->setCircularTabOrder();
	
	
}

void PropertyPane::keyPressEvent(QKeyEvent * keyEvent)
{
	if(keyEvent->key() == (Qt::Key_Return || Qt::Key_Enter)) update();
}