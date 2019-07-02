#include "stdafx.h"
#include "ScenePropertiesDlg.h"
#include "Scene.h"

ScenePropertiesDlg::ScenePropertiesDlg(
	Scene * scene, QWidget * parent) 
: 
QDialog(parent), dimValidator_(1, 9000, 4, 0)
{
	ui.setupUi(this);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint); 

	ui.widthEdit->setValidator(&dimValidator_);
	ui.heightEdit->setValidator(&dimValidator_);

	QString str;
	str.sprintf("%.4f", scene->screenWidth());
	ui.widthEdit->setText(str);
	str.sprintf("%.4f", scene->screenHeight());
	ui.heightEdit->setText(str);

	colorButton_ = new QPushButton("Set Color", this);
	connect(colorButton_, SIGNAL(clicked()), this, SLOT(onSetColor()));
	//colorLabel_ =new QLabel(this);
	//colorLabel_ = ui.colorLabel;
	//colorLabel_->setText("a");
	colorLabel_ = new ColorLabel;

	color_ = scene->bgColor();
	colorLabel_->setColor(color_);

	//int r = color_.r * 255;
	//int g = color_.g * 255;
	//int b = color_.b * 255;
	//r = min(r, 255); r = max(r, 0);
	//g = min(g, 255); g = max(g, 0);
	//b = min(b, 255); b = max(b, 0);

	//QColor color = QColor(r, g, b);

	//QPalette sample_palette;
	//sample_palette.setColor(QPalette::Window, color);
	//colorLabel_->setPalette(sample_palette);
	//colorLabel_->setAutoFillBackground(true);

	ui.gridLayout->addWidget(colorLabel_);
	ui.gridLayout->addWidget(colorButton_);

	

	connect(ui.widthEdit, SIGNAL(textEdited(const QString &)), 
		this, SLOT(onInputChanged(const QString &))); 
	connect(ui.heightEdit, SIGNAL(textEdited(const QString &)), 
		this, SLOT(onInputChanged(const QString &)));
	connect(ui.okButton, SIGNAL(clicked()), this, SLOT(onOk()));
	connect(ui.zoom, SIGNAL(currentIndexChanged(int)), this, SLOT(onInputChanged(const QString &)));
	onInputChanged("");
}

void ScenePropertiesDlg::onInputChanged(const QString &)
{
	bool validInput = true;
	int pos;
	
	QString qstr;

	qstr = ui.widthEdit->text();
	validInput &= 
		(dimValidator_.validate(qstr, pos) == QValidator::Acceptable);

	qstr = ui.heightEdit->text();
	validInput &= 
		(dimValidator_.validate(qstr, pos) == QValidator::Acceptable);

	validInput &= (ui.widthEdit->text().toFloat() > 0);
	validInput &= (ui.heightEdit->text().toFloat() > 0);

	ui.okButton->setEnabled(validInput);

}

void ScenePropertiesDlg::onOk()
{
	width_ = ui.widthEdit->text().toFloat();
	height_ = ui.heightEdit->text().toFloat();
	zoom_ = ui.zoom->currentIndex()?false:true;
}

void ScenePropertiesDlg::onSetColor()
{
	int r = color_.r * 255;
	int g = color_.g * 255;
	int b = color_.b * 255;
	r = min(r, 255); r = max(r, 0);
	g = min(g, 255); g = max(g, 0);
	b = min(b, 255); b = max(b, 0);
	QColor color = QColorDialog::getColor(QColor(r, g, b), this);
	if (color.isValid())
	{
		color_.r = (float)color.red()/255;
		color_.g = (float)color.green()/255;
		color_.b = (float)color.blue()/255;
		colorLabel_->setColor(color_);
	}
}

void ColorLabel::setColor(const Color & color)
{
	color_ = color;
	update();
}

void ColorLabel::paintEvent(QPaintEvent * event)
{
	QPainter p(this);	

	int r = color_.r * 255;
	int g = color_.g * 255;
	int b = color_.b * 255;
	r = min(r, 255); r = max(r, 0);
	g = min(g, 255); g = max(g, 0);
	b = min(b, 255); b = max(b, 0);

	p.setBrush(QColor(r, g, b));
	p.drawRect(10, 1, 20 - 1, height() - 2);
}
