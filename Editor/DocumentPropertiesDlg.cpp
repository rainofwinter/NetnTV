#include "stdafx.h"
#include "DocumentPropertiesDlg.h"
#include "Global.h"
#include "FileUtils.h"
#include "Utils.h"
#include "EditorGlobal.h"
#include "EditorDocumentTemplate.h"
#include "EditorDocument.h"
#include "DocumentTemplate.h"
#include "Command.h"

using namespace std;

class ScriptItem : public QTreeWidgetItem
{
public:
	ScriptItem(const QString & file) : QTreeWidgetItem(QStringList(file))
	{
		this->setFlags(Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsEnabled);
	}


};

class ScriptFileItem : public QTreeWidgetItem
{
public:
	ScriptFileItem(const std::string & scriptFile, const std::string & actualFile)
	{
		this->setText(0, stdStringToQString(scriptFile));
		this->setText(1, stdStringToQString(
			convertToRelativePath(Global::instance().readDirectory(), actualFile)));

		this->actualFile = actualFile;
		this->scriptFile = scriptFile;
		this->setFlags(Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsEnabled);
	}

	std::string actualFile;
	std::string scriptFile;

};


DocumentPropertiesDlg::DocumentPropertiesDlg(EditorDocument * document, QWidget * parent) :
	QDialog(parent), document_(document)
{
	Document * sceneDoc = document_->document();
	origTemplate_ = sceneDoc->documentTemplate();

	ui.setupUi(this);	
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint); 


	ui.orientationComboBox->addItem(tr("Horizontal"));
	ui.orientationComboBox->addItem(tr("Vertical"));
	ui.orientationComboBox->addItem(tr("Adaptive"));

	ui.orientationComboBox->setCurrentIndex((int)sceneDoc->orientation());
	
	QString str;
	str.sprintf("%d", sceneDoc->width());
	ui.widthEdit->setText(str);
	str.sprintf("%d", sceneDoc->height());
	ui.heightEdit->setText(str);

	ui.multitouchCheckbox->setChecked(sceneDoc->allowMultitouch());
	//ui.preDownloadCheckBox->setChecked(origTemplate_->isPreDownload());

	Global::instance().documentTemplates(&docTemplates_);
	int i = 0;
	int tempIndex = -1;
	BOOST_FOREACH(DocumentTemplate * docTemplate, docTemplates_)
	{
		if (sceneDoc->documentTemplate()->type() == docTemplate->type())
			tempIndex = i;
		ui.templateComboBox->addItem(
			stdStringToQString(docTemplate->typeStr()));

		++i;

	}	

	ui.templateComboBox->setCurrentIndex(tempIndex);
	const vector<string> & scriptFiles = sceneDoc->scriptFiles();

	ui.scriptsTreeWidget->setHeaderHidden(true);
	ui.scriptsTreeWidget->setIndentation(0);
	//ui.scriptsTreeWidget->setFrameShape(QFrame::NoFrame);
	ui.scriptsTreeWidget->setRootIsDecorated(false);
	ui.scriptsTreeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
	ui.scriptsTreeWidget->setDragEnabled(true);
	ui.scriptsTreeWidget->setDropIndicatorShown(true);
	ui.scriptsTreeWidget->setDefaultDropAction(Qt::MoveAction);
	ui.scriptsTreeWidget->setDragDropMode(QAbstractItemView::InternalMove);

	BOOST_FOREACH(string scriptFile, scriptFiles)
	{
		if (scriptFile.empty()) continue;
		ui.scriptsTreeWidget->addTopLevelItem(
			new ScriptItem(stdStringToQString(scriptFile)));
	}


	//ui.scriptFilesTreeWidget->setHeaderHidden(true);
	QStringList labels;
	labels.push_back("script-side file name");
	labels.push_back("actual name");
	ui.scriptFilesTreeWidget->setHeaderLabels(labels);
	ui.scriptFilesTreeWidget->setColumnWidth(0, 150);
	
	ui.scriptFilesTreeWidget->setIndentation(0);
	//ui.scriptFilesTreeWidget->setFrameShape(QFrame::NoFrame);
	ui.scriptFilesTreeWidget->setRootIsDecorated(false);
	ui.scriptFilesTreeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
	ui.scriptFilesTreeWidget->setDragEnabled(true);
	ui.scriptFilesTreeWidget->setDropIndicatorShown(true);
	ui.scriptFilesTreeWidget->setDefaultDropAction(Qt::MoveAction);
	ui.scriptFilesTreeWidget->setDragDropMode(QAbstractItemView::InternalMove);

	const map<string, string> & scriptAccessibleFiles = 
		sceneDoc->scriptAccessibleFiles();
	map<string, string>::const_iterator mitr;
	for (mitr = scriptAccessibleFiles.begin(); mitr != scriptAccessibleFiles.end(); ++mitr)
	{
		ui.scriptFilesTreeWidget->addTopLevelItem(
			new ScriptFileItem((*mitr).first, (*mitr).second));
	}

	//ui.remoteServerEdit->setText(stdStringToQString(sceneDoc->remoteReadServer()));
	
	connect(ui.okButton, SIGNAL(clicked()), this, SLOT(onOk()));
	connect(ui.addScriptButton, SIGNAL(clicked()), this, SLOT(onAddScript()));
	connect(ui.removeScriptButton, SIGNAL(clicked()), this, SLOT(onRemoveScript()));

	connect(ui.addScriptFileButton, SIGNAL(clicked()), this, SLOT(onAddScriptFile()));
	connect(ui.removeScriptFileButton, SIGNAL(clicked()), this, SLOT(onRemoveScriptFile()));

	connect(ui.scriptsTreeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(onScriptsSelChanged()));
	onScriptsSelChanged();

	connect(ui.scriptFilesTreeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(onScriptFilesSelChanged()));
	onScriptFilesSelChanged();
}

DocumentPropertiesDlg::~DocumentPropertiesDlg()
{
}

void DocumentPropertiesDlg::onScriptsSelChanged()
{
	QList<QTreeWidgetItem*> selItems = ui.scriptsTreeWidget->selectedItems();
	ui.removeScriptButton->setEnabled(!selItems.isEmpty());
}

void DocumentPropertiesDlg::onScriptFilesSelChanged()
{
	QList<QTreeWidgetItem*> selItems = ui.scriptFilesTreeWidget->selectedItems();
	ui.removeScriptFileButton->setEnabled(!selItems.isEmpty());
}

Orientation DocumentPropertiesDlg::orientation() const
{
	return (Orientation)ui.orientationComboBox->currentIndex();
}

int DocumentPropertiesDlg::width() const
{
	return ui.widthEdit->text().toInt();
}

int DocumentPropertiesDlg::height() const
{
	return ui.heightEdit->text().toInt();
}

DocumentTemplateSPtr DocumentPropertiesDlg::newDocumentTemplate() const
{
	return newTemplate_;
}

std::string DocumentPropertiesDlg::remoteReadServer() const
{	
	//string server = qStringToStdString(ui.remoteServerEdit->text());
	//boost::trim(server);
	//return server;
	return "";
}

bool DocumentPropertiesDlg::allowMultitouch() const
{
	return ui.multitouchCheckbox->isChecked();
}

bool DocumentPropertiesDlg::preDownload() const
{
	//return ui.preDownloadCheckBox->isChecked();
	return false;
}

void DocumentPropertiesDlg::onOk()
{
	int index = ui.templateComboBox->currentIndex();

	if (docTemplates_[index]->type() != origTemplate_->type())
	{
		EditorDocumentTemplate * edDoc = 
			EditorGlobal::instance().editorDocumentTemplate(docTemplates_[index]);	

		newTemplate_.reset(docTemplates_[index]->newInstance());
		newTemplate_->setDocument(document_->document());
		Command * cmd;
		if (edDoc)
		{
			if (cmd = edDoc->doProperties(newTemplate_.get(), document_, this))
			{			
				cmd->doCommand();
				delete cmd;
				accept();
			}
		}
		else
		{
			accept();
		}
	}
	else
	{
		accept();
	}
}


void DocumentPropertiesDlg::onAddScript()
{
	QString scriptFile = 
		getOpenFileName(tr("Script file"), QString(), QObject::tr("Script Files (*.js)"));

	if (!scriptFile.isEmpty())
	{
		ui.scriptsTreeWidget->addTopLevelItem(
			new ScriptItem(convertToRelativePath(scriptFile)));
	}
}

void DocumentPropertiesDlg::onRemoveScript()
{
	QList<QTreeWidgetItem *> selItems = ui.scriptsTreeWidget->selectedItems();

	BOOST_FOREACH(QTreeWidgetItem * selItem, selItems)
	{
		int index = ui.scriptsTreeWidget->indexOfTopLevelItem(selItem);
		delete ui.scriptsTreeWidget->takeTopLevelItem(index);
	}
}


void DocumentPropertiesDlg::onAddScriptFile()
{
	QStringList inputFiles = 
		getOpenFileNames(tr("Script accessible file"), QString(), QObject::tr("All files (*.*)"));

	if (!inputFiles.isEmpty())
	{
		BOOST_FOREACH(QString inputFile, inputFiles)
		{
			string actualFile = convertToRelativePath(qStringToStdString(inputFile));
			string scriptFile = getFileNameWithoutDirectory(actualFile);

			map<string, string> existing = scriptAccessibleFiles();
			if (existing.find(scriptFile) != existing.end())
			{
				QMessageBox::information(0, tr("Error"), tr("file already exists"));
				return;
			}

			ui.scriptFilesTreeWidget->addTopLevelItem(
				new ScriptFileItem(scriptFile, actualFile));
		}
	}
}

void DocumentPropertiesDlg::onRemoveScriptFile()
{
	QList<QTreeWidgetItem *> selItems = ui.scriptFilesTreeWidget->selectedItems();

	BOOST_FOREACH(QTreeWidgetItem * selItem, selItems)
	{
		int index = ui.scriptFilesTreeWidget->indexOfTopLevelItem(selItem);
		delete ui.scriptFilesTreeWidget->takeTopLevelItem(index);
	}
}



vector<string> DocumentPropertiesDlg::scriptFiles() const
{
	vector<string> files;

	QTreeWidgetItemIterator iter(ui.scriptsTreeWidget);
	while(*iter)
	{
		ScriptItem * item = (ScriptItem *)(*iter);
		files.push_back(qStringToStdString(item->text(0)));
		++iter;
	}	

	return files;
}

map<string, string> DocumentPropertiesDlg::scriptAccessibleFiles() const
{
	map<string, string> files;

	QTreeWidgetItemIterator iter(ui.scriptFilesTreeWidget);
	while(*iter)
	{
		ScriptFileItem * item = (ScriptFileItem *)(*iter);		
		files[item->scriptFile] = item->actualFile;
		++iter;
	}
	return files;
}