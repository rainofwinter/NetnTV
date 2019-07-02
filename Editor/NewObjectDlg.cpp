#include "stdafx.h"
#include "NewObjectDlg.h"
#include "Scene.h"
#include "SceneObject.h"
#include "Global.h"
#include "EditorGlobal.h"
#include "EditorDocument.h"
#include "EditorObject.h"
#include "Document.h"

#include "ModelFile.h"
#include "TAnimation.h"
#include "Map.h"
#include "BoundingBoxObject.h"
#include "LightObject.h"
#include "CameraObject.h"
#include "VideoObject.h"
#include "AudioObject.h"

#include "TurnTable.h"
#include "TextTable.h"
#include "Panorama.h"
#include "ImageGrid.h"

using namespace std;


Q_DECLARE_METATYPE(SceneObject *)
Q_DECLARE_METATYPE(EditorCustomObject *)

NewObjectDlg::NewObjectDlg(EditorDocument * document, QWidget * parent) 
: QDialog(parent)
{
	document_ = document;
	ui.setupUi(this);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint); 

	Global::instance().sceneObjects(&sceneObjs_);

	vector<SceneObject *> baseObjs = sceneObjs_;	
	vector<SceneObject *> mediaObjs;
	vector<SceneObject *> utilityObjs;
	vector<SceneObject *> _3dObjs;
	vector<EditorCustomObject *> customObjs;

	vector<SceneObject *>::iterator iter;

	for (iter = baseObjs.begin(); iter != baseObjs.end();)
	{
		SceneObject * obj = *iter;
		if (obj->type() == ModelFile().type() ||
			obj->type() == Map().type() ||
			obj->type() == LightObject().type() ||
			obj->type() == CameraObject().type() ||
			obj->type() == BoundingBoxObject().type())
		{
			_3dObjs.push_back(*iter);
			iter = baseObjs.erase(iter);
		}	
		else if (
			obj->type() == AudioObject().type() ||
			obj->type() == VideoObject().type()
			)
		{
			mediaObjs.push_back(*iter);
			iter = baseObjs.erase(iter);
		}
		else if (
			obj->type() == Panorama().type() ||
			obj->type() == TurnTable().type() ||
			obj->type() == TextTable().type() ||
			obj->type() == ImageGrid().type() ||
			obj->type() == TAnimationObject().type())
		{
			utilityObjs.push_back(*iter);
			iter = baseObjs.erase(iter);		
		}
		else
			++iter;
	}

	//////
	QTreeWidgetItem * qBaseObjsItem = new QTreeWidgetItem(QStringList(tr("Base")));	
	ui.treeWidget->addTopLevelItem(qBaseObjsItem);
	qBaseObjsItem->setExpanded(true);
	qBaseObjsItem->setFont(0, QFont("", -1, QFont::Bold));
	qBaseObjsItem->setForeground(0, QColor(128, 128, 128, 255));
	BOOST_FOREACH(SceneObject * obj, baseObjs)
	{
		QString entryStr = QString::fromLocal8Bit(obj->typeStr());
		QTreeWidgetItem * qItem = new QTreeWidgetItem(qBaseObjsItem, QStringList(entryStr));
		qItem->setData(0, Qt::UserRole, QVariant::fromValue(obj));		
		ui.treeWidget->addTopLevelItem(qItem);
	}

	//////
	QTreeWidgetItem * qMediaObjsItem = new QTreeWidgetItem(QStringList(tr("Media")));	
	ui.treeWidget->addTopLevelItem(qMediaObjsItem);
	qMediaObjsItem->setExpanded(true);
	qMediaObjsItem->setFont(0, QFont("", -1, QFont::Bold));
	qMediaObjsItem->setForeground(0, QColor(128, 128, 128, 255));
	BOOST_FOREACH(SceneObject * obj, mediaObjs)
	{
		QString entryStr = QString::fromLocal8Bit(obj->typeStr());
		QTreeWidgetItem * qItem = new QTreeWidgetItem(qMediaObjsItem, QStringList(entryStr));
		qItem->setData(0, Qt::UserRole, QVariant::fromValue(obj));
		ui.treeWidget->addTopLevelItem(qItem);
	}
	//////
	QTreeWidgetItem * qUtilityObjsItem = new QTreeWidgetItem(QStringList(tr("Utility")));	
	ui.treeWidget->addTopLevelItem(qUtilityObjsItem);
	qUtilityObjsItem->setExpanded(true);
	qUtilityObjsItem->setFont(0, QFont("", -1, QFont::Bold));
	qUtilityObjsItem->setForeground(0, QColor(128, 128, 128, 255));
	BOOST_FOREACH(SceneObject * obj, utilityObjs)
	{
		QString entryStr = QString::fromLocal8Bit(obj->typeStr());
		QTreeWidgetItem * qItem = new QTreeWidgetItem(qUtilityObjsItem, QStringList(entryStr));
		qItem->setData(0, Qt::UserRole, QVariant::fromValue(obj));
		ui.treeWidget->addTopLevelItem(qItem);
	}
	//////
	QTreeWidgetItem * q3dObjsItem = new QTreeWidgetItem(QStringList(tr("3d")));	
	ui.treeWidget->addTopLevelItem(q3dObjsItem);
	q3dObjsItem->setExpanded(true);
	q3dObjsItem->setFont(0, QFont("", -1, QFont::Bold));
	q3dObjsItem->setForeground(0, QColor(128, 128, 128, 255));
	BOOST_FOREACH(SceneObject * obj, _3dObjs)
	{
		QString entryStr = QString::fromLocal8Bit(obj->typeStr());
		QTreeWidgetItem * qItem = new QTreeWidgetItem(q3dObjsItem, QStringList(entryStr));
		qItem->setData(0, Qt::UserRole, QVariant::fromValue(obj));	
		ui.treeWidget->addTopLevelItem(qItem);
	}
	
	//////
	/*
	QTreeWidgetItem * qCustomObjsItem = new QTreeWidgetItem(QStringList(tr("Custom")));	
	ui.treeWidget->addTopLevelItem(qCustomObjsItem);
	qCustomObjsItem->setExpanded(true);
	qCustomObjsItem->setFont(0, QFont("", -1, QFont::Bold));
	qCustomObjsItem->setForeground(0, QColor(128, 128, 128, 255));
	EditorGlobal::instance().editorCustomObjects(&customObjs);	
	BOOST_FOREACH(EditorCustomObject * obj, customObjs)
	{
		QString entryStr = QString::fromLocal8Bit(obj->name());
		QTreeWidgetItem * qItem = new QTreeWidgetItem(qCustomObjsItem, QStringList(entryStr));
		qItem->setData(0, Qt::UserRole, QVariant::fromValue(obj));
		ui.treeWidget->addTopLevelItem(qItem);
	}*/
	//////

	connect(ui.treeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)),
		this, SLOT(onItemSelectionChanged(QTreeWidgetItem *, QTreeWidgetItem *)));
	onItemSelectionChanged(NULL, NULL);

	connect(ui.treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)),
		this, SLOT(onItemDoubleClicked(QTreeWidgetItem *, int)));

	ui.treeWidget->header()->close();
}

void NewObjectDlg::onItemSelectionChanged(QTreeWidgetItem *, QTreeWidgetItem *)
{
	QTreeWidgetItem * item = ui.treeWidget->currentItem();
	
	QVariant data;

	if (item) data = item->data(0, Qt::UserRole);
	
	if (data.canConvert<SceneObject *>() || data.canConvert<EditorCustomObject *>())	
		ui.okButton->setEnabled(true);
	else	
		ui.okButton->setEnabled(false);	
}

SceneObject * NewObjectDlg::createSceneObject() const
{	
	QTreeWidgetItem * item = ui.treeWidget->currentItem();
	EditorScene * edScene = 
		document_->editorScene(document_->selectedScene()).get();

	QVariant data = item->data(0, Qt::UserRole);

	if (data.canConvert<SceneObject *>())
	{
		SceneObject * sceneObj = data.value<SceneObject *>();

		EditorObject * edObj = 
			EditorGlobal::instance().createEditorObject(edScene, sceneObj);
		if (edObj) 
		{
			SceneObject * obj = edObj->createSceneObject(document_);
			delete edObj;
			return obj;
		}
		else 
		{
			SceneObject * newObj = sceneObj->clone(0);
			newObj->init(document_->document()->renderer());
			return newObj;
		}
	}
	else if (data.canConvert<EditorCustomObject *>())
	{
		EditorCustomObject * custObj = data.value<EditorCustomObject *>();
		EditorCustomObject * edObj = 
			EditorGlobal::instance().createEditorCustomObject(edScene, custObj);

		SceneObject * obj = edObj->createSceneObject(document_);
		delete edObj;
		return obj;

	}
	else return NULL;
}

void NewObjectDlg::onItemDoubleClicked(QTreeWidgetItem * item, int col)
{
	if (ui.okButton->isEnabled()) accept();
}