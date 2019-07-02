#pragma once
#include "MathStuff.h"
#include "Color.h"
#include "TextFont.h"
//#include <Qt/qmultilineedit.h>

class GLWidget;
class EditorDocument;

class UnitDoubleValidator : public QDoubleValidator
{
public:
	UnitDoubleValidator();
	virtual State validate(QString &, int &) const;
private:
};

class TimeValidator : public QDoubleValidator
{
public:
	TimeValidator();
private:
};

class PropertyPage;

class Property : public QObject
{
	Q_OBJECT
public:
	Property();
	Property(const QString & name);
	virtual ~Property() {}
	virtual QWidget * widget() const = 0;

	virtual void setDocument(EditorDocument * document) {}
	virtual void setView(GLWidget * widget) {}
	virtual void setName(const QString & name) {name_ = name;}
	const QString & name() const {return name_;}

	virtual PropertyPage * propertyPage() {return 0;}	
signals:
	void changed(Property * property);
protected slots:
	void changed();

protected:
	QString name_;
};

///////////////////////////////////////////////////////////////////////////////
class EditPropertyWidget : public QLineEdit
{
public:
	EditPropertyWidget();
private:
	virtual void focusInEvent(QFocusEvent * event);
};

class EditProperty : public Property
{
	Q_OBJECT
public:
	EditProperty(const QString & name, const QValidator * validator = 0);
	void setValue(const QString & value);

	virtual QWidget * widget() const
	{return lineEdit_;}

	QString value() const;

	void setEnabled(bool enabled);
protected slots:
	void changed();
private:
	EditPropertyWidget * lineEdit_;
	QString lastValue_;
	bool emitChanged_;
};

///////////////////////////////////////////////////////////////////
class EditTextPropertyWidget : public QTextEdit
{
	Q_OBJECT 

signals: 
    void editingFinished (); 

public:
	EditTextPropertyWidget();
private:
	virtual void focusInEvent(QFocusEvent * event);
	virtual void focusOutEvent(QFocusEvent * event);
};


class EditTextProperty : public Property
{
	Q_OBJECT	
public:
	EditTextProperty(const QString & name, const QValidator * validator = 0);
	void setValue(const QString & value);

	virtual QWidget * widget() const
	{return textEdit_;}	

	QString value() const;

	void setEnabled(bool enabled);
protected slots:
	void changed();
private:
	EditTextPropertyWidget * textEdit_;
	QString lastValue_;
	bool emitChanged_;
};

///////////////////////////////////////////////////////////////////////////////
class ComboBoxProperty : public Property
{
	Q_OBJECT
public:
	ComboBoxProperty(const QString & name);
	void setValue(int index);

	virtual QWidget * widget() const
	{return comboBox_;}

	void addItem(const QString & str);
	void clearItems();

	void setEnabled(bool enabled);
	int value() const;
protected slots:
	void changed(int index);
private:
	QComboBox * comboBox_;
	int lastValue_;
	bool emitChanged_;
};
///////////////////////////////////////////////////////////////////////////////
class FontProperty : public Property
{
	Q_OBJECT
public:
	FontProperty(const QString & name);

	virtual QWidget * widget() const
	{return button_;}

	const TextFont * value() const {return font_.get();}

	void setValue(const TextFont * font);
protected slots:
	//void changed(int index);
	void onClicked();
private:	
	QFontComboBox * comboBox_;
	QPushButton * button_;
	boost::scoped_ptr<TextFont> font_;
	
	int lastValue_;
	bool emitChanged_;
};
///////////////////////////////////////////////////////////////////////////////
class RadioGroupProperty : public Property
{
	Q_OBJECT
public:
	RadioGroupProperty(const QString & name);
	void setValue(int index);

	virtual QWidget * widget() const
	{return radioGroup_;}

	void addItem(const QString & str);

	int value() const;

private:
	void changed(int index);
private slots:
	void onClicked();
private:
	QWidget * radioGroup_;
	QHBoxLayout * layout_;
	std::vector<QRadioButton *> buttons_;
	int lastValue_;
	bool emitChanged_;
};

///////////////////////////////////////////////////////////////////////////////
//class ButtonProperty : public Property
//{
//	Q_OBJECT
//public:
//	ButtonProperty(const QString & name);
//
//private:
//	QPushButton * button_;
//};
//
//class ToolButtonProperty : public Property
//{
//	Q_OBJECT
//public:
//	ToolButtonProperty(const QString & name);
//	virtual QWidget * widget() const {return button_;}
//	void setValue(bool var);
//	bool value() const;
//
//private slots:
//	void onClicked();
//
//private:
//	QPushButton * button_;
//};

class ButtonProperty : public Property
{
	Q_OBJECT
public:
	ButtonProperty();
	ButtonProperty(const QString & name);
	virtual QWidget * widget() const {return button_;}

	void setEnable(bool b);
protected:
	QPushButton * button_;

	private slots:
		void onClicked();

};


class ToolButtonProperty : public ButtonProperty
{
	Q_OBJECT
public:
	ToolButtonProperty(const QString & name);
	void setValue(bool var);
	bool value() const;
};
///////////////////////////////////////////////////////////////////////////////
class ColorPropertyWidget : public QWidget
{
	Q_OBJECT
public:
	ColorPropertyWidget();
	~ColorPropertyWidget();
	const Color * color() const {return color_.get();}
	void setColor(const Color & color);
	void setColor(const Color * color);
signals:
	void changed();
private slots:
	void onClicked();
	void onColorSelected();
private:
	virtual void paintEvent(QPaintEvent * event);

	QToolButton * colorButton_;
	QColorDialog * colorDlg_;

	boost::scoped_ptr<Color> color_;
};

class ColorProperty : public Property
{
	Q_OBJECT
public:
	ColorProperty(const QString & name);
	void setValue(const Color * value);

	virtual QWidget * widget() const
	{return widget_;}

	Color value() const;
protected slots:
	void onChanged();
private:
	ColorPropertyWidget * widget_;
	QLineEdit * lineEdit_;	
	Color lastValue_;
};
///////////////////////////////////////////////////////////////////////////////
class BoolProperty : public Property
{
	Q_OBJECT
public:
	BoolProperty(const QString & name);
	void setValue(bool val);

	virtual QWidget * widget() const
	{return comboBox_;}

	void setEnabled(bool enabled);

	bool value() const;
protected slots:
	void onChanged(int index);
private:
	QComboBox * comboBox_;
	bool lastValue_;
	bool emitChanged_;
};

////////////////////////////////////////////////////////////////////////////////


class FileNameProperty : public Property
{
	Q_OBJECT
public:
	FileNameProperty(const QString & name, const QString & filter);
	void setValue(const QString & value);

	void setEnabled(bool enabled);

	virtual QWidget * widget() const
	{return widget_;}

	QString value() const;
protected slots:
	void onChanged();
private:
	QWidget * widget_;
	QString filter_;
	QLineEdit * fileEdit_;
	QToolButton * fileButton_;
	QString lastValue_;

	static QString lastDir_;
};
////////////////////////////////////////////////////////////////////////////////

class CheckBoxProperty : public Property
{
	Q_OBJECT
public:
	CheckBoxProperty(const QString & name);
	void setValue(bool var);

	virtual QWidget * widget() const
	{return checkBox_;}

	void setEnabled(bool enabled);
	bool value() const;
protected slots:
	void onChanged(bool val);
private:
	QCheckBox * checkBox_;
	bool lastValue_;
	bool emitChanged_;
};

class FontValidator : public QDoubleValidator
{
	Q_OBJECT
public:
	FontValidator(double bottom, double top, int decimals, QObject *parent = 0) : QDoubleValidator(bottom, top, decimals, parent){

	}

	//FontValidator(QObject * parent) : QDoubleValidator(parent) {}
	virtual void fixup(QString &input) const
	{
		input = QString::number(input.toFloat(),'f',1);
	}
};