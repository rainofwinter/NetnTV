#include "stdafx.h"
#include "EditorGroup.h"
#include "Group.h"
#include "MaskObject.h"
#include "EditorDocument.h"
using namespace std;

EditorGroup::EditorGroup()
{
	expanded_ = true;
}

EditorGroup::~EditorGroup()
{
}

SceneObject * EditorGroup::createSceneObject(EditorDocument * document) const
{
	return new Group;
}

boost::uuids::uuid EditorGroup::sceneObjectType() const
{
	return Group().type();
}

PropertyPage * EditorGroup::propertyPage() const
{
	return new EditorGroupPropertyPage(sceneObject_->group());
}

///////////////////////////////////////////////////////////////////////////////
EditorGroupPropertyPage::EditorGroupPropertyPage(Group * grp)
{	
	setGroupBox(false);
	
	core_ = new CorePropertiesPage;
	visual_ = new VisualPropertiesPage;
	
	setName(tr("Group"));
	addPropertyPage(core_);
	addSpacing(4);
	addPropertyPage(visual_);	
}

EditorGroupPropertyPage::~EditorGroupPropertyPage()
{
}

void EditorGroupPropertyPage::update()
{
	Group * grp = document_->selectedObject()->group();
	MaskObject * parentMaskObj = grp->parent()->maskObject();

	if (parentMaskObj && parentMaskObj->maskingObjectGroup() == grp)
		core_->setIdEnabled(false);
	else 
		core_->setIdEnabled(true);

	core_->update();
	visual_->update();
}
