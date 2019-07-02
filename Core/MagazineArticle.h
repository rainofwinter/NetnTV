#pragma once

#include "MagazinePage.h"

class Scene;
class ElementMapping;

class MagazineArticle
{
public:
	MagazineArticle()
	{
		this->name_ = "Article";
		this->curPage_ = 0;
	}

	MagazineArticle(const MagazineArticle & rhs)
	{
		name_ = rhs.name_;
		pages_.reserve(rhs.pages_.size());
		for (int i = 0; i < (int)rhs.pages_.size(); ++i)
		{
			MagazinePage * newPage = new MagazinePage(*rhs.pages_[i]);
			pages_.push_back(MagazinePageSPtr(newPage));
		}
		curPage_ = rhs.curPage_;
	}

	bool empty() const {return pages_.empty();}

	~MagazineArticle() {}

	void setCurPage(int pageIndex) {curPage_ = pageIndex;}

	Scene * curPagePtr() const 
	{
		if (pages_.empty()) return 0;
		return pages_[curPage_]->scene();
	}

	void addPage(const MagazinePageSPtr & page) {pages_.push_back(page);}
	void deletePages() {pages_.clear();}
	int curPage() const {return curPage_;}
	int numPages() const {return (int)pages_.size();}

	void remapReferences(const ElementMapping & mapping);
	
	void setName(const std::string & name) {name_ = name;}
	const std::string & name() const {return name_;}
	const std::vector<MagazinePageSPtr> & pages() const {return pages_;}
	MagazinePage * page(int i) const 
	{
		if (i < 0 || i >= (int)pages_.size()) return NULL;
		return pages_[i].get();
	}

	virtual unsigned char version() const {return 4;}
	virtual void write(Writer & writer) const;
	virtual void read(Reader & reader, unsigned char);

	void writeXml(XmlWriter & w) const;
	void readXml(XmlReader & r, xmlNode * parent);

private:
	std::string name_;
	std::vector<MagazinePageSPtr> pages_;
	int curPage_;

	MagazineArticle & operator = (const MagazineArticle & rhs);
};

typedef boost::shared_ptr<MagazineArticle> MagazineArticleSPtr;