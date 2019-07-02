#pragma once

class AnimationListPane;
class ChannelListPane;
class TimeLinePane;
class KeyFramePane;
class EditorDocument;
class MainWindow;

class AnimationPane : public QSplitter
{
	Q_OBJECT
public:
	AnimationPane(MainWindow * mainWindow);
	~AnimationPane();
	void setDocument(EditorDocument * document);
	TimeLinePane * timeLinePane() {return timeLinePane_;}
protected slots:
	void rangeChangedChannels(int min, int max);
	void valueChangedChannels(int value);
	void valueChangedTimeline(int value);
private:
	AnimationListPane * animationsPane_;
	ChannelListPane * channelsPane_;
	TimeLinePane * timeLinePane_;
	KeyFramePane * keyFramePane_;
	EditorDocument * document_;
};