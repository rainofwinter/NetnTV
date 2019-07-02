#pragma once
#include "Tool.h"
#include "MathStuff.h"
#include "Transform.h"
#include "TextTable.h"

class TextTable;
class TextEditTool;

///////////////////////////////////////////////////////////////////////////////
class TextTableTool : public Tool
{
	friend class TextTableControl;
	Q_OBJECT
public:
	enum Mode
	{
		Draw,
		Erase,
		EditText
	};

	
public:
	TextTableTool(GLWidget * gl);
	~TextTableTool();

	virtual void init();
	virtual void uninit();

	virtual void mousePressEvent(QMouseEvent * event);
	virtual void mouseReleaseEvent(QMouseEvent * event);
	virtual void mouseMoveEvent(QMouseEvent * event);

	virtual void setActive(bool val);
		
	virtual void draw();

	virtual bool isBeingUsed() const;

	void setTextTableObj(TextTable * textObj);

	void setMode(const Mode & mode);

	Mode mode() const {return mode_;}

private:
	void endTool();

	Vector2 getLocalPos(QMouseEvent * event) const;

	bool spanIntersect(int a1, int a2, int b1, int b2) const;
private slots:

	void onLostObjSelection();

	
private:
	TextTable * textTableObj_;
	bool isDragging_;

	bool grabbedBorder_;
	bool grabbedBorderIsVert_;
	int grabbedBorderIndex_;
	
	Mode mode_;

	boost::shared_ptr<TextTableGridSizes> origGridSizes_;
	
	Vector2 mousePressPos_;
	std::vector<TextTable::CandidateLine> candLines_;
	std::vector<TextTable::CandidateLine> drawLines_;
	boost::scoped_ptr<TextEditTool> textEditTool_;
};


