#pragma once

class EditorDocument;
class EventListPane;
class ActionListPane;
class MainWindow;

class EventActionPane : public QSplitter
{
public:
	EventActionPane(MainWindow * mainWindow);
	void setDocument(EditorDocument * document);
	virtual QSize sizeHint() const {return QSize(150, 150);}
private:
	EditorDocument * document_;
	EventListPane * eventListPane_;
	ActionListPane * actionListPane_;
};