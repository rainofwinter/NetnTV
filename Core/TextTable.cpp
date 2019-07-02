#include "stdafx.h"
#include "TextTable.h"
#include "Text.h"
#include "Text_2.h"
#include "GfxRenderer.h"
#include "Writer.h"
#include "Xml.h"
#include "Reader.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////////

TextTable::CellMerge::CellMerge(int row, int col, Type type)
{
	this->row = row;
	this->col = col;
	this->type = type;	
}

bool TextTable::CellMerge::operator < (const CellMerge & rhs) const
{
	if (type != rhs.type) return type < rhs.type;
	if (row != rhs.row) return row < rhs.row;
	return col < rhs.col;
}

///////////////////////////////////////////////////////////////////////////////

TextTable::CandidateLine::CandidateLine(Type type, int pos, int startGridIndex, int gridSpan)
{
	this->pos = pos;
	this->startGridIndex = startGridIndex;
	this->gridSpan = gridSpan;
	this->type = type;
}

bool TextTable::CandidateLine::operator < (const CandidateLine & rhs) const
{
	if (type != rhs.type) return type < rhs.type;
	if (pos != rhs.pos) return pos < rhs.pos;
	if (startGridIndex != rhs.startGridIndex) return startGridIndex < rhs.startGridIndex;
	return gridSpan < rhs.gridSpan;
}

void TextTable::CandidateLine::unite(const CandidateLine & rhs) 
{
	if (type != rhs.type || pos != rhs.pos) return;
	int start = min(startGridIndex, rhs.startGridIndex);
	int end = max(startGridIndex + gridSpan, rhs.startGridIndex + rhs.gridSpan);

	startGridIndex = start;
	gridSpan = end - start;
}

///////////////////////////////////////////////////////////////////////////////

void TextTable::TextCellData::write(Writer & writer) const
{
	writer.write(row);
	writer.write(col);
	writer.write(rowSpan);
	writer.write(colSpan);
	writer.write(text);
}

void TextTable::TextCellData::read(Reader & reader, unsigned char version)
{
	reader.read(row);
	reader.read(col);
	reader.read(rowSpan);
	reader.read(colSpan);

	if	(version < 1)
	{
		boost::shared_ptr<Text> textTemp;
		reader.read(textTemp);

		text.reset(new Text_2);
		text->convertText_2(textTemp.get());
	}
	else
		reader.read(text);
}


void TextTable::TextCellData::writeXml(XmlWriter & w) const
{
	w.writeTag("Row", row);
	w.writeTag("Col", col);
	w.writeTag("RowSpan", rowSpan);
	w.writeTag("ColSpan", colSpan);
	
	w.startTag("Text_2");
	Text_2* t = text.get();
	//w.startTag(t->xmlTag());
	t->writeXml(w);
	//w.endTag();
	w.endTag();
}

void TextTable::TextCellData::readXml(XmlReader & r, xmlNode * parent)
{
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;
		if (r.getNodeContentIfName(row, curNode, "Row"));
		else if (r.getNodeContentIfName(col, curNode, "Col"));
		else if (r.getNodeContentIfName(rowSpan, curNode, "RowSpan"));
		else if (r.getNodeContentIfName(colSpan, curNode, "ColSpan"));
		else if (r.isNodeName(curNode, "Text"))
		{
			boost::shared_ptr<Text> textTemp;
			textTemp.reset(new Text);
			textTemp.get()->readXml(r, curNode);
			
			text.reset(new Text_2);
			text.get()->convertText_2(textTemp.get());

			curNode = curNode->next;
		}
		else if (r.isNodeName(curNode, "Text_2"))
		{
			text.reset(new Text_2);
			text.get()->readXml(r, curNode);
			
			curNode = curNode->next;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////

int TextTable::sMinCellDim_ = 5;

void TextTable::create()
{
	border_ = 3;
	cellPadding_ = 3;

	gridStartX_ = 0;
	gridStartY_ = 0;

	colSizes_.push_back(640);
	rowSizes_.push_back(480);

	computeCumRowColSizes();
	createTextCells();
}

TextTable::TextTable()
{
	create();
	setId("TextTable");
}

TextTable::TextTable(const TextTable & rhs, ElementMapping * elementMapping)
: SceneObject(rhs, elementMapping)
{
	create();
	visualAttrib_ = rhs.visualAttrib_;
	rowSizes_ = rhs.rowSizes_;
	colSizes_ = rhs.colSizes_;
	cumRowSizes_ = rhs.cumRowSizes_;
	cumColSizes_ = rhs.cumColSizes_;
	cellMerges_ = rhs.cellMerges_;	
	/*
	prevCumRowSizes_ = rhs.prevCumRowSizes_;
	prevCumColSizes_ = rhs.prevCumColSizes_;	
	prevCellMerges_ = rhs.prevCellMerges_;
	*/
	prevCumRowSizes_.clear();
	prevCumColSizes_.clear();
	prevCellMerges_.clear();

	gridStartX_ = rhs.gridStartX_;
	gridStartY_ = rhs.gridStartY_;
	cellPadding_ = rhs.cellPadding_;
	border_ = rhs.border_;
/*
	std::set<TextCellData>::const_iterator iter;
	for (iter = rhs.textCells_.begin(); iter != rhs.textCells_.end(); ++iter)
	{
		const TextCellData & rhsTextCell = *iter;
		TextCellData data(rhsTextCell);
		data.text.reset(new Text(*rhsTextCell.text, elementMapping));
		textCells_.insert(data);
	}
*/
	createTextCells();
	std::set<TextCellData>::iterator lhsIter = textCells_.begin();
	std::set<TextCellData>::const_iterator rhsIter = rhs.textCells_.begin();
	for (; rhsIter != rhs.textCells_.end(); ++rhsIter, ++lhsIter)
	{
		const TextCellData & rhsTextCell = *rhsIter;		
		const TextCellData & lhsTextCell = *lhsIter;

		lhsTextCell.text->setTextString(rhsTextCell.text->textString());
		lhsTextCell.text->setProperties(rhsTextCell.text->properties());
	}
}


TextTable::~TextTable()
{
	uninit();

}

int TextTable::width() const
{
	return cumColSizes_.back() - cumColSizes_.front();
}

int TextTable::height() const
{
	return cumRowSizes_.back() - cumRowSizes_.front();
}

void TextTable::getCellMergeEnclosingRegion(int & row, int & col, int & rowSpan, int & colSpan)
{
	set<CellMerge>::iterator iter;
	
	int endRow = row + rowSpan - 1;
	int endCol = col + colSpan - 1;

	int newRow = row;
	int newCol = col;
	int newEndRow = endRow;
	int newEndCol = endCol;

	for (int i = row; i < row + rowSpan; ++i)
	{		
		iter = cellMerges_.find(CellMerge(i, col - 1, CellMerge::Horizontal));
		if (iter != cellMerges_.end()) newCol = col - 1;

		iter = cellMerges_.find(CellMerge(i, col + colSpan - 1, CellMerge::Horizontal));
		if (iter != cellMerges_.end()) newEndCol = endCol + 1;		
	}

	for (int i = col; i < col + colSpan; ++i)
	{		
		iter = cellMerges_.find(CellMerge(row - 1, i, CellMerge::Vertical));
		if (iter != cellMerges_.end()) newRow = row - 1;

		iter = cellMerges_.find(CellMerge(row + rowSpan - 1, i, CellMerge::Vertical));
		if (iter != cellMerges_.end()) newEndRow = endRow + 1;		
	}

	if (newRow != row || newEndRow != endRow || newCol != col || newEndCol != endCol)
	{
		row = newRow;
		col = newCol;
		rowSpan = newEndRow - newRow + 1;
		colSpan = newEndCol - newCol + 1;
		getCellMergeEnclosingRegion(row, col, rowSpan, colSpan);
	}
}

void TextTable::addCellMerge(int row, int col, CellMerge::Type type)
{
	cellMerges_.insert(CellMerge(row, col, type));

	int startRow = row;
	int startCol = col;
	int colSpan = 1;
	int rowSpan = 1;
	if (type == CellMerge::Vertical) 
		rowSpan = 2; 
	else 
		colSpan = 2;

	getCellMergeEnclosingRegion(startRow, startCol, rowSpan, colSpan);

	for (int i = startRow; i < startRow + rowSpan; ++i)
	{
		for (int j = startCol; j < startCol + colSpan - 1; ++j)			
			cellMerges_.insert(CellMerge(i, j, CellMerge::Horizontal));			
	}

	for (int i = startRow; i < startRow + rowSpan - 1; ++i)
	{
		for (int j = startCol; j < startCol + colSpan; ++j)			
			cellMerges_.insert(CellMerge(i, j, CellMerge::Vertical));			
	}
}

void TextTable::getSpansForGridCell(const std::set<CellMerge> & cellMerges, int rowInTextCell, int colInTextCell, 
	int * startRow, int * startCol, int * rowSpan, int * colSpan) const
{
	*startRow = rowInTextCell;
	*startCol = colInTextCell;

	for (int i = colInTextCell - 1; i >= 0; --i)
	{
		if (cellMerges.find(CellMerge(rowInTextCell, i, CellMerge::Horizontal)) != cellMerges.end())		
			*startCol = i;		
		else break;
	}

	for (int i = rowInTextCell - 1; i >= 0; --i)
	{
		if (cellMerges.find(CellMerge(i, colInTextCell, CellMerge::Vertical)) != cellMerges.end())		
			*startRow = i;		
		else break;
	}

	*rowSpan = rowInTextCell - *startRow + 1;
	*colSpan = colInTextCell - *startCol + 1;

	for (int i = colInTextCell; i < numCols(); ++i)
	{
		if (cellMerges.find(CellMerge(rowInTextCell, i, CellMerge::Horizontal)) != cellMerges.end())		
			(*colSpan)++;		
		else break;
	}

	for (int i = rowInTextCell; i < numRows(); ++i)
	{
		if (cellMerges.find(CellMerge(i, colInTextCell, CellMerge::Vertical)) != cellMerges.end())		
			(*rowSpan)++;		
		else break;
	}
}

void TextTable::computeCumRowColSizes()
{
	int cumRowSize = gridStartY_;
	int cumColSize = gridStartX_;
	
	cumRowSizes_.resize(rowSizes_.size() + 1);
	for (int i = 0; i < (int)rowSizes_.size(); ++i)
	{
		cumRowSizes_[i] = cumRowSize;
		cumRowSize += rowSizes_[i];
	}
	cumRowSizes_[rowSizes_.size()] = cumRowSize;
	
	cumColSizes_.resize(colSizes_.size() + 1);
	for (int i = 0; i < (int)colSizes_.size(); ++i)
	{
		cumColSizes_[i] = cumColSize;
		cumColSize += colSizes_[i];
	}
	cumColSizes_[colSizes_.size()] = cumColSize;
}

void TextTable::resizeTextCells()
{
	BOOST_FOREACH(const TextCellData & data, textCells_)
	{
		int x = cumColSizes_[data.col];
		int y = cumRowSizes_[data.row];
		int w = cumColSizes_[data.col + data.colSpan] - x;
		int h = cumRowSizes_[data.row + data.rowSpan] - y;

		Transform transform;
		transform.setTranslation(Vector3((float)x + cellPadding_, (float)y + cellPadding_, 0.0f));	
		data.text->setTransform(transform);
		data.text->setBoundaryHeight(max((float)h - 2*cellPadding_, 1.0f));
		data.text->setBoundaryWidth(max((float)w - 2*cellPadding_, 1.0f));
	}
	
	if (isInit())init(gl_);
}

void TextTable::textCellDims(const TextCellData & data,
	const std::vector<int> & cumRowSizes, const std::vector<int> & cumColSizes,
	int * x, int * y, int * width, int * height) const
{
	*x = cumColSizes[data.col];
	*y = cumRowSizes[data.row];
	*width = cumColSizes[data.col + data.colSpan] - *x;
	*height = cumRowSizes[data.row + data.rowSpan] - *y;
}

void TextTable::createTextCells()
{	
	std::set<TextCellData> newTextCells;	

	for (int i = 0; i < numRows(); ++i)
	{
		for (int j = 0; j < numCols(); ++j)
		{
			int startRow, startCol, rowSpan, colSpan;
			getSpansForGridCell(cellMerges_, i, j, &startRow, &startCol, &rowSpan, &colSpan);

			TextCellData findData;
			findData.row = startRow;
			findData.col = startCol;

			if (newTextCells.find(findData) != newTextCells.end()) continue;
			TextCellData data = newTextCell(startRow, startCol, rowSpan, colSpan);
			newTextCells.insert(data);
		}
	}
	
	bool wasPrevious = !prevCumColSizes_.empty();

	if (wasPrevious)
	{
		set<Text_2 *> alreadyCopied;
		BOOST_FOREACH(const TextCellData & newTextCell, newTextCells)
		{
			bool stringCopied = false;

			int oldCol = 0;
			while(prevCumColSizes_[oldCol + 1] <= cumColSizes_[newTextCell.col]) ++oldCol;
			int oldRow = 0;
			while(prevCumRowSizes_[oldRow + 1] <= cumRowSizes_[newTextCell.row]) ++oldRow;

			int startRow, startCol, rowSpan, colSpan;
			getSpansForGridCell(prevCellMerges_, oldRow, oldCol, 
				&startRow, &startCol, &rowSpan, &colSpan);

			TextCellData findCell;
			findCell.row = startRow;
			findCell.col = startCol;

			set<TextCellData>::iterator iter = textCells_.find(findCell);
			assert(iter != textCells_.end());

			const TextCellData & oldData = *iter;


			int oldX, oldY, oldWidth, oldHeight;
			int newX, newY, newWidth, newHeight;
			textCellDims(newTextCell,
				cumRowSizes_, cumColSizes_, &newX, &newY, &newWidth, &newHeight);

			textCellDims(oldData, prevCumRowSizes_, prevCumColSizes_, 
				&oldX, &oldY, &oldWidth, &oldHeight);

			if (oldX == newX && oldY == newY && oldWidth == newWidth && oldHeight == newHeight)
			{
				//const_cast is ok because TextCellData operator < doesn't care 
				//about .text field.
				const_cast<TextCellData &>(newTextCell).text = oldData.text;
				alreadyCopied.insert(oldData.text.get());
			}
			else
			{			
				newTextCell.text->setProperties(oldData.text->properties());

				if (alreadyCopied.find(oldData.text.get()) == alreadyCopied.end())
				{
					const wstring & newText = newTextCell.text->textString();
					const wstring & oldText = oldData.text->textString();
					wstring newString = newText;
					if (newString.empty())
						newString = oldText;
					else
						newString.insert(--newString.end(), oldText.begin(), oldText.end());

					newTextCell.text->setTextString(newString);
					alreadyCopied.insert(oldData.text.get());
				}
			}
		}
	}

	textCells_ = newTextCells;
	prevCumRowSizes_ = cumRowSizes_;
	prevCumColSizes_= cumColSizes_;
	prevCellMerges_ = cellMerges_;

	if (isInit()) init(gl_);

}

TextTable::TextCellData TextTable::newTextCell(int row, int col, int rowSpan, int colSpan)
{
	TextCellData data;
	data.text.reset(new Text_2);
	
	wstring emptyStr;
	data.text->setTextString(emptyStr);

	setAsSubObject(data.text.get());
	data.row = row;
	data.col = col;
	data.rowSpan = rowSpan;
	data.colSpan = colSpan;

	int x = cumColSizes_[col];
	int y = cumRowSizes_[row];
	int width = cumColSizes_[col + colSpan] - x;
	int height = cumRowSizes_[row + rowSpan] - y;

	Transform transform;
	transform.setTranslation(Vector3((float)x + cellPadding_, (float)y + cellPadding_, 0.0f));	
	data.text->setTransform(transform);
	data.text->setBoundaryHeight(max((float)height - 2*cellPadding_, 1.0f));
	data.text->setBoundaryWidth(max((float)width - 2*cellPadding_, 1.0f));

	return data;	
}

void TextTable::init(GfxRenderer * gl, bool firstTime)
{
	SceneObject::init(gl, firstTime);
	BOOST_FOREACH(const TextCellData & data, textCells_)
	{
		data.text->init(gl, parentScene_, firstTime);
	}
}

void TextTable::uninit()
{
	SceneObject::uninit();

	BOOST_FOREACH(const TextCellData & data, textCells_)
	{
		data.text->uninit();
	}

	drawVerts_.clear();
}

void TextTable::setParentScene(Scene * parentScene)
{
	SceneObject::setParentScene(parentScene);

	BOOST_FOREACH(const TextCellData & data, textCells_)
	{
		data.text->setParentScene(parentScene);
	}
}

void TextTable::drawObject(GfxRenderer * gl) const
{
	gl->useColorProgram();
	gl->setColorProgramColor(0.0f, 0.0f, 0.0f, 1.0f);

	drawVerts_.clear();

	BOOST_FOREACH(const TextCellData & data, textCells_)
	{
		float x = (float)cumColSizes_[data.col];
		float y = (float)cumRowSizes_[data.row];
		float w = (float)cumColSizes_[data.col + data.colSpan] - x;
		float h = (float)cumRowSizes_[data.row + data.rowSpan] - y;

		drawVerts_.push_back(Vector3(x, y, 0.0f));
		drawVerts_.push_back(Vector3(x + w, y, 0.0f));
		
		drawVerts_.push_back(Vector3(x + w, y, 0.0f));
		drawVerts_.push_back(Vector3(x + w, y + h, 0.0f));

		drawVerts_.push_back(Vector3(x + w, y + h, 0.0f));
		drawVerts_.push_back(Vector3(x, y + h, 0.0f));

		drawVerts_.push_back(Vector3(x, y + h, 0.0f));
		drawVerts_.push_back(Vector3(x, y, 0.0f));
	}
	
	gl->enableVertexAttribArrayPosition();
	gl->bindArrayBuffer(0);
	gl->vertexAttribPositionPointer(0, (char *)&drawVerts_[0]);	
	gl->applyCurrentShaderMatrix();
	glDrawArrays(GL_LINES, 0, drawVerts_.size());

	BOOST_FOREACH(const TextCellData & data, textCells_)
		data.text->draw(gl);	
}

void TextTable::setFont(const TextFont & font)
{
	BOOST_FOREACH(const TextCellData & data, textCells_)
	{
		std::vector<TextProperties_2> props = data.text->properties();
		props.front().font = font;
		data.text->setProperties(props);
	}
}

class EraseCandComparer
{
public:
	bool operator ()(
		const TextTable::CandidateLine & lhs, const TextTable::CandidateLine & rhs) const
	{
		if (lhs.type != rhs.type) return lhs.type < rhs.type;
		if (lhs.pos != rhs.pos) return lhs.pos < rhs.pos;

		int lhsEnd = lhs.startGridIndex + lhs.gridSpan;
		int rhsEnd = rhs.startGridIndex + rhs.gridSpan;
		if (spanIntersect(lhs.startGridIndex, lhsEnd, rhs.startGridIndex, rhsEnd))
			return false;
		
		if (lhs.startGridIndex != rhs.startGridIndex)
			return lhs.startGridIndex < rhs.startGridIndex;		

		return lhs.gridSpan < rhs.gridSpan;
	}

private:
	bool spanIntersect(int a1, int a2, int b1, int b2) const
	{
		return 
			a1 == b1 || a2 == b2 ||
			(a1 < b1 && a2 > b1) || 
			(a1 < b2 && a2 > b2) ||
			(a1 > b1 && a2 < b2);
	}
};

std::vector<TextTable::CandidateLine> 
TextTable::getEraseCandidateLines(const Vector2 & localPos, int snappingDist)
{
	vector<CandidateLine> candLines;

	set<CandidateLine, EraseCandComparer> candLinesTemp;

	int closestCol = -1, closestRow = -1;
	float closestColDist = FLT_MAX, closestRowDist = FLT_MAX;

	for (int i = 0; i < numRows(); ++i)
	{
		float dist = fabs(localPos.y - cumRowSizes_[i]);
		if (dist < closestRowDist)
		{
			closestRow = i;
			closestRowDist = dist;
		}
	}

	for (int i = 0; i < numCols(); ++i)
	{
		float dist = fabs(localPos.x - cumColSizes_[i]);
		if (dist < closestColDist)
		{
			closestCol = i;
			closestColDist = dist;
		}
	}

	bool colCloseEnough = closestColDist <= snappingDist;
	bool rowCloseEnough = closestRowDist <= snappingDist;
	colCloseEnough &= closestCol > 0 && closestCol < numCols();
	rowCloseEnough &= closestRow > 0 && closestRow < numRows();	

	BOOST_FOREACH(const TextCellData & data, textCells_)
	{
		set<CandidateLine, EraseCandComparer>::iterator iter;		
		
		if (rowCloseEnough && 
			(data.row == closestRow || data.row + data.rowSpan == closestRow))
		{
			CandidateLine newLine(CandidateLine::Horizontal, 
				cumRowSizes_[closestRow], data.col, data.colSpan);

			iter = candLinesTemp.find(newLine);
			
			if (iter != candLinesTemp.end())
			{				
				CandidateLine oldLine = *iter;				
				candLinesTemp.erase(iter);
				oldLine.unite(newLine);
				candLinesTemp.insert(oldLine);
			}
			else
				candLinesTemp.insert(newLine);		
		}

		if (colCloseEnough && 
			(data.col == closestCol || data.col + data.colSpan == closestCol))		
		{
			CandidateLine newLine(CandidateLine::Vertical, 
				cumColSizes_[closestCol], data.row, data.rowSpan);

			iter = candLinesTemp.find(newLine);
			
			if (iter != candLinesTemp.end())
			{
				CandidateLine oldLine = *iter;
				candLinesTemp.erase(iter);
				oldLine.unite(newLine);
				candLinesTemp.insert(oldLine);
			}
			else
				candLinesTemp.insert(newLine);
		}	
		
	}

	candLines.insert(candLines.begin(), candLinesTemp.begin(), candLinesTemp.end());
	return candLines;
}

std::vector<TextTable::CandidateLine> 
TextTable::getDrawCandidateLines(const Vector2 & localPos, int snappingDist)
{
	vector<CandidateLine> candLines;

	//horizontal lines
	bool shouldSnap = false;
	
	float minDist = FLT_MAX;
	int minDistIndex = -1;

	for (int i = 0; i < (int)cumRowSizes_.size(); ++i)
	{
		float dist = fabs(cumRowSizes_[i] - localPos.y);
		if (dist < snappingDist && dist < minDist)
		{
			minDist = dist;
			minDistIndex = i;
		}
	}

	int y = (int)localPos.y;

	if (minDistIndex >= 0) y = cumRowSizes_[minDistIndex];	
	
	BOOST_FOREACH(const TextCellData & data, textCells_)
	{
		const Transform & transform = data.text->transform();
		float left = transform.translation().x;
		float top = transform.translation().y;
		float right = data.text->boundaryWidth() + left;
		float bottom = data.text->boundaryHeight() + top;

		if (top < y && y < bottom)
		{
			candLines.push_back(CandidateLine(CandidateLine::Horizontal,
				y, data.col, data.colSpan));
		}
	}	

	//vertical lines
	shouldSnap = false;
	
	minDist = FLT_MAX;
	minDistIndex = -1;

	for (int i = 0; i < (int)cumColSizes_.size(); ++i)
	{
		float dist = fabs(cumColSizes_[i] - localPos.x);
		if (dist < snappingDist && dist < minDist)
		{
			minDist = dist;
			minDistIndex = i;
		}
	}

	int x = (int)localPos.x;

	if (minDistIndex >= 0) x = cumColSizes_[minDistIndex];	
	
	BOOST_FOREACH(const TextCellData & data, textCells_)
	{
		const Transform & transform = data.text->transform();
		float left = transform.translation().x;
		float top = transform.translation().y;
		float right = data.text->boundaryWidth() + left;
		float bottom = data.text->boundaryHeight() + top;

		if (left < x && x < right)
		{
			candLines.push_back(CandidateLine(CandidateLine::Vertical,
				x, data.row, data.rowSpan));
		}
	}	

	return candLines;
}

int TextTable::getHorzGridLineIndex(const Vector2 & pos, int * distance) const
{
	int minDist = INT_MAX;
	int minDistIndex = -1;

	TextCellData data;
	bool ret = getTextCellData(pos, &data);	
	
	if (ret)
	{
		int dist1 = abs(cumRowSizes_[data.row] - pos.y);
		int dist2 = abs(cumRowSizes_[data.row + data.rowSpan] - pos.y);
		if (dist1 < dist2)
		{
			minDist = dist1;
			minDistIndex = data.row;
		}
		else
		{
			minDist = dist2;
			minDistIndex = data.row + data.rowSpan;
		}
	}
	else
	{
		for (int i = 0; i < (int)cumRowSizes_.size(); ++i)
		{
			int dist = abs((int)cumRowSizes_[i] - pos.y);
			if (dist < minDist)
			{
				minDist = dist;
				minDistIndex = i;
			}
		}
	}

	*distance = minDist;
	return minDistIndex;	
}

int TextTable::getVertGridLineIndex(const Vector2 & pos, int * distance) const
{
	int minDist = INT_MAX;
	int minDistIndex = -1;

	TextCellData data;
	bool ret = getTextCellData(pos, &data);	
	
	if (ret)
	{
		int dist1 = abs(cumColSizes_[data.col] - pos.x);
		int dist2 = abs(cumColSizes_[data.col + data.colSpan] - pos.x);
		if (dist1 < dist2)
		{
			minDist = dist1;
			minDistIndex = data.col;
		}
		else
		{
			minDist = dist2;
			minDistIndex = data.col + data.colSpan;
		}
	}
	else
	{
		for (int i = 0; i < (int)cumColSizes_.size(); ++i)
		{
			int dist = abs((int)cumColSizes_[i] - pos.x);
			if (dist < minDist)
			{
				minDist = dist;
				minDistIndex = i;
			}
		}
	}

	*distance = minDist;
	return minDistIndex;
}

void TextTable::setVertGridLinePos(int index, int pos)
{
	cumColSizes_[index] = pos;	
	if (index > 0)
	{
		int adjustDelta = 0;
		if (cumColSizes_[index - 1] > cumColSizes_[index] - sMinCellDim_)
		{
			adjustDelta = (cumColSizes_[index] - sMinCellDim_) - 
				cumColSizes_[index - 1];
		}

		for (int i = index - 1; i >= 0; --i)
			cumColSizes_[i] += adjustDelta;
	}
	
	if (index < (int)cumColSizes_.size() - 1)
	{
		int adjustDelta = 0;
		if (cumColSizes_[index + 1] < cumColSizes_[index] + sMinCellDim_)
		{
			adjustDelta = (cumColSizes_[index] + sMinCellDim_) - 
				cumColSizes_[index + 1];
		}

		for (int i = index + 1; i < (int)cumColSizes_.size(); ++i)
			cumColSizes_[i] += adjustDelta;
	}	

	computeRowColSizes();
	resizeTextCells();
}

void TextTable::setHorzGridLinePos(int index, int pos)
{
	cumRowSizes_[index] = pos;
	
	if (index > 0)
	{
		int adjustDelta = 0;
		if (cumRowSizes_[index - 1] > cumRowSizes_[index] - sMinCellDim_)
		{
			adjustDelta = (cumRowSizes_[index] - sMinCellDim_) - 
				cumRowSizes_[index - 1];
		}

		for (int i = index - 1; i >= 0; --i)
			cumRowSizes_[i] += adjustDelta;
	}

	if (index < (int)cumColSizes_.size() - 1)
	{
		int adjustDelta = 0;
		if (cumRowSizes_[index + 1] < cumRowSizes_[index] + sMinCellDim_)
		{
			adjustDelta = (cumRowSizes_[index] + sMinCellDim_) - 
				cumRowSizes_[index + 1];
		}

		for (int i = index + 1; i < (int)cumRowSizes_.size(); ++i)
			cumRowSizes_[i] += adjustDelta;
	}	

	computeRowColSizes();
	resizeTextCells();
}

void TextTable::computeRowColSizes()
{
	gridStartX_ = cumColSizes_[0];
	gridStartY_ = cumRowSizes_[0];

	for (int i = 1; i < (int)cumColSizes_.size(); ++i)
		colSizes_[i - 1] = cumColSizes_[i] - cumColSizes_[i - 1];

	for (int i = 1; i < (int)cumRowSizes_.size(); ++i)
		rowSizes_[i - 1] = cumRowSizes_[i] - cumRowSizes_[i - 1];
}

int TextTable::getGridLineIndex(const CandidateLine & line) const
{
	int cutIndex = -1;
	const std::vector<int> * cumSizes;
	const std::vector<int> * sizes;	
	
	if (line.type == CandidateLine::Vertical)
	{
		cumSizes = &cumColSizes_;
		sizes = &colSizes_;
	}
	else
	{
		cumSizes = &cumRowSizes_;
		sizes = &rowSizes_;
	}

	bool fallsOnGridLine = false;
	for (int i = 0; i < (int)cumSizes->size(); ++i)
	{	
		cutIndex = i;
		if (cumSizes->at(i) == line.pos) 
		{
			fallsOnGridLine = true;
			break;
		}
		if (cumSizes->at(i) > line.pos) break;		
	}

	if (!fallsOnGridLine) return -1;
	return cutIndex;
}


bool TextTable::eraseAlongLine(const CandidateLine & line)
{
	int cutIndex = getGridLineIndex(line);

	if (cutIndex <= 0) return false;

	if (line.type == CandidateLine::Vertical)
	{
		if (cutIndex == (int)numCols()) return false;
		for (int i = line.startGridIndex; i < line.startGridIndex + line.gridSpan; ++i)	
			addCellMerge(i, cutIndex - 1, CellMerge::Horizontal);	
	}
	else
	{
		if (cutIndex == (int)numRows()) return false;
		for (int i = line.startGridIndex; i < line.startGridIndex + line.gridSpan; ++i)	
			addCellMerge(cutIndex - 1, i, CellMerge::Vertical);
	}
	
	return true;
}

bool TextTable::simplifyHorzGridLine(int gridLineIndex)
{
	if (gridLineIndex == 0 || gridLineIndex == numRows()) return false;		
	bool canSimplify = true;
	for (int i = 0; i < numCols(); ++i)
	{
		set<CellMerge>::iterator iter = cellMerges_.find(
			CellMerge(gridLineIndex - 1, i, CellMerge::Vertical));
		if (iter == cellMerges_.end())
		{
			canSimplify = false; 
			break;
		}
	}
	if (!canSimplify) return false;
	for (int i = 0; i < numCols(); ++i)
	{
		set<CellMerge>::iterator iter = cellMerges_.find(
			CellMerge(gridLineIndex - 1, i, CellMerge::Vertical));
		cellMerges_.erase(iter);
		iter = cellMerges_.find(
			CellMerge(gridLineIndex, i, CellMerge::Horizontal));
		if (iter != cellMerges_.end()) cellMerges_.erase(iter);
	}

	
	std::vector<CellMerge> cellMergesTemp(cellMerges_.begin(), cellMerges_.end());
	BOOST_FOREACH(CellMerge & cellMerge, cellMergesTemp)		
		if (cellMerge.row >= gridLineIndex) --cellMerge.row;
	cellMerges_ = set<CellMerge>(cellMergesTemp.begin(), cellMergesTemp.end());

	//modify the grid itself	
	vector<int>::iterator iter = rowSizes_.begin();
	iter += gridLineIndex - 1;
	int newSize = rowSizes_[gridLineIndex - 1] + rowSizes_[gridLineIndex];
	iter = rowSizes_.erase(iter);
	*iter = newSize;			
	computeCumRowColSizes();	
	return true;
}

bool TextTable::simplifyVertGridLine(int gridLineIndex)
{
	if (gridLineIndex == 0 || gridLineIndex == numCols()) return false;
	bool canSimplify = true;
	for (int i = 0; i < numRows(); ++i)
	{
		set<CellMerge>::iterator iter = cellMerges_.find(
			CellMerge(i, gridLineIndex - 1, CellMerge::Horizontal));
		if (iter == cellMerges_.end())
		{
			canSimplify = false; 
			break;
		}
	}
	if (!canSimplify) return false;
	for (int i = 0; i < numRows(); ++i)
	{
		set<CellMerge>::iterator iter = cellMerges_.find(
			CellMerge(i, gridLineIndex - 1, CellMerge::Horizontal));
		cellMerges_.erase(iter);
		iter = cellMerges_.find(
			CellMerge(i, gridLineIndex, CellMerge::Vertical));
		if (iter != cellMerges_.end()) cellMerges_.erase(iter);
	}
	std::vector<CellMerge> cellMergesTemp(cellMerges_.begin(), cellMerges_.end());
	BOOST_FOREACH(CellMerge & cellMerge, cellMergesTemp)	
		if (cellMerge.col >= gridLineIndex) --cellMerge.col;
	cellMerges_ = set<CellMerge>(cellMergesTemp.begin(), cellMergesTemp.end());
	//modify the grid itself	
	vector<int>::iterator iter = colSizes_.begin();
	iter += gridLineIndex - 1;
	int newSize = colSizes_[gridLineIndex - 1] + colSizes_[gridLineIndex];
	iter = colSizes_.erase(iter);
	*iter = newSize;			
	computeCumRowColSizes();	
	return true;
}


int TextTable::drawAlongLine(const CandidateLine & line)
{
	int cutIndex = -1;
	std::vector<int> * cumSizes;
	std::vector<int> * sizes;	
	
	if (line.type == CandidateLine::Vertical)
	{
		cumSizes = &cumColSizes_;
		sizes = &colSizes_;
	}
	else
	{
		cumSizes = &cumRowSizes_;
		sizes = &rowSizes_;
	}

	bool fallsOnGridLine = false;
	for (int i = 0; i < (int)cumSizes->size(); ++i)
	{		
		if (cumSizes->at(i) == line.pos) 
		{
			fallsOnGridLine = true;
			break;
		}
		if (cumSizes->at(i) > line.pos) break;
		cutIndex = i;
	}

	if (cutIndex < 0 || cutIndex >= (int)sizes->size()) return -1;	

	if (!fallsOnGridLine) //a new grid line was added
	{
		int newSize1 = line.pos - cumSizes->at(cutIndex);
		int newSize2 = cumSizes->at(cutIndex + 1) - line.pos;

		sizes->at(cutIndex) = newSize1;

		vector<int>::iterator iter = sizes->begin();
		iter += cutIndex + 1;
		sizes->insert(iter, newSize2);	
		computeCumRowColSizes();

		vector<CellMerge> newCellMerges;
		vector<CellMerge> cellMergesTemp(cellMerges_.begin(), cellMerges_.end());

		if (line.type == CandidateLine::Vertical)
		{
			BOOST_FOREACH(CellMerge & cellMerge, cellMergesTemp)
			{		
				if (cellMerge.type == CellMerge::Vertical && cellMerge.col == cutIndex)	
					newCellMerges.push_back(CellMerge(
							cellMerge.row, cellMerge.col + 1, cellMerge.type));

				else if (cellMerge.type == CellMerge::Horizontal && cellMerge.col == cutIndex)			
					++cellMerge.col;

				else if (cellMerge.col > cutIndex) 
					++cellMerge.col;			
			}
			cellMerges_ = set<CellMerge>(cellMergesTemp.begin(), cellMergesTemp.end());

			BOOST_FOREACH(const CellMerge & newCellMerge, newCellMerges)
				addCellMerge(newCellMerge.row, newCellMerge.col, newCellMerge.type);

			for (int i = 0; i < line.startGridIndex; ++i)	
				addCellMerge(i, cutIndex, CellMerge::Horizontal);	

			for (int i = line.startGridIndex + line.gridSpan; i < numRows(); ++i)	
				addCellMerge(i, cutIndex, CellMerge::Horizontal);
		}
		else
		{
			BOOST_FOREACH(CellMerge & cellMerge, cellMergesTemp)
			{		
				if (cellMerge.type == CellMerge::Horizontal && cellMerge.row == cutIndex)			
					newCellMerges.push_back(CellMerge(
						cellMerge.row + 1, cellMerge.col, cellMerge.type));

				else if (cellMerge.type == CellMerge::Vertical && cellMerge.row == cutIndex)			
					++cellMerge.row;

				else if (cellMerge.row > cutIndex) 
					++cellMerge.row;			
			}
			cellMerges_ = set<CellMerge>(cellMergesTemp.begin(), cellMergesTemp.end());

			BOOST_FOREACH(const CellMerge & newCellMerge, newCellMerges)
				addCellMerge(newCellMerge.row, newCellMerge.col, newCellMerge.type);

			for (int i = 0; i < line.startGridIndex; ++i)	
				addCellMerge(cutIndex, i, CellMerge::Vertical);	

			for (int i = line.startGridIndex + line.gridSpan; i < numCols(); ++i)	
				addCellMerge(cutIndex, i, CellMerge::Vertical);
		}
	}
	else
	{
		vector<CellMerge>::iterator iter;
		vector<CellMerge> cellMergesTemp(cellMerges_.begin(), cellMerges_.end());

		if (line.type == CandidateLine::Vertical)
		{			
			for (iter = cellMergesTemp.begin(); iter != cellMergesTemp.end();)
			{
				const CellMerge & cellMerge = *iter;
				if (cellMerge.type == CellMerge::Horizontal && 
					cellMerge.col == cutIndex &&
					cellMerge.row >= line.startGridIndex &&
					cellMerge.row < line.startGridIndex + line.gridSpan)
				{
					 iter = cellMergesTemp.erase(iter);					 
				}
				else ++iter;
			}
		}
		else
		{
			for (iter = cellMergesTemp.begin(); iter != cellMergesTemp.end();)
			{
				const CellMerge & cellMerge = *iter;
				if (cellMerge.type == CellMerge::Vertical && 
					cellMerge.row == cutIndex &&
					cellMerge.col >= line.startGridIndex &&
					cellMerge.col < line.startGridIndex + line.gridSpan)
				{
					iter = cellMergesTemp.erase(iter);					
				}
				else ++iter;
			}
		}

		cellMerges_ = set<CellMerge>(cellMergesTemp.begin(), cellMergesTemp.end());
	}	

	return cutIndex;
}

bool TextTable::drawAlongLines(const std::vector<CandidateLine> & lines)
{
	bool changed = false;
	BOOST_FOREACH(const CandidateLine & line, lines)
		changed |= (drawAlongLine(line) >= 0);	

	if (!changed) return false;

	createTextCells();

	return true;
}

void TextTable::simplifyGrid()
{
	for (int i = 1; i < numRows(); ++i)
	{
		if (simplifyHorzGridLine(i)) --i;
	}

	for (int i = 1; i < numCols(); ++i)
	{
		if (simplifyVertGridLine(i)) --i;
	}

}

bool TextTable::eraseAlongLines(const std::vector<CandidateLine> & lines)
{
	bool changed = false;
	BOOST_FOREACH(const CandidateLine & line, lines)	
		changed |= eraseAlongLine(line);	
	if (!changed) return false;
	simplifyGrid();
	createTextCells();
	return true;
}

void TextTable::candLinePixelSpan(const CandidateLine & line, int * startPos, int * len) const
{
	if (line.type == CandidateLine::Vertical)
	{
		*startPos = cumRowSizes_[line.startGridIndex];
		*len = cumRowSizes_[line.startGridIndex + line.gridSpan] - *startPos;
	}
	else
	{
		*startPos = cumColSizes_[line.startGridIndex];
		*len = cumColSizes_[line.startGridIndex + line.gridSpan] - *startPos;
	}
}

Text_2 * TextTable::getTextObj(const Vector2 & pos) const
{
	TextCellData data;
	if (getTextCellData(pos, &data))
	{
		return data.text.get();
	}
	return NULL;	
}

bool TextTable::getTextCellData(const Vector2 & pos, TextTable::TextCellData * textCellData) const
{
	BOOST_FOREACH(const TextCellData & data, textCells_)
	{
		int left = cumColSizes_[data.col];
		int top = cumRowSizes_[data.row];
		int right = cumColSizes_[data.col + data.colSpan];
		int bottom = cumRowSizes_[data.row + data.rowSpan];

		if (left <= pos.x && pos.x < right && top <= pos.y && pos.y < bottom)
		{
			*textCellData = data;
			return true;
		}	
	}

	return false;
}

BoundingBox TextTable::extents() const
{
	float x = (float)gridStartX_;
	float y = (float)gridStartY_;
	return BoundingBox(
		Vector3(x, y, 0.0f), 
		Vector3(x + (float)width(), y + (float)height(), 0.0f));
}

SceneObject * TextTable::intersect(Vector3 * out, const Ray & ray)
{
	
	if (!visualAttrib_.isVisible()) return 0;
	Matrix trans = parentTransform() * visualAttrib_.transformMatrix();

	float h = (float)height();
	float w = (float)width();
	float x = (float)gridStartX_;
	float y = (float)gridStartY_;

	Vector3 a = trans * Vector3(x, y, 0);
	Vector3 b = trans * Vector3(x, y + h, 0);
	Vector3 c = trans * Vector3(x + w, y + h, 0);
	Vector3 d = trans * Vector3(x + w, y, 0);

	if (triangleIntersectRay(out, a, b, c, ray) ||
		triangleIntersectRay(out, a, c, d, ray))
	{
		return this;
	}
	else return 0;

}

TextTableState * TextTable::getState() const
{
	TextTableState * state = new TextTableState;
	state->rowSizes_ = rowSizes_;
	state->colSizes_ = colSizes_;
	state->cellMerges_ = cellMerges_;
	state->textCells_ = textCells_;

	return state;
}

void TextTable::setState(const TextTableState * state)
{
	rowSizes_ = state->rowSizes_;
	colSizes_ = state->colSizes_;
	cellMerges_ = state->cellMerges_;
	textCells_ = state->textCells_;
	computeCumRowColSizes();
	prevCumRowSizes_ = cumRowSizes_;
	prevCumColSizes_ = cumColSizes_;
	prevCellMerges_ = cellMerges_;

	if (isInit())init(gl_);
}

TextTableGridSizes * TextTable::getGridSizes() const
{
	TextTableGridSizes * gridSizes = new TextTableGridSizes;
	gridSizes->cumRowSizes_ = cumRowSizes_;
	gridSizes->cumColSizes_ = cumColSizes_;
	return gridSizes;
}

void TextTable::setGridSizes(const TextTableGridSizes * gridSizes)
{
	cumRowSizes_ = gridSizes->cumRowSizes_;
	cumColSizes_ = gridSizes->cumColSizes_;
	computeRowColSizes();
	resizeTextCells();
	prevCumRowSizes_ = cumRowSizes_;
	prevCumColSizes_ = cumColSizes_;
	prevCellMerges_ = cellMerges_;	
}

void TextTable::write(Writer & writer) const
{
	writer.writeParent<SceneObject>(this);
	writer.write(visualAttrib_, "visualAttrib");

	writer.write(border_);
	writer.write(cellPadding_);

	writer.write(gridStartX_);
	writer.write(gridStartY_);

	writer.write(rowSizes_);
	writer.write(colSizes_);

	writer.write(cellMerges_);

	writer.write(textCells_);
	
}

void TextTable::read(Reader & reader, unsigned char version)
{
	reader.readParent<SceneObject>(this);
	reader.read(visualAttrib_);
	
	reader.read(border_);
	reader.read(cellPadding_);

	reader.read(gridStartX_);
	reader.read(gridStartY_);

	reader.read(rowSizes_);
	reader.read(colSizes_);

	reader.read(cellMerges_);

	reader.read(textCells_);

	BOOST_FOREACH(const TextCellData & data, textCells_)
	{
		setAsSubObject(data.text.get());
	}

	computeCumRowColSizes();
	//createTextCells();
	prevCumRowSizes_ = cumRowSizes_;
	prevCumColSizes_= cumColSizes_;
	prevCellMerges_ = cellMerges_;

}

void TextTable::writeXml(XmlWriter & w) const
{
	SceneObject::writeXml(w);
	w.startTag("VisualAttrib");
	visualAttrib_.writeXml(w);
	w.endTag();

	w.writeTag("Border", border_);
	w.writeTag("CellPadding", cellPadding_);
	w.writeTag("GridStartX", gridStartX_);
	w.writeTag("GridStartY", gridStartY_);
	for (unsigned int i = 0; i < (unsigned int)rowSizes_.size(); ++i)
		w.writeTag("RowSize", rowSizes_[i]);
	for (unsigned int i = 0; i < (unsigned int)colSizes_.size(); ++i)
		w.writeTag("ColSize", colSizes_[i]);
	std::set<CellMerge>::const_iterator cit;
	for (cit = cellMerges_.begin() ; cit != cellMerges_.end() ; ++cit)
	{
		w.startTag("CellMerge");
		w.writeTag("Type", (unsigned int)cit->type);
		w.writeTag("Row", cit->row);
		w.writeTag("Col", cit->col);
		w.endTag();
	}
	std::set<TextCellData>::const_iterator tit;
	for (tit = textCells_.begin() ; tit != textCells_.end() ; ++tit)
	{
		w.startTag("TextCellData");
		tit->writeXml(w);
		w.endTag();
	}
}

void TextTable::readXml(XmlReader & r, xmlNode * parent)
{
	SceneObject::readXml(r, parent);
	int temp = 0;
	std::string filename;
	textCells_.clear();
	cellMerges_.clear();

	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{
		if (curNode->type != XML_ELEMENT_NODE) continue;		
		if (r.isNodeName(curNode, "VisualAttrib"))
		{
			visualAttrib_.readXml(r, curNode);
			curNode = curNode->next;
		}
		else if (r.getNodeContentIfName(border_, curNode, "Border"));
		else if (r.getNodeContentIfName(cellPadding_, curNode, "CellPadding"));
		else if (r.getNodeContentIfName(gridStartX_, curNode, "GridStartX"));
		else if (r.getNodeContentIfName(gridStartY_, curNode, "GridStartY"));
		else if (r.getNodeContentIfName(temp, curNode, "RowSize")) rowSizes_.push_back(temp);
		else if (r.getNodeContentIfName(temp, curNode, "ColSize")) colSizes_.push_back(temp);
		else if (r.isNodeName(curNode, "CellMerge"))
		{
			xmlNode * child = curNode->children;
			CellMerge cellmerge;
			for( ; child ; child = child->next)
			{
				if(r.getNodeContentIfName(temp, child, "Type"))
					cellmerge.type = (CellMerge::Type)temp;
				else if(r.getNodeContentIfName(temp, child, "Row"))
					cellmerge.row = temp;
				else if(r.getNodeContentIfName(temp, child, "Col"))
					cellmerge.col = temp;
			}
			cellMerges_.insert(cellmerge);
			curNode = curNode->next;
		}
		else if (r.isNodeName(curNode, "TextCellData"))
		{
			TextCellData tcdata;
			tcdata.readXml(r, curNode);
			textCells_.insert(tcdata);
			curNode = curNode->next;
		}
	}
}