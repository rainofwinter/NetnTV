#include "stdafx.h"
#include "ListEditorWidget.h"
#include "Utils.h"

ListEditorWidget::ListEditorWidget(int lineEditX, int endMargin,
	QWidget * parent) 
:
QWidget(parent)
{
	lineEdit_ = new QLineEdit(this);
	QBoxLayout * layout = new QBoxLayout(QBoxLayout::LeftToRight, this);
	layout->setSpacing(0);
	layout->setMargin(0);
	setLayout(layout);
	layout->addSpacing(lineEditX);
	layout->addWidget(lineEdit_);
	layout->addSpacing(endMargin);	
	
	connect(lineEdit_, SIGNAL(returnPressed()), this, SIGNAL(editingFinished()));
	connect(lineEdit_, SIGNAL(editingFinished()), this, SIGNAL(editingFinished()));
	textInit_ = false;
}

void ListEditorWidget::setText(const std::string & text)
{
	if (textInit_) return;
	lineEdit_->setText(stdStringToQString(text));
	textInit_ = true;
}

void ListEditorWidget::focusInEvent(QFocusEvent * event)
{
	lineEdit_->setFocus();
	lineEdit_->selectAll();
}


void ListEditorWidget::Apply()
{
	emit editingFinished();
}

void ListEditorWidget::Cancel()
{
	emit editingCancelled();
}