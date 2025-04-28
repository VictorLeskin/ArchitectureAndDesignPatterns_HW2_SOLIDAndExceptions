///************************* ITELMA SP ****************************************
#ifndef SOLIDANDEXCEPTIONS_HPP
#define SOLIDANDEXCEPTIONS_HPP

#include <deque>
#include <string>
#include <memory>

class iCommand // interface class of command
{
public:
	virtual ~iCommand() = default;

	virtual void Execute() = 0;
	virtual const char* Type() = 0;
};

class iLogger // interface class of logger
{
public:
	virtual void WriteEvent(iCommand& command, std::exception &exc) = 0;
};

class cCommandsDeque
{
public:
	cCommandsDeque() {}

	bool empty() const { return commands.empty();  }
	iCommand &pop_front();
	void push_back();

protected:	
	std::deque<std::unique_ptr<iCommand*> > commands;

};

class cWriteToLogger : public iCommand // interface class of command
{
public:
	cWriteToLogger(iLogger* l) : logger(l) {}

	virtual void Execute() { logger->WriteEvent( ) }
	virtual const char* Type() { return "Logger"; };

protected:
	iLogger* logger;
};



class cRepeatCommand : public iCommand // interface class of command
{
public:
	cRepeatCommand(iCommand& c) : command(&c) 
	{
	}

	virtual void Execute() { command->Execute(); }
	virtual const char* Type() { return "Repeater"; };

protected:
	iCommand* command;
};


class cExceptionsHandler
{
public:
	cExceptionsHandler(std::deque<iCommand*>& c) : commandsDeque(&c), logger(nullptr) {}
	
	iCommand& Handle(iCommand& command, std::exception& e);

	void repeatCommand(iCommand& command, std::exception&)
	{
		commandsDeque->push_back(&command);
	}

	void writeToLogger(iCommand& command, std::exception &e)
	{
		logger->WriteEvent(command, e);
	}

	void Register(const char *commandType, const char *exceptionType, void (*)(iCommand &,std::exception &)) {}

	void setLogger(iLogger* l) { logger = l; }

protected:
	std::deque<iCommand*> *commandsDeque;
	iLogger* logger = nullptr;
};

class SOLIDAndExceptions
{
public:
    void executeCommands();

protected:
	cCommandsDeque commands;

};

#endif //#ifndef SOLIDANDEXCEPTIONS_HPP
