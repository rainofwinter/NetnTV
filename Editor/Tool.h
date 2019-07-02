#pragma once

class GLWidget;
class EditorDocument;

class Tool : public QObject
{
	Q_OBJECT
public:
	Tool(GLWidget * gl);
	virtual ~Tool();
	virtual void init();
	virtual void uninit();	
	/**
	@return handled
	*/
	virtual bool keyPressEvent(QKeyEvent * event) {return false;}
	virtual void mousePressEvent(QMouseEvent * event) {}
	virtual void mouseMoveEvent(QMouseEvent * event) {}
	virtual void mouseReleaseEvent(QMouseEvent * event) {}
	virtual void mouseDoublePressEvent(QMouseEvent * event) {}
	virtual void draw() {}
	virtual void setActive(bool val) {active_ = val;}
	bool isActive() const {return active_;}

	/**
	@return whether the mouse cursor is over the tool gizmo
	*/
	virtual bool hoveringOverGizmo(QMouseEvent * event) 
	{
		return false;
	}

	virtual bool isBeingUsed() const = 0;

	virtual std::string getToolName() { return toolName_;}

signals:
	void objectChanged();
	void appObjectChanged();
private slots:
	virtual void onObjectSelectionChanged() {}
protected:
	bool active_;
	GLWidget * glWidget_;
	GfxRenderer * gl_;
	EditorDocument * document_;
	std::string toolName_;
};