#pragma once
#include "Group.h"
#include "Image.h"
#include "Rectangle.h"
#include "Writer.h"
#include "ClickEvent.h"
#include "EventListener.h"
#include "ShowOnlyAction.h"
#include "SetVisibilityAction.h"
#include "ReceiveMessageEvent.h"
#include "ShowAllAction.h"
#include "EffectAction.h"
#include "ImageSetFileAction.h"

Group * getState(
	Group * statesGrp, Group * topGrp, Group * bottomGrp, 
	int topIndex, int bottomIndex)
{
	int numBottomChildren = bottomGrp->numChildren();
	return (Group *)statesGrp->children()
		[topIndex * (numBottomChildren + 1) + bottomIndex].get();
}

Group * setupState(
	Scene * scene,
	Group * statesGrp,
	Group * topGrp, Group * bottomGrp,
	Group * topRowGrp, Group * bottomRowGrp, 
	Group * topCheckGrp, Group * bottomCheckGrp,
	SceneObject * applyButton, int topIndex, int bottomIndex)
{
	Group * state = getState(statesGrp, topGrp, bottomGrp, topIndex, bottomIndex);
	
	int numTopImgs = topGrp->numChildren();
	int numBottomImgs = bottomGrp->numChildren();

	for (int i = 0; i < numTopImgs; ++i)
	{
		Image * img = (Image *)topRowGrp->children()[i].get();
		BoundingBox bb = img->worldExtents();
		Studio::Rectangle * clickRect = new Studio::Rectangle;

		Transform trans = clickRect->transform();
		trans.setTranslation(bb.minPt);
		clickRect->setWidth(bb.lengthX());
		clickRect->setHeight(bb.lengthY());
		clickRect->setTransform(trans);
		clickRect->visualAttrib()->setOpacity(0.0f);
		
		EventListener * tapListener = new EventListener(EventSPtr(new ClickEvent));
		ShowOnlyAction * stateTransition = new ShowOnlyAction;
		stateTransition->setTargetObject(statesGrp);
		stateTransition->setShowObject(getState(statesGrp, topGrp, bottomGrp, i+1, bottomIndex));		
		tapListener->addAction(ActionSPtr(stateTransition));

		ShowOnlyAction * showImg = new ShowOnlyAction;
		showImg->setTargetObject(topGrp);
		showImg->setShowObject(topGrp->children()[i].get());
		tapListener->addAction(ActionSPtr(showImg));

		ShowOnlyAction * showCheckImg = new ShowOnlyAction;
		showCheckImg->setTargetObject(topCheckGrp);
		showCheckImg->setShowObject(topCheckGrp->children()[i].get());
		tapListener->addAction(ActionSPtr(showCheckImg));

		if (bottomIndex > 0)
		{
			SetVisibilityAction * showApply = new SetVisibilityAction;
			showApply->setTargetObject(applyButton);
			showApply->setVisibility(true);
			tapListener->addAction(ActionSPtr(showApply));
		}

		clickRect->addListener(EventListenerSPtr(tapListener));

		stringstream ss;
		ss << "top" << i + 1;
		clickRect->setId(ss.str());
		state->addChild(SceneObjectSPtr(clickRect));
	}

	for (int i = 0; i < numBottomImgs; ++i)
	{
		Image * img = (Image *)bottomRowGrp->children()[i].get();
		BoundingBox bb = img->worldExtents();
		Studio::Rectangle * clickRect = new Studio::Rectangle;

		Transform trans = clickRect->transform();
		trans.setTranslation(bb.minPt);
		clickRect->setWidth(bb.lengthX());
		clickRect->setHeight(bb.lengthY());
		clickRect->setTransform(trans);
		clickRect->visualAttrib()->setOpacity(0.0f);

		EventListener * tapListener = new EventListener(EventSPtr(new ClickEvent));
		ShowOnlyAction * stateTransition = new ShowOnlyAction;
		stateTransition->setTargetObject(statesGrp);
		stateTransition->setShowObject(getState(statesGrp, topGrp, bottomGrp, topIndex, i+1));		
		tapListener->addAction(ActionSPtr(stateTransition));
		
		ShowOnlyAction * showImg = new ShowOnlyAction;
		showImg->setTargetObject(bottomGrp);
		showImg->setShowObject(bottomGrp->children()[i].get());
		tapListener->addAction(ActionSPtr(showImg));

		ShowOnlyAction * showCheckImg = new ShowOnlyAction;
		showCheckImg->setTargetObject(bottomCheckGrp);
		showCheckImg->setShowObject(bottomCheckGrp->children()[i].get());
		tapListener->addAction(ActionSPtr(showCheckImg));

		
		if (topIndex > 0)
		{
			SetVisibilityAction * showApply = new SetVisibilityAction;
			showApply->setTargetObject(applyButton);
			showApply->setVisibility(true);
			tapListener->addAction(ActionSPtr(showApply));
		}

		clickRect->addListener(EventListenerSPtr(tapListener));

		stringstream ss;
		ss << "bottom" << i + 1;
		clickRect->setId(ss.str());
		state->addChild(SceneObjectSPtr(clickRect));
	}

	//apply button
	if (topIndex > 0 && bottomIndex > 0)
	{
		stringstream ss;

		BoundingBox bb = applyButton->worldExtents();
		Studio::Rectangle * clickRect = new Studio::Rectangle;

		Transform trans = clickRect->transform();
		trans.setTranslation(bb.minPt);
		clickRect->setWidth(bb.lengthX());
		clickRect->setHeight(bb.lengthY());
		clickRect->setTransform(trans);
		clickRect->visualAttrib()->setOpacity(0.0f);

		EventListener * tapListener = new EventListener(EventSPtr(new ClickEvent));

		/*
		ShowOnlyAction * showImg = new ShowOnlyAction;
		showImg->setTargetObject(topBottomGrp);
		ss << "model" << numTopImgs - topIndex + 1 << numBottomImgs - bottomIndex + 1;
		showImg->setShowObject(scene->findObject(topBottomGrp, ss.str()).get());
		tapListener->addAction(ActionSPtr(showImg));	
		
		SetVisibilityAction * hideInitial = new SetVisibilityAction;
		hideInitial->setTargetObject(scene->findObject("fittingroom_back_model").get());
		hideInitial->setVisibility(false);
		tapListener->addAction(ActionSPtr(hideInitial));

		EffectAction * effect = new EffectAction;
		effect->setTargetObject(scene->findObject(topBottomGrp, ss.str()).get());
		effect->setEffect(EffectAction::FadeIn);
		tapListener->addAction(ActionSPtr(effect));*/

		ImageSetFileAction * setFile = new ImageSetFileAction;
		setFile->setTargetObject(scene->findObject("fittingroom_back_model").get());
		ss.str("");
		ss << "Res/fittingroom/model" << numTopImgs - topIndex + 1 << numBottomImgs - bottomIndex + 1 << ".jpg";
		setFile->setFileName(ss.str());
		tapListener->addAction(ActionSPtr(setFile));

		EffectAction * effect = new EffectAction;
		effect->setTargetObject(scene->findObject("fittingroom_back_model").get());
		effect->setEffect(EffectAction::FadeIn);
		tapListener->addAction(ActionSPtr(effect));


		clickRect->addListener(EventListenerSPtr(tapListener));

		ss.str("");
		ss << "apply";
		clickRect->setId(ss.str());
		state->addChild(SceneObjectSPtr(clickRect));
	}

	return state;
}

void setupFittingRoom(Document * document, const std::string & outputFile)
{

	std::string sceneName = "fittingroom_page1";
	std::string topGrpName = "topGrp";
	std::string topRowGrpName = "topRowGrp";
	std::string bottomRowGrpName = "bottomRowGrp";
	std::string bottomGrpName = "bottomGrp";
	std::string applyBtnName = "apply_btn";
	std::string topCheckGrpName = "topCheckGrp";
	std::string bottomCheckGrpName = "bottomCheckGrp";
	
	Scene * scene = document->findScene(sceneName);
	
	Group * topGrp = (Group *)scene->findObject(topGrpName).get();
	Group * bottomGrp = (Group *)scene->findObject(bottomGrpName).get();
	Group * topRowGrp = (Group *)scene->findObject(topRowGrpName).get();
	Group * bottomRowGrp = (Group *)scene->findObject(bottomRowGrpName).get();
	Group * topCheckGrp = (Group *)scene->findObject(topCheckGrpName).get();
	Group * bottomCheckGrp = (Group *)scene->findObject(bottomCheckGrpName).get();
	SceneObject * applyBtn = scene->findObject(applyBtnName).get();

	Group * states = new Group;
	
	for (int i = 0; i <= topGrp->numChildren(); ++i)
	{
		for (int j = 0; j <= bottomGrp->numChildren(); ++j)
		{
			Group * state = new Group;
			stringstream ss;
			ss << "state" << i << j;
			state->setId(ss.str());
			states->addChild(SceneObjectSPtr(state));		
		}
	}

	states->setId("states");
	scene->addElement(SceneObjectSPtr(states));


	for (int i = 0; i <= topGrp->numChildren(); ++i)
	{
		for (int j = 0; j <= bottomGrp->numChildren(); ++j)
		{
			setupState(scene, states, topGrp, bottomGrp, topRowGrp, bottomRowGrp, topCheckGrp, bottomCheckGrp, applyBtn, i, j);
		}
	}

	EventListener * reset = new EventListener(EventSPtr(
		new ReceiveMessageEvent("reset")));

	ShowOnlyAction * showFirstState = new ShowOnlyAction;
	showFirstState->setTargetObject(states);
	showFirstState->setShowObject(states->children()[0].get());
	reset->addAction(ActionSPtr(showFirstState));

	ShowAllAction * hideTop = new ShowAllAction;
	hideTop->setTargetObject(topGrp);
	hideTop->setVisible(false);
	reset->addAction(ActionSPtr(hideTop));

	ShowAllAction * hideBottom = new ShowAllAction;
	hideBottom->setTargetObject(bottomGrp);
	hideBottom->setVisible(false);
	reset->addAction(ActionSPtr(hideBottom));

	ShowAllAction * hideTopCheck = new ShowAllAction;
	hideTopCheck->setTargetObject(topCheckGrp);
	hideTopCheck->setVisible(false);
	reset->addAction(ActionSPtr(hideTopCheck));

	ShowAllAction * hideBottomCheck = new ShowAllAction;
	hideBottomCheck->setTargetObject(bottomCheckGrp);
	hideBottomCheck->setVisible(false);
	reset->addAction(ActionSPtr(hideBottomCheck));

	SetVisibilityAction * hideApply = new SetVisibilityAction;
	hideApply->setTargetObject(applyBtn);
	hideApply->setVisibility(false);
	reset->addAction(ActionSPtr(hideApply));

	scene->root()->addListener(EventListenerSPtr(reset));

	Writer(outputFile.c_str(), "").write(document);
}
