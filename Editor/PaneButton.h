#pragma once

class PaneButton : public QPushButton
{
public:
	PaneButton(const QString & str)
	{
		setText(str);
		setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
		QRect textSize = fontMetrics().boundingRect(str);
		hintSize_ = QSize(textSize.width() + 20, QPushButton::sizeHint().height());
	}
	PaneButton(const QPixmap & pixmap, QSize iconSize, QString strToolTip)
	{
		QIcon qIcon(pixmap);
		setIconSize(iconSize);
		setIcon(qIcon);
		setFixedSize(iconSize);
		setMaximumSize(iconSize);
		setToolTip(strToolTip);
		hintSize_ = QSize(iconSize);
		setStyleSheet("QPushButton{background-color: none; border: none;}" 
			"QPushButton:pressed:hover{background-color: rgb(170, 170, 170);}" 
			"QPushButton:hover{background-color: rgb(190, 190, 190);}");
	}
	virtual QSize sizeHint() const {return hintSize_;}
private:
	QSize hintSize_;
};