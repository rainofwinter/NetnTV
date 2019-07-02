#pragma once

class AboutDlg : public QDialog
{
	Q_OBJECT
public:
	AboutDlg(QWidget * parent);
private:
	virtual void paintEvent(QPaintEvent * event);
	virtual void mousePressEvent(QMouseEvent * event);
	virtual void keyPressEvent(QKeyEvent * event);
	QPixmap pixmap_;
};