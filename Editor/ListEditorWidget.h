#pragma once

class ListEditorWidget : public QWidget
{
	Q_OBJECT
public:
	ListEditorWidget(int lineEditX, int endMargin, QWidget * parent);
	QLineEdit * GetLineEdit() const {return lineEdit_;}
	void Apply();
	void Cancel();

	void setText(const std::string & text);
signals:
	void editingFinished();
	void editingCancelled();
protected:
	virtual void focusInEvent(QFocusEvent * event);
private:
	QLineEdit * lineEdit_;	
	/**
	hack:
	setEditorData is called repeatedly as an indirect result of the scene
	being constantly updated. Only actually set the text the first time
	setEditorData is called. Otherwise, the user will never be able to 
	edit the text because it keeps getting reset.
	*/
	bool textInit_;
};