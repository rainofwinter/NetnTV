#include "stdafx.h"
#include "CommandHistory.h"
#include "Command.h"
#include "EditorDocument.h"

using namespace boost;

CommandHistory::CommandHistory(EditorDocument * document)
{
	document_ = document;
	commandIndex_ = 0;
	maxCommands_ = 50;
}

CommandHistory::~CommandHistory()
{
}

void CommandHistory::doCommand(Command * commandPtr)
{	
	shared_ptr<Command> command(commandPtr);
	connectSignals(command.get());
	try
	{
		command->doCommand();
	}
	catch (const Exception & e)
	{
		throw e;
	}
	catch(const std::exception & e)
	{
		throw e;
	}

	processNewCommand(command);	
}

void CommandHistory::addCommand(Command * commandPtr)
{
	shared_ptr<Command> command(commandPtr);
	connectSignals(command.get());

	processNewCommand(command);	
	
}

void CommandHistory::processNewCommand(const shared_ptr<Command> & command)
{
	commands_.erase(commands_.begin() + commandIndex_, commands_.end());

	bool commandMerged = false;
	if (!commands_.empty())
	{
		shared_ptr<Command> & lastCommand = commands_.back();

		if (lastCommand->shouldMerge(command.get()))
		{
			lastCommand->merge(command.get());
			commandMerged = true;
		}
	}

	if (!commandMerged)
	{
		commands_.push_back(command);

		++commandIndex_;
		if (commandIndex_ > maxCommands_)
		{		
			commands_.pop_front();
			--commandIndex_;
		}

		emit commandHistoryChanged();
	}
}

bool CommandHistory::undoAvailable() const
{
	return commandIndex_ > 0;
}

bool CommandHistory::redoAvailable() const
{
	return commandIndex_ < commands_.size();
}

void CommandHistory::undoCommand()
{
	if (commandIndex_ == 0) return;
	--commandIndex_;
	commands_[commandIndex_]->undoCommand();

	emit commandHistoryChanged();
}

void CommandHistory::redoCommand()
{
	if (commandIndex_ == commands_.size()) return;

	commands_[commandIndex_]->doCommand();
	++commandIndex_;

	emit commandHistoryChanged();
}

void CommandHistory::connectSignals(Command * parentCommand)
{	
	std::vector<Command *> cmds = parentCommand->subCommands();
	cmds.push_back(parentCommand);

	BOOST_FOREACH(Command * command, cmds)
	{
		connect(command, SIGNAL(sceneSelectionChanged()), 
			document_, SIGNAL(sceneSelectionChanged()));
		connect(command, SIGNAL(sceneListChanged()), document_,
			SLOT(onSceneListChanged()));
		connect(command, SIGNAL(sceneChanged()), document_, SIGNAL(sceneChanged()));
		connect(command, SIGNAL(listenerChanged()), document_, SIGNAL(listenerChanged()));
		connect(command, SIGNAL(listenerListChanged()), 
			document_, SLOT(onListenerListChanged()));
		connect(command, SIGNAL(actionListChanged()), document_, 
			SLOT(onActionListChanged()));

		connect(command, SIGNAL(actionSelectionChanged()), document_, 
			SIGNAL(actionSelectionChanged()));

		connect(command, SIGNAL(actionChanged(Action *, Action *)), 
			document_, SLOT(onActionChanged(Action *, Action *)));	
		connect(command, SIGNAL(objectChanged()), document_, SIGNAL(objectChanged()));
		connect(command, SIGNAL(objectListChanged()), document_, SLOT(onObjectListChanged()));
		connect(command, SIGNAL(appObjectListChanged()), document_, SLOT(onAppObjectListChanged()));
		connect(command, SIGNAL(objectSelectionChanged()), document_, SIGNAL(objectSelectionChanged()));
		
		connect(command, SIGNAL(appObjectChanged()), document_, SIGNAL(appObjectChanged()));
		connect(command, SIGNAL(appObjectListChanged()), document_, SLOT(onAppObjectListChanged()));
		connect(command, SIGNAL(appObjectSelectionChanged()), document_, SIGNAL(appObjectSelectionChanged()));
			
		
		connect(command, SIGNAL(keyFrameDeleted()), document_, SLOT(onKeyFrameDeleted()));
		connect(command, SIGNAL(animationListChanged()), document_, SLOT(onAnimationListChanged()));
		connect(command, SIGNAL(animationChanged()), document_, SIGNAL(animationChanged()));
		connect(command, SIGNAL(channelListChanged()), document_, SLOT(onChannelListChanged()));
		connect(command, SIGNAL(channelSelectionChanged()), document_, SIGNAL(channelSelectionChanged()));


		connect(command, SIGNAL(channelChanged()), document_, SIGNAL(channelChanged()));
		connect(command, 
			SIGNAL(keyFrameTimeChanged(
				const std::vector<KeyFrameData> &, const std::vector<float> &)),
			document_, 
			SLOT(onKeyFrameTimeChanged(
				const std::vector<KeyFrameData> &, const std::vector<float> &)));
		connect(command, SIGNAL(keyFrameDeleted()),	document_, SLOT(onKeyFrameDeleted()));
	}
}
