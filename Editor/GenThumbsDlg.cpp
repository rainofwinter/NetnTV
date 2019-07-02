#include "stdafx.h"
#include "GenThumbsDlg.h"
#include "Document.h"
#include "Scene.h"
#include "GLWidget.h"
#include "Camera.h"
#include "Utils.h"
#include "EditorDocument.h"

using namespace boost;
using namespace std;

///////////////////////////////////////////////////////////////////////////////

GenThumbsDlg::GenThumbsDlg(				   
	EditorDocument * document,
	const QString & folder, const std::vector<Scene *> & scenes, 
	int thumbWidth, int thumbHeight, QWidget * parent) 

: 
	QDialog(parent), scenesToRender_(scenes), document_(document)
{
	ui.setupUi(this);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint & ~Qt::WindowCloseButtonHint); 
	
	folder_ = folder;

	thumbWidth_ = thumbWidth;
	thumbHeight_ = thumbHeight;

	requestCancel_ = false;
	connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(onCancel()));	
	ui.okButton->setEnabled(false);
	ui.progressBar->setRange(0, (int)scenesToRender_.size());
	ui.progressBar->setValue(0);
	

	curScene_ = 0;
/*
	textureRenderer_ = new TextureRenderer;
	Camera camera;
	camera.SetTo2DArea(
		thumbWidth/2, thumbHeight/2,
		thumbWidth, thumbHeight, 60, thumbWidth / thumbHeight);
	textureRenderer_->init(thumbWidth, thumbHeight);

	texture_ = new Texture;
	texture_->init(thumbWidth, thumbHeight, Texture::FormatRgba32, 0);
	textureRenderer_->setTexture(texture_);
	*/
	tempBuffer_ = (unsigned char *)malloc((thumbWidth+1) * (thumbHeight+1) * 2);
}

GenThumbsDlg::~GenThumbsDlg()
{
	timer_.stop();
	/*
	delete textureRenderer_;
	delete texture_;
	*/
	free(tempBuffer_);
}

void GenThumbsDlg::onCancel()
{
	requestCancel_ = true;	
}

void GenThumbsDlg::closeEvent(QCloseEvent * event)
{
	event->ignore();
	onCancel();	
}

void GenThumbsDlg::timerEvent(QTimerEvent * event)
{	
	if (requestCancel_) reject();
	if (curScene_ >= (int)scenesToRender_.size()) return;

	GfxRenderer * renderer = document_->renderer();
	GLWidget * glWidget = document_->glWidget();
	Scene * scene = scenesToRender_[curScene_];

	bool wasInit = scene->isinit();
	if (!wasInit) 
	{
		scene->init(renderer);
	}

	while(!scene->isLoaded())
	{
		scene->asyncLoadUpdate();
		/*
		Important: processEvents needs to be here. Otherwise something like the
		following can happen:

		The program is currently waiting for some scenes to load (maybe to render
		in the scenes list pane). But this is done through timer events.
		If the program is caught in this while loop, progress may never be made
		in the scenes list pane associated scene loading process.
		*/
		QApplication::processEvents();
		boost::this_thread::sleep(boost::posix_time::milliseconds(1));
	}

	ui.statusLabel->setText(tr("Generating thumbnail for scene: ") + 
		stdStringToQString(scene->name()));
	
	Camera camera;
	
	glWidget->screenShot(tempBuffer_, scene, thumbWidth_, thumbHeight_, false);	
	if (!wasInit) scene->uninit();	

	QImage image(tempBuffer_, thumbWidth_, thumbHeight_, QImage::Format_RGB16);

	QString filename;
	filename.sprintf("/thumb%d.jpg", curScene_);
	filename = folder_ + filename;
	bool saveRet = image.save(filename, "jpg", 100);
	thumbFileMap_[scene] = filename;
	
	
	if (!saveRet)
	{	
		QString msg = "Could not save file: " + filename;
		QMessageBox::information(this, tr("Error"), msg);
		reject();
		return;
	}

	curScene_++;
	ui.progressBar->setValue(curScene_);
	
	if (curScene_ >= (int)scenesToRender_.size()) 
	{
		ui.okButton->setEnabled(true);
		ui.cancelButton->setEnabled(false);
	}

}

void GenThumbsDlg::showEvent(QShowEvent * event)
{
	timer_.start(10, this);
}