#pragma once
#include "MainWindow.h"

class ObjectListPane;
class SceneObjectListPane;
class AppObjectListPane;

class ObjectListPane : public QWidget
{
Q_OBJECT
public:
	ObjectListPane(MainWindow * mainWindow);
	~ObjectListPane();

	void setDocument(EditorDocument * document);
	QSize sizeHint() const;
	MainWindow * parent() const {return mainWindow_;}

	void setObjectMode(MainWindow::ObjectMode);

protected:
	MainWindow * mainWindow_;
	QVBoxLayout * layout_;

	AppObjectListPane * appObjectListPane_;
	SceneObjectListPane * sceneObjectListPane_;

	MainWindow::ObjectMode mode_;
};


