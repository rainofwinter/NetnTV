#include "stdafx.h"
#include "NewAppObjectDlg.h"
#include "Scene.h"
#include "AppObject.h"
#include "Global.h"
#include "EditorGlobal.h"
#include "EditorDocument.h"
#include "EditorAppObject.h"

using namespace std;

NewAppObjectDlg::NewAppObjectDlg(EditorDocument * document, QWidget * parent) 
: QDialog(parent)
{
	document_ = document;
	ui.setupUi(this);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint); 

	Global::instance().appObjects(&sceneObjs_);

	BOOST_FOREACH(AppObject * obj, sceneObjs_)
	{
		ui.listWidget->addItem(QString::fromLocal8Bit(obj->typeStr()));
	}

	connect(ui.listWidget, SIGNAL(itemSelectionChanged()),
		this, SLOT(onItemSelectionChanged()));
	onItemSelectionChanged();

	connect(ui.listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem *)),
		this, SLOT(onItemDoubleClicked(QListWidgetItem *)));
}

void NewAppObjectDlg::onItemSelectionChanged()
{
	int row = ui.listWidget->currentIndex().row();

	ui.okButton->setEnabled(row >= 0);
}

AppObject * NewAppObjectDlg::createAppObject() const
{
	int index = ui.listWidget->currentIndex().row();

	
	EditorScene * edScene = 
		document_->editorScene(document_->selectedScene()).get();

	EditorAppObject * edObj = 
		EditorGlobal::instance().createEditorAppObject(edScene, sceneObjs_[index]);
	if (edObj) 
	{
		AppObject * obj = edObj->createAppObject();
		delete edObj;
		return obj;
	}
	else 
	{
		AppObject * newObj = sceneObjs_[index]->clone();
		newObj->init();
		return newObj;
	}
}

void NewAppObjectDlg::onItemDoubleClicked(QListWidgetItem * item)
{
	accept();
}