#pragma once
#include "KeyFrame.h"

class KeyFrameData;
class TimeLinePane;
class EditorDocument;
class PaneButton;

class TimeLineBar : public QWidget
{
	Q_OBJECT
public:
	TimeLineBar(TimeLinePane * parent);
	~TimeLineBar();

	void setDocument(EditorDocument * document);

	float getTimeFromPos(int x) const;
	int getPosFromTime(float time) const;

	TimeLinePane * parent_;
	EditorDocument * document_;

	int tickSpacing_;
	int indentSize_;
	int distPerSecond_;

	bool mouseDown_;
	bool isShiftKey_;
private:
	virtual void mousePressEvent(QMouseEvent * event);
	virtual void mouseMoveEvent(QMouseEvent * event);
	virtual void mouseReleaseEvent(QMouseEvent * event);
	virtual void paintEvent(QPaintEvent * event);	
private:
	static const int BUFFER_SIZE = 512;
	char strBuffer_[BUFFER_SIZE + 1];

	QFont font_;
	QString str_;	
};
////////////////////////////////////////////////////////////////////////////////
class AnimationChannel;

class ChannelBars : public QWidget
{
	Q_OBJECT
public:
	ChannelBars(TimeLinePane * parent, TimeLineBar * bar);	
	
	void setDocument(EditorDocument * document);
protected:
	virtual void mousePressEvent(QMouseEvent * event);
	virtual void mouseMoveEvent(QMouseEvent * event);
	virtual void mouseReleaseEvent(QMouseEvent * event);
	virtual void mouseDoubleClickEvent(QMouseEvent * event);
	virtual void paintEvent(QPaintEvent * event);
	virtual void wheelEvent(QWheelEvent * event);

	bool cursorKey(KeyFrameData * key, const QPoint & pt) const;
	bool cursorChannel(AnimationChannel ** channel, const QPoint & pt) const;
public:
	bool isShiftKey_;
	bool isCtrlKey_;

private:
	EditorDocument * document_;
	TimeLinePane * parent_;
	TimeLineBar * bar_;
	QLinearGradient barGrad_;
	
	int barHeight_;

	bool multiKeyDragging_;
	bool keyDragging_;
	bool keyDragged_;
	
	float draggingKeyTime_;	
	float draggingKeyInitTime_;

	float startKeyFrameTime_;
	float endKeyFrameTime_;

	float keyPressedTime_;
	
	QRubberBand * rubberBand_;
	QPoint dragPoint_;
	bool mouseDragging_;
};

////////////////////////////////////////////////////////////////////////////////


class TimeLinePane : public QWidget
{
	Q_OBJECT
public:
	TimeLinePane();
	~TimeLinePane();
	void setDocument(EditorDocument * document);

	void setVScrollValue(int val);
	void setVScrollRange(int min, int max);

	QPoint scrollPos() const;

	bool isPlaying() const {return isPlaying_;}

	QScrollBar * vScrollBar() const {return vScrollBar_;}

	TimeLineBar * timeLineBar() const {return timeLineBar_;}

	bool animateOnTimeChanged() const;

public slots:
	void onPlay();
	void onStop();
	void onSetKey();
signals:
	void rangeChanged(int min, int max);
	void valueChanged(int value);
protected slots:
	void hScrollChanged(int val);
	void animationSelectionChanged();
	void timeLineTimeChanged();
	void timeEdited();
	void channelSelectionChanged();
	void onUpdate();
	void onDelete();
	void timeLineSpacing(int value);
	void onTimeZoomIn();
	void onTimeZoomOut();

protected:
	virtual void keyPressEvent(QKeyEvent *);
	virtual void keyReleaseEvent(QKeyEvent *);
	virtual void timerEvent(QTimerEvent * event);
	virtual void paintEvent(QPaintEvent * event);	

public:
	QScrollBar * vScrollBar_, * hScrollBar_;
	QFrame * scrollArea_;

private:
	QBasicTimer timer_;
	QElapsedTimer elapsed_;
	float startTime_;
	bool isPlaying_;
	bool playStartAnimState_;

	PaneButton * playButton_;
	PaneButton * stopButton_;
	PaneButton * updateButton_;
	PaneButton * timeLineZoomIn_;
	PaneButton * timeLineZoomOut_;

	QPushButton * setKeyButton_;
	QPushButton * deleteButton_;

	QLineEdit * timeEdit_;
	QDoubleValidator * timeValidator_;
	QComboBox * typeCombo_;
	TimeLineBar * timeLineBar_;
	ChannelBars * channelBars_;

	QSlider * timeLineSlider_;
	EditorDocument * document_;
};