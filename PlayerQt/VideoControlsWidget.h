#pragma once

class VideoControlsWidget : public QWidget
{
	Q_OBJECT
public:
	VideoControlsWidget();
	~VideoControlsWidget();
	virtual QSize sizeHint() const;
	virtual void paintEvent(QPaintEvent *);
	void setFullscreenEnabled(bool val) {fullScreenEnabled_ = val;}
	void setSeekPosition(float pos);
	void setTime(int time);
	bool isPressedBeingHandled() const {return mousePressed_;}

public:
	virtual void mousePressEvent(QMouseEvent * event);
	virtual void mouseMoveEvent(QMouseEvent * event);
	virtual void mouseReleaseEvent(QMouseEvent * event);

signals:
	void videoPlay();
	void videoStop();
	void videoFullscreen();
	void seekPressed();
	void seekReleased();	
	void seekChanged(float);
	
private:
	int height_;
	int buttonY_;
	int buttonLeftMargin_;
	int buttonRightMargin_;
	int playBarLeftX_;
	int playBarRightX_;
	int playBarY_;

	bool mousePressed_;
	bool playPressed_;
	bool stopPressed_;
	bool fullScreenPressed_;
	int pointPressedDelta_;

	bool fullScreenEnabled_;

	float seekPos_;

	int pointXMin_, pointXMax_, pointX_, pointY_;

	QPixmap barBack_, fullActive_, fullInactive_, 
		play_, stop_, playBar_, playBarLeft_, playBarRight_, 
		playBarPlayed_, playBarPlayedLeft_, playBarPlayedRight_, point_;

	QFont textFont_;
	int textWidth_, textHeight_;
	QString text_;
};
