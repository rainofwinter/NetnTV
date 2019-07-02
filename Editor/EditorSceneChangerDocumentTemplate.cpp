#include "stdafx.h"
#include "EditorDocument.h"
#include "EditorSceneChangerDocumentTemplate.h"
#include "SceneChangerDocumentTemplate.h"
#include "SceneChangerDlg.h"
#include "Command.h"

boost::uuids::uuid EditorSceneChangerDocumentTemplate::templateType() const
{
	return  SceneChangerDocumentTemplate().type();
}

Command * EditorSceneChangerDocumentTemplate::doProperties(
	DocumentTemplate * docTemplate,
	EditorDocument * document, QWidget * parent)
{	
	SceneChangerDocumentTemplate * typedDocTemplate = 
		(SceneChangerDocumentTemplate *)docTemplate;

	SceneChangerDlg dlg(parent, 
		document, 0, typedDocTemplate->scenes(), typedDocTemplate->allowDrag());
	
	if (dlg.exec())
	{
		return new ChangeSceneChangerDocumentCmd(
			document->document(), typedDocTemplate, dlg.scenes(), dlg.allowDragging());
	}
	else
	{
		return 0;
	}	
}