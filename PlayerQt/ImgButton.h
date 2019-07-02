#pragma once

class ImgButton : public QWidget
{
	Q_OBJECT
public:
	ImgButton(const QString & imgName, QWidget * parent = 0);
	virtual QSize sizeHint() const;
	virtual void paintEvent(QPaintEvent * event);
	virtual void mousePressEvent(QMouseEvent * event);
	
signals:
	void clicked();

private:
	QPixmap pixmap_;
};

