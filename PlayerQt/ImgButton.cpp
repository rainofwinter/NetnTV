#include "stdafx.h"
#include "ImgButton.h"

QSize ImgButton::sizeHint() const
{
	return pixmap_.size();
}

ImgButton::ImgButton(const QString & imgName, QWidget * parent)
: QWidget(parent)
{
	pixmap_ = QPixmap(imgName);
}

void ImgButton::paintEvent(QPaintEvent * event)
{
	QPainter p(this);
	p.drawPixmap(QPoint(0, 0), pixmap_);
}

void ImgButton::mousePressEvent(QMouseEvent * event)
{
	emit clicked();
}