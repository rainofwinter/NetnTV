#pragma once
#include "DocumentTemplate.h"
class EditorScene;
class EventPropertyPage;
class EditorDocument;
class Command;

class EditorDocumentTemplate
{
public:	
	EditorDocumentTemplate() {docTemplate_ = 0;}
	virtual ~EditorDocumentTemplate() {}
	virtual EditorDocumentTemplate * clone() const = 0;

	/**
	@return Command to apply property changes, or 0 if cancelled. The commnad
	will be newly allocated memory. Note: the command must initialize the 
	template. The template may be in an uninitialized state when the command
	is issued.
	*/
	virtual Command * doProperties(		
		DocumentTemplate * docTemplate,		
		EditorDocument * document, QWidget * parent) = 0;

	virtual boost::uuids::uuid templateType() const = 0;

	void setDocumentTemplate(DocumentTemplate * docTemplate)
	{
		docTemplate_ = docTemplate;
	}

	DocumentTemplate * documentTemplate() const {return docTemplate_;}

	
protected:	
	DocumentTemplate * docTemplate_;	
};