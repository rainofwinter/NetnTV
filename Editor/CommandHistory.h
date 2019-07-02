#pragma once
#include "EditorTypes.h"
class Command;
class EditorDocument;

class CommandHistory : public QObject
{
	Q_OBJECT
public:
	CommandHistory(EditorDocument * document);
	~CommandHistory();

	void doCommand(Command * command);
	void addCommand(Command * command);
	void undoCommand();
	void redoCommand();

	bool undoAvailable() const;
	bool redoAvailable() const;

signals:
	void sceneChanged();
	void objectChanged();
	void commandHistoryChanged();
	void animationListChanged();
	void animationChanged();
	void channelListChanged();
	
private:
	void processNewCommand(const boost::shared_ptr<Command> & command);
	void connectSignals(Command * command);
private:
	std::deque<boost::shared_ptr<Command>> commands_;
	int commandIndex_;
	EditorDocument * document_;

	int maxCommands_;
};