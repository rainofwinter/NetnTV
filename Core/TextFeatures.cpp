#include "stdafx.h"
#include "TextFeatures.h"
#include "textSelHandleImg.h"
#include "textSelMemoImg.h"
#include "Document.h"
#include "Texture.h"
#include "GfxRenderer.h"
#include "Text.h"
#include "Text_2.h"
#include "Global.h"
#include "Scene.h"
#include "DocumentTemplate.h"
#include "Camera.h"
#include "Root.h"
#include "SceneChanger.h"
#include "SubScene.h"
#include "SceneObject.h"

using namespace std;
using boost::lexical_cast;
void *LibPNGDecodeImage(char * buf, int buff_len, int *width, int *height);

bool memoIdCompare(Memo * lhs, Memo * rhs)
{
	return lhs->memoIndex < rhs->memoIndex;
}

template <typename T>
bool getXmlNodeContent(T & val, xmlNode * node, const char * name)
{
	if (!node) return false;
	if (!xmlStrEqual(node->name, (xmlChar *)name)) return false;
	if (node->type != XML_ELEMENT_NODE) return false;
	if (!node->children) return false;
	if (node->children->type != XML_TEXT_NODE) return false;

	try 
	{
		val = lexical_cast<T>(node->children->content);
	} catch(...) 
	{
		return false;
	}

	return true;
}

TextFeatures::TextFeatures(Document * document)
{
	document_ = document;
	visible_ = true;
	curMemoIndex_ = 0;

	selMode = Unselected;	
	startSelTime_ = FLT_MAX;
	textObj_ = NULL;
	startIndex_ = endIndex_ = -1;
	pressedHandle_ = -1;
	handle0ReverseX_ = handle1ReverseX_ = false;
}

TextFeatures::~TextFeatures()
{
}

void TextFeatures::show(bool val)
{
	visible_ = val;
	document_->triggerRedraw();
}

void TextFeatures::uninit()
{
	memoTex_.reset();
	handleTex_.reset();
}

void TextFeatures::registerDrawTextObj(SceneObject * text)
{
	drawTexts_.insert(text);
}

void TextFeatures::selectPart(SceneObject * text, unsigned startIndex, unsigned endIndex)
{
	newlySelected_ = true;

	if (textObj_)
	{
		Global::instance().clearTextSelect();
	}

	textObj_ = text;
	startSelTime_ = FLT_MAX;
	selMode = Selecting;	
	startIndex_ = startIndex;
	endIndex_ = endIndex;
	if(text->type() == Text().type())
		((Text*)text)->selectPart(startIndex_, endIndex_);
	else if(text->type() == Text_2().type())
		((Text_2*)text)->selectPart(startIndex_, endIndex_);

	Global::instance().selectedTextPart();
}

bool TextFeatures::update(float curTime)
{
	if (textObj_)
	{
		if (!textObj_->parentScene()->isCurrentScene()) 
			Global::instance().clearTextSelect();
	}
	drawTexts_.clear();
	

	if (curTime - startSelTime_ > 0.5f)
	{
		Vector2 checkPos = toDocPos(textObj_, Vector2(0.0f, 0.0f));
		if (checkPos == startSelPos_)
		{
			newlySelected_ = true;
			selectPart(textObj_, startIndex_, endIndex_);		
			return true;
		}
		else startSelTime_ = FLT_MAX;
		
	}

	return false;
}

bool TextFeatures::isCurrentScene(Scene * scene) const
{
	if (!textObj_) return false;
	return textObj_->parentScene() == scene;
}

void TextFeatures::draw()
{	
	GfxRenderer * gl = document_->renderer();	

	if (!visible_) return;
	if (drawTexts_.empty()) return;

	drawMemos_.clear();
	drawMemoCoords_.clear();

	gl->useTextureProgram();
	gl->setTextureProgramOpacity(1.0f);

	int width, height;
	unsigned char * bits = (unsigned char *)LibPNGDecodeImage((char *)textSelMemoImgData, sizeof(textSelMemoImgData), &width, &height);

	BOOST_FOREACH(SceneObject * drawText, drawTexts_)
	{
		if(drawText->type() == Text().type())
		{
			Text * dText = (Text *) drawText;
			pair<MemoMap::iterator, MemoMap::iterator> ret = memos_.equal_range(drawText);
		
			memoW_ = textSelMemoImgWidth * 0.5f;
			memoH_ = textSelMemoImgHeight * 0.5f;

			dText->setMemoIconSize(memoW_, memoH_);
			dText->setDocSize(document_->width(), document_->height());
			if (!drawText->parentScene()->isCurrentScene()) continue;

			std::vector<textMemoIndex> memos;
			for (MemoMap::iterator itr1 = ret.first; itr1 != ret.second; ++itr1)
			{
				drawMemos_.push_back((*itr1).second.get());
			
				textMemoIndex memo;
				memo.startIndex = (*itr1).second.get()->charIndex;
				memo.endIndex = (*itr1).second.get()->endIndex;

				memos.push_back(memo);
			}

			if(!memos.empty())
			dText->setMemos(memos, document_->curScene(), bits, gl, width, height);
		}
		else if(drawText->type() == Text_2().type())
		{
			Text_2 * dText = (Text_2 *) drawText;
			pair<MemoMap::iterator, MemoMap::iterator> ret = memos_.equal_range(drawText);
		
			memoW_ = textSelMemoImgWidth * 0.5f;
			memoH_ = textSelMemoImgHeight * 0.5f;

			dText->setMemoIconSize(memoW_, memoH_);
			dText->setDocSize(document_->width(), document_->height());
			if (!drawText->parentScene()->isCurrentScene()) continue;

			std::vector<textMemoIndex> memos;
			for (MemoMap::iterator itr1 = ret.first; itr1 != ret.second; ++itr1)
			{
				drawMemos_.push_back((*itr1).second.get());
			
				textMemoIndex memo;
				memo.startIndex = (*itr1).second.get()->charIndex;
				memo.endIndex = (*itr1).second.get()->endIndex;

				memos.push_back(memo);
			}

			if(!memos.empty())
			dText->setMemos(memos, document_->curScene(), bits, gl, width, height);
		}
	}	

	free(bits);

	sort(drawMemos_.begin(), drawMemos_.end(), memoIdCompare);

	gl->pushMatrix();
	gl->loadMatrix(Matrix::Identity());

	
	Camera camera;
	float cw = document_->width();
	float ch = document_->height();
	camera.SetTo2DArea(cw/2, ch/2, cw, ch, 60.0f, cw/ch);
	gl->setCameraMatrix(camera.GetMatrix());
	
	if (selMode == Selecting)
	{
		//draw sel handles
		textSelHandleCoords();	

		gl->useTextureProgram();
		gl->setTextureProgramOpacity(1.0f);


		if (!handleTex_)
			handleTex_.reset(new Texture);

		if (!handleTex_->isInit())
		{
			int width, height;
			unsigned char * bits = (unsigned char *)LibPNGDecodeImage((char *)textSelHandleImgData, sizeof(textSelHandleImgData), &width, &height);
			handleTex_->init(gl, width, height, Texture::UncompressedRgba32, bits);
			free(bits);
		}

		gl->use(handleTex_.get());	

		if (handle0ReverseX_)
			gl->drawRect(handle0Pos_.x + handleW_, handle0Pos_.y - handleH_, -handleW_, handleH_);			
		else
			gl->drawRect(handle0Pos_.x - handleW_, handle0Pos_.y - handleH_, handleW_, handleH_);

		if (handle1ReverseX_)
			gl->drawRect(handle1Pos_.x - handleW_, handle1Pos_.y + handleH_, handleW_, -handleH_);			
		else
			gl->drawRect(handle1Pos_.x + handleW_, handle1Pos_.y + handleH_, -handleW_, -handleH_);
	}

	gl->popMatrix();
}

void TextFeatures::textSelHandleCoords()
{
	GfxRenderer * gl = document_->renderer();
	float dpiScale = document_->dpiScale();
	handleW_ = 0.9f * dpiScale * textSelHandleImgWidth;
	handleH_ = 0.9f * dpiScale * textSelHandleImgHeight;

	

	if(textObj_->type() == Text().type())
	{
		Vector2 posTop;
		int posHeight, posWidth;

		Text * textObj = (Text*) textObj_;
		textObj->posAtIndex(startIndex_, &posTop, &posHeight, &posWidth);	
		handle0Pos_ = toDocPos(textObj_, posTop);

		if (handle0Pos_.x - handleW_ < 0)
			handle0ReverseX_ = true;
		else
			handle0ReverseX_ = false;

		textObj->posAtIndex(endIndex_, &posTop, &posHeight, &posWidth);
		handle1Pos_ = toDocPos(textObj_, posTop + Vector2(0.0f, posHeight));

		if (handle1Pos_.x + handleW_ > document_->width())
			handle1ReverseX_ = true;
		else
			handle1ReverseX_ = false;
	}
	else if(textObj_->type() == Text_2().type())
	{
		Vector2 posTop;
		float posHeight, posWidth;

		Text_2 * textObj = (Text_2*) textObj_;
		textObj->posAtIndex(startIndex_, &posTop, &posHeight, &posWidth);	
		handle0Pos_ = toDocPos(textObj_, posTop);

		if (handle0Pos_.x - handleW_ < 0)
			handle0ReverseX_ = true;
		else
			handle0ReverseX_ = false;

		textObj->posAtIndex(endIndex_, &posTop, &posHeight, &posWidth);
		handle1Pos_ = toDocPos(textObj_, posTop + Vector2(0.0f, posHeight));

		if (handle1Pos_.x + handleW_ > document_->width())
			handle1ReverseX_ = true;
		else
			handle1ReverseX_ = false;
	}
}



void TextFeatures::startTextSel(SceneObject * selTextObj, int textSelStartIndex, int textSelEndIndex)
{
	startSelTime_ = Global::currentTime();

	startSelPos_ = toDocPos(selTextObj, Vector2(0.0f, 0.0f));

	selMode = Pressed;
	textObj_ = selTextObj;
	startIndex_ = textSelStartIndex;
	endIndex_ = textSelEndIndex;
}

Vector2 TextFeatures::selectedTextPos() const
{
	if (!textObj_) return Vector2();

	
	if(textObj_->type() == Text().type())
	{
		Text * textObj = (Text *) textObj_;

		float docWidth = document_->width();
		float docHeight = document_->height();

		Vector2 posTop;
		int posHeight;
		int posWidth;	
	
		textObj->posAtIndex(startIndex_, &posTop, &posHeight, &posWidth);	
		posTop = toDocPos(textObj_, posTop);
		Vector3 wpos = Vector3(posTop.x, posTop.y, 0.0f);

		Vector3 projCoord = document_->renderer()->project(wpos);
		Vector2 sc = document_->deviceToScreenCoords(Vector2(projCoord.x, projCoord.y));

	
		sc.x = sc.x * docWidth / document_->origWidth();
		sc.y = sc.y * docHeight / document_->origHeight();
		return sc;
	}
	else if(textObj_->type() == Text_2().type())
	{
		Text_2 * textObj = (Text_2 *) textObj_;

		float docWidth = document_->width();
		float docHeight = document_->height();

		Vector2 posTop;
		float posHeight;
		float posWidth;	
	
		textObj->posAtIndex(startIndex_, &posTop, &posHeight, &posWidth);	
		posTop = toDocPos(textObj_, posTop);
		Vector3 wpos = Vector3(posTop.x, posTop.y, 0.0f);

		Vector3 projCoord = document_->renderer()->project(wpos);
		Vector2 sc = document_->deviceToScreenCoords(Vector2(projCoord.x, projCoord.y));

	
		sc.x = sc.x * docWidth / document_->origWidth();
		sc.y = sc.y * docHeight / document_->origHeight();
		return sc;
	}
	return Vector2();
}

void TextFeatures::defaultPressHandleDelta()
{
	if (pressedHandle_ == 1)
		pressHandleDelta_ = Vector2(-handleW_/2.0f, -handleH_/2.0f);
	else
		pressHandleDelta_ = Vector2(handleW_/2.0f, handleH_/2.0f);
}

bool TextFeatures::isTouchingTextSelHandle(int index, const Vector2 & startPos)
{
	Scene * curScene = document_->documentTemplate()->curScene();

	if (index == 0)
	{
		Vector2 p = handle0Pos_;
		int tw = handleW_;
		int th = handleH_;
		if (handle0ReverseX_)
		{
			if (startPos.x > p.x && startPos.x < p.x + tw && startPos.y > p.y - th && startPos.y < p.y)
			{
				pressHandleDelta_ = p - startPos;
				return true;
			}
		}
		else
		{
			if (startPos.x > p.x - tw && startPos.x < p.x && startPos.y > p.y - th && startPos.y < p.y)
			{
				pressHandleDelta_ = p - startPos;
				return true;
			}
		}		
	}
	else
	{	
		Vector2 p = handle1Pos_;
		int tw = handleW_;
		int th = handleH_;
		if (handle1ReverseX_)
		{			
			if (startPos.x > p.x - tw && startPos.x < p.x && startPos.y > p.y && startPos.y < p.y + th)
			{
				pressHandleDelta_ = p - startPos;
				return true;
			}
		}
		else
		{
			if (startPos.x > p.x && startPos.x < p.x + tw && startPos.y > p.y && startPos.y < p.y + th)
			{
				pressHandleDelta_ = p - startPos;
				return true;
			}
		}		
	}

	return false;
}

bool TextFeatures::doTextSelHandleDrag(int selHandleIndex, const Vector2 & docPos)
{
	int posIndex, propertyIndex;
	Vector2 delta;

	Vector2 sp = toLocalScenePos(textObj_, docPos + pressHandleDelta_);
	
	if (pressedHandle_ == 0)
	{
		if(textObj_->type() == Text().type())
		{
			Text * textObj = (Text *) textObj_;
			Vector2 localPos = textObj->toLocalPos(sp);
			localPos.y += 1;
			posIndex = textObj->indexAtPos(
				localPos, 
				&propertyIndex);

			if (posIndex <= endIndex_) 
			{
				startIndex_ = posIndex;		
				return true;
			}
		}
		else if(textObj_->type() == Text_2().type())
		{
			Text_2 * textObj = (Text_2 *) textObj_;
			Vector2 localPos = textObj->toLocalPos(sp);
			localPos.y += 1;
			posIndex = textObj->indexAtPos(
				localPos, 
				&propertyIndex);

			if (posIndex <= endIndex_) 
			{
				startIndex_ = posIndex;		
				return true;
			}
		}
	}
	else
	{
		if(textObj_->type() == Text().type())
		{
			Text * textObj = (Text *) textObj_;
			posIndex = textObj->indexAtPos(
				textObj->toLocalPos(sp), 
				&propertyIndex);

			if (posIndex >= startIndex_) 
			{
				endIndex_ = posIndex;
				return true;
			}
		}
		else if(textObj_->type() == Text_2().type())
		{
			Text_2 * textObj = (Text_2 *) textObj_;
			posIndex = textObj->indexAtPos(
				textObj->toLocalPos(sp), 
				&propertyIndex);

			if (posIndex >= startIndex_) 
			{
				endIndex_ = posIndex;
				return true;
			}
		}
	}

	return false;
}


bool TextFeatures::pressEvent(const Vector2 & docPos, int pressId)
{
	if (pressId != 0) return false;

	
	pressedMemo_ = NULL;
	newlySelected_ = false;

	setSelPos(docPos);

	//text sel handles
	if (textObj_) 
	{
		textSelHandleCoords();

		if (isTouchingTextSelHandle(0, docPos))
		{
			//clicked on handle 1
			pressedHandle_ = 0;
			return true;
		}

		if (isTouchingTextSelHandle(1, docPos))
		{
			//clicked on handle 1
			pressedHandle_ = 1;
			return true;
		}
				
		startSelTime_ = FLT_MAX;
		Global::instance().clearTextSelect();
	}

	//memos

	//memos
	GfxRenderer * gl = document_->renderer();

	Scene * curScene = document_->documentTemplate()->curScene();
	
	ContainerObjects containerObjs;
	if(!curScene)
		return false;
	curScene->getContainerObjects(&containerObjs);
	AppObjectOffset offset = document_->documentTemplate()->totalTextFeaturesOffset(containerObjs);
	
	Vector2 invClickPt(
		(docPos.x - offset.dx) / offset.sx, 
		(docPos.y - offset.dy) / offset.sy);

	Ray mouseRay = Ray(Vector3(invClickPt.x, invClickPt.y, -1000.0f), Vector3(0.0f, 0.0f, 1.0f));

	if(memoClick(docPos, mouseRay, document_->curScene())) return true;

	return false;
}

bool TextFeatures::memoClick(const Vector2 & docPos, Ray mouseRay, Scene * scene)
{
	Vector3 pt;
	SceneObject *rayObj = scene->intersect(mouseRay, &pt);

	if(rayObj)
	{		
		SceneObject * selMemoText = 0;
		Memo * pressedMemo = 0;

		for (int i =  drawMemos_.size() - 1; i >= 0; --i)
		{
			Vector3 scale;

			Vector3 curSceneScale = document_->curScene()->currentScale();

			if(drawMemos_[i]->textObj->parentScene() != document_->curScene())
			{
				SceneObject * containerObj = drawMemos_[i]->textObj->parentScene()->curContainerObject();

				Vector3 parentSceneScale = drawMemos_[i]->textObj->parentScene()->currentScale();

				while(containerObj)
				{
					Vector3 cScale = containerObj->parentScene()->currentScale();

					parentSceneScale = Vector3(parentSceneScale.x * cScale.x, parentSceneScale.y * cScale.y, parentSceneScale.z * cScale.z);

					if(containerObj->parentScene() == document_->curScene())
						break;

					containerObj = containerObj->parentScene()->curContainerObject();
				}

				scale = parentSceneScale;
			}
			else
				scale = curSceneScale;

			
			if(drawMemos_[i]->textObj->type() == Text().type())
			{
				Vector2 posTop;
				int posHeight;
				int posWidth;	
				Text * textObj = (Text *) drawMemos_[i]->textObj;
				textObj->posAtIndex(drawMemos_[i]->charIndex, &posTop, &posHeight, &posWidth);
				posTop.x = posTop.x - ((memoW_ / scale.x)/2);
				posTop.y = posTop.y - ((memoH_ / scale.y)/2);
				Vector2 textPos = toDocPos(drawMemos_[i]->textObj, posTop);

				if (textPos.x < docPos.x && docPos.x < textPos.x + memoW_ &&
					textPos.y < docPos.y && docPos.y < textPos.y + memoH_)
				{
					pressedMemo = drawMemos_[i];
					selMemoText = pressedMemo->textObj;
					break;
				}
			}
			else if(drawMemos_[i]->textObj->type() == Text_2().type())
			{
				Vector2 posTop;
				float posHeight;
				float posWidth;	
				Text_2 * textObj = (Text_2 *) drawMemos_[i]->textObj;
				textObj->posAtIndex(drawMemos_[i]->charIndex, &posTop, &posHeight, &posWidth);
				posTop.x = posTop.x - ((memoW_ / scale.x)/2);
				posTop.y = posTop.y - ((memoH_ / scale.y)/2);
				Vector2 textPos = toDocPos(drawMemos_[i]->textObj, posTop);

				if (textPos.x < docPos.x && docPos.x < textPos.x + memoW_ &&
					textPos.y < docPos.y && docPos.y < textPos.y + memoH_)
				{
					pressedMemo = drawMemos_[i];
					selMemoText = pressedMemo->textObj;
					break;
				}
			}
		}

		if(pressedMemo)
		{
			if(rayObj->type() == Root().type())
			{
				pressedMemo_ = pressedMemo;
			
				if (pressedMemo_)
				{
					textObj_ = pressedMemo_->textObj;
					return true;
				}
			}
			else
			{
				SceneObject * rayObjCheck = 0;
				if(rayObj->parent()->type() == Root().type())
					rayObjCheck = rayObj;
				else
				{
					rayObjCheck = rayObj->parent();
					while(rayObjCheck->parent()->type() != Root().type())
						rayObjCheck = rayObjCheck->parent();
				}

				SceneObject * selCheckObj = 0;
				if(selMemoText->parentScene() != scene)
				{
					selCheckObj = selMemoText->parentScene()->curContainerObject();
					while(selCheckObj->parentScene() != scene)
					{
						selCheckObj = selCheckObj->parentScene()->curContainerObject();
					}				
		
					while(selCheckObj->parent()->type() != Root().type())
						selCheckObj = selCheckObj->parent();
				}
				else
				{
					if(selMemoText->parent()->type() == Root().type())
						selCheckObj = selMemoText;
					else
					{
						selCheckObj = selMemoText->parent();
						while(selCheckObj->parent()->type() != Root().type())
							selCheckObj = selCheckObj->parent();
					}
				}

				bool indexCheck = false;

				if(selCheckObj != rayObjCheck)
				{
					if(selCheckObj->parent() == rayObjCheck->parent())
					{
						int textIndex = -1, rayIndex = -1;
						std::vector<SceneObjectSPtr> pChildren = selCheckObj->parent()->children();

						for(int i = 0; (int)pChildren.size(); i++)
						{
							if(pChildren[i].get() == selCheckObj) textIndex = i;
							if(pChildren[i].get() == rayObjCheck) rayIndex = i;
							if(textIndex != -1 && rayIndex != -1) break;
						}
						if(textIndex > rayIndex) indexCheck = true;
					}
				}
				else
					indexCheck = true;

				if(indexCheck)
				{
					pressedMemo_ = pressedMemo;
			
					if (pressedMemo_)
					{
						textObj_ = pressedMemo_->textObj;
						return true;
					}
				}
			}
		}
	}
	return false;
}

void TextFeatures::clearTextSelect()
{
	if (textObj_)
	{
		if(textObj_->type() == Text().type())
			((Text*)textObj_)->clearTextSelect();
		else if(textObj_->type() == Text_2().type())
			((Text_2*)textObj_)->clearTextSelect();
	
	}
	selMode = Unselected;
	textObj_ = NULL;
	document_->triggerRedraw();
}

SceneObject * TextFeatures::selTextObj(int * startIndex, int * endIndex) const
{
	if (textObj_)
	{
		*startIndex = startIndex_;
		*endIndex = endIndex_;
	}

	return textObj_;
}

bool TextFeatures::moveEvent(const Vector2 & argDocPos, int pressId)
{
	if (pressId != 0) return false;
	Scene * curScene = document_->documentTemplate()->curScene();

	ContainerObjects containerObjs;
	if(!curScene)
		return false;
	curScene->getContainerObjects(&containerObjs);
	AppObjectOffset offset = document_->documentTemplate()->totalTextFeaturesOffset(containerObjs);
	
	Vector2 docPos(
		(argDocPos.x - offset.dx) / offset.sx, 
		(argDocPos.y - offset.dy) / offset.sy);
	
	if (newlySelected_)
	{
		return true;
	}
	else if ((selPos_ - argDocPos).magnitude() > 16 && pressedHandle_ < 0)
	{
		if (textObj_)
		{
			if(textObj_->type() == Text().type())
				((Text*)textObj_)->clearTextSelect();
			else if(textObj_->type() == Text_2().type())
				((Text_2*)textObj_)->clearTextSelect();

		}
		selMode = Unselected;
		textObj_ = NULL;
		startSelTime_ = FLT_MAX;
		pressedHandle_ = -1;
		return false;
	}

	if (pressedHandle_ < 0) 
		return false;

	Vector2 scenePos = toLocalScenePos(textObj_, docPos);
	
	if (!doTextSelHandleDrag(pressedHandle_, docPos))
	{
		pressedHandle_ = !pressedHandle_;
		defaultPressHandleDelta();			
		doTextSelHandleDrag(pressedHandle_, docPos);
	}

	if(textObj_->type() == Text().type())
		((Text*)textObj_)->selectPart(startIndex_, endIndex_);
	else if(textObj_->type() == Text_2().type())
		((Text_2*)textObj_)->selectPart(startIndex_, endIndex_);

	Global::instance().selectedTextPart();
	document_->triggerRedraw();

	return true;
}

bool TextFeatures::releaseEvent(const Vector2 & pos, int pressId, bool isClick)
{
	if (pressId != 0) return false;

	if (pressedHandle_ >= 0) 
	{
		pressedHandle_ = -1;
		return true;
	}
	if (isClick)
	{
		if (pressedMemo_)
		{		
			Global::instance().textMemoClicked(pressedMemo_);
			return true;		
		}
	}
	startSelTime_ = FLT_MAX;

	//if (newlySelected_) return true;

	return false;
}

Vector2 TextFeatures::toLocalScenePos(const SceneObject * obj, const Vector2 & docPos) const
{
	//return obj->parentTransform() * obj->visualAttrib()->transformMatrix();
	//return sceneData_[obj->parentScene()].modelMatrix * obj->parentTransform() * obj->visualAttrib()->transformMatrix();

	Scene * scene = obj->parentScene();
	ContainerObjects containerObjs;
	scene->getContainerObjects(&containerObjs);

	Vector2 ret = docPos;
	for (int i = containerObjs.size() - 1; i >= 0; --i)
	{
		SceneObject * container = containerObjs[i].first;
		if (container) 
		{
			ret = container->convertToChildSceneCoords(ret);
		}
	}

	return ret;
}

Vector2 TextFeatures::toDocPos(SceneObject * obj, const Vector2 & pos) const
{
	//return sceneData_[obj->parentScene()].modelMatrix * obj->parentTransform() * obj->visualAttrib()->transformMatrix();

	Scene * scene = obj->parentScene();
	ContainerObjects containerObjs;
	scene->getContainerObjects(&containerObjs);

	Vector2 ret = obj->parentTransform() * obj->visualAttrib()->transformMatrix() * Vector3(pos.x, pos.y, 0.0f);

	AppObjectOffset offset = document_->documentTemplate()->totalTextFeaturesOffset(containerObjs);
	ret.x = ret.x * offset.sx  + offset.dx;
	ret.y = ret.y * offset.sy  + offset.dy;

	return ret;
}

void TextFeatures::showText(SceneObject * text, unsigned charIndex)
{
	if(text->type() == Text().type())
	{
		Text * textObj = (Text *)text;
		Scene * scene = text->parentScene();
		ContainerObjects contObjs;
		scene->getContainerObjects(&contObjs);

	
		Vector2 posTop;
		int posHeight, posWidth;
		if (!textObj->isInit())
		{
			textObj->init(document_->renderer());
			textObj->posAtIndex(charIndex, &posTop, &posHeight, &posWidth);
			textObj->uninit();
		}
		else
		{
			textObj->posAtIndex(charIndex, &posTop, &posHeight, &posWidth);
		}
	

		Matrix m = text->parentTransform() * text->visualAttrib()->transformMatrix();
		Vector2 coords = m * Vector3(posTop.x, posTop.y, 0.0f);
		Vector2 outCoords;

		for (int i = 0; i < (int)contObjs.size(); ++i)
		{
			SceneObject * sceneObj = contObjs[i].first;
			Scene * scene = contObjs[i].second;
			if (sceneObj)
			{
				sceneObj->showPoint(outCoords, scene, coords);
				coords = outCoords;
			}
			else
			{
				document_->documentTemplate()->sceneSet(scene);
			}
		}

		document_->triggerRedraw();
	}
	else if(text->type() == Text_2().type())
	{
		Text_2 * textObj = (Text_2 *)text;
		Scene * scene = text->parentScene();
		ContainerObjects contObjs;
		scene->getContainerObjects(&contObjs);

	
		Vector2 posTop;
		float posHeight, posWidth;
		if (!textObj->isInit())
		{
			textObj->init(document_->renderer());
			textObj->posAtIndex(charIndex, &posTop, &posHeight, &posWidth);
			textObj->uninit();
		}
		else
		{
			textObj->posAtIndex(charIndex, &posTop, &posHeight, &posWidth);
		}
	

		Matrix m = text->parentTransform() * text->visualAttrib()->transformMatrix();
		Vector2 coords = m * Vector3(posTop.x, posTop.y, 0.0f);
		Vector2 outCoords;

		for (int i = 0; i < (int)contObjs.size(); ++i)
		{
			SceneObject * sceneObj = contObjs[i].first;
			Scene * scene = contObjs[i].second;
			if (sceneObj)
			{
				sceneObj->showPoint(outCoords, scene, coords);
				coords = outCoords;
			}
			else
			{
				document_->documentTemplate()->sceneSet(scene);
			}
		}

		document_->triggerRedraw();
	}
}

void TextFeatures::setMemo(Memo * existingMemo, const std::string & utf8Str)
{	
	bool check = utf8Str != "";
	if(check)
	{
		int len = utf8Str.length();
		int i = 1;
		BOOST_FOREACH(const char &c, utf8Str){
			if(c == '\r' || c == '\b' || c == '\n' || c == '\t' || c == ' ')
				i++;
		}

		if(len <= i) check = false;
	}
	if (existingMemo)
	{
		if(check) existingMemo->memo = utf8Str;
		else deleteMemo(existingMemo);
	}
	else
	{
		if (!textObj_) return;
		if (!check) return;
		Memo * memo = new Memo;
		memo->memoIndex = curMemoIndex_++;
		memo->charIndex = startIndex_;	
		memo->endIndex = endIndex_;
		memo->textObj = textObj_;
		memo->memo = utf8Str;
		memos_.insert(make_pair(textObj_, MemoSPtr(memo)));	

		if(textObj_->type() == Text().type())
			((Text *)textObj_)->addMemo(startIndex_, endIndex_, document_->curScene());
		else if(textObj_->type() == Text_2().type())
			((Text_2 *)textObj_)->addMemo(startIndex_, endIndex_, document_->curScene());
	}	
	document_->triggerRedraw();
}

Memo * TextFeatures::curMemo() const
{
	if (!textObj_) return NULL;
	pair<MemoMap::const_iterator, MemoMap::const_iterator> ret = memos_.equal_range(textObj_);

	for (MemoMap::const_iterator itr1 = ret.first; itr1 != ret.second; ++itr1)
	{
		Memo * memo = (*itr1).second.get();

		if (memo->charIndex == startIndex_)
		{
			return memo;
		}
	}

	return NULL;
}

void TextFeatures::getMemos(std::vector<Memo *> & memos) const
{
	memos.clear();
	for (MemoMap::const_iterator itr1 = memos_.begin(); itr1 != memos_.end(); ++itr1)
	{
		Memo * memo = (*itr1).second.get();
		memos.push_back(memo);
	}
	sort(memos.begin(), memos.end(), memoIdCompare);
}

void TextFeatures::deleteMemo(Memo * memo)
{
	if(memo->textObj->type() == Text().type())
		((Text *) memo->textObj)->deleteMemo(memo->charIndex, memo->endIndex);
	else if(memo->textObj->type() == Text_2().type())
		((Text_2 *) memo->textObj)->deleteMemo(memo->charIndex, memo->endIndex);

	MemoMap::iterator itr1;
	for (itr1 = memos_.begin(); itr1 != memos_.end();)
	{
		Memo * curMemo = (*itr1).second.get();
		/*if (curMemo == memo)
		{
			memos_.erase(itr1++);
			break;
		}*/
		if(curMemo->charIndex == memo->charIndex && curMemo->memo == memo->memo && curMemo->memoIndex == memo->memoIndex
			&& curMemo->textObj == memo->textObj && curMemo->endIndex == memo->endIndex)
		{
			memos_.erase(itr1++);
			break;
		}
		else
			++itr1;
	}

	document_->triggerRedraw();
}


void TextFeatures::addHighlight(const Color & color)
{
	if (!textObj_) return;

	if(textObj_->type() == Text().type())
	{
		Text * textObj = (Text *)textObj_;

		textObj->addHighlight(color);	

		highlights_.erase(textObj_);
		const vector<Highlight> & highlights = textObj->highlights();

		for (int i = 0; i < (int)highlights.size(); ++i)
		{
			highlights_.insert(make_pair(textObj_, highlights[i]));
		}	
		document_->triggerRedraw();
	}
	else if(textObj_->type() == Text_2().type())
	{
		Text_2 * textObj = (Text_2 *)textObj_;

		textObj->addHighlight(color);	

		highlights_.erase(textObj_);
		const vector<Highlight> & highlights = textObj->highlights();

		for (int i = 0; i < (int)highlights.size(); ++i)
		{
			highlights_.insert(make_pair(textObj_, highlights[i]));
		}	
		document_->triggerRedraw();
	}
}

void TextFeatures::removeHighlight()
{
	if (!textObj_) return;
	if(textObj_->type() == Text().type())
	{
		((Text*)textObj_)->removeHighlight(startIndex_, endIndex_);
		highlights_.erase(textObj_);
		const vector<Highlight> & highlights = ((Text_2*)textObj_)->highlights();

		for (int i = 0; i < (int)highlights.size(); ++i)
		{
			highlights_.insert(make_pair(textObj_, highlights[i]));
		}	
		document_->triggerRedraw();
	}
	else if(textObj_->type() == Text_2().type())
	{
		((Text_2*)textObj_)->removeHighlight(startIndex_, endIndex_);
		highlights_.erase(textObj_);
		const vector<Highlight> & highlights = ((Text_2*)textObj_)->highlights();

		for (int i = 0; i < (int)highlights.size(); ++i)
		{
			highlights_.insert(make_pair(textObj_, highlights[i]));
		}	
		document_->triggerRedraw();
	}
}

bool TextFeatures::saveXml(const std::string & fileName)
{
	if (memos_.empty() && highlights_.empty())
	{
		unlink(fileName.c_str());
		return false;
	}
	
	xmlTextWriterPtr writer = NULL;

	BOOST_SCOPE_EXIT( (&writer)) {
		if (writer) xmlFreeTextWriter(writer);
	} BOOST_SCOPE_EXIT_END

	int rc = 0;

	writer = xmlNewTextWriterFilename(fileName.c_str(), 0);
	if (!writer) return false;
	
	rc = xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL);
	if (rc < 0) return false;

	const vector<SceneSPtr> & scenes = document_->scenes();
	map<Scene *, unsigned> sceneIndices;
	for (int i = 0; i < (int)scenes.size(); ++i)
	{
		sceneIndices[scenes[i].get()] = i;
	}

	//write memos
	map<Scene *, vector<Memo *> > sceneMemos;

	MemoMap::iterator itr1;
	for (itr1 = memos_.begin(); itr1 != memos_.end(); ++itr1)
	{
		MemoSPtr memo = (*itr1).second;
		sceneMemos[memo->textObj->parentScene()].push_back(memo.get());
	}

	unsigned int numScenesWMemos = (unsigned int)sceneMemos.size();
	xmlTextWriterStartElement(writer, (xmlChar *)"TextFeatures");
	//memos
	
	map<Scene *, vector<Memo *> >::iterator itr2;
	for (itr2 = sceneMemos.begin(); itr2 != sceneMemos.end(); ++itr2)
	{
		Scene * scene = (*itr2).first;
		vector<Memo *> & memos  = (*itr2).second;

		static boost::uuids::uuid textType = Text().type();
		static boost::uuids::uuid text_2Type = Text_2().type();
		vector<SceneObject *> textObjs;
		vector<SceneObject *> text_2Objs;
		scene->getSceneObjectsByID(textType, &textObjs);
		scene->getSceneObjectsByID(text_2Type, &text_2Objs);

		map<SceneObject *, unsigned> textIndices;
		int cnt = 0;

		for (int i = 0; i < (int)textObjs.size(); ++i, cnt++)
			textIndices[textObjs[i]] = cnt;

		for (int i = 0; i < (int)text_2Objs.size(); ++i, cnt++)
			textIndices[text_2Objs[i]] = cnt;

		unsigned sceneIndex = sceneIndices[scene];		

		for (unsigned i = 0; i < (unsigned)memos.size(); ++i)
		{
			xmlTextWriterStartElement(writer, (xmlChar *)"Memo");
			xmlTextWriterWriteElement(writer, (xmlChar *)"memoIndex", (xmlChar *)lexical_cast<string>(memos[i]->memoIndex).c_str());
			xmlTextWriterWriteElement(writer, (xmlChar *)"sceneIndex", (xmlChar *)lexical_cast<string>(sceneIndex).c_str());
			unsigned textIndex = textIndices[(SceneObject *)memos[i]->textObj];				
			xmlTextWriterWriteElement(writer, (xmlChar *)"textIndex", (xmlChar *)lexical_cast<string>(textIndex).c_str());			
			xmlTextWriterWriteElement(writer, (xmlChar *)"charIndex", (xmlChar *)lexical_cast<string>(memos[i]->charIndex).c_str());	
			xmlTextWriterWriteElement(writer, (xmlChar *)"endIndex", (xmlChar *)lexical_cast<string>(memos[i]->endIndex).c_str());
			xmlTextWriterWriteElement(writer, (xmlChar *)"memo", (xmlChar *)memos[i]->memo.c_str());
			xmlTextWriterEndElement(writer);
		}		
	}
	
	//write highlights
	map<Scene *, vector<pair<SceneObject *, Highlight> > > sceneHighlights;

	HighlightMap::iterator itr3;
	for (itr3 = highlights_.begin(); itr3 != highlights_.end(); ++itr3)
	{
		SceneObject * textObj = (*itr3).first;
		Highlight & highlight = (*itr3).second;
		sceneHighlights[textObj->parentScene()].push_back(make_pair(textObj, highlight));
	}

	unsigned int numScenesWHighlights = (unsigned int)sceneHighlights.size();
	

	map<Scene *, vector<pair<SceneObject *, Highlight> > >::iterator itr4;
	for (itr4 = sceneHighlights.begin(); itr4 != sceneHighlights.end(); ++itr4)
	{
		
		Scene * scene = (*itr4).first;
		const vector<pair<SceneObject *, Highlight> > & highlights  = (*itr4).second;

		static boost::uuids::uuid textType = Text().type();
		static boost::uuids::uuid text_2Type = Text_2().type();
		vector<SceneObject *> textObjs;
		vector<SceneObject *> text_2Objs;
		scene->getSceneObjectsByID(textType, &textObjs);
		scene->getSceneObjectsByID(text_2Type, &text_2Objs);

		map<SceneObject *, unsigned> textIndices;

		int cnt = 0;

		for (int i = 0; i < (int)textObjs.size(); ++i, cnt++)
			textIndices[textObjs[i]] = cnt;

		for (int i = 0; i < (int)text_2Objs.size(); ++i, cnt++)
			textIndices[text_2Objs[i]] = cnt;

		unsigned sceneIndex = sceneIndices[scene];	
		

		unsigned numHilights = highlights.size();
		for (unsigned i = 0; i < numHilights; ++i)
		{
			xmlTextWriterStartElement(writer, (xmlChar *)"Highlight");
			const Highlight & h = highlights[i].second;
			const SceneObject * text = highlights[i].first;
			xmlTextWriterWriteElement(writer, (xmlChar *)"sceneIndex", (xmlChar *)lexical_cast<string>(sceneIndex).c_str());
			unsigned textIndex = textIndices[(SceneObject *)text];	
			xmlTextWriterWriteElement(writer, (xmlChar *)"textIndex", (xmlChar *)lexical_cast<string>(textIndex).c_str());			
			xmlTextWriterWriteElement(writer, (xmlChar *)"startIndex", (xmlChar *)lexical_cast<string>(h.startIndex).c_str());
			xmlTextWriterWriteElement(writer, (xmlChar *)"endIndex", (xmlChar *)lexical_cast<string>(h.endIndex).c_str());

			xmlTextWriterStartElement(writer, (xmlChar *)"color");				
			xmlTextWriterWriteElement(writer, (xmlChar *)"r", (xmlChar *)lexical_cast<string>(h.color.r).c_str());
			xmlTextWriterWriteElement(writer, (xmlChar *)"g", (xmlChar *)lexical_cast<string>(h.color.g).c_str());
			xmlTextWriterWriteElement(writer, (xmlChar *)"b", (xmlChar *)lexical_cast<string>(h.color.b).c_str());
			xmlTextWriterWriteElement(writer, (xmlChar *)"a", (xmlChar *)lexical_cast<string>(h.color.a).c_str());
			xmlTextWriterEndElement(writer);//color
			xmlTextWriterEndElement(writer); //Highlight

		}		
	}

	xmlTextWriterEndElement(writer); //TextFeatures

	xmlTextWriterEndDocument(writer);
	return true;
}

bool TextFeatures::loadXmlMemo(std::map<unsigned, std::vector<SceneObject *> > & texts, xmlNode * memoNode)
{
	unsigned memoIndex = UINT_MAX, sceneIndex = UINT_MAX, textIndex = UINT_MAX, endIndex = UINT_MAX, charIndex = UINT_MAX;
	string memo;

	try
	{
		for (xmlNode * node = memoNode->children; node; node = node->next)
		{
			if (node->type != XML_ELEMENT_NODE) continue;
			if (getXmlNodeContent(memoIndex, node, "memoIndex")) continue;
			if (getXmlNodeContent(sceneIndex, node, "sceneIndex")) continue;
			if (getXmlNodeContent(textIndex, node, "textIndex")) continue;
			if (getXmlNodeContent(charIndex, node, "charIndex")) continue;
			if (getXmlNodeContent(endIndex, node, "endIndex")) continue;
			if (getXmlNodeContent(memo, node, "memo")) continue;
		}

	} catch (...) {return false;}

	if (memoIndex == UINT_MAX) return false;
	if (sceneIndex >= document_->scenes().size()) return false;	
	Scene * scene = document_->scenes()[sceneIndex].get();

	map<unsigned, vector<SceneObject *> >::iterator itr1 = texts.find(sceneIndex);

	vector<SceneObject *> textObjs;
	vector<SceneObject *> text_2Objs;
	if (itr1 == texts.end())
	{				
		scene->getSceneObjectsByID(Text().type(), &textObjs);
		scene->getSceneObjectsByID(Text_2().type(), &text_2Objs);

		for(int i = 0; i < (int) text_2Objs.size(); i++)
			textObjs.push_back(text_2Objs[i]);

		texts[sceneIndex] = textObjs;

		
	}
	else
		textObjs = (*itr1).second;
	

	if (textIndex >= textObjs.size()) return false;
	SceneObject * text = (SceneObject *)textObjs[textIndex];

	if(text->type() == Text().type())
	{
		if (!((Text*)text)->canSelectPart()) return false;
		if (charIndex >= ((Text*)text)->textString().size()) return false;
	}
	else if(text->type() == Text_2().type())
	{
		if (!((Text_2*)text)->canSelectPart()) return false;
		if (charIndex >= ((Text_2*)text)->textString().size()) return false;
	}
	Memo * memoObj = new Memo;
	memoObj->memoIndex = memoIndex;
	if (curMemoIndex_ < memoObj->memoIndex + 1) curMemoIndex_ = memoObj->memoIndex + 1;
	memoObj->charIndex = charIndex;	
	memoObj->textObj = text;
	memoObj->memo = memo;
	memoObj->endIndex = endIndex;
	memos_.insert(make_pair(text, MemoSPtr(memoObj)));	
	
	return true;
}

bool TextFeatures::loadXmlHighlight(std::map<unsigned, std::vector<SceneObject *> > & texts, xmlNode * hiliteNode)
{
	unsigned sceneIndex = UINT_MAX, textIndex = UINT_MAX, startIndex = UINT_MAX, endIndex = UINT_MAX;
	Color color;

	try
	{
		for (xmlNode * node = hiliteNode->children; node; node = node->next)
		{
			if (node->type != XML_ELEMENT_NODE) continue;
			if (getXmlNodeContent(startIndex, node, "startIndex")) continue;
			if (getXmlNodeContent(endIndex, node, "endIndex")) continue;
			if (getXmlNodeContent(sceneIndex, node, "sceneIndex")) continue;
			if (getXmlNodeContent(textIndex, node, "textIndex")) continue;	

			if (xmlStrEqual(node->name, (xmlChar *)"color"))
			{
				for (xmlNode * c = node->children; c; c = c->next)
				{
					if (c->type != XML_ELEMENT_NODE) continue;
					if (getXmlNodeContent(color.r, c, "r")) continue;
					if (getXmlNodeContent(color.g, c, "g")) continue;
					if (getXmlNodeContent(color.b, c, "b")) continue;
					if (getXmlNodeContent(color.a, c, "a")) continue;
				}				
			}
		}
	} catch (...) {return false;}

	if (sceneIndex >= document_->scenes().size()) return false;	
	Scene * scene = document_->scenes()[sceneIndex].get();

	map<unsigned, vector<SceneObject *> >::iterator itr1 = texts.find(sceneIndex);

	vector<SceneObject *> textObjs;
	vector<SceneObject *> text_2Objs;
	if (itr1 == texts.end())
	{				
		scene->getSceneObjectsByID(Text().type(), &textObjs);
		scene->getSceneObjectsByID(Text_2().type(), &text_2Objs);

		for(int i = 0; i < (int) text_2Objs.size(); i++)
			textObjs.push_back(text_2Objs[i]);

		texts[sceneIndex] = textObjs;		
	}
	else
		textObjs = (*itr1).second;


	if (textIndex >= textObjs.size()) return false;
	SceneObject * text = (SceneObject *)textObjs[textIndex];
	
	if(text->type() == Text().type())
	{
		if (!((Text*)text)->canSelectPart()) return false;
		if (endIndex > ((Text*)text)->textString().size()) return false;
	}
	else if(text->type() == Text_2().type())
	{
		if (!((Text_2*)text)->canSelectPart()) return false;
		if (endIndex > ((Text_2*)text)->textString().size()) return false;
	}
	if (startIndex >= endIndex) return false;

	Highlight h;
	h.color = color;
	h.endIndex = endIndex;
	h.startIndex = startIndex;

	highlights_.insert(make_pair(text, h));
	
	if(text->type() == Text().type())
		((Text*)text)->addHighlight(h);
	else if(text->type() == Text_2().type())
		((Text_2*)text)->addHighlight(h);

	return true;
}

bool TextFeatures::loadXml(const std::string & fileName)
{
	xmlDocPtr doc = NULL;

	BOOST_SCOPE_EXIT( (&doc)) {
		if (doc) xmlFreeDoc(doc);
	} BOOST_SCOPE_EXIT_END

		int rc = 0;

	doc = xmlReadFile(fileName.c_str(), NULL, 0);
	if (!doc) return false;


	const vector<SceneSPtr> & scenes = document_->scenes();

	curMemoIndex_ = 0;
	xmlNode * root = xmlDocGetRootElement(doc);
	if (!root) return false;

	map<unsigned, vector<SceneObject *> > texts;

	for (xmlNode * node = root->children; node; node = node->next)
	{
		if (node->type != XML_ELEMENT_NODE) continue;
		if (xmlStrEqual(node->name, (xmlChar *)"Memo"))
		{
			loadXmlMemo(texts, node);
		}
		else if (xmlStrEqual(node->name, (xmlChar *)"Highlight"))
		{
			loadXmlHighlight(texts, node);
		}
	}

	return true;
}

void TextFeatures::search(std::vector<TFSearchResult> & searchResults, const std::string & utf8Str) const
{
	searchResults.clear();

	vector<Scene *> scenes;
	document_->documentTemplate()->getScenes(scenes);
	vector<unsigned> foundIndices;

	wstring searchStrW;
	utf8::utf8to16(utf8Str.begin(), utf8Str.end(), back_inserter(searchStrW));

	static boost::uuids::uuid textType = Text().type();
	static boost::uuids::uuid text_2Type = Text_2().type();
	for (int i = 0; i < (int)scenes.size(); ++i)
	{
		vector<SceneObject *> textObjs;
		vector<SceneObject *> text_2Objs;
		scenes[i]->getSceneObjectsByID(textType, &textObjs);
		scenes[i]->getSceneObjectsByID(text_2Type, &text_2Objs);

		for (int j = 0; j < (int)textObjs.size(); ++j)
		{
			SceneObject * text = (SceneObject *)textObjs[j];
			
			((Text*)text)->search(foundIndices, searchStrW);

			if (!foundIndices.empty())
			{
				TFSearchResult res;
				res.text = text;
				res.indices = foundIndices;
				searchResults.push_back(res);
			}
		}
		for (int j = 0; j < (int)text_2Objs.size(); ++j)
		{
			SceneObject * text = (SceneObject *)text_2Objs[j];
			
			((Text_2*)text)->search(foundIndices, searchStrW);

			if (!foundIndices.empty())
			{
				TFSearchResult res;
				res.text = text;
				res.indices = foundIndices;
				searchResults.push_back(res);
			}
		}
	}
}

std::string TextFeatures::selectedText() const
{
	if (!textObj_) return "";

	if(textObj_->type() == Text().type())
	{
		std::wstring wstr = ((Text*)textObj_)->textPartString(startIndex_, endIndex_);
		std::string ret;
		utf8::utf16to8(wstr.begin(), wstr.end(), back_inserter(ret));

		return ret;
	}
	else if(textObj_->type() == Text_2().type())
	{
		std::wstring wstr = ((Text_2*)textObj_)->textPartString(startIndex_, endIndex_);
		std::string ret;
		utf8::utf16to8(wstr.begin(), wstr.end(), back_inserter(ret));

		return ret;
	}
	return "";
}