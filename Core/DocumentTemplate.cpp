#include "stdafx.h"
#include "DocumentTemplate.h"
#include "SceneObject.h"
#include "ScriptProcessor.h"

using namespace std;

JSClass jsDocumentTemplateClass = InitClass(
	"DocumentTemplate", JSCLASS_HAS_PRIVATE | JSCLASS_MARK_IS_TRACE, JS_PropertyStub, 
	JS_StrictPropertyStub, 0, 0);

JSFunctionSpec DocumentTemplateFuncs[] = {

    JS_FS_END
};



///////////////////////////////////////////////////////////////////////////////

JSObject * DocumentTemplate::createBaseScriptObjectProto(ScriptProcessor * s)
{			
	JSObject * jsSceneObjectProto = JS_InitClass(
		s->jsContext(), s->jsGlobal(), 0, &jsDocumentTemplateClass,
		0, 0, 0, DocumentTemplateFuncs, 0, 0);

	s->rootJSObject(jsSceneObjectProto);
	return jsSceneObjectProto;
}

JSObject * DocumentTemplate::createScriptObjectProto(ScriptProcessor * s)
{			
	return createBaseScriptObjectProto(s);
}

JSObject * DocumentTemplate::getScriptObject(ScriptProcessor * s)
{
	if (!scriptObject_)
	{
		scriptObject_ = JS_NewObject(
			s->jsContext(), &jsDocumentTemplateClass, s->documentTemplateProto(this), 0);
		JS_SetPrivate(s->jsContext(), scriptObject_, this);
	}

	return scriptObject_;
}

DocumentTemplate::DocumentTemplate()
{
	baseAppObjectOffset_ = AppObjectOffset();
	dpiScale_ = 1;
	scriptObject_ = NULL;
	isPreDownload_ = false;
}


bool ContainerObjectsComparer::operator()(
	const ContainerObjects & lhs, const ContainerObjects & rhs) const
{
	if (lhs.size() == rhs.size())
	{
		int count = (int)lhs.size();
		char lessThan = -1;

		for (int i = 0; i < count; ++i)
		{			
			if (lhs[i].first == rhs[i].first)
			{
				if (lhs[i].second == rhs[i].second) continue;				
				else
					return lhs[i].second < rhs[i].second;
			}
			else
				return lhs[i].first < rhs[i].first;
		}

		return false;
	}
	else
		return lhs.size() < rhs.size();
}

AppObjectOffset DocumentTemplate::totalAppObjectOffset(
	const ContainerObjects & containerObjects) const
{
	AppObjectOffset ret;
	ContainerObjects::const_iterator iter;

	for(iter = containerObjects.begin(); iter != containerObjects.end(); ++iter)
	{
		SceneObject * obj = (*iter).first;
		if (obj) ret.addOffset(obj->appObjectOffset());		
	}	

	ret.addOffset(baseAppObjectOffset_);
	return ret;
}


bool DocumentTemplate::isCurrentScene(Scene * scene) const
{
	return true;
}

AppObjectOffset DocumentTemplate::totalTextFeaturesOffset(
	const ContainerObjects & containerObjects) const
{
	AppObjectOffset ret;
	ContainerObjects::const_iterator iter;

	for(iter = containerObjects.begin(); iter != containerObjects.end(); ++iter)
	{
		SceneObject * obj = (*iter).first;
		if (obj) ret.addOffset(obj->textFeaturesOffset());		
	}	

	ret.addOffset(baseAppObjectOffset_);
	return ret;
}