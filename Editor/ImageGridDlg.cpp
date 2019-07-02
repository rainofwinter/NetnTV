#include "stdafx.h"
#include "ImageGridDlg.h"
#include "GenThumbsDlg.h"
#include "Document.h"
#include "Scene.h"
#include "SceneObject.h"
#include "Global.h"
#include "EditorGlobal.h"
#include "EditorObject.h"
#include "GenericList.h"
#include "Utils.h"

using namespace std;


ImageGridDlg::ImageGridDlg(
	QWidget * parent, Document * document,
	const vector<string> & files) : QDialog(parent)
{
	document_ = document;
	ui.setupUi(this);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint); 
	
	setupList(&filesList_, ui.filesListWidget);
	
	BOOST_FOREACH(string file, files)
	{
		filesList_->addTopLevelItem(stdStringToQString(file), 0);
	}

	filesList_->setReorderable(true);
	filesList_->setEditable(false);
	
	ui.okButton->setEnabled(!filesList_->items().empty());

	connect(ui.addImagesButton, SIGNAL(clicked()), this, SLOT(onNewFiles()));
	connect(ui.removeImagesButton, SIGNAL(clicked()), this, SLOT(onDeleteFiles()));
	connect(ui.okButton, SIGNAL(clicked()), this, SLOT(onOk()));
	connect(filesList_, SIGNAL(selectionChanged()), this, SLOT(onFileSelectionChanged()));

	onFileSelectionChanged();
}

void ImageGridDlg::setupList(GenericList ** list, QWidget * container)
{
	*list = new GenericList();
	QHBoxLayout * layout = new QHBoxLayout();
	layout->setMargin(0);
	layout->setSpacing(0);
	layout->addWidget(*list);
	container->setLayout(layout);
}

ImageGridDlg::~ImageGridDlg()
{
}

void ImageGridDlg::onNewFiles()
{
	QStringList fileNames = getOpenFileNames(QObject::tr("Images"), QString(), 
		QObject::tr("Images (*.png *.jpg *.jpeg)"));

	BOOST_FOREACH(QString file, fileNames)
	{
		filesList_->addTopLevelItem(convertToRelativePath(file), 0);
	}
	ui.okButton->setEnabled(!filesList_->items().empty());
}

void ImageGridDlg::onFileSelectionChanged()
{
	const vector<GenericListItem *> & selItems = filesList_->selectedItems();
	ui.removeImagesButton->setEnabled(!selItems.empty());
	
}

void ImageGridDlg::onDeleteFiles()
{
	filesList_->deleteSelectedItems();
	ui.okButton->setEnabled(!filesList_->items().empty());
}


void ImageGridDlg::onOk()
{
	const vector<GenericListItem *> & items = filesList_->items();
	fileNames_.clear();
	BOOST_FOREACH(GenericListItem * item, items)
		fileNames_.push_back(qStringToStdString(item->string));
	accept();
}
