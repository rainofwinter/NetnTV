#pragma once
#include "SceneObject.h"
#include "Attrib.h"
///////////////////////////////////////////////////////////////////////////////
class TextFont;
class TextTableState;
class TextTableGridSizes;

class TextTable : public SceneObject
{
public:
	struct CellMerge
	{
		enum Type {Horizontal = 0, Vertical} type;
		int row;
		int col;

		CellMerge() {}
		CellMerge(int row, int col, Type type);
		bool operator < (const CellMerge & rhs) const;			
	};

	struct TextCellData
	{
		boost::shared_ptr<Text_2> text;
		
		///topmost grid row
		int row;

		///leftmost grid column
		int col;

		int rowSpan;
		int colSpan;

		bool operator < (const TextCellData & rhs) const
		{
			if (row != rhs.row) return row < rhs.row;
			return col < rhs.col;
		}

		unsigned char version() const {return 1;}
		void write(Writer & writer) const;
		void read(Reader & reader, unsigned char);
		const char * xmlTag() const {return "TextCellData";}
		void writeXml(XmlWriter & w) const;
		void readXml(XmlReader & r, xmlNode * node);
	};

	struct CandidateLine
	{
		int pos;		
		int startGridIndex;
		int gridSpan;
		
		enum Type
		{
			Horizontal,
			Vertical
		} type;

		void unite(const CandidateLine & rhs);
		
		CandidateLine() {}
		CandidateLine(Type type, int pos, int startGridIndex, int gridSpan);
		bool operator < (const CandidateLine & rhs) const;
	};

public:

	virtual const char * typeStr() const 
	{
		return "TextTable";
	}
	
	virtual boost::uuids::uuid type() const 
	{
		static boost::uuids::uuid uuid = 
			sUuidGen_("45EEEEEE-ECDC-B745-1A12-51D537798745");
		return uuid;
	}
	virtual SceneObject * clone(ElementMapping * elementMapping) const 
	{
		return new TextTable(*this, elementMapping);
	}

	virtual void remapReferences(const ElementMapping & mapping) 
	{SceneObject::remapReferences(mapping);}

	TextTable();	
	TextTable(const TextTable & rhs, ElementMapping * elementMapping);
	
	~TextTable();

	virtual void drawObject(GfxRenderer * gl) const;
		
	virtual void init(GfxRenderer * gl, bool firstTime = false);
	virtual void uninit();

	virtual VisualAttrib * visualAttrib() {return &visualAttrib_;}

	virtual BoundingBox extents() const;

	virtual unsigned char version() const {return 0;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);
	virtual const char * xmlTag() const {return "TextTable";}
	virtual void writeXml(XmlWriter & w) const;
	virtual void readXml(XmlReader & r, xmlNode * node);

	virtual SceneObject * intersect(Vector3 * intPt, const Ray & ray);

	void addCellMerge(int row, int col, CellMerge::Type type);

	void expandRegion();

	int numRows() const {return (int)rowSizes_.size();}
	int numCols() const {return (int)colSizes_.size();}

	int gridStartX() const {return gridStartX_;}
	int gridStartY() const {return gridStartY_;}

	int width() const;
	int height() const;

	void setFont(const TextFont & font);

	std::vector<CandidateLine> getDrawCandidateLines(
		const Vector2 & localPos, 
		int snappingDist);

	std::vector<CandidateLine> getEraseCandidateLines(
		const Vector2 & localPos, 
		int snappingDist);


	bool drawAlongLines(const std::vector<CandidateLine> & lines);
	bool eraseAlongLines(const std::vector<CandidateLine> & lines);
	void candLinePixelSpan(const CandidateLine & line, int * startPos, int * len) const;

	Text_2 * getTextObj(const Vector2 & pos) const;	
	bool getTextCellData(const Vector2 & pos, TextCellData * textCellData) const;

	void simplifyGrid();

	TextTableState * getState() const;
	void setState(const TextTableState * state);

	TextTableGridSizes * getGridSizes() const;
	void setGridSizes(const TextTableGridSizes * gridSizes);

	int getVertGridLineIndex(const Vector2 & pos, int * distance) const;
	int getHorzGridLineIndex(const Vector2 & pos, int * distance) const;

	void setVertGridLinePos(int index, int pos); 
	void setHorzGridLinePos(int index, int pos); 

private:
	void create();
	TextTable(const TextTable & rhs);
	TextTable & operator = (const TextTable & rhs);

	int drawAlongLine(const CandidateLine & line);
	bool eraseAlongLine(const CandidateLine & line);

	bool simplifyHorzGridLine(int gridLineIndex);
	bool simplifyVertGridLine(int gridLineIndex);
	
	int getGridLineIndex(const CandidateLine & line) const;

	void getCellMergeEnclosingRegion(int & row, int & col, int & rowSpan, int & colSpan);

	/**	
	Create textcells based on rowSizes_, colSizes_, and cellMerges_.
	Preserve text in previous text cells as much as possible.

	Populates textCells_.	
	*/
	void createTextCells();

	/**
	Resize text cells based on rowSizes_ and colSizes_
	Needs cumRowSizes_ and cumColSizes_ to be valid.
	*/
	void resizeTextCells();
	
	/**
	helper function for createTextCells
	*/
	TextCellData newTextCell(int row, int col, int rowSpan, int colSpan);

	void getSpansForGridCell(const std::set<CellMerge> & cellMerges, int rowInTextCell, int colInTextCell, 
		int * startRow, int * startCol, int * rowSpan, int * colSpan) const;

	/**
	Compute cumRowSizes based on rowSizes.
	Compute cumColSizes based on colSizes.
	*/
	void computeCumRowColSizes();
	/**
	Compute gridStartX_, gridStartY_ and row col sizes from cum row col sizes.
	*/
	void computeRowColSizes();
	
	void textCellDims(const TextCellData & data, 
		const std::vector<int> & cumRowSizes, const std::vector<int> & cumColSizes,
		int * x, int * y, int * width, int * height) const;

	virtual void setParentScene(Scene * parentScene);
	
private:
	VisualAttrib visualAttrib_;

	std::vector<int> rowSizes_;
	std::vector<int> colSizes_;	
	std::vector<int> cumRowSizes_;
	std::vector<int> cumColSizes_;

	std::set<CellMerge> cellMerges_;
	std::set<TextCellData> textCells_;

	mutable std::vector<Vector3> drawVerts_;

	///cumRowSizes_ before text cell structure changes during createTextCells()
	std::vector<int> prevCumRowSizes_;
	///cumColSizes_ before text cell structure changes during createTextCells()
	std::vector<int> prevCumColSizes_;

	std::set<CellMerge> prevCellMerges_;

	int gridStartX_;
	int gridStartY_;


	int cellPadding_;
	int border_;

	static int sMinCellDim_;
};

///////////////////////////////////////////////////////////////////////////////

typedef boost::shared_ptr<TextTableState> TextTableStateSPtr;

class TextTableState
{
friend class TextTable;
	
private:

	std::vector<int> rowSizes_;
	std::vector<int> colSizes_;
	std::set<TextTable::CellMerge> cellMerges_;
	std::set<TextTable::TextCellData> textCells_;
};

///////////////////////////////////////////////////////////////////////////////

typedef boost::shared_ptr<TextTableGridSizes> TextTableGridSizesSPtr;

class TextTableGridSizes
{
	friend class TextTable;
public:
	bool operator == (const TextTableGridSizes & rhs) const
	{
		return cumRowSizes_ == rhs.cumRowSizes_ &&
			cumColSizes_ == rhs.cumColSizes_;
	}
private:
	std::vector<int> cumRowSizes_;
	std::vector<int> cumColSizes_;
};