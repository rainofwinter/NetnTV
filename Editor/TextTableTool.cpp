#include "stdafx.h"
#include "TextTableTool.h"
#include "GLWidget.h"
#include "Attrib.h"
#include "Camera.h"
#include "EditorDocument.h"
#include "EditorObject.h"
#include "Command.h"
#include "Scene.h"
#include "Text_2.h"
#include "TextTable.h"
#include "MainWindow.h"
#include "Utils.h"

#include "EditorScene.h"
#include "TextEditTool.h"

using namespace std;


TextTableTool::TextTableTool(GLWidget * gl) : Tool(gl)
{	
	connect(document_, SIGNAL(objectSelectionChanged()), this, SLOT(onLostObjSelection()));
	connect(document_, SIGNAL(objectListChanged()), this, SLOT(onLostObjSelection()));

	isDragging_ = false;
	grabbedBorder_ = false;
	grabbedBorderIsVert_ = false;
	grabbedBorderIndex_ = 0;
	textTableObj_ = NULL;

	mode_ = Draw;

	textEditTool_.reset(new TextEditTool(gl));

	toolName_ = "TextTableTool";
}

TextTableTool::~TextTableTool()
{
}

void TextTableTool::setMode(const Mode & mode)
{
	Mode oldMode = mode_;
	mode_ = mode;
	if (oldMode == EditText && mode != EditText)
	{
		if (textEditTool_->isActive())
			textEditTool_->setActive(false);
	}
	
}

void TextTableTool::endTool()
{
	textTableObj_ = NULL;
	glWidget_->setToolMode(GLWidget::Select);
}

void TextTableTool::onLostObjSelection()
{
	if (textTableObj_ != document_->selectedObject())
		endTool();
}


void TextTableTool::setTextTableObj(TextTable * textTableObj)
{
	textTableObj_ = textTableObj;
}

void TextTableTool::init()
{
}

void TextTableTool::uninit()
{
}

Vector2 TextTableTool::getLocalPos(QMouseEvent * event) const
{
	Vector2 mousePosDevice = glWidget_->windowToDeviceCoords(
		Vector2(event->posF().x(), event->posF().y()));

	Ray ray = glWidget_->renderer()->unproject(mousePosDevice);
	Vector3 out;
	textTableObj_->intersect(&out, ray);
	
	Matrix invTrans = 
		(textTableObj_->parentTransform() * (textTableObj_->visualAttrib()->transformMatrix())).inverse();

	Vector3 invClickPt = invTrans * out;
	return Vector2(invClickPt.x, invClickPt.y);
}

void TextTableTool::mousePressEvent(QMouseEvent * event)
{
	Vector2 localPos = getLocalPos(event);	

	//first see if a grid line was clicked on
	int vDist, hDist;
	int vIndex = textTableObj_->getVertGridLineIndex(localPos, &vDist);
	int hIndex = textTableObj_->getHorzGridLineIndex(localPos, &hDist);

	int grabDist = 8;

	grabbedBorder_ = false;
	if (hDist < vDist)
	{
		if (hDist < grabDist / 2)
		{
			grabbedBorder_ = true;
			grabbedBorderIndex_ = hIndex;
			grabbedBorderIsVert_ = false;
		}
	}
	else
	{
		if (vDist < grabDist / 2)
		{
			grabbedBorder_ = true;
			grabbedBorderIndex_ = vIndex;
			grabbedBorderIsVert_ = true;
		}
	}

	if (mode_ == Erase) grabbedBorder_ = false;

	if (!grabbedBorder_)
	{
		if (mode_ == Draw)
		{
			candLines_ = textTableObj_->getDrawCandidateLines(localPos, 8);
		}
		else if (mode_ == Erase)
		{
			candLines_ = textTableObj_->getEraseCandidateLines(localPos, 8);
		}
		else if (mode_ == EditText)
		{
			Text_2 * text = textTableObj_->getTextObj(localPos);
			
			if (textEditTool_->textObj() != text && textEditTool_->isActive())
				textEditTool_->setActive(false);		
			
			if (text)
			{
				textEditTool_->setTextObj(text);

				if (!textEditTool_->isActive())
					textEditTool_->setActive(true);
			}

			if (textEditTool_->isActive())
			{
				textEditTool_->mousePressEvent(event);		
			}
		}
	}
	else
	{
		origGridSizes_.reset(textTableObj_->getGridSizes());
	}

	drawLines_.clear();
	mousePressPos_ = localPos;
	isDragging_ = true;
	
}



void TextTableTool::mouseReleaseEvent(QMouseEvent * event)
{	
	if (isDragging_)
	{
		if (grabbedBorder_)
		{
			TextTableGridSizesSPtr newGridSizes(textTableObj_->getGridSizes());

			if (!(*newGridSizes == *origGridSizes_))
			{
				document_->doCommand(new ChangeTextGridSizesCmd(
					textTableObj_, origGridSizes_, newGridSizes));
			}
		}
		else
		{
			TextTableStateSPtr oldState(textTableObj_->getState());
			bool tableStructureChanged = false;
			if (mode_ == Draw)
			{			
				tableStructureChanged = textTableObj_->drawAlongLines(drawLines_);			
			}
			else if (mode_ == Erase)
			{
				tableStructureChanged = textTableObj_->eraseAlongLines(drawLines_);
			}
			else if (mode_ == EditText)
			{
				if (textEditTool_->isActive())
				{		
					textEditTool_->mouseReleaseEvent(event);
				}
			}

			if (tableStructureChanged)
			{
				document_->doCommand(new ChangeTextTableStateCmd(
					textTableObj_, oldState, TextTableStateSPtr(textTableObj_->getState())));
			}

			drawLines_.clear();
		}
	}
	isDragging_ = false;
	
}

bool TextTableTool::spanIntersect(int a1, int a2, int b1, int b2) const
{
	if (b1 > b2) swap(b1, b2);

	return 
		(a1 < b1 && a2  > b1) || 
		(a1 < b2 && a2  > b2) ||
		(a1 > b1 && a2 < b2);
}

void TextTableTool::mouseMoveEvent(QMouseEvent * event)
{
	if (textEditTool_->isActive())
	{
		textEditTool_->mouseMoveEvent(event);
	}

	if (!isDragging_) return;

	Vector2 localPos = getLocalPos(event);

	if (grabbedBorder_)
	{
		if (grabbedBorderIsVert_)
			textTableObj_->setVertGridLinePos(grabbedBorderIndex_, (int)localPos.x);
		else
			textTableObj_->setHorzGridLinePos(grabbedBorderIndex_, (int)localPos.y);
		
	}
	else if (mode_ == Draw || mode_ == Erase)
	{

		bool isVertical = false;
		if (fabs(localPos.y - mousePressPos_.y) > fabs(localPos.x - mousePressPos_.x))	
			isVertical = true;
			
		drawLines_.clear();
		BOOST_FOREACH(const TextTable::CandidateLine & candLine, candLines_)
		{
			int startPos, len;
			textTableObj_->candLinePixelSpan(candLine, &startPos, &len);
			int candLineEnd = startPos + len;

			if (isVertical && candLine.type == TextTable::CandidateLine::Vertical)
			{			
				if (spanIntersect(startPos, candLineEnd, mousePressPos_.y, localPos.y))		
					drawLines_.push_back(candLine);		
			}
			else if (!isVertical && candLine.type == TextTable::CandidateLine::Horizontal)
			{
				if (spanIntersect(startPos, candLineEnd, mousePressPos_.x, localPos.x))		
					drawLines_.push_back(candLine);		
			}
		}
	}
	
	
}

void TextTableTool::setActive(bool val)
{
	Tool::setActive(val);
	
	if (val)	
	{
	}
	else
	{
		if (textEditTool_->isActive())
			textEditTool_->setActive(false);
	}
	
}

void TextTableTool::draw()
{
	GfxRenderer * gl = glWidget_->renderer();
	float time = Global::currentTime();

	//glEnable(GL_COLOR_LOGIC_OP);
	//glLogicOp(GL_XOR);
	gl->useColorProgram();

	if (mode_ == Draw)
		gl->setColorProgramColor(0.2f, 0.2f, 1.0f, 1.0f);
	else if (mode_ == Erase)
		gl->setColorProgramColor(1.0f, 0.2f, 0.2f, 1.0f);

	gl->pushMatrix();

	Matrix totalTransform = textTableObj_->parentTransform() * (textTableObj_->visualAttrib()->transformMatrix());
	gl->loadMatrix(totalTransform);


	Vector3 vertices[2];
	gl->enableVertexAttribArrayPosition();
	gl->bindArrayBuffer(0);
	gl->vertexAttribPositionPointer(0, (char *)vertices);	
	gl->applyCurrentShaderMatrix();

	if (mode_ != EditText)

	BOOST_FOREACH(TextTable::CandidateLine & drawLine, drawLines_)
	{
		int startPos, len;
		textTableObj_->candLinePixelSpan(drawLine, &startPos, &len);
		
		if (drawLine.type == TextTable::CandidateLine::Horizontal)
		{
			vertices[0] = Vector3(startPos, drawLine.pos, 0.0f);
			vertices[1] = Vector3(startPos + len, drawLine.pos, 0.0f);
		}
		else
		{
			vertices[0] = Vector3(drawLine.pos, startPos, 0.0f);
			vertices[1] = Vector3(drawLine.pos, startPos + len, 0.0f);
		}
		
		
		glDrawArrays(GL_LINES, 0, 2);
	}

	//glDisable(GL_COLOR_LOGIC_OP);
	gl->popMatrix();

	if (textEditTool_->isActive()) textEditTool_->draw();
}

bool TextTableTool::isBeingUsed() const
{
	return false;
}


