#pragma once
#include "Tool.h"
#include "MathStuff.h"
#include "Transform.h"
#include "Text_2.h"
#include "ui_TextEditWindow.h"

class SceneObject;
class MainWindow;
class TextProperties_2;
class TextEditTool;
class Command;
///////////////////////////////////////////////////////////////////////////////
struct CursorPosData
{
	CursorPosData() {anchor = pos = 0;}
	CursorPosData(int anchor, int pos)
	{
		this->anchor = anchor;
		this->pos = pos;
	}

	CursorPosData(int index)
	{
		anchor = pos = index;
	}

	int anchor;
	int pos;
};

class TextEditToolOperation 
{
public:
	virtual ~TextEditToolOperation() {}

	virtual CursorPosData doOp(TextEditTool * textEditTool) = 0;
	virtual CursorPosData undoOp(TextEditTool * textEditTool) = 0;
};


typedef boost::shared_ptr<TextEditToolOperation> TextEditToolOperationSPtr;

class TextOperation : public TextEditToolOperation
{
public: 
	TextOperation();
	TextOperation(TextEditTool * textEditTool, const std::wstring & text, 
		const std::vector<TextProperties_2> & properties, 
		unsigned int startIndex, unsigned int endIndex);
	TextOperation(TextEditTool * textEditTool, const std::wstring & text, 
		const std::vector<TextProperties_2> & properties, 
		unsigned int startIndex, unsigned int endIndex, const std::vector<LineStyle> & origlines, const std::vector<LineStyle> & newlines);
	
	CursorPosData doOp(TextEditTool * textEditTool);
	CursorPosData undoOp(TextEditTool * textEditTool);
private:
	unsigned startIndex_, endIndex_;	
	std::wstring text_;
	std::vector<TextProperties_2> properties_;

	unsigned origStartIndex_, origEndIndex_;
	std::wstring origText_;
	std::vector<TextProperties_2> origProperties_;

	std::vector<LineStyle> linesStyle_, origlinesStyle_;

};

///////////////////////////////////////////////////////////////////////////////

class PropertiesOperation : public TextEditToolOperation
{
public:

	PropertiesOperation();
	PropertiesOperation(TextEditTool * textEditTool, 
		const std::vector<TextProperties_2> & properties,
		unsigned int startIndex, unsigned int endIndex);

	CursorPosData doOp(TextEditTool * textEditTool);
	CursorPosData undoOp(TextEditTool * textEditTool);

private:

	unsigned startIndex_, endIndex_;
	std::vector<TextProperties_2> properties_, origProperties_;

	CursorPosData cursorPosData_;
};

class LineStyleOperation : public TextEditToolOperation
{
public:

	LineStyleOperation();
	LineStyleOperation(TextEditTool * textEditTool, 
		const std::vector<LineStyle> & origproperties,const std::vector<LineStyle> & newproperties);

	CursorPosData doOp(TextEditTool * textEditTool);
	CursorPosData undoOp(TextEditTool * textEditTool);

private:
	std::vector<LineStyle> properties_, origProperties_;

	CursorPosData cursorPosData_;

};


///////////////////////////////////////////////////////////////////////////////

class GlobalTextOperation : public TextEditToolOperation
{
public:

	GlobalTextOperation();
	GlobalTextOperation(TextEditTool * textEditTool, 
		Command * globalTextCommand);

	CursorPosData doOp(TextEditTool * textEditTool);
	CursorPosData undoOp(TextEditTool * textEditTool);

private:
	boost::shared_ptr<Command> command_;
	CursorPosData cursorPosData_;
};

///////////////////////////////////////////////////////////////////////////////

class TextEditWindow : public QWidget
{
	Q_OBJECT
public:
	TextEditWindow(MainWindow * mainWindow);
	void setText(const QString & text);
signals:
	void closed();
private slots:
	void onDone();
private:
	virtual void focusInEvent(QFocusEvent * event);
	virtual void closeEvent(QCloseEvent *);
private:
	Ui::TextEditWindow ui;
};

///////////////////////////////////////////////////////////////////////////////
class TextEditTool;

class TextEditControl : public QTextEdit
{
	Q_OBJECT
public:
	TextEditControl(TextEditTool * textEditTool, QWidget * parent);
	virtual void keyPressEvent(QKeyEvent * event);
	virtual void keyReleaseEvent(QKeyEvent * event);
	virtual void inputMethodEvent(QInputMethodEvent * event);

	bool isImeEditing() const {return !imePreeditText_.isEmpty();}
	QString imePreeditText() const {return imePreeditText_;}

	QString getText();

	bool isShiftPressed() const {return shiftPressed_;}
	bool isCtrlPressed() const {return ctrlPressed_;}

	void redo();
	void undo();

	void curLineNumber(int &nStart, int &nEnd);

signals:
	void imeTextChanged();

private:

	QString imePreeditText_;

	TextEditTool * textEditTool_;
	bool isImeEditing_;

	bool shiftPressed_;
	bool ctrlPressed_;

	
};



///////////////////////////////////////////////////////////////////////////////
struct TextState
{
	std::wstring text;
	std::vector<TextProperties_2> props;
	float boundaryWidth;
	float boundaryHeight;
	float letterSpacing;
	Text_2::LineSpacingMode lineSpacingMode;
	float lineSpacing;
	bool canSelectPart;
	std::vector<LineStyle> lineStyles;

	void clear();
	void getFrom(Text_2 * text);
	void applyTo(Text_2 * text);
	bool operator != (const TextState & rhs) const;
};

class TextEditTool : public Tool
{
	friend class TextEditControl;

	Q_OBJECT
public:
	TextEditTool(GLWidget * gl);
	~TextEditTool();

	virtual void init();
	virtual void uninit();

	virtual void mousePressEvent(QMouseEvent * event);
	virtual void mouseDoublePressEvent(QMouseEvent * event);
	virtual void mouseReleaseEvent(QMouseEvent * event);
	virtual void mouseMoveEvent(QMouseEvent * event);
	virtual bool keyPressEvent(QKeyEvent * event);

	virtual void setActive(bool val);
		
	virtual void draw();

	CursorPosData getCursorPosData() const;

	virtual bool isBeingUsed() const;

	void setTextObj(Text_2 * textObj);
	Text_2 * textObj() const {return textObj_;}
	void doCommitTextCommand();

	void doHome();
	void doEnd();
	void doLeft();
	void doRight();
	void doUp();
	void doDown();
	void selectAll();
	
	void doEnter();

	bool isSelectedPart() const;
	int charStartIndex() const;
	int charEndIndex() const;
	int charCurIndex() const;
	
	void getCurLineIndex(int &nStart, int &nEnd );
	
	EditorDocument* doc() {return document_;}
	void doDel();
	void cut();
	void copy();
	void paste();
	bool doInputKeyPress(int key);

	void doSetCursorPos(int index);
	void doSetCursorSelection(CursorPosData d);
	void restoreFocus();


	void doOp(TextEditToolOperation * op,  bool fromUserTypedText);
	void undoOp();
	void redoOp();

	TextProperties_2 * nextCharProps() const {return nextCharProps_.get();}
	void setNextCharProps(const TextProperties_2 & props);

	void insertImg(std::string &image);

	void refresh();

signals:	
	void cursorPosChanged();
private:
	
	
	void endTool();
	void getPropsAtPos(std::vector<TextProperties_2> & props, int index) const;

	void syncQTextEditWithProp();

	std::vector<LineStyle> prevLineStyle_, newLineStyle_;
private slots:

	void onWindowClosed();
	void onLostObjSelection();

	void textChangedInControl();
	void imeTextChangedInControl();
	void cursorChangedInControl();
private:
	Text_2 * textObj_;	
	TextEditWindow * inspectWnd_;

	TextEditControl * qTextEdit_;
	Vector2 cursorPos_;	
	float cursorHeight_;
	float curCharWidth_;

	float keepCursorVisibleTill_;

	bool isDragging_;

	bool disableWindowClosedHandling_;

	std::vector<TextProperties_2> copiedProps_;
	std::wstring copiedString_;

	std::deque<TextEditToolOperationSPtr> undoOps_;
	std::deque<TextEditToolOperationSPtr> redoOps_;

	int charStartIndex_;
	int charEndIndex_;

	bool imeAltered_;
	std::wstring oldText_;
	std::vector<TextProperties_2> oldProps_;

	TextState firstTextState_;
	
	bool respondToTextChanged_;

	CursorPosData lastCursorPosData_;

	boost::scoped_ptr<TextProperties_2> nextCharProps_;
	boost::scoped_ptr<TextProperties_2> objectProps_;
};

///////////////////////////////////////////////////////////////////////////////

#include "Command.h"

class ChangeTextCmd : public Command
{
public:
	ChangeTextCmd(Text_2 * text, const TextState & newTextState);
	virtual void doCommand();
	virtual void undoCommand();
	virtual QString name() const {return tr("Change text");}
private:
	Text_2 * text_;
	TextState oldState_, newState_;	
};
