#include "stdafx.h"
#include "EditorMagazineDocumentTemplate.h"
#include "MagazineDocumentTemplate.h"
#include "ArticleManagerDlg.h"
#include "Command.h"
#include "EditorDocument.h"

boost::uuids::uuid EditorMagazineDocumentTemplate::templateType() const
{
	return MagazineDocumentTemplate().type();
}

Command * EditorMagazineDocumentTemplate::doProperties(
	DocumentTemplate * docTemplate,
	EditorDocument * document, QWidget * parent)
{
	
	MagazineDocumentTemplate * typedDocTemplate = 
		(MagazineDocumentTemplate *)docTemplate;

	ArticleManagerDlg dlg(parent, 
		document, typedDocTemplate);
	
	if (dlg.exec())
	{
		return new ChangeArticlesCmd(
			document->document(), typedDocTemplate, dlg.articles(), dlg.uiImgFiles(),
			dlg.resetArticleFirstPage(), dlg.bookmarks(), dlg.aspect(), dlg.homeButtonFunction(),
			dlg.doToContents(), dlg.toContentsIndex(), dlg.transitionMode());
	}
	else
	{
		return 0;
	}	
}