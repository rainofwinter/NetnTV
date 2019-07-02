#include "stdafx.h"
#include "EditorDocument.h"
#include "EditorSingleSceneDocumentTemplate.h"
#include "SingleSceneDocumentTemplate.h"
#include "SingleSceneDocumentDlg.h"
#include "Command.h"

boost::uuids::uuid EditorSingleSceneDocumentTemplate::templateType() const
{
	return SingleSceneDocumentTemplate().type();
}

Command * EditorSingleSceneDocumentTemplate::doProperties(
	DocumentTemplate * docTemplate,
	EditorDocument * document, QWidget * parent)
{	
	SingleSceneDocumentTemplate * typedDocTemplate = 
		(SingleSceneDocumentTemplate *)docTemplate;

	SingleSceneDocumentDlg dlg(document->document(), typedDocTemplate, parent);
	
	if (dlg.exec())
	{
		return new ChangeSingleSceneDocumentCmd(document->document(),
			typedDocTemplate, dlg.scene());
	}
	else
	{
		return 0;
	}	
}