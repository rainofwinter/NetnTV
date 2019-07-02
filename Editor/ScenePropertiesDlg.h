#pragma once
#include "ui_scenePropertiesDlg.h"
class Scene;


class ColorLabel : public QWidget
{
	Q_OBJECT
public: 
	void setColor(const Color & color);
private:
	virtual void paintEvent(QPaintEvent * event);
	Color color_;
};


class ScenePropertiesDlg : public QDialog
{
	Q_OBJECT
public:
	ScenePropertiesDlg(Scene * scene, QWidget * parent);

	float width() const {return width_;}
	float height() const {return height_;}
	bool zoom() const {return zoom_;}
	Color color() const {return color_;}
protected slots:
	void onInputChanged(const QString &);
	void onOk();
	void onSetColor();
private:
	QDoubleValidator dimValidator_;	
	Ui::ScenePropertiesDlg ui;

	float width_, height_;
	bool zoom_;
	QPushButton * colorButton_;
	//QLabel * colorLabel_;
	ColorLabel * colorLabel_;
	Color color_;
};