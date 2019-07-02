#include "stdafx.h"
#include "KeyFramePane.h"
#include "PaneButton.h"
#include "Property.h"
#include "EditorDocument.h"
#include "AnimationChannel.h"
#include "Interpolator.h"
#include "Command.h"

using namespace std;

KeyFramePropertyPage::KeyFramePropertyPage()
{
	interps_.push_back(&gLinearInterpolator);
	interps_.push_back(&gStepInterpolator);
	interps_.push_back(&gEaseInInterpolator);
	interps_.push_back(&gEaseOutInterpolator);

	setGroupBox(false);
	time_ = new EditProperty(tr("time"), &timeValidator_);
	addProperty(time_, PropertyPage::Vertical);
	addSpacing(2);
	interpolation_ = new ComboBoxProperty(tr("interpolation"));

	foreach(Interpolator * a, interps_)
		interpolation_->addItem(QString::fromLocal8Bit(a->name()));
	addProperty(interpolation_, PropertyPage::Vertical);
	addSpring();
	setMargins(6, 3, 6, 3);
}

KeyFramePropertyPage::~KeyFramePropertyPage()
{
}

void KeyFramePropertyPage::clear()
{
	interpolation_->setValue(-1);
	time_->setValue(QString());
}

void KeyFramePropertyPage::update()
{
	const vector<KeyFrameData> & keys = document_->selectedKeyFrames();

	if (keys.empty())
	{
		time_->setValue("");
		interpolation_->setValue(-1);
		return;
	}


	KeyFrameData key = keys.back();

	QString str;
	float time = key.time;
	str.sprintf("%.4f", time);
	time_->setValue(str);

	Interpolator * interp = key.ptr()->interpolator();	
	interpolation_->setValue(-1);
	int interpIndex = 0;
	foreach(Interpolator * a, interps_)
	{
		if (a == interp)
			interpolation_->setValue(interpIndex);
		++interpIndex;
	}
}

void KeyFramePropertyPage::onChanged(Property * property)
{
	PropertyPage::onChanged(property);
	float time = time_->value().toFloat();
	int interpIndex = interpolation_->value();
	if (interpIndex < 0) return;
	Interpolator * interpolator = interps_[interpIndex];

	const vector<KeyFrameData> & keys = document_->selectedKeyFrames();
	//must check if keys is empty
	//ex: user may have clicked on empty space in channels area, this will
	//cause a deselect (keys being empty) but will also cause any edit boxes
	//in this page to lose focus triggering an editFinished signal
	if (!keys.empty())
	{
		try {
			document_->doCommand(new SetKeyFramePropertiesCmd(
				document_, keys.back(), time, interpolator));
		} 
		catch (Exception & e) {
			QMessageBox::information(0, tr("Error"), tr(e.what()));
		}
		catch(exception & e) {
			QMessageBox::information(0, tr("Error"), tr(e.what()));
		}
	}
	update();
}


//////////////////////////////////////////////////////////////////////////////

KeyFramePane::KeyFramePane(MainWindow * mainWindow)
{
	document_ = 0;
	QBoxLayout * layout = new QBoxLayout(QBoxLayout::TopToBottom, this);

	layout->setMargin(0);
	layout->setSpacing(0);
	label_ = new QLabel(tr("KeyFrame"));
	label_->setMinimumWidth(50);	
	layout->addWidget(label_);

	scrollArea_ = new QScrollArea;
	scrollArea_->setWidgetResizable(true);
	keyFrameProperties_ = new KeyFramePropertyPage;
	scrollArea_->setWidget(keyFrameProperties_->widget());
	layout->addWidget(scrollArea_);
	//layout->addSpacing(::GetSystemMetrics(SM_CYHSCROLL));

	/*
	QBoxLayout * btnLayout = new QBoxLayout(QBoxLayout::LeftToRight);
	btnLayout->setMargin(0);
	btnLayout->setSpacing(0);
	btnLayout->addWidget(delButton_ = new PaneButton(tr("Delete")));
	//btnLayout->addWidget(editButton_ = new PaneButton(tr("Details...")));
	layout->addLayout(btnLayout);

	connect(delButton_, SIGNAL(clicked()), this, SLOT(onDelete()));
	*/
	changeButtonsState();
}

KeyFramePane::~KeyFramePane()
{
	delete keyFrameProperties_;
}

void KeyFramePane::setDocument(EditorDocument * document)
{
	if (document_) document_->disconnect(this);
	document_ = document;
	keyFrameProperties_->setDocument(document);	
	
	if (document_)
	{
		connect(document_, SIGNAL(keyFrameSelectionChanged()), 
			this, SLOT(keyFrameSelectionChanged()));
		
		connect(document_, SIGNAL(channelChanged()), 
			this, SLOT(keyFrameSelectionChanged()));

		keyFrameSelectionChanged();
	}
	else
	{
		label_->setText(QString());
		keyFrameProperties_->clear();
	}
}

void KeyFramePane::changeButtonsState()
{
	document_->selectedKeyFrames();
}

void KeyFramePane::keyFrameSelectionChanged()
{
	if (!document_) 
	{
		setEnabled(false);
		keyFrameProperties_->update();
		return;
	}

	const vector<KeyFrameData> & keys = document_->selectedKeyFrames();

	
	if (keys.empty())
	{
		setEnabled(false);
		label_->setText(tr("No keyframe selected"));		
	}
	else
	{
		setEnabled(true);
		AnimationChannel * channel = keys.back().channel;
		label_->setText(
			QString("<b>") + tr("Keyframe: ") + "</b> " + 
			QString::fromLocal8Bit(channel->name()));			
	}
	keyFrameProperties_->update();
}

/*
void KeyFramePane::onDelete()
{
	document_->doCommand(new DeleteKeyFramesCmd(
		document_, document_->selectedKeyFrames()));	
}
*/
