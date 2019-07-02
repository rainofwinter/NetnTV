#include "stdafx.h"
#include "AboutDlg.h"
#include "buildStr.h"

AboutDlg::AboutDlg(QWidget * parent) : QDialog(parent)
{
	setWindowFlags(
		Qt::Window | Qt::Dialog | Qt::FramelessWindowHint); 

	
	pixmap_.load(":/data/Resources/splash.png");	
	resize(pixmap_.width(), pixmap_.height());
	setMask(pixmap_.mask());
	
}

void AboutDlg::paintEvent(QPaintEvent * event)
{
	QPainter painter(this);
	painter.drawPixmap(0, 0, pixmap_);
	QString aboutStr;
	aboutStr.sprintf("%s", buildInfoStr);
	QFontMetrics metrics = painter.fontMetrics();	
	int sideMargin = 20;
	int topMargin = 10;
	
	QFont font;	
	painter.setFont(font);

	painter.setPen(Qt::black);

	int x = pixmap_.width() - metrics.width(aboutStr) - sideMargin, y = topMargin + metrics.height();
	painter.drawText(x, y, aboutStr);

	font.setBold(true);
	painter.setFont(font);

	aboutStr = "Build:";
	x -= (metrics.width(aboutStr) + metrics.width('W'));	
	painter.drawText(x, y, aboutStr);
}

void AboutDlg::mousePressEvent(QMouseEvent * event)
{
	accept();
}

void AboutDlg::keyPressEvent(QKeyEvent * event)
{
	accept();
}