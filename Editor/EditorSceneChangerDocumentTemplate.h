#pragma once
#include "EditorDocumentTemplate.h"

class Document;
class Command;
class EditorSceneChangerDocumentTemplate : public EditorDocumentTemplate
{
public:
	virtual EditorDocumentTemplate * clone() const
	{
		return new EditorSceneChangerDocumentTemplate(*this);
	}

	virtual boost::uuids::uuid templateType() const;

	/**
	@return Command to apply property changes, or 0 if cancelled. The commnad
	will be newly allocated memory
	*/
	virtual Command * doProperties(
		DocumentTemplate * docTemplate, 
		EditorDocument * document, QWidget * parent);
private:
};