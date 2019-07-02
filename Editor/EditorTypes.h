#pragma once

class EditorScene;
typedef boost::shared_ptr<EditorScene> EditorSceneSPtr;

class EditorObject;
typedef boost::shared_ptr<EditorObject> EditorObjectSPtr;

class EditorAppObject;
typedef boost::shared_ptr<EditorAppObject> EditorAppObjectSPtr;

class EditorAction;
typedef boost::shared_ptr<EditorAction> EditorActionSPtr;

class Command;
typedef boost::shared_ptr<Command> CommandSPtr;