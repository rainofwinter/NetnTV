#include "stdafx.h"
#include "PcScene.h"
#include "PcVideoPlayer.h"
#include "PcAudioPlayer.h"
#include "PcPlayerSupport.h"

#include "AppImage.h"
#include "AppWebContent.h"


#include "Camera.h"
#include "Utils.h"
#include "GfxRenderer.h"

#include "VideoControlsWidget.h"

#include <QtWebKit/QWebView>
#include <QtWebKit/QWebPage>
#include <QWebSettings>

using namespace std;


Clipper::Clipper(QGraphicsItem * item)
{
	item_ = item;
	item->setParentItem(this);
	cx_ = cy_ = cw_ = ch_ = 0.0f;
	setFlag(QGraphicsItem::ItemClipsChildrenToShape, false);
	setHandlesChildEvents(false);	
	setPen(QPen(Qt::NoPen));
}



void Clipper::setClip(bool clip, float cx, float cy, float cw, float ch)
{
	setFlag(QGraphicsItem::ItemClipsChildrenToShape, clip);
	clip_ = clip;
	cx_ = cx;
	cy_ = cy;
	cw_ = cw;
	ch_ = ch;
	this->setRect(cx, cy, cw, ch);
}


///////////////////////////////////////////////////////////////////////////////
class TimeLabel : public QLabel
{
public:
	TimeLabel() : QLabel()
	{
		QPalette pal(palette());
		pal.setColor(QPalette::Background, QColor(0, 0, 0, 16));
		setAutoFillBackground(true);
		setPalette(pal);
		setAlignment(Qt::AlignCenter);
	}
	virtual QSize sizeHint() const {return QSize(0, 5);}
};


Q_DECLARE_METATYPE(AppObject *)

PcScene::PcScene(QGLWidget * viewport, Document * document, 
				 PcVideoPlayer * videoPlayer, PcAudioPlayer * audioPlayer, PcPlayerSupport * playerSupport)
{
	viewport_ = viewport;
	document_ = document;
	videoPlayer_ = videoPlayer;
	audioPlayer_ = audioPlayer;
	isInit_ = false;
	videoPlayer_->setDocument(document);

	playerSupport->setPcScene(this);

	GLenum err=glewInit();	

	lButtonDown_ = false;
	clickedAppObject_ = 0;

	videoControlsWidget_ = new VideoControlsWidget;


	videoControlsWidget_->setVisible(false);
	

	connect(videoControlsWidget_, SIGNAL(videoPlay()), this, SLOT(onVideoPlay()));
	connect(videoControlsWidget_, SIGNAL(videoStop()), this, SLOT(onVideoStop()));
	connect(videoControlsWidget_, SIGNAL(seekPressed()), this, SLOT(onSeekPressed()));
	connect(videoControlsWidget_, SIGNAL(seekReleased()), this, SLOT(onSeekReleased()));
	connect(videoControlsWidget_, SIGNAL(seekChanged(float)), this, SLOT(onSeekChanged(float)));
	isSeekSliderPressed_ = false;
	connect(videoControlsWidget_, SIGNAL(videoFullscreen()), this, SLOT(onVideoFullscreen()));
	
	showAppLayer_ = true;
	addWidget(videoControlsWidget_);
}

PcScene::~PcScene()
{
	map<AppObject *, Clipper *>::iterator iter;
	for (iter = sceneItems_.begin(); iter != sceneItems_.end(); ++iter)
	{
		delete (*iter).second;
	}
	sceneItems_.clear();
}

void PcScene::onSeekPressed()
{
	isSeekSliderPressed_ = true;
}
void PcScene::onSeekReleased()
{
	isSeekSliderPressed_ = false;

}

void PcScene::onSeekChanged(float value)
{
	videoPlayer_->seek(value * videoPlayer_->duration());
}

void PcScene::init()
{
	document_->start();	
	document_->init();	
}

void PcScene::onVideoPlay()
{
	videoPlayer_->play();
}

void PcScene::onVideoStop()
{
	videoPlayer_->stop();
}

void PcScene::onVideoFullscreen()
{
	videoPlayer_->toggleFullscreen();
}

void PcScene::showAppLayer(bool show)
{
	showAppLayer_ = show;
}

void PcScene::drawBackground(QPainter *painter, const QRectF &rect)
{	
	painter->setRenderHint(QPainter::SmoothPixmapTransform);

	//must reinitialize opengl because it seems Qt graphics scene framework
	//sets its own settings every frame
	glEnable (GL_BLEND);	
	glEnable(GL_STENCIL_TEST);
	glClearStencil(0);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	
	document_->draw();
	
	if (showAppLayer_)
	{
		GfxRenderer * renderer = document_->renderer();	
		videoPlayer_->draw(document_->renderer());
	}

	glDisable(GL_BLEND);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_DEPTH_TEST);

	//these 2 lines prevent bsod
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

bool PcScene::update()
{
	audioPlayer_->update();
	bool docNeedRedraw = document_->update();
	

	map<AppObject *, Clipper *>::iterator iter;
	for (iter = sceneItems_.begin(); iter != sceneItems_.end(); ++iter)
	{
		AppObject * appObject = (*iter).first;
		Clipper * clipper = (*iter).second;
		updateAppObject(appObject, clipper);				
	}		


	if (videoPlayer_->isShowControls())
	{
	
		videoControlsWidget_->setVisible(true);
		videoControlsWidget_->setFullscreenEnabled(!videoPlayer_->wasFullScreenInitially());
		if (videoPlayer_->isActivatedFullScreen())
		{
			videoControlsWidget_->move(0, document_->height() - videoControlsWidget_->height());
			videoControlsWidget_->resize(document_->width(), videoControlsWidget_->height());
		}
		else
		{
			int x, y, w, h;
			videoPlayer_->drawRegion(&x, &y, &w, &h);

			videoControlsWidget_->move(x, y + h - videoControlsWidget_->height());
			videoControlsWidget_->resize(w, videoControlsWidget_->height());
		}
		videoControlsWidget_->setVisible(showAppLayer_);
		if (!isSeekSliderPressed_)		
		{
			videoControlsWidget_->setSeekPosition(videoPlayer_->currentTime() / videoPlayer_->duration());
		}
		videoControlsWidget_->setTime((int)videoPlayer_->currentTime());
	}
	else
	{
		videoControlsWidget_->setVisible(false);
	}

	return docNeedRedraw || videoPlayer_->isActivated();
}

void PcScene::updateAppObject(AppObject * appObject, Clipper * clipper)
{
	static boost::uuids::uuid imageType = AppImage().type();
	static boost::uuids::uuid webContentType = AppWebContent().type();
	
	QGraphicsItem * qItem = clipper->item();

	float x, y, w, h;
	bool clip;
	float cx, cy, cw, ch;

	document_->appObjectBoundingBox(appObject,
		&x, &y, &w, &h, &clip, &cx, &cy, &cw, &ch);
	
	clipper->setClip(clip, cx, cy, cw, ch);

	if (appObject->type() == webContentType)
	{
		//QGraphicsWebView * webView = (QGraphicsWebView *)qItem;
		//webView->resize(appObject->width(), appObject->height());		
		QGraphicsProxyWidget * webItem = (QGraphicsProxyWidget *)qItem;
		webItem->widget()->resize(appObject->width(), appObject->height());
	}

	//BoundingBox bbox = document_->appObjectBoundingBox(appObject);
	QRectF brect = qItem->boundingRect();
	qreal scaleX = (qreal)w / (qreal)brect.width();
	qreal scaleY = (qreal)h/ (qreal)brect.height();

	QTransform transform;
	transform.translate(x, y);
	transform.scale(scaleX, scaleY);	
	qItem->setOpacity(appObject->opacity());	
	qItem->setTransform(transform);	


	bool isVisible = true;
	isVisible &= (appObject->visible() && !videoPlayer_->isActivatedFullScreen());
	isVisible &= showAppLayer_;

	qItem->setVisible(isVisible);	
}


void PcScene::addAppImage(AppImage * appImage)
{
	QGraphicsPixmapItem * newItem = 
		new QGraphicsPixmapItem(QPixmap(stdStringToQString(appImage->fileName())));
	Clipper * clipper = new Clipper(newItem);		
	addItem(clipper);	
	QVariant var;
	var.setValue((AppObject *)appImage);
	newItem->setData(0, var);
	updateAppObject(appImage, clipper);
	sceneItems_[appImage] = clipper;

}
/*
class ExWebPage : public QWebPage
{
	bool ExWebPage::acceptNavigationRequest(QWebFrame * frame, const QNetworkRequest & request, NavigationType type)
	{
		//do what you need here.
		return true;
	}
};
*/
void PcScene::addAppWebContent(AppWebContent * appWebContent)
{
	//QGraphicsWebView * webView = new QGraphicsWebView;
	QWebView * webWidget = new QWebView;
	QGraphicsProxyWidget * webItem = this->addWidget(webWidget);

	//ExWebPage * page = new ExWebPage();
	//webWidget->setPage(page);
	//QWebSettings * webSettings = webWidget->page()->settings();
	//QWebSettings * webSettings = webWidget->settings();
	//webSettings->setAttribute(QWebSettings::LocalContentCanAccessFileUrls, true);
	//webSettings->setAttribute(QWebSettings::LocalContentCanAccessRemoteUrls, true);
	//webSettings->setAttribute(QWebSettings::LocalStorageEnabled, true);
	//webSettings->setAttribute(QWebSettings::JavascriptEnabled, true);
	//webSettings->setAttribute(QWebSettings::JavaEnabled, true);
	//webSettings->setAttribute(QWebSettings::JavascriptCanOpenWindows, true);
	//webSettings->setAttribute(QWebSettings::JavascriptCanCloseWindows, true);
	//webSettings->setAttribute(QWebSettings::JavascriptCanAccessClipboard, true);
	//webSettings->setAttribute(QWebSettings::OfflineStorageDatabaseEnabled, true);
	//webSettings->setAttribute(QWebSettings::OfflineWebApplicationCacheEnabled, true);
	//webSettings->setAttribute(QWebSettings::LocalStorageDatabaseEnabled, true);
	//webSettings->setAttribute(QWebSettings::FrameFlatteningEnabled, true);

	Clipper * clipper = new Clipper(webItem);
	
	if (appWebContent->mode() == AppWebContent::ModeUrl)
		webWidget->setUrl(QUrl(stdStringToQString(appWebContent->url())));
	else if (appWebContent->mode() == AppWebContent::ModeHtml)
		webWidget->setHtml(QString::fromUtf8(appWebContent->html().c_str()));

	this->addItem(clipper); 	
	QVariant var;
	var.setValue((AppObject *)appWebContent);
	webItem->setData(0, var);
	updateAppObject(appWebContent, clipper);
	sceneItems_[appWebContent] = clipper;
}

void PcScene::removeAppObject(AppObject * appObject)
{
	map<AppObject *, Clipper *>::iterator iter = sceneItems_.find(appObject);
	if (iter != sceneItems_.end()) 		
	{
		removeItem((*iter).second);
		delete (*iter).second;
		sceneItems_.erase(iter);		
	}

}

void PcScene::sendAppObjectToFront(AppObject * appObject)
{
	//sceneItems_[appObject]->setZValue();
	//TODO
}

void PcScene::resize(int w, int h)
{
	videoPlayer_->deactivate();
	videoPlayer_->resize(w, h);
	document_->setSize(w, h, 1, 1);
}

//-----------------------------------------------------------------------------

void PcScene::mousePressEvent(QGraphicsSceneMouseEvent  * event)
{
	QGraphicsScene::mousePressEvent(event);	

	//check if event was processed by child window (text ui)
	if (event->isAccepted()) return;
	
	if (event->button() != Qt::LeftButton) return;

	const QPointF & mousePos = event->scenePos();
	int mouseX = mousePos.x();
	int mouseY = mousePos.y();

	AppObject * appObj = 0;
	QGraphicsItem * qItem = itemAt(mouseX, mouseY);
	if (qItem) appObj = qItem->data(0).value<AppObject *>();

	clickedAppObject_ = appObj;

	bool propagate = true;

/*	if (appObj && appObj->handleEvents()) propagate = false;
	else if (showAppLayer_ && videoPlayer_->isBlockingCursor(mouseX, mouseY)) propagate = false;*/	

	if (!videoControlsWidget_->isPressedBeingHandled())
	{
		document_->pressEvent(Vector2((float)mouseX, (float)mouseY), 0, propagate);	
		lButtonDown_ = true;
	}	
}

void PcScene::keyPressEvent(QKeyEvent *event)
{
	QGraphicsScene::keyPressEvent(event);

	//check if event was processed by child window (text ui)
	if (event->isAccepted()) return;

	if (!event->isAutoRepeat())
		document_->keyPressEvent(event->nativeVirtualKey());
}

void PcScene::keyReleaseEvent(QKeyEvent *event)
{
	QGraphicsScene::keyReleaseEvent(event);

	//check if event was processed by child window (text ui)
	if (event->isAccepted()) return;

	if (!event->isAutoRepeat())
		document_->keyReleaseEvent(event->nativeVirtualKey());
}

void PcScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
	mousePressEvent(event);
}

void PcScene::mouseReleaseEvent(QGraphicsSceneMouseEvent  * event)
{
	QGraphicsScene::mouseReleaseEvent(event);

	//check if event was processed by child window (text ui)
	if (event->isAccepted()) return;

	if (event->button() != Qt::LeftButton) return;
	const QPointF & mousePos = event->scenePos();
	int mouseX = mousePos.x();
	int mouseY = mousePos.y();

	AppObject * appObj = 0;
	QGraphicsItem * qItem = itemAt(mouseX, mouseY);
	if (qItem) appObj = qItem->data(0).value<AppObject *>();

	if (appObj && clickedAppObject_ == appObj && appObj->handleEvents())
	{			
		document_->appObjectTapEvent(appObj);
	}
	else
	{
		if (lButtonDown_) document_->releaseEvent(Vector2((float)mouseX, (float)mouseY), 0);
	}

	lButtonDown_ = false;
	clickedAppObject_ = NULL;
}

void PcScene::mouseMoveEvent(QGraphicsSceneMouseEvent  * event)
{	
	QGraphicsScene::mouseMoveEvent(event);

	//check if event was processed by child window (text ui)
	if (event->isAccepted()) return;

	const QPointF & mousePos = event->scenePos();
	int mouseX = mousePos.x();
	int mouseY = mousePos.y();
	if (videoPlayer_->isActivatedFullScreen()) return;

	if (clickedAppObject_)
	{
		AppObject * appObj = 0;
		QGraphicsItem * qItem = itemAt(mouseX, mouseY);
		if (qItem) appObj = qItem->data(0).value<AppObject *>();

		if (appObj != clickedAppObject_)
			clickedAppObject_ = NULL;
	}

	if (lButtonDown_)
	{
		document_->moveEvent(Vector2(mouseX, mouseY), 0);
		if (update()) document_->triggerRedraw();
	}
}
