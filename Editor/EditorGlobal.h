#pragma once
#include "EditorTypes.h"
#include "Types.h"

class EditorScene;
class EditorObject;
class EditorCustomObject;
class EditorAppObject;
class EditorDocumentTemplate;
class Document;
class DocumentTemplate;
class SceneObject;
class AnimationChannel;
class EditorEvent;
class EditorAction;
class Action;
class Event;
class ElementMapping;
class Color;

typedef BOOL (WINAPI *PGFRI)(LPCWSTR, DWORD *, LPVOID, DWORD);


class EditorGlobal
{
public:
	static EditorGlobal & instance()
	{
		static EditorGlobal global;
		return global;
	}

	EditorObject * createEditorObject(
		EditorScene * parentScene, SceneObject * sceneObj) const;

	EditorCustomObject * createEditorCustomObject(
		EditorScene * edScene, EditorCustomObject * edCustomObj) const;

	EditorAppObject * createEditorAppObject(
		EditorScene * parentScene, AppObject * appObject) const;

	EditorAction * editorAction(Action * action) const;
	EditorAction * createEditorAction(
		EditorScene * edScene, Action * action) const;

	EditorDocumentTemplate * editorDocumentTemplate(
		DocumentTemplate * docTemplate) const;

	EditorEvent * editorEvent(Event * event) const;
	
	QSettings * settings() {return &settings_;}

	QString playerLocation() const;
	void setPlayerLocation(const QString & playerLocation)
	{
		settings_.setValue("player", playerLocation);
	}

	QString pdf2imgLocation() const;
	QString pdf2textLocation() const;
	QString xpdfrcLocation() const;

	QString settingsString(const QString & key)
	{
		return settings_.value(key).toString();
	}

	void setSettingsString(const QString & key, const QString & value)
	{
		settings_.setValue(key, value);
	}


	float jointDrawSize() const;
	float gridSpacing() const;
	void setGridSpacing(float gridSpacing);
	bool showGrid() const;
	void setShowGrid(bool showGrid);
	Color sceneOutlineColor() const;
	Color AnimationGuidlineColor() const;
	void setSceneOutlineColor(const Color & color);
	void setTempfileInterval(int interval);
	int tempfileInterval() const;


	//--------------------------------------------------------------------------

	QString programDirectory() const
	{
		return programDirectory_;
	}

	void copyObjects(const std::vector<SceneObject *> & objects);
	bool pasteObjectsAvailable() const {return !clipboardObjs_.empty();}
	std::vector<SceneObjectSPtr> pasteObjects(Document * document, Scene * scene) const;
	
	void copyScenes(const std::vector<Scene *> & scenes);
	bool pasteScenesAvailable() const {return !clipboardScenes_.empty();}
	std::vector<SceneSPtr> pasteScenes(Document * document) const;

	void copyActions(const std::vector<Action *> & actions);
	bool pasteActionsAvailable() const {return !clipboardActions_.empty();}
	std::vector<ActionSPtr> pasteActions(Document * document, Scene * scene) const;


	void copyAnimChannels(const std::vector<AnimationChannel *> & channels);
	bool pasteAnimChannelsAvailable() const {return !clipboardAnimChannels_.empty();}
	std::vector<AnimationChannelSPtr> pasteAnimChannels(); 
	
	BOOL getFontResourceInfo(LPCWSTR p1, DWORD * p2, LPVOID p3, DWORD p4);
	int logPixelsY() const {return logPixelsY_;}

	boost::mt19937 & ran() {return ran_;}
	static boost::uuids::uuid randomUuid();

	void editorCustomObjects(std::vector<EditorCustomObject *> * editorCustomObjects) const;

	//void setStudio4UXFolder(std::string str);

private:	
	EditorGlobal();
	~EditorGlobal();

	static boost::mt19937 ran_;

	PGFRI getFontResourceInfoFunc_;
	int logPixelsY_;

	void setDirectories();

	QString programDirectory_;
	

	/**
	lookup which EditorObject type corresponds to a given SceneObject type
	*/
	std::map<boost::uuids::uuid, EditorObject *> editorObjs_;
	std::map<boost::uuids::uuid, EditorCustomObject *> editorCustomObjs_;
	std::map<boost::uuids::uuid, EditorAppObject *> editorAppObjs_;
	std::map<boost::uuids::uuid, EditorAction *> editorActions_;
	std::map<boost::uuids::uuid, EditorEvent *> editorEvents_;

	std::map<boost::uuids::uuid, EditorDocumentTemplate *> editorDocumentTemplates_;

	QSettings settings_;

	std::vector<SceneObjectSPtr> clipboardObjs_;
	boost::scoped_ptr<ElementMapping> clipboardObjMapping_;

	std::vector<SceneSPtr> clipboardScenes_;
	boost::scoped_ptr<ElementMapping> clipboardSceneMapping_;

	std::vector<ActionSPtr> clipboardActions_;

	std::vector<AnimationChannelSPtr> clipboardAnimChannels_;
	
};