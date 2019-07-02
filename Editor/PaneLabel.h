#pragma once

class PaneLabel : public QLabel
{
public:
	PaneLabel()
	{		
		init();
	}

	PaneLabel(const QString & text) : QLabel(text)
	{
		init();
	}

private:
	void init()
	{
		this->setStyleSheet(
			"border: 0px;"
			"background-image: url(:/data/Resources/paneLabelBg.png);"
			"background-repeat:repeat-x;");
		this->setMinimumHeight(25);
		this->setMaximumHeight(25);
	}
};