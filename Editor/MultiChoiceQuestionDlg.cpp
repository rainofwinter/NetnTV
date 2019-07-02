#include "stdafx.h"
#include "MultiChoiceQuestionDlg.h"
#include "CustomObject.h"
#include "EditorMultiChoiceQuestion.h"
#include <QDomDocument>
#include "utils.h"
using namespace std;

MultiChoiceQuestionDlg::MultiChoiceQuestionDlg(QWidget * parent, CustomObject * obj) : QDialog(parent){
	ui.setupUi(this);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint); 

	ui.choicesList->setSelectionMode(QAbstractItemView::ExtendedSelection);

	connect(ui.okButton, SIGNAL(clicked()), this, SLOT(onOk()));
	connect(ui.addButton, SIGNAL(clicked()), this, SLOT(onAdd()));
	connect(ui.deleteButton, SIGNAL(clicked()), this, SLOT(onDelete()));
	connect(ui.fontBrowseButton, SIGNAL(clicked()), this, SLOT(onFontBrowse()));
	connect(ui.uncheckedImgBrowseButton, SIGNAL(clicked()), this, SLOT(onUncheckedBrowse()));
	connect(ui.checkedImgBrowseButton, SIGNAL(clicked()), this, SLOT(onCheckedBrowse()));
	connect(ui.checkAnsBrowseButton, SIGNAL(clicked()), this, SLOT(onCheckAnsBrowse()));
	connect(ui.choicesList, SIGNAL(itemSelectionChanged()), this, SLOT(onChoicesSelChanged()));
	connect(ui.correctImgBrowseButton, SIGNAL(clicked()), this, SLOT(onCorrectBrowse()));
	connect(ui.incorrectImgBrowseButton, SIGNAL(clicked()), this, SLOT(onIncorrectBrowse()));
	
	if (obj) getDataFromObj(obj);

	onChoicesSelChanged();
}


void MultiChoiceQuestionDlg::getDataFromObj(CustomObject * obj)
{	
	QDomDocument doc;
	doc.setContent(QString::fromUtf8(obj->data().c_str()));
	QDomNodeList list;
	QDomNamedNodeMap attrMap;

	list = doc.elementsByTagName("font");
	QString fontFile = list.at(0).toElement().text();
	
	list = doc.elementsByTagName("fontSize");
	int fontSize = list.at(0).toElement().text().toInt();

	list = doc.elementsByTagName("width");
	int width = list.at(0).toElement().text().toInt();

	list = doc.elementsByTagName("height");
	int height = list.at(0).toElement().text().toInt();
	
	list = doc.elementsByTagName("text");
	QString text = list.at(0).toElement().text();

	list = doc.elementsByTagName("uncheckedImage");
	QString uncheckedImage = list.at(0).toElement().text();

	list = doc.elementsByTagName("checkedImage");
	QString checkedImage = list.at(0).toElement().text();

	list = doc.elementsByTagName("checkAnswerImage");
	QString checkAnswerImage = list.at(0).toElement().text();

	list = doc.elementsByTagName("correctImage");
	QString correctImage = list.at(0).toElement().text();

	list = doc.elementsByTagName("incorrectImage");
	QString incorrectImage = list.at(0).toElement().text();

	list = doc.elementsByTagName("choice");
	vector<QString> choices;
	for (int i = 0; i < (int)list.length(); ++i)	
		choices.push_back(list.at(i).toElement().text());

	list = doc.elementsByTagName("correctChoice");
	int correctChoice = list.at(0).toElement().text().toInt();

	list = doc.elementsByTagName("columns");
	int columns = list.at(0).toElement().text().toInt();
	
	QString str;
	str.sprintf("%d", width); ui.widthEdit->setText(str);
	str.sprintf("%d", height); ui.heightEdit->setText(str);
	ui.fontEdit->setText(fontFile);
	str.sprintf("%d", fontSize); ui.fontSizeEdit->setText(str);
	ui.uncheckedImgEdit->setText(uncheckedImage);
	ui.checkedImgEdit->setText(checkedImage);
	ui.correctImgEdit->setText(correctImage);
	ui.incorrectImgEdit->setText(incorrectImage);
	ui.checkAnsEdit->setText(checkAnswerImage);
	ui.questionTextEdit->setPlainText(text);
	ui.columnsSpinBox->setValue(columns);

	ui.choicesList->clear();
	for (int i = 0; i < (int)choices.size(); ++i)	
		ui.choicesList->addItem(choices[i]);

	ui.correctChoiceComboBox->clear();
	for (int i = 0; i < (int)choices.size(); ++i)	
		ui.correctChoiceComboBox->addItem(choices[i]);	
	ui.correctChoiceComboBox->setCurrentIndex(correctChoice);
	
}


void MultiChoiceQuestionDlg::onFontBrowse()
{
	QString fileName = getOpenFileName(tr("Font file"), QString(), tr("Font files (*.ttf *.otf)")); 
	if (!fileName.isEmpty()) ui.fontEdit->setText(fileName);
}

void MultiChoiceQuestionDlg::onUncheckedBrowse()
{
	QString fileName = getOpenFileName(tr("Image file"), QString(), tr("Image files (*.jpg *.png)")); 
	if (!fileName.isEmpty()) ui.uncheckedImgEdit->setText(fileName);
}

void MultiChoiceQuestionDlg::onCheckedBrowse()
{
	QString fileName = getOpenFileName(tr("Image file"), QString(), tr("Image files (*.jpg *.png)")); 
	if (!fileName.isEmpty()) ui.checkedImgEdit->setText(fileName);
}

void MultiChoiceQuestionDlg::onCheckAnsBrowse()
{
	QString fileName = getOpenFileName(tr("Image file"), QString(), tr("Image files (*.jpg *.png)")); 
	if (!fileName.isEmpty()) ui.checkAnsEdit->setText(fileName);
}

void MultiChoiceQuestionDlg::onCorrectBrowse()
{
	QString fileName = getOpenFileName(tr("Image file"), QString(), tr("Image files (*.jpg *.png)")); 
	if (!fileName.isEmpty()) ui.correctImgEdit->setText(fileName);
}

void MultiChoiceQuestionDlg::onIncorrectBrowse()
{
	QString fileName = getOpenFileName(tr("Image file"), QString(), tr("Image files (*.jpg *.png)")); 
	if (!fileName.isEmpty()) ui.incorrectImgEdit->setText(fileName);
}

void MultiChoiceQuestionDlg::onChoicesSelChanged()
{
	ui.deleteButton->setEnabled(!ui.choicesList->selectedItems().empty());
}

void MultiChoiceQuestionDlg::onAdd()
{
	QString choice = QInputDialog::getText(this, tr("Add choice"), tr("choice"));
	if (choice.isEmpty()) return;
	ui.choicesList->addItem(choice);
	refreshComboBox();
}

void MultiChoiceQuestionDlg::refreshComboBox()
{
	ui.correctChoiceComboBox->clear();
	for (int i = 0; i < (int)ui.choicesList->count(); ++i)
		ui.correctChoiceComboBox->addItem(ui.choicesList->item(i)->text());
}

void MultiChoiceQuestionDlg::onDelete()
{
	QList<QListWidgetItem *> items = ui.choicesList->selectedItems();	
	for (int i = 0; i < (int)items.size(); ++i)
		ui.choicesList->takeItem(ui.choicesList->row(items[i]));

	refreshComboBox();
}

void MultiChoiceQuestionDlg::onOk()
{
	QString fontFile = ui.fontEdit->text();
	int fontSize = ui.fontSizeEdit->text().toInt();
	int width = ui.widthEdit->text().toInt();
	int height = ui.heightEdit->text().toInt();
	int columns = ui.columnsSpinBox->text().toInt();
	QString text = ui.questionTextEdit->toPlainText();
	QString unchecked = ui.uncheckedImgEdit->text();
	QString checked = ui.checkedImgEdit->text();
	QString checkAns = ui.checkAnsEdit->text();
	QString correctImg = ui.correctImgEdit->text();
	QString incorrectImg = ui.incorrectImgEdit->text();
	
	vector<QString> choices;
	for (int i = 0; i < (int)ui.choicesList->count(); ++i)
		choices.push_back(ui.choicesList->item(i)->text());

	int correctChoice = ui.correctChoiceComboBox->currentIndex();
	
	QString errMsg;
	if (fontSize == 0 || fontSize > 100) errMsg = "Invalid font size";
	if (width == 0 || width > 2048) errMsg = "Invalid width";
	if (height == 0 || height > 2048) errMsg = "Invalid height";
	if (checked.isEmpty()) errMsg = "Checked image is empty";
	if (checkAns.isEmpty()) errMsg = "Check Answer image is empty";
	if (correctImg.isEmpty()) errMsg = "Correct image is empty";
	if (incorrectImg.isEmpty()) errMsg = "Incorrect image is empty";
	if (correctChoice < 0) errMsg = "Invalid correct choice";
	if (choices.size() == 0) errMsg = "Not enough choices";

	if (!errMsg.isEmpty())
	{
		QMessageBox::information(0, tr("Error"), errMsg);
		return;
	}

	xml_ = QString("<data>") +
		"<width>" + QString::number(width) + "</width>" +
		"<height>" + QString::number(height) + "</height>" +
		"<text>" + text + "</text>" +
		"<font>" + fontFile + "</font>" +
		"<fontSize>" + QString::number(fontSize) + "</fontSize>" +
		"<uncheckedImage>" + unchecked + "</uncheckedImage>" +
		"<checkedImage>" + checked + "</checkedImage>" +
		"<checkAnswerImage>" + checkAns + "</checkAnswerImage>" +
		"<correctImage>" + correctImg + "</correctImage>" +
		"<incorrectImage>" + incorrectImg + "</incorrectImage>";

	BOOST_FOREACH(QString choice, choices)
		xml_ += "<choice>" + choice + "</choice>";

	xml_ += 
		"<correctChoice>" + QString::number(correctChoice) + "</correctChoice>" +
		"<columns>" + QString::number(columns) + "</columns>" +
		 "</data>";


	accept();
}