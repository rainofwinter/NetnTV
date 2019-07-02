#include "stdafx.h"
#include "TextEditTool.h"
#include "GLWidget.h"
#include "Attrib.h"
#include "Camera.h"
#include "EditorDocument.h"
#include "EditorObject.h"
#include "Command.h"
#include "Scene.h"
#include "Text_2.h"
#include "MainWindow.h"
#include "Utils.h"
#include "Reader.h"
#include "Writer.h"
#include "EditorScene.h"
#include "Command.h"
#include "PropertyPage.h"
#include "TextEditObject.h"
using namespace std;


///////////////////////////////////////////////////////////////////////////////

TextOperation::TextOperation()
{
	startIndex_ = endIndex_ = 0;
	origStartIndex_ = origEndIndex_ = 0;
}

TextOperation::TextOperation(TextEditTool * textEditTool, const std::wstring & text, 
	const std::vector<TextProperties_2> & properties, 
	unsigned int startIndex, unsigned int endIndex)
{
	Text_2 * textObj = textEditTool->textObj();
	startIndex_ = startIndex;
	endIndex_ = endIndex;
	text_ = text;
	properties_ = properties;

	origStartIndex_ = startIndex_;
	origEndIndex_ = startIndex_ + text_.size();
	origText_ = textObj->textPartString(startIndex_, endIndex_);
	textObj->getProperties(origProperties_, startIndex_, endIndex_, true);

	origlinesStyle_.clear();
	linesStyle_.clear();
}

TextOperation::TextOperation(TextEditTool * textEditTool, const std::wstring & text, 
	const std::vector<TextProperties_2> & properties, 
	unsigned int startIndex, unsigned int endIndex, const std::vector<LineStyle> & origlines, const std::vector<LineStyle> & newlines)
{
	Text_2 * textObj = textEditTool->textObj();
	startIndex_ = startIndex;
	endIndex_ = endIndex;
	text_ = text;
	properties_ = properties;

	origStartIndex_ = startIndex_;
	origEndIndex_ = startIndex_ + text_.size();
	origText_ = textObj->textPartString(startIndex_, endIndex_);
	textObj->getProperties(origProperties_, startIndex_, endIndex_, true);

	origlinesStyle_ = origlines;
	linesStyle_ = newlines;
}

CursorPosData TextOperation::doOp(TextEditTool * textEditTool)
{
	Text_2 * text = textEditTool->textObj();
	text->removeText(startIndex_, endIndex_);
	
	text->insertText(text_, properties_, startIndex_);
	
	if(linesStyle_.size() > 0)
		text->setLinesStyle(linesStyle_);

	return CursorPosData(startIndex_ + text_.size());
}

CursorPosData TextOperation::undoOp(TextEditTool * textEditTool)
{
	Text_2 * text = textEditTool->textObj();
	text->removeText(origStartIndex_, origEndIndex_);
	text->insertText(origText_, origProperties_, origStartIndex_);

	if(origlinesStyle_.size() > 0)
		text->setLinesStyle(origlinesStyle_);

	return CursorPosData(endIndex_);
}

///////////////////////////////////////////////////////////////////////////////

PropertiesOperation::PropertiesOperation(TextEditTool * textEditTool, 
	const std::vector<TextProperties_2> & properties,
	unsigned int startIndex, unsigned int endIndex)
{
	properties_ = properties;
	startIndex_ = startIndex;
	endIndex_ = endIndex;

	textEditTool->textObj()->getProperties(origProperties_, startIndex_, endIndex_, true);
	cursorPosData_ = textEditTool->getCursorPosData();
}

CursorPosData PropertiesOperation::doOp(TextEditTool * textEditTool)
{
	Text_2 * text = textEditTool->textObj();
	text->applyProperties(properties_, startIndex_, endIndex_);
	return cursorPosData_;
}

CursorPosData PropertiesOperation::undoOp(TextEditTool * textEditTool)
{
	Text_2 * text = textEditTool->textObj();
	text->applyProperties(origProperties_, startIndex_, endIndex_);
	return cursorPosData_;
}

LineStyleOperation::LineStyleOperation(TextEditTool * textEditTool, 
	const std::vector<LineStyle> & origproperties, const std::vector<LineStyle> & newproperties)
{
	properties_ = newproperties;
	origProperties_ = origproperties;

	//textEditTool->textObj()->getProperties(origProperties_, startIndex_, endIndex_, true);
	cursorPosData_ = textEditTool->getCursorPosData();
}

CursorPosData LineStyleOperation::doOp(TextEditTool * textEditTool)
{
	Text_2 * text = textEditTool->textObj();
	text->setLinesStyle(properties_);
	return cursorPosData_;
}

CursorPosData LineStyleOperation::undoOp(TextEditTool * textEditTool)
{
	Text_2 * text = textEditTool->textObj();
	text->setLinesStyle(origProperties_);
	return cursorPosData_;
}

///////////////////////////////////////////////////////////////////////////////

GlobalTextOperation::GlobalTextOperation(TextEditTool * textEditTool, 
	Command * globalTextCommand)
{
	command_.reset(globalTextCommand);
	cursorPosData_ = textEditTool->getCursorPosData();
	
}

CursorPosData GlobalTextOperation::doOp(TextEditTool * textEditTool)
{
	command_->doCommand();	
	return cursorPosData_;
}

CursorPosData GlobalTextOperation::undoOp(TextEditTool * textEditTool)
{
	command_->undoCommand();	
	return cursorPosData_;
}


///////////////////////////////////////////////////////////////////////////////

void TextState::getFrom(Text_2 * t)
{
	text = t->textString();
	props = t->properties();
	boundaryWidth = t->boundaryWidth();
	boundaryHeight = t->boundaryHeight();
	letterSpacing = t->letterSpacing();
	lineSpacingMode = t->lineSpacingMode();
	lineSpacing = t->lineSpacing();
	canSelectPart = t->canSelectPart();
	lineStyles = t->getLinesStyle();
}

void TextState::applyTo(Text_2 * t)
{
	t->setTextString(text);
	t->setProperties(props);
	t->setBoundaryWidth(boundaryWidth);
	t->setBoundaryHeight(boundaryHeight);
	t->setLetterSpacing(letterSpacing);
	t->setLineSpacingMode(lineSpacingMode);
	t->setLineSpacing(lineSpacing);
	t->setCanSelectPart(canSelectPart);
	t->setLinesStyle(lineStyles);
}

bool TextState::operator != (const TextState & rhs) const
{
	return text != rhs.text ||
		props != rhs.props ||
		boundaryWidth != rhs.boundaryWidth ||
		boundaryHeight != rhs.boundaryHeight ||
		letterSpacing != rhs.letterSpacing ||
		lineSpacingMode != rhs.lineSpacingMode ||
		lineSpacing != rhs.lineSpacing ||
		canSelectPart != rhs.canSelectPart||
		lineStyles != rhs.lineStyles;
}

void TextState::clear()
{
	text.clear();
	props.clear();
	lineStyles.clear();
}
///////////////////////////////////////////////////////////////////////////////

TextEditWindow::TextEditWindow(MainWindow * mainWindow) : QWidget(mainWindow, Qt::Window)
{
	ui.setupUi(this);		
	connect(ui.doneButton, SIGNAL(clicked()), this, SLOT(onDone()));
}

void TextEditWindow::onDone()
{
	close();
}

void TextEditWindow::closeEvent(QCloseEvent * evt)
{
	QWidget::closeEvent(evt);
	emit closed();	
}

void TextEditWindow::setText(const QString & text)
{
	//ui.componentText->setPlainText(text);
}

void TextEditWindow::focusInEvent(QFocusEvent * event)
{
	QWidget::focusInEvent(event);
}

///////////////////////////////////////////////////////////////////////////////
TextEditControl::TextEditControl(TextEditTool * textEditTool, QWidget * parent)
	: QTextEdit(parent)
{
	setAcceptRichText(false);
	textEditTool_ = textEditTool;
	isImeEditing_ = false;
	shiftPressed_ = false;
	ctrlPressed_ = false;

// 	QSizePolicy localSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
 	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff); 

	setFrameStyle(QFrame::NoFrame);
	//viewport()->setAutoFillBackground(false);
	//setStyleSheet("background-color: transparent;border:0px solid black;");
	//setStyleSheet("background-color: transparent;border:1px solid black;");
}

void TextEditControl::curLineNumber(int &nStart, int &nEnd){
	QTextCursor cursor = textCursor();
	QString qText = getText();

	if(!cursor.hasSelection()){
		qText = qText.mid(0, cursor.position());
		nStart = nEnd = qText.count("\n");
	}else{
		QString qTextBeforeSelection = qText.mid(0, cursor.selectionStart());
		qText = qText.mid(0, cursor.selectionEnd());

		nStart = qTextBeforeSelection.count("\n");
		nEnd = qText.count("\n");
	}
}

void TextEditControl::keyPressEvent(QKeyEvent * event)
{		
	int key = event->key();

	if (key == Qt::Key_Shift) 
		shiftPressed_ = true;

	if (key == Qt::Key_Control) 
		ctrlPressed_ = true;

	bool bExcuteKeyEvent = false;
	bool bUpdatePropertyPage = false;
	
	if (key == Qt::Key_Home){ 
		textEditTool_->doHome();
		bUpdatePropertyPage = true;
	}else if (key == Qt::Key_End){ 
		textEditTool_->doEnd();
		bUpdatePropertyPage = true;
	}else if (key == Qt::Key_Left){ 
		textEditTool_->doLeft();
		bUpdatePropertyPage = true;
	}else if (key == Qt::Key_Right){ 
		textEditTool_->doRight();
		bUpdatePropertyPage = true;
	}else if (key == Qt::Key_Up){ 
		textEditTool_->doUp();
		bUpdatePropertyPage = true;
	}else if (key == Qt::Key_Down) {
		textEditTool_->doDown();
		bUpdatePropertyPage = true;
	}else if (key == Qt::Key_PageUp)
	{
		//do nothing
	}
	else if (key == Qt::Key_PageDown)
	{
		//do nothing
	}
	else if (key == Qt::Key_Z && ctrlPressed_)
	{
		undo();
		bUpdatePropertyPage = true;
	}
	else if (key == Qt::Key_Y && ctrlPressed_)
	{
		redo();
		bUpdatePropertyPage = true;
	}
	else if (key == Qt::Key_Delete)
	{
		textEditTool_->doDel();
		bUpdatePropertyPage = true;
	}
	else if (key == Qt::Key_A && ctrlPressed_)
	{
		textEditTool_->selectAll();
		bUpdatePropertyPage = true;
	}
	else if (key == Qt::Key_X && ctrlPressed_)
	{
		textEditTool_->cut();
		bUpdatePropertyPage = true;
	}
	else if (key == Qt::Key_C && ctrlPressed_)
	{
		textEditTool_->copy();
	}
	else if (key == Qt::Key_V && ctrlPressed_)
	{
		textEditTool_->paste();
		bUpdatePropertyPage = true;
	}else
	{

		if (key == Qt::Key_Return || (key == Qt::Key_Enter && !ctrlPressed_))
		{
			textEditTool_->doEnter();

			bExcuteKeyEvent = true;

		}else if(key != Qt::Key_Shift && key != Qt::Key_Control && !ctrlPressed_){
			bExcuteKeyEvent = textEditTool_->doInputKeyPress(key);
			bUpdatePropertyPage = true;
		}
	}

	if( bExcuteKeyEvent)
		QTextEdit::keyPressEvent(event);
	
	if(bUpdatePropertyPage){
		PropertyPage *pPropertyPage = textEditTool_->doc()->getPropertyPage(textEditTool_->textObj());
		pPropertyPage->update();
	}

	//textEditTool_->keyPressedInControl();
}

void TextEditControl::keyReleaseEvent(QKeyEvent * event)
{
	if (event->key() == Qt::Key_Shift)
	{
		shiftPressed_ = false;
	}
	else if (event->key() == Qt::Key_Control)
	{
		ctrlPressed_ = false;
	}
}


void TextEditControl::inputMethodEvent(QInputMethodEvent * event)
{
	textEditTool_->doInputKeyPress(Qt::Key_Any);

	QTextEdit::inputMethodEvent(event);
	imePreeditText_ = event->preeditString();

	emit imeTextChanged();
}

QString TextEditControl::getText()
{/*
	QString str = this->toPlainText();
	
	QTextCursor textCursor = this->textCursor();
	int index = textCursor.position();

	if (!imePreeditText_.isEmpty())		
		str.insert(index, imePreeditText_);

	return str;
	*/
	return this->toPlainText();
}

void TextEditControl::undo()
{
	//QTextEdit::undo();
	textEditTool_->undoOp();
}
void TextEditControl::redo()
{
	//QTextEdit::redo();
	textEditTool_->redoOp();
}


///////////////////////////////////////////////////////////////////////////////

TextEditTool::TextEditTool(GLWidget * gl) : Tool(gl)
{	
	textObj_ = NULL;
	disableWindowClosedHandling_ = false;
	inspectWnd_ = new TextEditWindow(gl->mainWindow());
	connect(inspectWnd_, SIGNAL(closed()), this, SLOT(onWindowClosed()));
	connect(document_, SIGNAL(objectSelectionChanged()), this, SLOT(onLostObjSelection()));
	connect(document_, SIGNAL(objectListChanged()), this, SLOT(onLostObjSelection()));

	qTextEdit_ = new TextEditControl(this, gl->mainWindow());
	
	connect(qTextEdit_, SIGNAL(textChanged()), this, SLOT(textChangedInControl()));
	connect(qTextEdit_, SIGNAL(imeTextChanged()), this, SLOT(imeTextChangedInControl()));
	//connect(qTextEdit_, SIGNAL(cursorPositionChanged()), this, SLOT(cursorChangedInControl()));
	
	keepCursorVisibleTill_ = 0;
	cursorHeight_ = 0;
	curCharWidth_ = 0;

	isDragging_ = false;

	copiedProps_.clear();
	respondToTextChanged_ = true;

	cursorPos_.x = -1;
	cursorPos_.y = -1;

	toolName_ = "TextEditTool";
}

TextEditTool::~TextEditTool()
{
}

void TextEditTool::insertImg(std::string &image){
	QTextCursor cursor = qTextEdit_->textCursor();
	std::string fileName = convertToRelativePath(image);
	int trWidth, trHeight;
	ImageObjectProperties imgProp;
	QTextDocumentFragment fragment = QTextDocumentFragment::fromHtml("<img src='" + QString::fromStdString(fileName) +  "'>");	

	std::vector<TextProperties_2> props;
	textObj_->getProperties(props, cursor.position(), cursor.position(), true);

	objectProps_.reset(new TextProperties_2(props.front()));

	imgProp.fileName = fileName;
	imgProp.bMakeTexture = false;
	imgProp.property_type = IMAGE_OBJECT;


	textObj_->addImageTexture(fileName);
	(*objectProps_).SetObjectProperties((ObjectProperties *)&imgProp);

	cursor.insertFragment(fragment);
	
}

void TextEditTool::getPropsAtPos(std::vector<TextProperties_2> & props, int index) const
{
	unsigned getIndex = index;
	if (getIndex > 0) getIndex--;
	textObj_->getProperties(props, getIndex, getIndex, true);
}

void TextEditTool::imeTextChangedInControl()
{
	if (!respondToTextChanged_) return;
	QString imePreeditText = qTextEdit_->imePreeditText();
		
	textObj_->setTextString(oldText_);
	textObj_->setProperties(oldProps_);	

	vector<TextProperties_2> props;
	if (nextCharProps_)
		props.push_back(*nextCharProps_);
	else
		getPropsAtPos(props, charStartIndex_);

	if(props.size() > 0){
		props.front().ResetObjectProperties();
	}

	TextOperation op(this, imePreeditText.toStdWString(), props, charStartIndex_, charStartIndex_, prevLineStyle_, newLineStyle_);
	op.doOp(this);	

	imeAltered_ = true;
	textObj_->init(document_->renderer());

	textObj_->posAtIndex(charStartIndex_, &cursorPos_, &cursorHeight_, &curCharWidth_);

	nextCharProps_.reset(new TextProperties_2(props.front()));
	prevLineStyle_.clear();
	newLineStyle_.clear();
}

void TextEditTool::textChangedInControl()
{
	if (!respondToTextChanged_) return;

	if (imeAltered_)
	{
		textObj_->setTextString(oldText_);
		textObj_->setProperties(oldProps_);	
		imeAltered_ = false;
	}

	wstring newText = qTextEdit_->getText().toStdWString();	
	int newTextSize = (int)newText.size();
	int origTextSize = (int)oldText_.size();

	int numSelChars = charEndIndex_ - charStartIndex_;
	int numNewChars = newTextSize - origTextSize + numSelChars;

	wstring newlyAddedText;
	int startIndex = charStartIndex_;
	int endIndex = charEndIndex_;

	if (numNewChars > 0)
	{
		newlyAddedText = newText.substr(charStartIndex_, numNewChars);
	}
	else if (numNewChars < 0)
	{
		startIndex += numNewChars;
	}
			
	if (startIndex != endIndex || !newlyAddedText.empty())
	{
		vector<TextProperties_2> props;

		if(newlyAddedText.length() == 1 && newlyAddedText.at(0) == IMAGE_OBJECT_STRING){
			props.push_back(*objectProps_);
		}else{
			if (nextCharProps_)
				props.push_back(*nextCharProps_);
			else
				getPropsAtPos(props, startIndex);

			vector<TextProperties_2> tempProps;

			BOOST_FOREACH(TextProperties_2 prop, props){

				if(prop.isObjectProperty()){
					TextProperties_2 temp;
					temp.font = prop.font;
					temp.color = prop.color;
					temp.lineSpacing = prop.lineSpacing;
					temp.lineSpacingMode = prop.lineSpacingMode;
					temp.link = prop.link;
					temp.underline_ = prop.underline_;
					tempProps.push_back(temp);
				}else{
					tempProps.push_back(prop);
				}
			}

			props.clear();

			BOOST_FOREACH(TextProperties_2 prop, tempProps){

				props.push_back(prop);
			}
		}			
		
		TextOperation * op = new TextOperation(this, newlyAddedText, props, startIndex, endIndex, prevLineStyle_, newLineStyle_);
		
		
		doOp(op, true);
		nextCharProps_.reset();
		objectProps_.reset();
		prevLineStyle_.clear();
		newLineStyle_.clear();
	}
	
	if(startIndex != endIndex){
		PropertyPage *pPropertyPage = document_->getPropertyPage(textObj_);
		pPropertyPage->update();
	}
}



void TextEditTool::cursorChangedInControl(){

	
}

void TextEditTool::endTool()
{
	textObj_ = NULL;
	
	glWidget_->setToolMode(glWidget_->prevMode());
}

void TextEditTool::onLostObjSelection()
{
	if (!active_)
	{
		undoOps_.clear();
		redoOps_.clear();
		oldText_.clear();
		oldProps_.clear();
		firstTextState_.clear();

		nextCharProps_.reset();
		
		return;
	}

	if (textObj_ != document_->selectedObject())
	{
		endTool();
	}
}

void TextEditTool::onWindowClosed()
{
	if (disableWindowClosedHandling_) return;
	endTool();
}

void TextEditTool::setTextObj(Text_2 * textObj)
{
	textObj_ = textObj;
}

void TextEditTool::init()
{
}

void TextEditTool::uninit()
{
}

void TextEditTool::refresh()
{
	charStartIndex_ = 0;
	charEndIndex_ = 0;
	active_ = false;
}

void TextEditTool::mouseDoublePressEvent(QMouseEvent * event){
	
	if (!textObj_) return;
	
	Vector2 mousePosDevice = glWidget_->windowToDeviceCoords(
		Vector2(event->posF().x(), event->posF().y()));

	Ray ray = glWidget_->renderer()->unproject(mousePosDevice);
	Vector3 out;
	bool clickedGizmo = textObj_->intersect(&out, ray) != NULL;

	if (!clickedGizmo)
	{
		glWidget_->setToolMode(glWidget_->prevMode());
		return;
	}

	Matrix invTrans = 
		(textObj_->parentTransform() * (textObj_->visualAttrib()->transformMatrix())).inverse();

	Vector3 invClickPt = invTrans * out;
	Vector2 localPos(invClickPt.x, invClickPt.y);

	int propertyIndex;
	int charIndex = textObj_->indexAtPos(localPos, &propertyIndex);

	std::vector<TextProperties_2> targetProperties;
	getPropsAtPos(targetProperties, charIndex + 1);

}

void TextEditTool::mousePressEvent(QMouseEvent * event)
{		
	if (!textObj_) return;
	Vector2 mousePosDevice = glWidget_->windowToDeviceCoords(
		Vector2(event->posF().x(), event->posF().y()));


	Ray ray = glWidget_->renderer()->unproject(mousePosDevice);
	Vector3 out;
	bool clickedGizmo = textObj_->intersect(&out, ray) != NULL;

	if (!clickedGizmo)
	{
		glWidget_->setToolMode(glWidget_->prevMode());
		return;
	}

	Matrix invTrans = 
		(textObj_->parentTransform() * (textObj_->visualAttrib()->transformMatrix())).inverse();

	Vector3 invClickPt = invTrans * out;
	Vector2 localPos(invClickPt.x, invClickPt.y);

	int propertyIndex;
	int charIndex = textObj_->indexAtPos(localPos, &propertyIndex);


	qTextEdit_->setFocus();

	if (charIndex < 0) charIndex = 0;
	doSetCursorPos(charIndex);
	textObj_->posAtIndex(charIndex, &cursorPos_, &cursorHeight_, &curCharWidth_);

	isDragging_ = true;

}

int TextEditTool::charCurIndex() const
{
	if (cursorPos_.x >= 0 && cursorPos_.y >= 0)
	{
		int propertyIndex;
		int charIndex = textObj_->indexAtPos(cursorPos_, &propertyIndex);
		return charIndex;
	}
	else
		return 0;
}




void TextEditTool::mouseReleaseEvent(QMouseEvent * event)
{	
	if(!textObj_)
	{
		endTool();
		return;
	}
	isDragging_ = false;
	Vector2 mousePosDevice = glWidget_->windowToDeviceCoords(
		Vector2(event->posF().x(), event->posF().y()));

	Ray ray = glWidget_->renderer()->unproject(mousePosDevice);
	Vector3 out;
	textObj_->intersect(&out, ray);

	Matrix invTrans = 
		(textObj_->parentTransform() * (textObj_->visualAttrib()->transformMatrix())).inverse();

	Vector3 invClickPt = invTrans * out;
	Vector2 localPos(invClickPt.x, invClickPt.y);

	int propertyIndex;
	int charIndex = textObj_->indexAtPos(localPos, &propertyIndex);

}

void TextEditTool::mouseMoveEvent(QMouseEvent * event)
{
	if(!textObj_)
	{
		endTool();
		return;
	}
	if (isDragging_ && qTextEdit_->hasFocus())
	{
		Vector2 mousePosDevice = glWidget_->windowToDeviceCoords(
			Vector2(event->posF().x(), event->posF().y()));

		Ray ray = glWidget_->renderer()->unproject(mousePosDevice);
		Vector3 out;
		textObj_->intersect(&out, ray);

		Matrix invTrans = 
			(textObj_->parentTransform() * (textObj_->visualAttrib()->transformMatrix())).inverse();

		Vector3 invClickPt = invTrans * out;
		Vector2 localPos(invClickPt.x, invClickPt.y);

		int propertyIndex;
		int charIndex = textObj_->indexAtPos(localPos, &propertyIndex);


		if (charIndex >= 0)
		{
			doSetCursorPos(charIndex);
			QTextCursor cursor = qTextEdit_->textCursor();
			textObj_->posAtIndex(cursor.position(), &cursorPos_, &cursorHeight_, &curCharWidth_);
		}

	}

	Vector2 mousePosDevice = glWidget_->windowToDeviceCoords(
		Vector2(event->posF().x(), event->posF().y()));

	Ray ray = glWidget_->renderer()->unproject(mousePosDevice);
	Vector3 intPt;
	SceneObject * intersect = textObj_->intersect(&intPt, ray);
	if (intersect)
		glWidget_->setCursor(Qt::IBeamCursor);
	else
		glWidget_->setCursor(Qt::ArrowCursor);
}

bool TextEditTool::keyPressEvent(QKeyEvent * event)
{
	qTextEdit_->setFocus();
	qTextEdit_->keyPressEvent(event);
	return true;
}
void TextEditTool::syncQTextEditWithProp(){

	respondToTextChanged_ = false;

	qTextEdit_->setPlainText(QString::fromStdWString(textObj_->textString()));		

	QTextCursor targetcursor = qTextEdit_->textCursor();
	std::vector<TextProperties_2> textProperties= textObj_->properties();

	BOOST_FOREACH(TextProperties_2 props, textProperties){
		QTextCharFormat format;
		format.setFontPointSize(props.font.pointSize);
		targetcursor.setPosition(props.index, QTextCursor::MoveAnchor);

		if(props.isObjectProperty()){
			const ObjectProperties *pObjectProp = props.GetObjectProperty();

			if(pObjectProp && pObjectProp->property_type == IMAGE_OBJECT){
				ImageObjectProperties *pTargetProp = (ImageObjectProperties *)pObjectProp;
				QTextDocumentFragment fragment = QTextDocumentFragment::fromHtml("<img src='" + QString::fromStdString(pTargetProp->fileName) +  "'>");	

				targetcursor.deleteChar();
				targetcursor.insertFragment(fragment);

			}
		}

		targetcursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
		targetcursor.setCharFormat(format);
	}

	respondToTextChanged_ = true;
}

void TextEditTool::setActive(bool val)
{
	Tool::setActive(val);

	if (val)	
	{
		//inspectWnd_->show();
		qTextEdit_->resize(320, 200);
		qTextEdit_->move(-600, 0);

		qTextEdit_->show();
		qTextEdit_->setFocus();

		respondToTextChanged_ = true;

		lastCursorPosData_.anchor = 0;
		lastCursorPosData_.pos = 0;

		
		if (textObj_)
		{
			syncQTextEditWithProp();

			QTextCursor cursor = qTextEdit_->textCursor();
			charStartIndex_ = charEndIndex_ = cursor.position();
			textObj_->posAtIndex(cursor.position(), &cursorPos_, &cursorHeight_, &curCharWidth_);
			oldText_ = textObj_->textString();
			oldProps_ = textObj_->properties();
			firstTextState_.getFrom(textObj_);

			PropertyPage *pPropertyPage = document_->getPropertyPage(textObj_);

			pPropertyPage->update();
		}
		else
		{
			qTextEdit_->setPlainText("");
			oldText_.clear();
			oldProps_.clear();
			firstTextState_.clear();
		}

	}
	else
	{
		disableWindowClosedHandling_ = true;
		inspectWnd_->close();
		qTextEdit_->hide();
		glWidget_->mainWindow()->setFocus();

		if (textObj_) doCommitTextCommand();
		disableWindowClosedHandling_ = false;

		undoOps_.clear();
		redoOps_.clear();
		oldText_.clear();
		oldProps_.clear();
		firstTextState_.clear();

		nextCharProps_.reset();

		if(textObj_){
			PropertyPage *pPropertyPage = document_->getPropertyPage(textObj_);
			pPropertyPage->update();
		}

		
	}

	if (document_->selectedObject()) emit cursorPosChanged();
}

void TextEditTool::doCommitTextCommand()
{
	std::wstring newText = textObj_->textString();
	vector<TextProperties_2> newProps = textObj_->properties();
	TextState newTextState;
	newTextState.getFrom(textObj_);
	if (newTextState != firstTextState_)
	{
		firstTextState_.applyTo(textObj_);		
		document_->doCommand(new ChangeTextCmd(textObj_, newTextState));
		firstTextState_ = newTextState;
	}
}

void TextEditTool::draw()
{
	if(!textObj_)
	{
		endTool();
		return;
	}

	GfxRenderer * gl = glWidget_->renderer();
	float time = Global::currentTime();

	glEnable(GL_COLOR_LOGIC_OP);
	glLogicOp(GL_XOR);
	gl->useColorProgram();
	gl->setColorProgramColor(0.5f, 0.5f, 1.0f, 1.0f);

	gl->pushMatrix();

	Matrix totalTransform = textObj_->parentTransform() * (textObj_->visualAttrib()->transformMatrix());
	gl->loadMatrix(totalTransform);

	//draw cursor
	if (qTextEdit_->hasFocus())
	{
		if (keepCursorVisibleTill_ > time || (int)time % 2)
		{	

			Vector3 vertices[2];
			vertices[0] = Vector3(cursorPos_.x, cursorPos_.y, 0.0f);
			vertices[1] = Vector3(cursorPos_.x, cursorPos_.y + cursorHeight_, 0.0f);

			gl->enableVertexAttribArrayPosition();
			gl->bindArrayBuffer(0);
			gl->vertexAttribPositionPointer(0, (char *)vertices);	
			gl->applyCurrentShaderMatrix();
			glDrawArrays(GL_LINES, 0, 2);
		}
	}
	

	//draw selection rects
	QTextCursor cursor = qTextEdit_->textCursor();
	int selStart = cursor.selectionStart();
	int selEnd = cursor.selectionEnd();

	if (selStart != selEnd)
	{
		float lineY0, lineY1;
		int lineIndex0, lineStartIndex0, lineEndIndex0;
		textObj_->lineDataForChar(selStart, &lineY0, &lineIndex0, &lineStartIndex0, &lineEndIndex0);
		int lineIndex1, lineStartIndex1, lineEndIndex1;
		textObj_->lineDataForChar(selEnd, &lineY1, &lineIndex1, &lineStartIndex1, &lineEndIndex1);

		Vector2 posTop0; float posHeight0, posWidth0;
		textObj_->posAtIndex(selStart, &posTop0, &posHeight0, &posWidth0);
		Vector2 posTop1; float posHeight1, posWidth1;
		textObj_->posAtIndex(selEnd, &posTop1, &posHeight1, &posWidth1);

		if (lineIndex0 == lineIndex1)
		{
			gl->drawRect(posTop0.x, posTop0.y, posTop1.x - posTop0.x, posHeight0);
		}
		else
		{
			int lineStartIndexP, lineEndIndexP;
			float lineYP, lineWidthP, lineHeightP;
			
			float linelen = 0.0;
			
			
			float drawX = 0.0;
			float drawY = 0.0;
			float width = 0.0;				
			float height = 0.0;

			for (int i = lineIndex0 ; i <= lineIndex1; ++i){
				float moveX = 0.0;
				const WordWrapPoint_2 line = textObj_->getLineByIndex(i);
				const LineStyle lineStyle = textObj_->getLineStyleByCharIndex(line.wrapAtIndex);

				textObj_->lineDataForLine(i, &lineYP, &lineWidthP, &lineHeightP, &lineStartIndexP, &lineEndIndexP);

				if(textObj_->textDirection() == Text_2::DRight)
					drawX = line.nStartx;
				else
					drawX = textObj_->getDisplayTextWidth() - line.nStartx;

				drawY = lineYP;
				height = lineHeightP;

				if(textObj_->textDirection() == Text_2::DRight){
					if(lineStyle.textAlign_ == LineStyle::Center){
						moveX = (float)(textObj_->getDisplayTextWidth() / 2.0 - (float)lineWidthP / 2.0);
					}else if(lineStyle.textAlign_ == LineStyle::Right){
						moveX =  (float)(textObj_->getDisplayTextWidth() - (float)lineWidthP + ENTER_SIZE);
					}
				}else{
					if(lineStyle.textAlign_ == LineStyle::Center){
						moveX = - ((float)(textObj_->getDisplayTextWidth() / 2.0 - (float)lineWidthP / 2.0));
					}else if(lineStyle.textAlign_ == LineStyle::Right){
						moveX = - ENTER_SIZE;
				
					}
				}
				
				drawX += moveX;
	
				float justifySpacing = 0; 

				if(lineStyle.textAlign_ == LineStyle::Justify_Left)
					justifySpacing = ENTER_SIZE;

				if(i == lineIndex0){
					drawX = posTop0.x;
				}
				
				if(i == lineIndex0){
					if(textObj_->textDirection() == Text_2::DRight)
					{
						width = moveX + lineWidthP - posTop0.x + justifySpacing;
						
						if(lineStyle.textAlign_ == LineStyle::Justify_Left)
						{
							const LineStyle nextlineStyle = textObj_->getLineStyleByCharIndex(lineEndIndexP+1);

							if(nextlineStyle.index_ == lineStyle.index_)
								width += textObj_->getDisplayTextWidth() - lineWidthP;
						}
					}
					else
						width =  - posTop0.x;

					
				}
				else if(i == lineIndex1){
					width = posTop1.x - drawX;
					
				}
				else
				{
					if(textObj_->textDirection() == Text_2::DRight)
					{
						width = (float)lineWidthP - line.nStartx + justifySpacing;
						if(lineStyle.textAlign_ == LineStyle::Justify_Left)
						{
							const LineStyle nextlineStyle = textObj_->getLineStyleByCharIndex(lineEndIndexP+1);

							if(nextlineStyle.index_ == lineStyle.index_)
								width += textObj_->getDisplayTextWidth() - lineWidthP;
						}
					}
					else
					{
						drawX -= ENTER_SIZE;
						width = -((float)lineWidthP - line.nStartx - ENTER_SIZE);
					}
					
				}

				

				gl->drawRect(drawX, drawY, width, height);

			}
		}
	}

	glDisable(GL_COLOR_LOGIC_OP);

	gl->popMatrix();

}

CursorPosData TextEditTool::getCursorPosData() const
{
	QTextCursor cursor = qTextEdit_->textCursor();
	return CursorPosData(cursor.anchor(), cursor.position());
}

bool TextEditTool::isBeingUsed() const
{
	return isActive();
}

void TextEditTool::doHome()
{
	QTextCursor cursor = qTextEdit_->textCursor();
	float curLineY;
	int lineIndex, lineStartIndex, lineEndIndex;

	textObj_->lineDataForChar(cursor.position(), &curLineY, &lineIndex, &lineStartIndex, &lineEndIndex);
	doSetCursorPos(lineStartIndex);
}

void TextEditTool::doEnd()
{
	QTextCursor cursor = qTextEdit_->textCursor();
	float curLineY;
	int lineIndex, lineStartIndex, lineEndIndex;
	textObj_->lineDataForChar(cursor.position(), &curLineY, &lineIndex, &lineStartIndex, &lineEndIndex);
	int newIndex = lineEndIndex - 1;
	if (lineIndex == textObj_->numLines() - 1)
		newIndex = lineEndIndex;
	doSetCursorPos(newIndex);
}

void TextEditTool::doLeft()
{
	QTextCursor cursor = qTextEdit_->textCursor();
	int index = cursor.position() - 1;
	if (index < 0) index = 0;
	doSetCursorPos(index);	
}

void TextEditTool::doRight()
{
	QTextCursor cursor = qTextEdit_->textCursor();

	int index = cursor.position() + 1;
	if (index < 0) index = 0;
	doSetCursorPos(index);	
}

void TextEditTool::doSetCursorPos(int index)
{	
	nextCharProps_.reset();

	QTextCursor cursor = qTextEdit_->textCursor();
	if (qTextEdit_->isShiftPressed() || isDragging_)
		cursor.setPosition(index, QTextCursor::KeepAnchor);
	else
		cursor.setPosition(index);
	qTextEdit_->setTextCursor(cursor);
	qTextEdit_->setFocus();
			
	textObj_->posAtIndex(cursor.position(), &cursorPos_, &cursorHeight_, &curCharWidth_);

	keepCursorVisibleTill_ = Global::currentTime() + 1;

	int pos0 = cursor.anchor();
	int pos1 = cursor.position();
	charStartIndex_ = pos0 < pos1 ? pos0 : pos1;
	charEndIndex_ = pos0 > pos1 ? pos0 : pos1;

	lastCursorPosData_.anchor = cursor.anchor();
	lastCursorPosData_.pos = cursor.position();

	if (!active_ || textObj_ != document_->selectedObject()) return;

	PropertyPage *pPropertyPage = document_->getPropertyPage(textObj_);

	pPropertyPage->update();

	emit cursorPosChanged();
}

void TextEditTool::doSetCursorSelection(CursorPosData d)
{
	QTextCursor cursor = qTextEdit_->textCursor();
	cursor.setPosition(d.anchor);
	cursor.setPosition(d.pos, QTextCursor::KeepAnchor);
	
	QString str = qTextEdit_->toPlainText();
	int posCheck = cursor.position();
	
	qTextEdit_->setTextCursor(cursor);
	qTextEdit_->setFocus();

	
	

	textObj_->posAtIndex(cursor.position(), &cursorPos_, &cursorHeight_, &curCharWidth_);

	keepCursorVisibleTill_ = Global::currentTime() + 1;

	int pos0 = cursor.anchor();
	int pos1 = cursor.position();
	charStartIndex_ = pos0 < pos1 ? pos0 : pos1;
	charEndIndex_ = pos0 > pos1 ? pos0 : pos1;

	lastCursorPosData_.anchor = cursor.anchor();
	lastCursorPosData_.pos = cursor.position();

	emit cursorPosChanged();
}

void TextEditTool::restoreFocus()
{
	doSetCursorSelection(lastCursorPosData_);
}

void TextEditTool::doUp()
{
	QTextCursor cursor = qTextEdit_->textCursor();
	int oldIndex = cursor.position();
	float curLineY;
	int lineIndex, lineStartIndex, lineEndIndex;

	textObj_->lineDataForChar(cursor.position(), &curLineY, &lineIndex, &lineStartIndex, &lineEndIndex);	
	int newLineIndex = lineIndex - 1;
	if (newLineIndex < 0)
		doSetCursorPos(lineStartIndex);	
	else
	{
		float newLineY, newLineWidth, newLineHeight;

		int newLineStartIndex, newLineEndIndex;
		textObj_->lineDataForLine(newLineIndex, 
			&newLineY, &newLineWidth, &newLineHeight, &newLineStartIndex, &newLineEndIndex);

		int newIndex = newLineEndIndex - 1;
		if (newLineWidth > cursorPos_.x)
		{
			int propertyIndex;
			newIndex = textObj_->indexAtPos(
				Vector2(cursorPos_.x + curCharWidth_/2, newLineY + newLineHeight/2), &propertyIndex);
		}
		doSetCursorPos(newIndex);		
	}
}

void TextEditTool::doDown()
{
	QTextCursor cursor = qTextEdit_->textCursor();
	int oldIndex = cursor.position();
	float curLineY;
	int lineIndex, lineStartIndex, lineEndIndex;
	textObj_->lineDataForChar(cursor.position(), &curLineY, &lineIndex, &lineStartIndex, &lineEndIndex);	
	int newLineIndex = lineIndex + 1;
	if (newLineIndex >= textObj_->numLines())
		doSetCursorPos(lineEndIndex);
	else
	{
		float newLineY, newLineWidth, newLineHeight;
		int newLineStartIndex, newLineEndIndex;
		textObj_->lineDataForLine(newLineIndex, 
			&newLineY, &newLineWidth, &newLineHeight, &newLineStartIndex, &newLineEndIndex);

		int newIndex = newLineEndIndex - 1;
		if (newLineWidth > cursorPos_.x)
		{
			int propertyIndex;
			newIndex = textObj_->indexAtPos(
				Vector2(cursorPos_.x + curCharWidth_/2, newLineY + newLineHeight/2), &propertyIndex);
		}

		doSetCursorPos(newIndex);	
	}
}

void TextEditTool::selectAll()
{
	CursorPosData d;
	d.anchor = 0;
	d.pos = textObj_->textString().size();
	doSetCursorSelection(d);
}

void TextEditTool::doDel()
{
	int nStart = 0, nEnd = 0; 
	bool bLineStyleUpdate = false;
	QString qText = QString::fromStdWString(textObj_->textString());

	if(charStartIndex_ >= qText.size()) return;

	qTextEdit_->curLineNumber(nStart, nEnd);

	int nLineStartCharIndex = textObj_->getFirstCharIndexByLine(nStart);
	int nLineEndCharIndex = textObj_->getFirstCharIndexByLine(nEnd);

	if(nStart == nEnd){
		
		nStart ++;

		if(qText.at(charStartIndex_) == ('\n')){

			bLineStyleUpdate = true;
			nEnd++;
		}
		
	
	}else if(nStart != nEnd) {

		if(charStartIndex_ != nLineStartCharIndex || charEndIndex_ - charStartIndex_ == qText.size()){
			nStart ++;
		}else
			nEnd --;

		
		bLineStyleUpdate = true;

	}

	std::vector<LineStyle> prevLineStyle, newLineStyle;

	if(bLineStyleUpdate){

		prevLineStyle = textObj_->getLinesStyle();
		
		textObj_->delLineStyle(nStart, nEnd);

		newLineStyle = textObj_->getLinesStyle();

	}
	
	
	TextOperation * op;
	
	if (charStartIndex_ == charEndIndex_){

		if(bLineStyleUpdate)
			op = new TextOperation(this, L"", vector<TextProperties_2>(), charStartIndex_, charEndIndex_ + 1, prevLineStyle, newLineStyle);
		else
			op = new TextOperation(this, L"", vector<TextProperties_2>(), charStartIndex_, charEndIndex_ + 1);
	}else{

		if(bLineStyleUpdate)
			op = new TextOperation(this, L"", vector<TextProperties_2>(), charStartIndex_, charEndIndex_, prevLineStyle, newLineStyle);
		else
			op = new TextOperation(this, L"", vector<TextProperties_2>(), charStartIndex_, charEndIndex_);
	}

	doOp(op, false);
}

void TextEditTool::doEnter(){
	
	int nStart = 0, nEnd = 0;
	bool bChange = false;
	
	QString qText = QString::fromStdWString(textObj_->textString());

	prevLineStyle_ = textObj_->getLinesStyle();

	qTextEdit_->curLineNumber(nStart, nEnd);

	if(charStartIndex_ == charEndIndex_ || (nStart == nEnd && qText.at(charEndIndex_ - 1) != ('\n'))){
		bChange = true;
		textObj_->addLineStyle(nStart);
	}else if(nStart != nEnd){

		const QChar qEndChar = qText.at(charEndIndex_ - 1);
		bChange =true;

		if(qEndChar == ('\n')){
			
			textObj_->delLineStyle(nStart + 1, nEnd);
			textObj_->addLineStyle(nStart);
		}else{
			textObj_->delLineStyle(nStart + 1, nEnd - 1);
		}

	}

	if(bChange){
		newLineStyle_ = textObj_->getLinesStyle();
	}else
		prevLineStyle_.clear();
	
}

bool TextEditTool::doInputKeyPress(int key){
	int nStart = 0, nEnd = 0;
	bool bChange = false;
	bool bUpdateTextEdit = true;

	QString qText = QString::fromStdWString(textObj_->textString());

	prevLineStyle_ = textObj_->getLinesStyle();

	qTextEdit_->curLineNumber(nStart, nEnd);

	if(charStartIndex_ == charEndIndex_ && key == Qt::Key_Backspace){
		
		

		if(textObj_->getLinesStyle()[nStart].lineIncreasement_.type != NONE 
			&& ( textObj_->getLinesStyle()[nStart].charIndex == charStartIndex_ || qText.at(charStartIndex_ - 1) == '\n'))
		{

			bChange = true;
			LineStyle lineStyle;

			lineStyle.index_ = nStart;
			lineStyle.lineIncreasement_.type = NONE;
			textObj_->applyLineStyle(lineStyle, LineStyle::LineStyleIncleasement, 0);

			bUpdateTextEdit = false;
		}else if( charStartIndex_ > 0 && qText.at(charStartIndex_ - 1) == '\n'){

			bChange = true;
			textObj_->delLineStyle(nStart, nEnd);
		}

	}else if(nStart != nEnd){
		nStart ++;

		bChange = true;

		textObj_->delLineStyle(nStart, nEnd);
	}

	if(bChange){
		newLineStyle_ = textObj_->getLinesStyle();
	}else
		prevLineStyle_.clear();

	if(!bUpdateTextEdit && bChange){
		TextOperation * op = new TextOperation(this, L"", vector<TextProperties_2>(), charStartIndex_, charEndIndex_, prevLineStyle_, newLineStyle_);
		doOp(op, false);
		prevLineStyle_.clear();
		newLineStyle_.clear();
	}


	return bUpdateTextEdit;
}

void TextEditTool::getCurLineIndex(int &nStart, int &nEnd ){
	qTextEdit_->curLineNumber(nStart, nEnd);

}

void TextEditTool::cut()
{
	if (charStartIndex_ == charEndIndex_) return;

	this->copy();
	
	doDel();
	//TextOperation * op = new TextOperation(this, L"", vector<TextProperties_2>(), charStartIndex_, charEndIndex_);
	//doOp(op, false);
}
void TextEditTool::copy()
{
	if (charStartIndex_ == charEndIndex_) return;
	
	QMimeData * mimeData = new QMimeData();
	QByteArray encodedData;
	QDataStream stream(&encodedData, QIODevice::WriteOnly);
	
	vector<TextProperties_2> copiedProps;
	vector<LineStyle> copiedLineStyles;

	textObj_->getProperties(copiedProps, charStartIndex_, charEndIndex_, true);
	const LineStyle &startLineStyle = textObj_->getLineStyleByCharIndex(charStartIndex_);
	const LineStyle &endLineStyle = textObj_->getLineStyleByCharIndex(charEndIndex_);
	
	textObj_->getLinesStyle(copiedLineStyles, startLineStyle.index_, endLineStyle.index_, true);

	std::wstring copiedString = textObj_->textPartString(charStartIndex_, charEndIndex_);

	string tempFileName = qStringToStdString(QDir::tempPath()) + "/";
	tempFileName += "copied.tmp";

	Writer * writer = new Writer(tempFileName.c_str(), NULL);

	writer->write(copiedProps);

	writer->write(copiedLineStyles.size(), "Line Style Length");

	BOOST_FOREACH(const LineStyle &curLineStyle, copiedLineStyles){
		writer->write(curLineStyle.index_, "Line Style index");
		writer->write(curLineStyle.tabIndex_, "Line Style Tab index");

		writer->write(curLineStyle.textAlign_, "Line Align type");

		writer->write(curLineStyle.padding_.left, "Line Style Padding left");
		writer->write(curLineStyle.padding_.top, "Line Style Padding top");
		writer->write(curLineStyle.padding_.right, "Line Style Padding right");
		writer->write(curLineStyle.padding_.bottom, "Line Style Padding bottom");


		writer->write(curLineStyle.lineIncreasement_.type, "Line Style type");

		if(curLineStyle.lineIncreasement_.type != NONE){
			writer->write(curLineStyle.lineIncreasement_.index, "Line Increasement index");
			writer->write(curLineStyle.lineIncreasement_.depth, "Line Increasement depth");
		}
	}

	writer->write(copiedString);

	delete writer;
	
	

	vector<unsigned char> bytes;
	FILE * file = fopen(tempFileName.c_str(), "rb");
	fseek(file, 0, SEEK_END);	
	long fileSize = ftell(file);
	fseek(file, 0, SEEK_SET);
	bytes.resize(fileSize);
	fread(&bytes[0], 1, fileSize, file);
	fclose(file);
	unlink(tempFileName.c_str());

	stream.writeRawData((char *)&bytes[0], fileSize);
	mimeData->setData("application/text_with_properties" , encodedData);

	QClipboard *clipboard = QApplication::clipboard();
	clipboard->setMimeData(mimeData);
	
	clipboard->setText(QString::fromStdWString(copiedString));
}

void TextEditTool::paste()
{
	
	QClipboard *clipboard = QApplication::clipboard();
	const QMimeData * mimeData = clipboard->mimeData();


	std::wstring pastedText;
	std::vector<TextProperties_2> pastedProperties;
	std::vector<LineStyle> pastedLineStyles;

	if (mimeData->hasFormat("application/text_with_properties"))
	{
		QByteArray byteArray = mimeData->data("application/text_with_properties");
		
		string tempFileName = qStringToStdString(QDir::tempPath()) + "/";
		tempFileName += "pasted.tmp";

		FILE * file = fopen(tempFileName.c_str(), "wb");		
		fwrite(byteArray.data(), 1, byteArray.size(), file);
		fclose(file);

		Reader * reader = new Reader(tempFileName.c_str());
		reader->read(pastedProperties);
		
		int lineNumber = 0;
		reader->read(lineNumber);

		for(int i = 0; i < lineNumber; i ++){
			LineStyle curLineStyle;

			reader->read(curLineStyle.index_);
			reader->read(curLineStyle.tabIndex_);

			reader->read(curLineStyle.textAlign_);

			reader->read(curLineStyle.padding_.left);
			reader->read(curLineStyle.padding_.top);
			reader->read(curLineStyle.padding_.right);
			reader->read(curLineStyle.padding_.bottom);


			reader->read(curLineStyle.lineIncreasement_.type);

			if(curLineStyle.lineIncreasement_.type != NONE){
				reader->read(curLineStyle.lineIncreasement_.index);
				reader->read(curLineStyle.lineIncreasement_.depth);
			}

			pastedLineStyles.push_back(curLineStyle);
		}

		reader->read(pastedText);

		delete reader;

		unlink(tempFileName.c_str());

	}
	else if (!clipboard->text().isEmpty())
	{

		if(clipboard->mimeData()->hasHtml()){
			pastedText = CreateTextPropertyWithMime(clipboard->mimeData()->html(), MIME_HTML, pastedProperties, pastedLineStyles);
		}else if(clipboard->mimeData()->hasText()){
			pastedText = CreateTextPropertyWithMime(clipboard->mimeData()->text(), MIME_PLAINTEXT, pastedProperties, pastedLineStyles);
		}
	}

	if (!pastedText.empty())
	{
		TextOperation * op  = NULL;
		if(pastedLineStyles.size() == 1)
			op = new TextOperation(this, pastedText, pastedProperties, charStartIndex_, charEndIndex_);
		else{

			if(charStartIndex_ != charEndIndex_)
				doDel();

			LineStyle curPosLine = textObj_->getLineStyleByCharIndex(charStartIndex_);

			std::vector<LineStyle> prevLineStyle = textObj_->getLinesStyle();
			std::vector<LineStyle> newLineStyle;

			int index = curPosLine.index_;
			
			BOOST_FOREACH(const LineStyle &curLine, pastedLineStyles){

				LineStyle lineStyle = curLine;

				if(index == curPosLine.index_){
					lineStyle.index_ = curPosLine.index_;
					textObj_->applyLineStyle(lineStyle, LineStyle::LineStyleIncleasement, 0);
					textObj_->applyLineStyle(lineStyle, LineStyle::LineStyleAlign, 0);
					textObj_->applyLineStyle(lineStyle, LineStyle::LineStyleTap, 0);
				}else{
					lineStyle.index_ = index - 1;
					textObj_->addLineStyle(lineStyle, index - 1);
				}

				index ++;

			}

			newLineStyle = textObj_->getLinesStyle();
			op = new TextOperation(this, pastedText, pastedProperties, charStartIndex_, charEndIndex_, prevLineStyle, newLineStyle);
		}
		doOp(op, false);	
	}	
}

int TextEditTool::charStartIndex() const
{
	return charStartIndex_;
}

int TextEditTool::charEndIndex() const
{
	return charEndIndex_;
}

bool TextEditTool::isSelectedPart() const
{
	return charStartIndex_ != charEndIndex_;
}


void TextEditTool::doOp(TextEditToolOperation * op, bool fromUserTypedText)
{
	const unsigned int MAXOPS = 1024;
	TextEditToolOperationSPtr opSptr(op);
	undoOps_.push_back(opSptr);
	CursorPosData cursorData = opSptr->doOp(this);
	redoOps_.clear();
	if (undoOps_.size() > MAXOPS)
		undoOps_.pop_front();

	if (!fromUserTypedText)
	{
		syncQTextEditWithProp();
	}	
	
	textObj_->init(document_->renderer());

	doSetCursorSelection(cursorData);

	oldText_ = textObj_->textString();
	oldProps_ = textObj_->properties();

	nextCharProps_.reset();

}

void TextEditTool::undoOp()
{
	if (undoOps_.empty()) return;

	CursorPosData cursorData = undoOps_.back()->undoOp(this);	
	redoOps_.push_front(undoOps_.back());
	undoOps_.pop_back();

	syncQTextEditWithProp();
	
	textObj_->init(document_->renderer());

	doSetCursorSelection(cursorData);

	oldText_ = textObj_->textString();
	oldProps_ = textObj_->properties();

	nextCharProps_.reset();
}

void TextEditTool::redoOp()
{
	if (redoOps_.empty()) return;
	CursorPosData cursorData = redoOps_.front()->doOp(this);	
	undoOps_.push_back(redoOps_.front());
	redoOps_.pop_front();

	syncQTextEditWithProp();

	textObj_->init(document_->renderer());

	doSetCursorSelection(cursorData);	

	oldText_ = textObj_->textString();
	oldProps_ = textObj_->properties();

	nextCharProps_.reset();

}

///////////////////////////////////////////////////////////////////////////////

void TextEditTool::setNextCharProps(const TextProperties_2 & props)
{
	nextCharProps_.reset(new TextProperties_2(props, false));
}

///////////////////////////////////////////////////////////////////////////////

ChangeTextCmd::ChangeTextCmd(Text_2 * text, const TextState & state)
{
	text_ = text;
	newState_ = state;
	oldState_.getFrom(text);
}

void ChangeTextCmd::doCommand()
{
	Document * doc = text_->parentScene()->parentDocument();
	newState_.applyTo(text_);
	text_->init(doc->renderer());
	emit objectChanged();
}

void ChangeTextCmd::undoCommand()
{
	Document * doc = text_->parentScene()->parentDocument();
	oldState_.applyTo(text_);
	text_->init(doc->renderer());
	emit objectChanged();
}
