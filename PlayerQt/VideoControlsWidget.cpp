#include "stdafx.h"
#include "VideoControlsWidget.h"

VideoControlsWidget::VideoControlsWidget()
{
	barBack_.load(":/PlayerQt/Resources/playbackControl/bar_back.png");
	fullActive_.load(":/PlayerQt/Resources/playbackControl/full_active.png");
	fullInactive_.load(":/PlayerQt/Resources/playbackControl/full_inactive.png");
	play_.load(":/PlayerQt/Resources/playbackControl/play.png");
	stop_.load(":/PlayerQt/Resources/playbackControl/stop.png");
	playBar_.load(":/PlayerQt/Resources/playbackControl/playbar.png");
	playBarLeft_.load(":/PlayerQt/Resources/playbackControl/playbar_left.png");
	playBarRight_.load(":/PlayerQt/Resources/playbackControl/playbar_right.png");
	playBarPlayed_.load(":/PlayerQt/Resources/playbackControl/playbar_played.png");
	playBarPlayedLeft_.load(":/PlayerQt/Resources/playbackControl/playbar_played_left.png");
	playBarPlayedRight_.load(":/PlayerQt/Resources/playbackControl/playbar_played_right.png");
	point_.load(":/PlayerQt/Resources/playbackControl/point.png");

	height_ = barBack_.height();
	buttonY_ = height_/2 - play_.height()/2;

	buttonLeftMargin_ = 4;
	buttonRightMargin_ = 4;

	playBarLeftX_ = buttonLeftMargin_ + play_.width() + stop_.width() + 4;	
	playBarY_ = height_/2 - playBar_.height()/2;

	setStyleSheet("background: transparent; border: none");
	setAutoFillBackground(false);

	mousePressed_ = false;
	fullScreenEnabled_ = true;
	playPressed_ = false;
	stopPressed_ = false;
	fullScreenPressed_ = false;
	pointPressedDelta_ = INT_MIN;
	seekPos_ = 0.0f;

	pointX_ = pointY_ = 0.0f;

	textFont_ = QFont("Arial", 8);
	QFontMetrics fm(textFont_);
	text_ = "00:00:00";
	textWidth_ = fm.width(text_);
	textHeight_ = fm.height();
}

VideoControlsWidget::~VideoControlsWidget()
{

}

void VideoControlsWidget::setTime(int time)
{	
	int s = time % 60; time /= 60;
	int m = time % 60; time /= 60;
	int h = time % 60; time /= 60;
	text_.sprintf("%02d:%02d:%02d", h, m, s);	
}

void VideoControlsWidget::mousePressEvent(QMouseEvent * event)
{
	int x = event->pos().x();
	int y = event->pos().y();
	int tx;
	mousePressed_ = true;

	if (y >= buttonY_ && y <= buttonY_ + play_.height())
	{
		tx = buttonLeftMargin_;
		if (x >= tx && x <= tx + play_.width())
			playPressed_ = true;

		tx += play_.width();
		if (x >= tx && x <= tx + stop_.width())
			stopPressed_ = true;	

		tx = width() - buttonRightMargin_ - fullActive_.width();
		if (x >= tx && x <= tx + fullActive_.width())
			fullScreenPressed_ = true;
	}

	tx = pointX_;
	if (x >= tx && x <= tx + point_.width() && y >= pointY_ && y <= pointY_ + point_.height())
	{
		pointPressedDelta_ = tx - x;
		emit seekPressed();
	}
	else if (x >= playBarLeftX_ && x <= playBarRightX_ && y >= playBarY_ && y <= playBarY_ + playBar_.height())
	{
		pointPressedDelta_ = -point_.width()/2;
		seekPos_ = (float)(x + pointPressedDelta_ - pointXMin_)/(pointXMax_ - pointXMin_);
		if (seekPos_ < 0.0f) seekPos_ = 0.0f;
		if (seekPos_ > 1.0f) seekPos_ = 1.0f;

		emit seekPressed();
	}
}

void VideoControlsWidget::mouseMoveEvent(QMouseEvent * event)
{
	QPoint pos = event->pos();
	if (pointPressedDelta_ != INT_MIN)
	{
		seekPos_ = (float)(pos.x() + pointPressedDelta_ - pointXMin_) / (pointXMax_ - pointXMin_);
		if (seekPos_ < 0.0f) seekPos_ = 0.0f;
		if (seekPos_ > 1.0f) seekPos_ = 1.0f;
		update();
	}

	
}

void VideoControlsWidget::mouseReleaseEvent(QMouseEvent * event)
{
	int x = event->pos().x();
	int y = event->pos().y();

	if (y >= buttonY_ && y <= buttonY_ + play_.height())
	{
		int tx = buttonLeftMargin_;
		if (x >= tx && x <= tx + play_.width() && playPressed_)
			emit videoPlay();

		tx += play_.width();
		if (x >= tx && x <= tx + stop_.width() && stopPressed_)
			emit videoStop();

		tx = width() - buttonRightMargin_ - fullActive_.width();
		if (x >= tx && x <= tx + fullActive_.width() && fullScreenPressed_)
			emit videoFullscreen();
	}

	
	if (pointPressedDelta_ != INT_MIN)	
	{
		emit seekReleased();		
		emit seekChanged(seekPos_);
	}

	mousePressed_ = false;
	playPressed_ = false;
	stopPressed_ = false;
	fullScreenPressed_ = false;
	pointPressedDelta_ = INT_MIN;
}

QSize VideoControlsWidget::sizeHint() const
{
	return QSize(0, height_);
}


void VideoControlsWidget::paintEvent(QPaintEvent * event)
{
	QPainter painter(this);

	int w = width();
	int h = height();
	playBarRightX_ = w - buttonRightMargin_ - fullActive_.width() - 4 - textWidth_ - 3;
	
	painter.drawPixmap(0, 0, w, h, barBack_);
	painter.drawPixmap(buttonLeftMargin_, buttonY_, play_.width(), play_.height(), play_);
	painter.drawPixmap(buttonLeftMargin_ + play_.width(), buttonY_, stop_.width(), stop_.height(), stop_);


	if (fullScreenEnabled_)
		painter.drawPixmap(w - fullActive_.width() - buttonRightMargin_, buttonY_, fullActive_.width(), fullActive_.height(), fullActive_);
	else
		painter.drawPixmap(w - fullActive_.width() - buttonRightMargin_, buttonY_, fullActive_.width(), fullActive_.height(), fullInactive_);

	if (playBarRightX_ > playBarLeftX_)
	{
		pointXMin_ = playBarLeftX_ + playBarLeft_.width() - point_.width() / 2;
		pointXMax_ = playBarRightX_ - point_.width()/2 - playBarRight_.width();
		pointX_ = pointXMin_ + seekPos_ * (pointXMax_ - pointXMin_);
		pointY_ = height_/2 - point_.height()/2;

		painter.drawPixmap(playBarLeftX_, playBarY_, playBarPlayedLeft_.width(), playBarPlayedLeft_.height(), playBarPlayedLeft_);
		
		painter.drawPixmap(playBarLeftX_ + playBarPlayedLeft_.width(), playBarY_, 
			pointX_ + point_.width()/2 - playBarLeftX_, playBarPlayed_.height(), playBarPlayed_);
		
		painter.drawPixmap(pointX_ + point_.width()/2, playBarY_, 
			playBarRightX_ - pointX_ - point_.width()/2 - playBarRight_.width(), playBar_.height(), playBar_);

		painter.drawPixmap(playBarRightX_ - playBarRight_.width(), playBarY_, playBarRight_.width(), playBarRight_.height(), playBarRight_);


		painter.drawPixmap(pointX_, pointY_,
			point_.width(), point_.height(), point_);	
		
	}

	QPen pen(QColor(255, 255, 255, 255));
	painter.setPen(pen);
	painter.drawText(playBarRightX_ + 3, height_/2 + textHeight_/2, text_);

	
};

void VideoControlsWidget::setSeekPosition(float pos)
{
	seekPos_ = pos;
	update();
}