#pragma once
class EditorDocument;
class EditorScene;
class PropertyPage;
class GLWidget;
class MainWindow;

class PropertyPane : public QScrollArea
{
	Q_OBJECT
public:
	enum ObjectMode
	{
		ModeSceneObject,
		ModeAppObject
	};
public:
	PropertyPane();
	~PropertyPane();

	void setDocument(EditorDocument * document);
	void setView(GLWidget * widget);

	virtual QSize sizeHint() const;

	void setObjectMode(ObjectMode mode);

public slots:
	void objectSelectionChanged();
	void appObjectSelectionChanged();
	void updateCurPage();
private:
	void setPage(PropertyPage * page);
	virtual void closeEvent(QCloseEvent * event);
protected:
	virtual void keyPressEvent(QKeyEvent * keyEvent);

private:
	EditorScene * scene_;
	EditorDocument * document_;
	GLWidget * view_;
	PropertyPage * curPropertyPage_;
	QWidget * curPageWidget_;
	QWidget * widget_;
	bool ignoreModeChanged_;
	
	QVBoxLayout * layout_;

	
	ObjectMode mode_;
};