#include "stdafx.h"
#include "Property.h"
#include "GetFontFile.h"
#include "Utils.h"
#include "FileUtils.h"
#include "EditorGlobal.h"

using namespace std;

UnitDoubleValidator::UnitDoubleValidator() :
QDoubleValidator(0, 1, 4, 0)
{
	setNotation(QDoubleValidator::StandardNotation);
}

QValidator::State UnitDoubleValidator::validate(QString & str, int & pos) const
{
	float val = str.toFloat();
	if (val > 1)
	{
		return Invalid;
	}
	return QDoubleValidator::validate(str, pos);
}
////////////////////////////////////////////////////////////////////////////////
TimeValidator::TimeValidator() :
QDoubleValidator(0, 9999999999, 4, 0)
{
	setNotation(QDoubleValidator::StandardNotation);
}

////////////////////////////////////////////////////////////////////////////////
Property::Property()
{
}

Property::Property(const QString & name)
{
	name_ = name;
}

void Property::changed()
{

	if (widget() && widget()->hasFocus())
	{

		QWidget * parent = widget();
		while (1)
		{
			if (!parent->parentWidget()) break;
			parent = parent->parentWidget();
		}
		parent->setFocus();
	}
	emit changed(this);
}

////////////////////////////////////////////////////////////////////////////////
EditPropertyWidget::EditPropertyWidget()
{
	setMaximumHeight(18);
}

void EditPropertyWidget::focusInEvent(QFocusEvent * event)
{
	QLineEdit::focusInEvent(event);	
}

EditProperty::EditProperty(const QString & name, const QValidator * validator)
{
	emitChanged_ = true;
	setName(name);
	lineEdit_ = new EditPropertyWidget();
	lineEdit_->setValidator(validator);
	connect(lineEdit_, SIGNAL(editingFinished()), 
		this, SLOT(changed()));
}


void EditProperty::changed()
{
	if (!emitChanged_) return;
	QString newValue = lineEdit_->text();
	if (lastValue_ != newValue)
	{
		lastValue_ = newValue;
		Property::changed();
	}
}

void EditProperty::setEnabled(bool enabled)
{
	lineEdit_->setEnabled(enabled);
}

void EditProperty::setValue(const QString & value)
{
	emitChanged_ = false;
	lineEdit_->setText(value);	
	lastValue_ = value;
	emitChanged_ = true;
}

QString EditProperty::value() const
{
	return lineEdit_->text();
}

////////////////////////////////////////////////////////////////////////////////
EditTextPropertyWidget::EditTextPropertyWidget()
{
	setMaximumHeight(200);
}

void EditTextPropertyWidget::focusInEvent(QFocusEvent * event)
{
	QTextEdit::focusInEvent(event);	
}

void EditTextPropertyWidget::focusOutEvent(QFocusEvent * event)
{
	QTextEdit::focusOutEvent(event);	
	emit editingFinished ();
}

EditTextProperty::EditTextProperty(const QString & name, const QValidator * validator)
{
	emitChanged_ = true;
	setName(name);

	textEdit_ = new EditTextPropertyWidget();	
	
	connect(textEdit_, SIGNAL(editingFinished()), 
		this, SLOT(changed()));
}

void EditTextProperty::changed()
{
	if (!emitChanged_) return;
	QString newValue = textEdit_->toPlainText();
	if (lastValue_ != newValue)
	{
		lastValue_ = newValue;
		Property::changed();
	}
}

void EditTextProperty::setEnabled(bool enabled)
{
	textEdit_->setEnabled(enabled);
}

void EditTextProperty::setValue(const QString & value)
{
	emitChanged_ = false;
	textEdit_->setText(value);
	emitChanged_ = true;
}

QString EditTextProperty::value() const
{
	return textEdit_->toPlainText();
}

////////////////////////////////////////////////////////////////////////////////

ComboBoxProperty::ComboBoxProperty(const QString & name)
{
	emitChanged_ = true;
	setName(name);
	comboBox_ = new QComboBox();
	comboBox_->setMaximumHeight(18);
	comboBox_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	connect(comboBox_, SIGNAL(currentIndexChanged(int)), 
		this, SLOT(changed(int)));

}

void ComboBoxProperty::addItem(const QString & str)
{
	emitChanged_ = false;
	comboBox_->addItem(str);
	emitChanged_ = true;
}

void ComboBoxProperty::clearItems()
{
	emitChanged_ = false;
	comboBox_->clear();
	emitChanged_ = true;
}

void ComboBoxProperty::setEnabled(bool enabled)
{
	comboBox_->setEnabled(enabled);
}

void ComboBoxProperty::changed(int index)
{
	if (!emitChanged_) return;
	int newValue = comboBox_->currentIndex();
	if (lastValue_ != newValue)
	{
		lastValue_ = newValue;
		Property::changed();
	}	
}

void ComboBoxProperty::setValue(int index)
{
	emitChanged_ = false;
	comboBox_->setCurrentIndex(index);
	lastValue_ = index;
	emitChanged_ = true;
}

int ComboBoxProperty::value() const
{
	return comboBox_->currentIndex();
}
////////////////////////////////////////////////////////////////////////////////

FontProperty::FontProperty(const QString & name)
{
	emitChanged_ = true;
	setName(name);

	
	
	/*
	comboBox_->setMaximumHeight(18);
	comboBox_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	connect(comboBox_, SIGNAL(currentIndexChanged(int)), 
		this, SLOT(changed(int)));*/


	
	button_ = new QPushButton();	
	connect(button_, SIGNAL(clicked()),
		this, SLOT(onClicked()));

	setValue(NULL);

}



void FontProperty::onClicked()
{
	QWidget * dlgParent = widget();
	while (dlgParent->parent()) dlgParent = dlgParent->parentWidget();

	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));
	if (font_)
	{
		lf = getFontData(font_->fontFile, font_->faceIndex);
		if (font_->bold) lf.lfWeight = FW_BOLD;
		if (font_->italic) lf.lfItalic = 255;
		lf.lfHeight = -MulDiv(font_->pointSize, EditorGlobal::instance().logPixelsY(), 72);
	}
	else
	{
		lf.lfHeight = -MulDiv(16, EditorGlobal::instance().logPixelsY(), 72);
	}

/*
	QFont qFont(QString::fromStdWString(lf.lfFaceName));
	qFont.setItalic(lf.lfItalic == 255);
	switch(lf.lfWeight)
	{
	case FW_LIGHT: qFont.setWeight(QFont::Light); break;
	case FW_DEMIBOLD: qFont.setWeight(QFont::DemiBold); break;
	case FW_BOLD: qFont.setWeight(QFont::Bold); break;
	case FW_BLACK: qFont.setWeight(QFont::Black); break;
	default:
		qFont.setWeight(QFont::Normal);
	}
	qFont.setPointSize(font_.pointSize);	*/
	

	CHOOSEFONT chooseFont;
	wchar_t szFontStyle[1024];
	chooseFont.lStructSize = sizeof(CHOOSEFONT);
	chooseFont.hwndOwner = dlgParent->winId();
	chooseFont.hDC = 0;	
	chooseFont.lpLogFont = &lf;	
	chooseFont.lpszStyle = szFontStyle;
	chooseFont.Flags = CF_SCREENFONTS | CF_INITTOLOGFONTSTRUCT | CF_TTONLY | CF_SCALABLEONLY | CF_NOVERTFONTS;

/*
	QFontDialog dlg;	
	dlg.setCurrentFont(qFont);
	if (dlg.exec())
	*/
	if (ChooseFont(&chooseFont))	
	{
		LOGFONT getFontLf;

		/*
		QFont font = dlg.currentFont();	

		lstrcpy(lf.lfFaceName, font.family().toStdWString().c_str());
		
		switch(font.weight())
		{
		case QFont::Light: lf.lfWeight = FW_LIGHT; break;
		case QFont::Normal: lf.lfWeight = FW_NORMAL; break;
		case QFont::DemiBold: lf.lfWeight = FW_DEMIBOLD; break;
		case QFont::Bold: lf.lfWeight = FW_BOLD; break;
		case QFont::Black: lf.lfWeight = FW_BLACK; break;
		}
		lf.lfItalic = font.italic()?255:0;	
		
		TextFont textFont;
		

		wstring fileName;		
		int faceIndex;		
		bool ret = GetFontFile(lf, &fileName, &faceIndex, &getFontLf);
		if (!ret) return;
		string fileNameA(fileName.begin(), fileName.end());

		textFont.fontFile = fileNameA;
		textFont.faceIndex = faceIndex;
		textFont.bold = 
			font.weight() != QFont::Normal && 
			font.weight() != QFont::Light;

		textFont.italic = font.italic();
		textFont.pointSize = font.pointSize();
		
		*/		

		TextFont textFont;

		wstring fileName;
		int faceIndex;		
		bool ret = GetFontFile(lf, &fileName, &faceIndex, &getFontLf);	
		if (!ret) return;

		textFont.fontFile = convertToRelativePath(string(fileName.begin(), fileName.end()));
		textFont.faceIndex = faceIndex;
		textFont.italic = lf.lfItalic != 0;
		textFont.bold = lf.lfWeight > FW_NORMAL;		
		textFont.pointSize = 		
			-MulDiv(lf.lfHeight, 72, EditorGlobal::instance().logPixelsY());
			

		//handle the cases where italics is baked into the font file
		if (getFontLf.lfItalic) textFont.italic = false;
		
		//handle cases where bold is baked into the font file
		if (getFontLf.lfWeight > FW_NORMAL) textFont.bold = false;
		

		setValue(&textFont);
		Property::changed();
	}
}

void FontProperty::setValue(const TextFont * font)
{

	
	QString btnText;
	
	if (font)
	{
		font_.reset(new TextFont(*font));
		btnText.sprintf("%s, face %d", 
			getFileNameWithoutDirectory(font->fontFile).c_str(), 
			font->faceIndex);

		if (font->bold) btnText += " bold";
		if (font->italic) btnText += " italic";
		button_->setText(btnText);
	}
	else
	{
		font_.reset();
		button_->setText("");
	}	
}


///////////////////////////////////////////////////////////////////////////////

RadioGroupProperty::RadioGroupProperty(const QString & name)
{
	emitChanged_ = true;
	setName(name);

	radioGroup_ = new QWidget();
	layout_ = new QHBoxLayout(radioGroup_);
	layout_->setMargin(0);
	layout_->setSpacing(0);
	lastValue_ = -1;
}

void RadioGroupProperty::addItem(const QString & str)
{
	QRadioButton * btn = new QRadioButton(str);

	layout_->addWidget(btn);
	connect(btn, SIGNAL(clicked()), this, SLOT(onClicked()));

}


void RadioGroupProperty::onClicked()
{
	QRadioButton * senderBtn = qobject_cast<QRadioButton *>(sender());
	int newValue = -1;
	for (int i = 0; i < (int)buttons_.size(); ++i)
		if (buttons_[i] == senderBtn) newValue = i;
	if (lastValue_ != newValue)
	{
		lastValue_ = newValue;
		Property::changed();
	}	
}

void RadioGroupProperty::setValue(int index)
{
	emitChanged_ = false;		
	buttons_[index]->setChecked(true);
	lastValue_ = index;
	emitChanged_ = true;
}

int RadioGroupProperty::value() const
{
	return lastValue_;
}

///////////////////////////////////////////////////////////////////////////////

//ButtonProperty::ButtonProperty(const QString  & name)
//{
//	button_ = new QPushButton;
//}
//
/////////////////////////////////////////////////////////////////////////////////
//
//ToolButtonProperty::ToolButtonProperty(const QString  & name)
//{
//	button_ = new QPushButton;
//	button_->setCheckable(true);
//	// ======= temp ======= 
//	QIcon icon;
//	// icon.addFile(QString::fromUtf8(":/data/Resources/FileNew.png"), QSize(), QIcon::Normal, QIcon::Off);
//	if(name == "left")
//		icon.addFile(QString::fromUtf8(":/data/Resources/properties/left_align.png"), QSize(), QIcon::Normal);
//	else if(name == "center")
//		icon.addFile(QString::fromUtf8(":/data/Resources/properties/center_align.png"), QSize(), QIcon::Normal);
//	else if(name == "right")
//		icon.addFile(QString::fromUtf8(":/data/Resources/properties/right_align.png"), QSize(), QIcon::Normal);
//	else if(name == "justify")
//		icon.addFile(QString::fromUtf8(":/data/Resources/properties/justify_align.png"), QSize(), QIcon::Normal);
//
//	button_->setIcon(icon);
//	// ====================
//	// button_->setStyleSheet("* {  background-color: rgb(92,92,92) }");
//	button_->setFlat(true);
//	connect(button_, SIGNAL(clicked()), this, SLOT(onClicked()));
//}
//
//void ToolButtonProperty::setValue(bool var)
//{
//	button_->setChecked(var);
//}
//bool ToolButtonProperty::value() const
//{
//	return button_->isChecked();
//}
//void ToolButtonProperty::onClicked()
//{
//	Property::changed();	
//}

ButtonProperty::ButtonProperty() : Property(){
	button_ = new QPushButton;

	connect(button_, SIGNAL(clicked()), this, SLOT(onClicked()));
}
ButtonProperty::ButtonProperty(const QString  & name) : Property(name)
{
	button_ = new QPushButton(name);

	connect(button_, SIGNAL(clicked()), this, SLOT(onClicked()));
}

void ButtonProperty::onClicked()
{
	Property::changed();	
}

void ButtonProperty::setEnable(bool b)
{
	button_->setEnabled(b);
}

///////////////////////////////////////////////////////////////////////////////

ToolButtonProperty::ToolButtonProperty(const QString  & name) : ButtonProperty(name)
{
	
	bool bHasIcon = true;
	// ======= temp ======= 
	QIcon icon;
	// icon.addFile(QString::fromUtf8(":/data/Resources/FileNew.png"), QSize(), QIcon::Normal, QIcon::Off);
	if(name == "left")
		icon.addFile(QString::fromUtf8(":/data/Resources/properties/left_align.png"), QSize(), QIcon::Normal);
	else if(name == "center")
		icon.addFile(QString::fromUtf8(":/data/Resources/properties/center_align.png"), QSize(), QIcon::Normal);
	else if(name == "right")
		icon.addFile(QString::fromUtf8(":/data/Resources/properties/right_align.png"), QSize(), QIcon::Normal);
	else if(name == "justify_left")
		icon.addFile(QString::fromUtf8(":/data/Resources/properties/justify_align.png"), QSize(), QIcon::Normal);
	else
		bHasIcon = false;

	if(bHasIcon)
		button_->setText("");

	button_->setCheckable(true);

	if(bHasIcon)
		button_->setIcon(icon);
	// ====================
	// button_->setStyleSheet("* {  background-color: rgb(92,92,92) }");
	button_->setFlat(true);
	
}

void ToolButtonProperty::setValue(bool var)
{
	button_->setChecked(var);
}
bool ToolButtonProperty::value() const
{
	return button_->isChecked();
}

////////////////////////////////////////////////////////////////////////////////
ColorPropertyWidget::ColorPropertyWidget()
{
	QHBoxLayout * layout = new QHBoxLayout(this);
	layout->setSpacing(0);
	layout->setMargin(0);
	layout->addSpacing(20);

	
	colorButton_ = new QToolButton;
	colorButton_->setText(tr("Set color..."));
	colorButton_->setMaximumHeight(20);
	colorButton_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	layout->addWidget(colorButton_);
		
	colorDlg_ = new QColorDialog(QApplication::activeWindow());
	connect(colorButton_, SIGNAL(clicked()), this, SLOT(onClicked()));
}

ColorPropertyWidget::~ColorPropertyWidget()
{
	delete colorDlg_;
}

void ColorPropertyWidget::setColor(const Color * color)
{
	if (color)
		color_.reset(new Color(*color));
	else
		color_.reset();
	update();
}

void ColorPropertyWidget::paintEvent(QPaintEvent * event)
{
	QPainter p(this);	

	if (color_)
	{	
		int r = color_->r * 255;
		int g = color_->g * 255;
		int b = color_->b * 255;
		r = min(r, 255); r = max(r, 0);
		g = min(g, 255); g = max(g, 0);
		b = min(b, 255); b = max(b, 0);

		p.setBrush(QColor(r, g, b));
		p.drawRect(0, 1, 20 - 1, height() - 2);
	}
}

void ColorPropertyWidget::onClicked()
{
	if (color_)
	{
		int r = color_->r * 255;
		int g = color_->g * 255;
		int b = color_->b * 255;
		r = min(r, 255); r = max(r, 0);
		g = min(g, 255); g = max(g, 0);
		b = min(b, 255); b = max(b, 0);

		colorDlg_->setCurrentColor(QColor(r, g, b));
	}
	else
		colorDlg_->setCurrentColor(QColor(0, 0, 0));

	
	colorDlg_->open(this, SLOT(onColorSelected()));
	
}

void ColorPropertyWidget::onColorSelected()
{
	QColor color = colorDlg_->selectedColor();
	color_.reset(new Color);
	color_->r = (float)color.red()/255;
	color_->g = (float)color.green()/255;
	color_->b = (float)color.blue()/255;
	update();
	emit changed();
}


ColorProperty::ColorProperty(const QString & name)
{
	setName(name);


	widget_ = new ColorPropertyWidget();
	connect(widget_, SIGNAL(changed()), this, SLOT(onChanged()));
}

void ColorProperty::onChanged()
{
	Property::changed();	
}

void ColorProperty::setValue(const Color * value)
{
	widget_->setColor(value);
}

Color ColorProperty::value() const
{
	if (widget_->color())
		return *widget_->color();
	else 
		return Color(0.0f, 0.0f, 0.0f, 1.0f);	
}
////////////////////////////////////////////////////////////////////////////////

BoolProperty::BoolProperty(const QString & name)
{
	emitChanged_ = true;
	setName(name);
	comboBox_ = new QComboBox();
	comboBox_->setMaximumHeight(18);
	connect(comboBox_, SIGNAL(currentIndexChanged(int)), 
		this, SLOT(onChanged(int)));

	comboBox_->addItem(QObject::tr("true"));
	comboBox_->addItem(QObject::tr("false"));

}

void BoolProperty::onChanged(int index)
{
	if (!emitChanged_) return;
	bool newValue = comboBox_->currentIndex()?false:true;
	if (lastValue_ != newValue)
	{
		lastValue_ = newValue;
		Property::changed();
	}	
}

void BoolProperty::setValue(bool val)
{
	emitChanged_ = false;
	comboBox_->setCurrentIndex(val?0:1);
	lastValue_ = val;
	emitChanged_ = true;
}

void BoolProperty::setEnabled(bool enabled)
{
	comboBox_->setEnabled(enabled);
}

bool BoolProperty::value() const
{
	return comboBox_->currentIndex()?false:true;
}

////////////////////////////////////////////////////////////////////////////////
QString FileNameProperty::lastDir_;

FileNameProperty::FileNameProperty(const QString & name, const QString & filter)
{
	filter_ = filter;
	setName(name);

	widget_ = new QWidget;
	QHBoxLayout * layout = new QHBoxLayout(widget_);
	layout->setSpacing(0);
	layout->setMargin(0);

	fileEdit_ = new QLineEdit;
	fileEdit_->setReadOnly(true);
	layout->addWidget(fileEdit_);
	fileButton_ = new QToolButton();
	fileButton_->setText("...");
	
	layout->addWidget(fileButton_);
	connect(fileButton_, SIGNAL(clicked()), this, SLOT(onChanged()));

}

void FileNameProperty::setEnabled(bool enabled)
{
	widget_->setEnabled(enabled);
}

void FileNameProperty::onChanged()
{

	QString dirStr;
	if (fileEdit_->text().isEmpty())
		dirStr = lastDir_;
	else
		dirStr = fileEdit_->text();

	/*
	QString fileName = QFileDialog::getOpenFileName(
		0, QObject::tr("File"), dirStr, 
		filter_);
	*/

	QString fileName = getOpenFileName(QObject::tr("File"), dirStr, 
		filter_);

	if (!fileName.isEmpty())
	{
		string sFileName = qStringToStdString(fileName);
		string dir = getDirectory(sFileName);
		string relFileName = convertToRelativePath(Global::instance().readDirectory(), sFileName);		
		fileEdit_->setText(stdStringToQString(relFileName));

		lastDir_ = stdStringToQString(dir);
		Property::changed();
	}
}

void FileNameProperty::setValue(const QString & fileName)
{	
	fileEdit_->setText(fileName);		
}

QString FileNameProperty::value() const
{
	return fileEdit_->text();
}

////////////////////////////////////////////////////////////////////////////////

CheckBoxProperty::CheckBoxProperty(const QString & name)
{
	emitChanged_ = true;
	setName(name);
	checkBox_ = new QCheckBox(name);
	connect(checkBox_, SIGNAL(toggled(bool)), 
		this, SLOT(onChanged(bool)));
}

void CheckBoxProperty::onChanged(bool val)
{
	if (!emitChanged_) return;
	bool newValue = checkBox_->checkState();
	if (lastValue_ != newValue)
	{
		lastValue_ = newValue;
		Property::changed();
	}	
}

void CheckBoxProperty::setValue(bool val)
{
	emitChanged_ = false;
	
	if(val)
		checkBox_->setCheckState(Qt::CheckState::Checked);
	else
		checkBox_->setCheckState(Qt::CheckState::Unchecked);

	lastValue_ = val;
	emitChanged_ = true;
}

void CheckBoxProperty::setEnabled(bool enabled)
{
	checkBox_->setEnabled(enabled);
}

bool CheckBoxProperty::value() const
{
	return checkBox_->checkState();
}
