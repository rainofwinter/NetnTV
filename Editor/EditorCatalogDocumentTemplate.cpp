#include "stdafx.h"
#include "EditorCatalogDocumentTemplate.h"
#include "CatalogDocumentTemplate.h"
#include "Command.h"
#include "EditorDocument.h"
#include "CatalogDlg.h"


boost::uuids::uuid EditorCatalogDocumentTemplate::templateType() const
{
	return CatalogDocumentTemplate().type();
}


Command * EditorCatalogDocumentTemplate::doProperties(
	DocumentTemplate * docTemplate,
	EditorDocument * document, QWidget * parent)
{
	
	CatalogDocumentTemplate * typedDocTemplate = 
		(CatalogDocumentTemplate *)docTemplate;

	CatalogDlg dlg(parent, 
		document, typedDocTemplate->scenes());
	
	if (dlg.exec())
	{
		return new ChangeCatalogDocumentCmd(
			document->document(), typedDocTemplate, dlg.scenes(), dlg.uiImgFiles(), dlg.thumbFileMap());
			//document->document(), typedDocTemplate, dlg.articles(), dlg.uiImgFiles(),
			//dlg.resetArticleFirstPage(), dlg.bookmarks(), dlg.aspect(), dlg.homeButtonFunction(),
			//dlg.doToContents(), dlg.toContentsIndex());
	}
	else
	{
		return 0;
	}	
}