#include "stdafx.h"
#include "TimeLinePane.h"
#include "EditorDocument.h"
#include "EditorObject.h"
#include "Animation.h"
#include "AnimationChannel.h"
#include "PaneButton.h"
#include "Command.h"
#include "KeyFrame.h"
#include "Utils.h"

using namespace std;

TimeLineBar::TimeLineBar(TimeLinePane * parent)
{
	document_ = 0;
	parent_ = parent;
	tickSpacing_ = 6;
	indentSize_ = 5;
	distPerSecond_ = 60;
	setStyleSheet(
		"background-color: white;"
		"background-repeat:repeat-x;"
		"border: none;"
		);
	setMinimumHeight(20);
	setMaximumHeight(20);

	font_ = QFont("Arial", 7);
	mouseDown_ = false;
	isShiftKey_ = false;
}

TimeLineBar::~TimeLineBar()
{
}

void TimeLineBar::setDocument(EditorDocument * document)
{
	if (document_) document_->disconnect(this);
	document_ = document;	
	if (document_)
	{
		connect(document_, SIGNAL(timeLineTimeChanged()), this, SLOT(update()));
		connect(document_, SIGNAL(timeLineTimeChanging()), this, SLOT(update()));
	}
}

float TimeLineBar::getTimeFromPos(int x) const
{return (float)(parent_->scrollPos().x() + x - indentSize_) / distPerSecond_;}

int TimeLineBar::getPosFromTime(float time) const
{return (int)(time * distPerSecond_ - parent_->scrollPos().x() + indentSize_);}

void TimeLineBar::mousePressEvent(QMouseEvent * event)
{
	if (!parent_->isEnabled() || parent_->isPlaying()) return;
	mouseDown_ = true;
	float time = max(0.0f, getTimeFromPos(event->pos().x()));	
	document_->setTimeLineTime(time);
	parent_->update();
}

void TimeLineBar::mouseMoveEvent(QMouseEvent * event)
{
	if (mouseDown_)
	{
		document_->scrubTimeLineTime(max(0.0f, getTimeFromPos(event->pos().x())));
		if(isShiftKey_)
			document_->scrubTimeLineTime(removeFloatPointNumber(max(0.0f, getTimeFromPos(event->pos().x())), 1));
		QPoint markerPos;
		QToolTip::showText(event->globalPos(), QString::number(document_->timeLineTime(), 'f', 4));
		parent_->update();
	}
}

void TimeLineBar::mouseReleaseEvent(QMouseEvent * event)
{
	if (mouseDown_)	mouseDown_ = false;
}

void TimeLineBar::paintEvent(QPaintEvent * event)
{
	QStyleOption opt;
    opt.init(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
	
	
	int tickTopY = 2;

	int zeroPos = - parent_->scrollPos().x();
	int tickStartPos = zeroPos + indentSize_;

	painter.setPen(Qt::SolidLine);
	painter.setFont(font_);

	int tickBottomY = 20;
	int count = 0;

	int tickX = tickStartPos;

	if (tickX < 0)
	{
		int skipTicks = -(int)tickX / tickSpacing_;
		tickX += skipTicks * tickSpacing_;
		count += skipTicks;
	}

	for (; tickX < width(); tickX += tickSpacing_)
	{
		if (count % 10 == 0)
		{
			tickTopY = 2;
			float tickTime = getTimeFromPos(tickX);
			
			int hours = (int)(tickTime / 3600);
			int minutes = (int)(tickTime / 60);			
			int seconds = (int)tickTime % 60;
			
			/*
			str_.sprintf( 
				"%d:%02d:%02d", hours, minutes, seconds);  
			*/

			str_.sprintf("%d", (int)tickTime);
			QFontMetrics fm(font_);	
			QRect boundingBox = fm.tightBoundingRect(str_);			
			
			int textX = tickX - boundingBox.width() / 2;
			if (textX < 2) textX = 2;

			painter.setPen(QColor(120, 120, 120, 255));
			painter.drawText(tickX + 3, tickBottomY / 2, str_);

			/*
			painter.drawText(
				tickX - boundingBox.width()/2, 
				tickBottomY + boundingBox.height() + 4, 
				str_);			
			*/
		}

		else if(count % 5 == 0)
		{
			tickTopY = tickBottomY / 2;
		}

		else
		{
			tickTopY = 15;
		}

		painter.drawLine(tickX, tickTopY, tickX, tickBottomY);
		
		count++;
	}

	if (parent_->isEnabled())
	{
		int markerX = getPosFromTime(document_->timeLineTime());
		QPixmap markerBarPixmap;
		markerBarPixmap.load(":/data/Resources/timeline/slider.png");
		painter.drawPixmap(markerX - 5, 0, markerBarPixmap.width(), markerBarPixmap.height(), markerBarPixmap);
		painter.setBrush(Qt::red);
		painter.setPen(Qt::NoPen);
		painter.drawRect(markerX, markerBarPixmap.height(), 1, height());
	}
}
////////////////////////////////////////////////////////////////////////////////
ChannelBars::ChannelBars(TimeLinePane * parent, TimeLineBar * bar)
{
	document_ = 0;
	parent_ = parent;
	barHeight_ = 20;


	barGrad_ = QLinearGradient(QPointF(0, 0), QPointF(0, barHeight_));
    barGrad_.setColorAt(0, QColor(210, 210, 210));
	barGrad_.setColorAt(1, QColor(210, 210, 210));
    
	barGrad_.setSpread(QGradient::RepeatSpread);

	bar_ = bar;

	setMouseTracking(true);
	keyDragging_ = false;
	mouseDragging_ = false;

	startKeyFrameTime_ = 100000.0f;
	endKeyFrameTime_ = 0.0f;
	multiKeyDragging_ = false;
	draggingKeyInitTime_ = draggingKeyTime_ = 0.0f;

	isShiftKey_ = false;
	isCtrlKey_ = false;
}


void ChannelBars::setDocument(EditorDocument * document)
{
	if (document_) document_->disconnect(this);
	document_ = document;
	
	if (document_)
	{
		connect(document_, SIGNAL(animationSelectionChanged()), this, SLOT(update()));
		connect(document_, SIGNAL(objectSelectionChanged()), this, SLOT(update()));
		connect(document_, SIGNAL(channelListChanged()), this, SLOT(update()));
		connect(document_, SIGNAL(keyFrameSelectionChanged()), this, SLOT(update()));
		connect(document_, SIGNAL(channelChanged()), this, SLOT(update()));
	}
}

void ChannelBars::mousePressEvent(QMouseEvent * event)
{
	if (event->button() == Qt::LeftButton)
	{
		KeyFrameData data;
		vector<KeyFrameData> selKeys = document_->selectedKeyFrames();
		
		if (cursorKey(&data, event->pos()))
		{				
			if (isShiftKey_)
			{
				selKeys.push_back(data);
			}
			else if (isCtrlKey_)
			{
				vector<KeyFrameData>::iterator iter;
				bool erased = false;
				for (iter = selKeys.begin(); iter != selKeys.end();)
				{
					if (*iter == data)
					{
						erased = true;
						iter = selKeys.erase(iter);
					}
					else iter++;
				}
				if (!erased) selKeys.push_back(data);
			}
			else if (!document_->isKeyFrameSelected(data))
			{
				selKeys.clear();
				selKeys.push_back(data);
			}

			document_->setSelectedKeyFrames(selKeys);
		
			keyDragging_ = true;
			keyDragged_ = false;
			draggingKeyTime_ = data.time;	
			draggingKeyInitTime_ = draggingKeyTime_;
		}

		else
		{
			keyPressedTime_ = bar_->getTimeFromPos(event->pos().x());
			if(selKeys.size() > 1)
			{
				vector<KeyFrameData>::iterator srcIter;
				vector<KeyFrameData>::iterator dstIter;
				for (srcIter = selKeys.begin(); srcIter != selKeys.end() - 1; srcIter++)
				{
					int selectedKeyFrameCount = 1;
					int startX = bar_->getPosFromTime(srcIter->time);
					int endX = bar_->getPosFromTime(srcIter->time);

					if(srcIter->time < startKeyFrameTime_)
						startKeyFrameTime_ = srcIter->time;
					if(srcIter->time > endKeyFrameTime_)
						endKeyFrameTime_ = srcIter->time;
					
					AnimationChannel * channel;
					for(dstIter = selKeys.begin() + 1; dstIter != selKeys.end(); dstIter++)
					{
						if(srcIter->channel == dstIter->channel)
						{
							int dstX = bar_->getPosFromTime(dstIter->time);
							if(startX > dstX) startX = dstX;
							if(endX < dstX) endX = dstX;

							++selectedKeyFrameCount;
							if(selectedKeyFrameCount >= 2)
								channel = dstIter->channel;
						}
					}
					AnimationChannel * mousePosChannel;
					cursorChannel(&mousePosChannel, event->pos());
					if(selectedKeyFrameCount >= 2 && mousePosChannel == channel &&
						event->pos().x() > startX && event->pos().x() < endX)
					{
						keyDragging_ = true;
						keyDragged_ = false;
						draggingKeyTime_ = bar_->getTimeFromPos(event->pos().x());
						draggingKeyInitTime_ = draggingKeyTime_;
						multiKeyDragging_ = true;
						startKeyFrameTime_ = bar_->getTimeFromPos(startX);
						endKeyFrameTime_ = bar_->getTimeFromPos(endX);
					}
				}
			}

			if(!keyDragging_)
			{
				dragPoint_ = event->pos();
				rubberBand_ = new QRubberBand(QRubberBand::Rectangle, this);
				rubberBand_->setGeometry(QRect(dragPoint_, QSize()));
				rubberBand_->show();
				mouseDragging_ = true;
				selKeys.clear();
			}
			document_->setSelectedKeyFrames(selKeys);
		}

		AnimationChannel * channel;
		if (cursorChannel(&channel, event->pos()))
		{
			vector<AnimationChannel *> selChannels;
			selChannels.push_back(channel);
			document_->setSelectedChannels(selChannels);
		}
	}
}

void ChannelBars::mouseMoveEvent(QMouseEvent * event)
{
	int auxX = event->x();
	int auxY = event->y();
	int posMaxX = parent_->scrollArea_->width()-1;
	int posMaxY = parent_->scrollArea_->height()-1;
	parent_->hScrollBar_->setSingleStep(10);
	parent_->vScrollBar_->setSingleStep(10);
	if(auxX <= 0)
	{
		parent_->hScrollBar_->setValue(parent_->hScrollBar_->value() - parent_->hScrollBar_->singleStep());
		if(parent_->hScrollBar_->value() > 0)
			dragPoint_.setX(dragPoint_.x() + parent_->hScrollBar_->singleStep());
	}
	else if(auxX >= posMaxX)
	{
		parent_->hScrollBar_->setValue(parent_->hScrollBar_->value() + parent_->hScrollBar_->singleStep());
		if(parent_->hScrollBar_->value() < parent_->hScrollBar_->width())
			dragPoint_.setX(dragPoint_.x() - parent_->hScrollBar_->singleStep());
	}
	if(auxY <= 0)
	{
		parent_->vScrollBar_->setValue(parent_->vScrollBar_->value() - parent_->vScrollBar_->singleStep());
		//	if(parent_->vScrollBar_->value() > 0)
		//		dragPoint_.setY(dragPoint_.y() + parent_->vScrollBar_->singleStep());
	}
	else if(auxY >= posMaxY)
	{
		parent_->vScrollBar_->setValue(parent_->vScrollBar_->value() + parent_->vScrollBar_->singleStep());
		//	if(parent_->vScrollBar_->value() < parent_->vScrollBar_->height())
		//		dragPoint_.setY(dragPoint_.y() - parent_->vScrollBar_->singleStep());
	}

	if (keyDragging_ && !mouseDragging_)
	{
		keyDragged_ = true;
		TimeLineBar * bar = parent_->timeLineBar();

		draggingKeyTime_ = bar->getTimeFromPos(event->pos().x());
		if(isShiftKey_ && !multiKeyDragging_)
		{
			draggingKeyTime_ = bar->getTimeFromPos(event->pos().x());
			draggingKeyTime_ = removeFloatPointNumber(draggingKeyTime_, 1);
		}
		if(multiKeyDragging_)
		{
			if (draggingKeyTime_ < keyPressedTime_ - startKeyFrameTime_) draggingKeyTime_ = keyPressedTime_ - startKeyFrameTime_;
		}
		else
			if (draggingKeyTime_ < 0) draggingKeyTime_ = 0;		

		if(!multiKeyDragging_)
			QToolTip::showText(event->globalPos(), QString::number(draggingKeyTime_, 'f', 4));		
		/*
		if(multiKeyDragging_)
		{
			startKeyFrameTime_ += draggingKeyTime_;
			endKeyFrameTime_ += draggingKeyTime_;
			if (startKeyFrameTime_ < 0)
				draggingKeyTime_ = startKeyFrameTime_;
		}
		*/
		update();
	}

	else if (!keyDragging_ && mouseDragging_)
	{
		Animation * anim = document_->selectedAnimation();		
		SceneObject * selSceneObj = document_->selectedObject();

		AnimationChannel * minChannel = 0;
		float minTime = -1;	

		if(!anim->channels(selSceneObj))
			return;

		rubberBand_->setGeometry(QRect(dragPoint_, event->pos()).normalized());

		KeyFrameData data;
		vector<KeyFrameData> selKeys = document_->selectedKeyFrames();

		QRect rubberRect = rubberBand_->geometry();

		int y = -parent_->scrollPos().y() * barHeight_;
		
		int startX = rubberRect.left();
		int endX = rubberRect.right();
		int startY = rubberRect.top();
		int endY = rubberRect.bottom();

		int startChannel = startY / barHeight_;
		int endChannel = endY / barHeight_;

		foreach(const AnimationChannelSPtr & channel, *anim->channels(selSceneObj))
		{
			if(startChannel <= (y / barHeight_) && endChannel >= (y / barHeight_))
			{
				int numKeyFrames = channel->numKeyFrames();
				for(int i = 0; i < numKeyFrames; i++)
				{
					KeyFrame * key = channel->keyFrame(i);
					float time = key->time();
					int x = bar_->getPosFromTime(time);
					
					minTime = time;
					minChannel = channel.get();
					data = KeyFrameData(minChannel, minTime);

					if(startX <= x && endX >= x)
					{
						bool isKeyFrameDataFlag = false;
						for(vector<KeyFrameData>::iterator iter = selKeys.begin(); iter != selKeys.end(); iter++)
						{
							if(*iter == data)
								isKeyFrameDataFlag = true;
						}
						if(!isKeyFrameDataFlag)
							selKeys.push_back(data);
					}
					else
					{
						for(vector<KeyFrameData>::iterator iter = selKeys.begin(); iter != selKeys.end();)
						{
							if((*iter).time == time) iter = selKeys.erase(iter);
							else ++iter;
						}
					}
					document_->setSelectedKeyFrames(selKeys);
				}
			}
			else
			{
				int numKeyFrames = channel->numKeyFrames();
				for(int i = 0; i < numKeyFrames; i++)
				{
					KeyFrame * key = channel->keyFrame(i);
					float time = key->time();
					int x = bar_->getPosFromTime(time);

					minTime = time;
					minChannel = channel.get();
					data = KeyFrameData(minChannel, minTime);
					
					for(vector<KeyFrameData>::iterator iter = selKeys.begin(); iter != selKeys.end();)
					{
							if((*iter).time == time) iter = selKeys.erase(iter);
							else ++iter;
					}
					document_->setSelectedKeyFrames(selKeys);
				}
			}
			y += barHeight_;
		}
	}
}

void ChannelBars::mouseReleaseEvent(QMouseEvent * event)
{
	if (keyDragging_ && keyDragged_)
	{
		try 
		{
			document_->doCommand(new ChangeKeyFrameTimesCmd(
				document_, document_->selectedKeyFrames(), 
				draggingKeyTime_ - draggingKeyInitTime_));
		}
		catch(const Exception & e)
		{
			QMessageBox::information(0, tr("Error"), tr(e.what()));
		}
		update();
	}
	keyDragging_ = false;
	multiKeyDragging_ = false;

	if(mouseDragging_)
	{
		mouseDragging_ = false;
		rubberBand_->hide();
	}
}

void ChannelBars::mouseDoubleClickEvent(QMouseEvent * event)
{
	if (event->button() == Qt::LeftButton)
	{				
		KeyFrameData data;
		if (cursorKey(&data, event->pos()))
		{
			Animation * anim = document_->selectedAnimation();
			document_->setTimeLineTime(data.time);
			update();
		}
		else 
		{
			int x = event->pos().x();
			float time = max(0.0f, parent_->timeLineBar()->getTimeFromPos(x));
			document_->setTimeLineTime(time);
			update();
		}
	}
}

bool ChannelBars::cursorChannel(
	AnimationChannel ** retChannel, const QPoint & pt) const
{
	Animation * anim = document_->selectedAnimation();		
	SceneObject * selSceneObj = document_->selectedObject();
	if (!selSceneObj) return false;	
	if (!anim->channels(selSceneObj)) return false;

	int y = -parent_->scrollPos().y() * barHeight_;
	foreach(const AnimationChannelSPtr & channel, *anim->channels(selSceneObj))
	{
		if (channel->object() != selSceneObj) continue;

		if (y < pt.y() && pt.y() < y + barHeight_) 
		{
			*retChannel = channel.get(); 
			return true;
		}
		
		y += barHeight_;
	}

	return false;
}

bool ChannelBars::cursorKey(KeyFrameData * key, const QPoint & pt) const
{
	Animation * anim = document_->selectedAnimation();		
	SceneObject * selSceneObj = document_->selectedObject();
	if (!selSceneObj) return false;
	if (!anim->channels(selSceneObj)) return false;

	float mouseTime = 
		parent_->timeLineBar()->getTimeFromPos(pt.x());
	int y = -parent_->scrollPos().y() * barHeight_;

	int mouseX = pt.x();
	int mouseY = pt.y();

	int minDist = INT_MAX;
	float minTimeDelta = FLT_MAX;
	AnimationChannel * minChannel = 0;
	float minTime = -1;	

	foreach(const AnimationChannelSPtr & channel, *anim->channels(selSceneObj))
	{
		if (channel->object() != selSceneObj) continue;
		y += barHeight_;
		if (mouseY < y - barHeight_ || mouseY > y) continue;
		int numKeyFrames = channel->numKeyFrames();
		for (int i = 0; i < numKeyFrames; ++i)
		{
			KeyFrame * key = channel->keyFrame(i);
			float time = key->time();
			int x = bar_->getPosFromTime(time);
			
			float timeDelta = fabs(time - mouseTime);
			if (timeDelta < minTimeDelta)
			{
				minTimeDelta = timeDelta;
				minDist = abs(x - mouseX);
				minTime = time;
				minChannel = channel.get();
			}
		}	
	}
	
	if (minDist < 4)
	{			
		*key = KeyFrameData(minChannel, minTime);
		return true;
	}	
	return false;
}

void ChannelBars::paintEvent(QPaintEvent * event)
{
	if (!document_) return;
	Animation * anim = document_->selectedAnimation();
	if (!anim) return;
	SceneObject * selSceneObj = document_->selectedObject();
	if (!selSceneObj) return;

	QPainter p(this);
	QPen pen(QColor(0, 0, 0, 20), 1, Qt::DashLine);

	int y;
	p.setPen(Qt::NoPen);

	const vector<AnimationChannelSPtr> * channels = anim->channels(selSceneObj);
	if (!channels) return;
	//first draw bar backgrounds and unselected keyframes
	y = -parent_->scrollPos().y() * barHeight_;

	float duration = anim->duration();

	BOOST_FOREACH(const AnimationChannelSPtr & channel, *channels)
	{
		p.setPen(Qt::NoPen);
		if (channel->object() != selSceneObj) continue;
		p.setBrush(barGrad_);
		p.drawRect(0, y, width(), barHeight_);
		p.setBrush(QColor(0, 0, 0, 30));
		p.drawRect(0, y, bar_->getPosFromTime(duration), barHeight_);

		int numKeyFrames = channel->numKeyFrames();
		if(numKeyFrames >= 2)
		{
			KeyFrame * startKey = channel->keyFrame(0);
			KeyFrame * endKey = channel->keyFrame(numKeyFrames-1);
			int startX = bar_->getPosFromTime(startKey->time());
			int endX = bar_->getPosFromTime(endKey->time());
			p.setBrush(QColor(0, 0, 0, 30));
			p.drawRect(startX, y, endX-startX, barHeight_);
		}

		int startX = -1, endX = -1;

		for (int i = 0; i < numKeyFrames; ++i)
		{			
			if (!document_->isKeyFrameSelected(channel.get(), i))
			{
				QPixmap keyFrameBarPixmap;
				keyFrameBarPixmap.load(":/data/Resources/timeline/cursorOff.png");
				KeyFrame * key = channel->keyFrame(i);
				int x = bar_->getPosFromTime( key->time());
				p.drawPixmap(x - 4, y, 7, barHeight_, keyFrameBarPixmap);
			}			
		}
		y += barHeight_;
	}

	//go back and draw selected keyframes
	y = -parent_->scrollPos().y() * barHeight_;
	BOOST_FOREACH(const AnimationChannelSPtr & channel, *channels)
	{
		if (channel->object() != selSceneObj) continue;
		int SelectedKeyFrameCount = 0;
		int startKeyFrame = 100, endKeyFrame = 0;
		int startKeyFrameX, endKeyFrameX;
		int numKeyFrames = channel->numKeyFrames();
		for (int i = 0; i < numKeyFrames; ++i)
		{
			if (document_->isKeyFrameSelected(channel.get(), i))
			{
				KeyFrame * key = channel->keyFrame(i);
				int x = bar_->getPosFromTime(key->time());
				
				if (keyDragging_)
				{
					float timeDelta = draggingKeyTime_ - draggingKeyInitTime_;
					float initTime = key->time();
					x = parent_->timeLineBar()->getPosFromTime(initTime + timeDelta);
				}
				if(startKeyFrame > i)
				{
					startKeyFrame = i;
					startKeyFrameX = x;
				}
				if(endKeyFrame < i)
				{
					endKeyFrame = i;
					endKeyFrameX = x;
				}
				SelectedKeyFrameCount++;
				QPixmap keyFrameBarPixmap;
				keyFrameBarPixmap.load(":/data/Resources/timeline/cursorSelect.png");
				p.drawPixmap(x - 4, y, 7, barHeight_, keyFrameBarPixmap);

			}
		}
		if(SelectedKeyFrameCount >= 2)
		{
			p.setBrush(QColor(255, 0, 0, 50));
			p.drawRect(startKeyFrameX, y, endKeyFrameX - startKeyFrameX, barHeight_);
		}
		y += barHeight_;
	}

	int tickTopY = 0;
	int zeroPos = - parent_->scrollPos().x();
	int tickStartPos = zeroPos + bar_->indentSize_;
	int count = 0;
	int tickX = tickStartPos;

	for(; tickX < width(); tickX += bar_->tickSpacing_)
	{
		if(!(count % 10))
		{
			tickTopY = 0;
			p.setPen(QColor(0, 0, 0, 25));
			p.drawLine(tickX-1, tickTopY, tickX-1, height());
		}
		count++;
	}

	for(y = barHeight_; y < height(); y += barHeight_)
	{
		p.setPen(QColor(0, 0, 0, 25));
		p.drawLine(0, y, width(), y);
	}

	if (parent_->isEnabled())
	{
		int markerX = bar_->getPosFromTime(document_->timeLineTime());
		p.setBrush(Qt::red);
		p.setPen(Qt::NoPen);
		p.drawRect(markerX-1, 0, 1, height());
	}

}

void ChannelBars::wheelEvent(QWheelEvent * event)
{
	QApplication::sendEvent(parent_->hScrollBar_, event);	
}


///////////////////////////////////////////////////////////////////////////////
TimeLinePane::TimeLinePane()
{
	document_ = 0;
	isPlaying_ = false;

	timeLineBar_ = new TimeLineBar(this);
	channelBars_ = new ChannelBars(this, timeLineBar_);
	vScrollBar_ = new QScrollBar(Qt::Vertical);
	hScrollBar_ = new QScrollBar(Qt::Horizontal);
	vScrollBar_->setRange(0, 0);
	vScrollBar_->setVisible(false);
	hScrollBar_->setRange(0, 100000);
	hScrollBar_->setPageStep(100);

	//scrollArea_->setWidget(channelBars_);
	//scrollArea_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);	
	QVBoxLayout * layout = new QVBoxLayout;
	layout->setMargin(0);
	layout->setSpacing(0);
	setLayout(layout);
	layout->addWidget(timeLineBar_);

	scrollArea_ = new QFrame;
	scrollArea_->setFrameShape(QFrame::StyledPanel);
	QGridLayout * gridLayout = new QGridLayout;
	gridLayout->setMargin(0);
	gridLayout->setSpacing(0);
	gridLayout->addWidget(channelBars_, 0, 0);
	gridLayout->addWidget(vScrollBar_, 0, 1);
	gridLayout->addWidget(hScrollBar_, 1, 0);
	scrollArea_->setLayout(gridLayout);
	layout->addWidget(scrollArea_);

	QHBoxLayout * btnLayout = new QHBoxLayout;
	btnLayout->setMargin(0);
	btnLayout->setSpacing(0);
	btnLayout->addWidget(playButton_ = new PaneButton(QPixmap(":/data/Resources/timeline/play.png"), QSize(30, 30), "Play"));
	btnLayout->addSpacing(5);
	btnLayout->addWidget(stopButton_ = new PaneButton(QPixmap(":/data/Resources/timeline/pause.png"), QSize(30, 30), "Pause"));
	btnLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
	btnLayout->addSpacing(10);	
	btnLayout->addWidget(timeLineZoomOut_ = new PaneButton(QPixmap(":/data/Resources/timeline/minus.png"), QSize(30, 30), "Zoom Out"));
	timeLineSlider_ = new QSlider(Qt::Horizontal);
	timeLineSlider_->setTickPosition(QSlider::TicksBelow);
	timeLineSlider_->setMinimum(1);
	timeLineSlider_->setMaximum(5);
	timeLineSlider_->setTickInterval(1);
	timeLineSlider_->setValue(3);
	timeLineSlider_->setFixedWidth(100);
	timeLineSlider_->setFixedHeight(20);
	btnLayout->addWidget(timeLineSlider_);
	btnLayout->addWidget(timeLineZoomIn_ = new PaneButton(QPixmap(":/data/Resources/timeline/plus.png"), QSize(30, 30), "Zoom In"));
	btnLayout->addSpacing(20);
	btnLayout->addWidget(new QLabel(tr("Time")));
	btnLayout->addSpacing(5);
	btnLayout->addWidget(timeEdit_ = new QLineEdit());	
	timeValidator_ = new QDoubleValidator(0, 999999999999, 4, 0);
	timeEdit_->setValidator(timeValidator_);
	btnLayout->addSpacing(20);	

	updateButton_ = new PaneButton(QPixmap(":/data/Resources/refresh.png"), QSize(25, 25), "Refresh");
	updateButton_->setCheckable(true);

//	btnLayout->addWidget(updateButton_);
//	btnLayout->addWidget(setKeyButton_ = new PaneButton(QPixmap(":/data/Resources/timeline/setkey.png"), QSize(30, 30), "Set Key"));	
//	btnLayout->addSpacing(5);
//	btnLayout->addWidget(deleteButton_ = new PaneButton(QPixmap(":/data/Resources/timeline/timeDelete.png"), QSize(30, 30), "Delete"));

	setKeyButton_ = new QPushButton();
	setKeyButton_->setIcon(QIcon(QPixmap(":/data/Resources/timeline/setkey.png")));
	setKeyButton_->setText("Set Key");
	setKeyButton_->setLayoutDirection(Qt::LeftToRight);

	deleteButton_ = new QPushButton();
	deleteButton_->setIcon(QIcon(QPixmap(":/data/Resources/timeline/timeDelete.png")));
	deleteButton_->setText("Delete Key");
	deleteButton_->setLayoutDirection(Qt::LeftToRight);

	btnLayout->addWidget(setKeyButton_);
	btnLayout->addWidget(deleteButton_);
//	btnLayout->addSpacing(5);
	layout->addLayout(btnLayout);
	timeEdit_->setMaximumWidth(75);

	connect(vScrollBar_, SIGNAL(valueChanged(int)), 
		this, SIGNAL(valueChanged(int)));

	connect(hScrollBar_, SIGNAL(valueChanged(int)), 
		this, SLOT(hScrollChanged(int)));

	connect(timeEdit_, SIGNAL(editingFinished()), this, SLOT(timeEdited()));
	connect(playButton_, SIGNAL(clicked()),	this, SLOT(onPlay()));
	connect(stopButton_, SIGNAL(clicked()),	this, SLOT(onStop()));
	connect(setKeyButton_, SIGNAL(clicked()), this, SLOT(onSetKey()));
	connect(updateButton_, SIGNAL(clicked()), this, SLOT(onUpdate()));
	connect(deleteButton_, SIGNAL(clicked()), this, SLOT(onDelete()));
	connect(timeLineZoomIn_, SIGNAL(clicked()), this, SLOT(onTimeZoomIn()));
	connect(timeLineZoomOut_, SIGNAL(clicked()), this, SLOT(onTimeZoomOut()));
	connect(timeLineSlider_, SIGNAL(valueChanged(int)), this, SLOT(timeLineSpacing(int)));
	setEnabled(false);

	setFocusPolicy(Qt::ClickFocus);
}


TimeLinePane::~TimeLinePane()
{
	timer_.stop();
	delete timeValidator_;
	delete updateButton_;
}

bool TimeLinePane::animateOnTimeChanged() const
{
	return updateButton_->isChecked();
}

void TimeLinePane::hScrollChanged(int val)
{
	timeLineBar_->update();
	channelBars_->update();
}

void TimeLinePane::setVScrollValue(int val)
{
	vScrollBar_->setValue(val);
	channelBars_->update();
}

void TimeLinePane::setVScrollRange(int min, int max)
{
	vScrollBar_->setRange(min, max);
	channelBars_->update();
}

QPoint TimeLinePane::scrollPos() const
{
	return QPoint(hScrollBar_->value(), vScrollBar_->value());
}

void TimeLinePane::setDocument(EditorDocument * document)
{
	timer_.stop();
	if (document_) document_->disconnect(this);
	document_ = document;
	channelBars_->setDocument(document);
	timeLineBar_->setDocument(document);

	if (document_)
	{
		connect(document_, SIGNAL(animationSelectionChanged()), 
			this, SLOT(animationSelectionChanged()));
		animationSelectionChanged();

		connect(document_, SIGNAL(timeLineTimeChanged()),
			this, SLOT(timeLineTimeChanged()));
		connect(document_, SIGNAL(timeLineTimeChanging()),
			this, SLOT(timeLineTimeChanged()));
		timeLineTimeChanged();

		connect(document, SIGNAL(channelSelectionChanged()),
			this, SLOT(channelSelectionChanged()));
		
		updateButton_->setChecked(document_->updateOnTimeChange());
	}
	else
	{
		timeEdit_->setText(QString());
	}
}

void TimeLinePane::keyPressEvent(QKeyEvent * keyEvent)
{
	QWidget::keyPressEvent(keyEvent); 
	
	if(keyEvent->key() == Qt::Key_Shift)
	{
		timeLineBar_->isShiftKey_ = true;
		channelBars_->isShiftKey_ = true;
	}
	if(keyEvent->key() == Qt::Key_Control)
		channelBars_->isCtrlKey_ = true;
	

	if(keyEvent->key() == Qt::Key_F6)
		onSetKey();
	if(keyEvent->key() == Qt::Key_F7 || keyEvent->key() == Qt::Key_Delete)
		onDelete();
	if(keyEvent->key() == Qt::Key_Space)
		isPlaying_ == false ? onPlay() : onStop();
	if(keyEvent->key() == Qt::Key_Plus || keyEvent->key() == Qt::Key_Equal)
	{
		int sliderValue = timeLineSlider_->value();
		timeLineSlider_->setValue(++sliderValue);
	}
	if(keyEvent->key() == Qt::Key_Minus)
	{
		int sliderValue = timeLineSlider_->value();
		timeLineSlider_->setValue(--sliderValue);
	}
	if(keyEvent->key() == Qt::Key_PageUp)
		hScrollBar_->setValue(hScrollBar_->value() - hScrollBar_->pageStep());
	if(keyEvent->key() == Qt::Key_PageDown)
		hScrollBar_->setValue(hScrollBar_->value() + hScrollBar_->pageStep());
}

void TimeLinePane::keyReleaseEvent(QKeyEvent * keyEvent)
{
	QWidget::keyReleaseEvent(keyEvent);

	if(keyEvent->key() == Qt::Key_Shift)
	{
		timeLineBar_->isShiftKey_ = false;
		channelBars_->isShiftKey_ = false;
	}

	if(keyEvent->key() == Qt::Key_Control)
		channelBars_->isCtrlKey_ = false;
}

void TimeLinePane::timerEvent(QTimerEvent * event)
{
	if (event->timerId() == timer_.timerId())
	{
		double time = (double)elapsed_.elapsed()/1000;
		document_->scrubTimeLineTime(startTime_ + time);
		channelBars_->update();
	}
}

void TimeLinePane::paintEvent(QPaintEvent * event)
{
//	QPainter p(this);
}

void TimeLinePane::animationSelectionChanged()
{
	setEnabled(document_->selectedAnimation());
}

void TimeLinePane::timeLineTimeChanged()
{
	QString str;
	str.sprintf("%.4f", document_->timeLineTime());
	timeEdit_->setText(str);	
	int markerX = timeLineBar_->getPosFromTime(document_->timeLineTime());

	if(markerX > width() || markerX < 0)
		hScrollBar_->setValue(hScrollBar_->value() + markerX);
}

void TimeLinePane::timeEdited()
{	
	float time = timeEdit_->text().toFloat();
	document_->setTimeLineTime(time);
	timeLineBar_->update();
	channelBars_->update();
}

void TimeLinePane::channelSelectionChanged()
{
	const vector<AnimationChannel *> & selChans = document_->selectedChannels();
	setKeyButton_->setEnabled(!selChans.empty());
}

void TimeLinePane::onPlay()
{
	timer_.start(10, this);
	elapsed_.start();
	startTime_ = document_->timeLineTime();
	isPlaying_ = true;

	//make sure animateOnTimeChanged is turned on during animation
	playStartAnimState_ = document_->updateOnTimeChange();
	if (!document_->updateOnTimeChange())
	{
		document_->setUpdateOnTimeChange(true);
		updateButton_->setChecked(true);
	}
}

void TimeLinePane::onStop()
{
	if (isPlaying_)
	{
		timer_.stop();
		isPlaying_ = false;
		double time = (double)elapsed_.elapsed()/1000;
		document_->setTimeLineTime(startTime_ + time);
		//restore the update button state to its initial state before animation
		//playback was initiated
		document_->setUpdateOnTimeChange(playStartAnimState_);
		updateButton_->setChecked(playStartAnimState_);
		timeLineBar_->update();
		channelBars_->update();
	}
}

void TimeLinePane::onSetKey()
{
	const vector<AnimationChannel *> & chans = document_->selectedChannels();

	Animation * anim = document_->selectedAnimation();

	SceneObject * obj = document_->selectedObject();

	vector<AnimationChannelSPtr> setKeyChans;
	foreach(AnimationChannel * chan, chans)
		setKeyChans.push_back(anim->findChannel(chan));	
	
	try {
		document_->doCommand(new SetKeyFramesCmd(document_,
			anim, obj, setKeyChans, document_->timeLineTime())); 
	} catch(exception & e) {
		QMessageBox::information(0, tr("Error"), tr(e.what()));
	}
}

void TimeLinePane::onUpdate()
{	
	bool enabled = updateButton_->isChecked();
	document_->setUpdateOnTimeChange(enabled);
	
	/*
	if (enabled)
	{		
		//trigger an update of the scene based on the current animation time
		float time = timeEdit_->text().toFloat();
		document_->setTimeLineTime(time);
	}	*/
}

void TimeLinePane::onDelete()
{
	document_->doCommand(new DeleteKeyFramesCmd(
		document_, document_->selectedKeyFrames()));	
}

void TimeLinePane::timeLineSpacing(int sliderValue)
{
	timeLineBar_->tickSpacing_ = sliderValue * 2;
	timeLineBar_->distPerSecond_ = sliderValue * 2 * 10;
	timeLineBar_->update();
	channelBars_->update();
}

void TimeLinePane::onTimeZoomIn()
{
	timeLineSlider_->setValue(timeLineSlider_->value() + 1);
	timeLineBar_->update();
	channelBars_->update();
}

void TimeLinePane::onTimeZoomOut()
{
	timeLineSlider_->setValue(timeLineSlider_->value() - 1);
	timeLineBar_->update();
	channelBars_->update();
}