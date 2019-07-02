#include "stdafx.h"
#include "ObjectListPane.h"
#include "SceneObjectListPane.h"
#include "AppObjectListPane.h"

ObjectListPane::ObjectListPane(MainWindow * mainWindow)
{	
	setWindowTitle(tr("Objects"));

	mainWindow_ = mainWindow;
	appObjectListPane_ = new AppObjectListPane(mainWindow);	
	sceneObjectListPane_ = new SceneObjectListPane(mainWindow);
	
	layout_ = new QVBoxLayout;
	layout_->setMargin(0);
	layout_->setSpacing(0);
	layout_->addWidget(sceneObjectListPane_);
	setLayout(layout_);

	mode_ = MainWindow::ModeSceneObject;
}

ObjectListPane::~ObjectListPane()
{
	delete appObjectListPane_;
	delete sceneObjectListPane_;
}

void ObjectListPane::setObjectMode(MainWindow::ObjectMode mode)
{
	if (mode_ == mode) return;
	mode_ = mode;
	layout_->takeAt(0);
	
	switch(mode)
	{
	case MainWindow::ModeAppObject:			
		layout_->addWidget(appObjectListPane_);	
		appObjectListPane_->show();
		sceneObjectListPane_->hide();
		break;
	case MainWindow::ModeSceneObject:		
		layout_->addWidget(sceneObjectListPane_);
		sceneObjectListPane_->show();
		appObjectListPane_->hide();
		break;
	}
}

QSize ObjectListPane::sizeHint() const
{
	return QSize(200,150);
}

void ObjectListPane::setDocument(EditorDocument * document)
{
	sceneObjectListPane_->setDocument(document);
	appObjectListPane_->setDocument(document);
}
