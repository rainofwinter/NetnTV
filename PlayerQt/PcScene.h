#pragma once
#include <QtWebKit/QGraphicsWebView>
#include <QGLWidget>

class Document;
class PcVideoPlayer;
class PcAudioPlayer;
class PcPlayerSupport;

class AppImage;
class AppWebContent;

class VideoControlsWidget;


class Clipper : public QGraphicsRectItem
{
public:
	Clipper(QGraphicsItem * item);
	QGraphicsItem * item() const {return item_;}
	void setClip(bool clip, float cx, float cy, float cw, float ch);
private:
	QGraphicsItem * item_;
	bool clip_;
	float cx_, cy_, cw_, ch_;	

};

///////////////////////////////////////////////////////////////////////////////
class TimeLabel;

class PcScene : public QGraphicsScene
{
	Q_OBJECT
public:
	PcScene(QGLWidget * viewport, Document * document, 
		PcVideoPlayer * videoPlayer, PcAudioPlayer * audioPlayer, PcPlayerSupport * playerSupport);

	~PcScene();

	void init();

	void addAppImage(AppImage * appImage);
	void addAppWebContent(AppWebContent * appWebContent);

	void removeAppObject(AppObject * appObject);
	void sendAppObjectToFront(AppObject * appObject);

	void resize(int x, int y);

	bool update();

	void showAppLayer(bool show);
	bool isShowAppLayer() const {return showAppLayer_;}

	Document * document() const {return document_;}
private slots:
	void onVideoPlay();
	void onVideoStop();
	void onVideoFullscreen();
	void onSeekPressed();
	void onSeekReleased();
	void onSeekChanged(float value);
private:
	virtual void drawBackground(QPainter *painter, const QRectF &rect);
	virtual void mousePressEvent(QGraphicsSceneMouseEvent  *);
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent  *);
	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent  *);
	virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

	virtual void keyPressEvent(QKeyEvent *event);
	virtual void keyReleaseEvent(QKeyEvent *event);

	void updateAppObject(AppObject *, Clipper *);

	Document * document_;
	PcVideoPlayer * videoPlayer_;
	PcAudioPlayer * audioPlayer_;
	bool redrawTriggered_;
	QGLWidget * viewport_;
	bool isInit_;

	VideoControlsWidget * videoControlsWidget_;

	std::map<AppObject *, Clipper *> sceneItems_;	

	AppObject * clickedAppObject_;
	bool lButtonDown_;
	bool showAppLayer_;

	bool isSeekSliderPressed_;
};