#include "stdafx.h"
#include "MagazineArticle.h"
#include "ElementMapping.h"
#include "Reader.h"
#include "Writer.h"
#include "Xml.h"
#include "Exception.h"

using namespace std;

void MagazineArticle::remapReferences(const ElementMapping & mapping)
{
	vector<MagazinePageSPtr>::iterator iter = pages_.begin();
	for (; iter != pages_.end();)
	{
		Scene * mappedScene = mapping.mapScene((*iter)->scene());
		if (!mappedScene)
		{
			iter = pages_.erase(iter);
		}
		else
		{
			(*iter)->setScene(mappedScene);
			++iter;
		}
	}	
}

void MagazineArticle::write(Writer & writer) const
{
	writer.write(name_, "name");
	writer.write(pages_, "pages");	
}

void MagazineArticle::writeXml(XmlWriter & w) const
{
	w.writeTag("Name", name_);
	w.startTag("Pages");
	BOOST_FOREACH(MagazinePageSPtr page, pages_)
	{
		w.startTag("Page");
		page->writeXml(w);
		w.endTag();
	}
	w.endTag();
}

void MagazineArticle::readXml(XmlReader & r, xmlNode * parent)
{
	for(xmlNode * curNode = parent->children; curNode; curNode = curNode->next)
	{		
		if (curNode->type != XML_ELEMENT_NODE) continue;

		if (r.getNodeContentIfName(name_, curNode, "Name"));
		else if (r.isNodeName(curNode, "Pages"))
		{
			for(xmlNode * pageNode = curNode->children; pageNode; pageNode = pageNode->next)
			{		
				if (pageNode->type != XML_ELEMENT_NODE) continue;
				if (r.isNodeName(pageNode, "Page"))
				{					
					MagazinePageSPtr page(new MagazinePage);
					page->readXml(r, pageNode);
					pages_.push_back(page);
					pageNode = pageNode->next;				
				}
			}
		}
	}
}

void MagazineArticle::read(Reader & reader, unsigned char version)
{
	reader.read(name_);

	if (version < 3)
	{
		throw Exception("Unsupported version");
		/*
		vector<Scene *> pageScenes;
		reader.read(pageScenes);
		vector<string> thumbnailFiles;
		reader.read(thumbnailFiles);

		pages_.clear();
		for (int i = 0; i < (int)pageScenes.size(); ++i)
		{
			pages_.push_back(MagazinePageSPtr(
				new Page(pageScenes[i], thumbnailFiles[i])));
		}
		*/
	}
	else
	{
		reader.read(pages_);
	}

}