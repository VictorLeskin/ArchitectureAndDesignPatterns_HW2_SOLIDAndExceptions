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
	iCommand& pop_front()
	{
		iCommand *ret = commands.front();
		commands.pop_front();
		return *ret;
	}
	void push_back(iCommand* command)
	{
		commands.push_back(command);
	}

protected:	
	std::deque<iCommand*> commands;

};

class cWriteToLogger : public iCommand
{
public:
	cWriteToLogger(iLogger &l, iCommand& c, std::exception& e) : logger(&l), command(&c), exc(&e) {}

	virtual void Execute() { logger->WriteEvent( *command, *exc ); }
	virtual const char* Type() { return "Logger"; };

protected:
	iLogger* logger;
	iCommand *command;
	std::exception *exc;
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

	void Register(const char* commandType, const char* exceptionType, void (cExceptionsHandler::*)(iCommand&, std::exception&));

	void setLogger(iLogger* l) { logger = l; }

protected:
	std::deque<iCommand*> *commandsDeque;
	iLogger* logger = nullptr;
};

class SOLIDAndExceptions
{
public:
    void executeCommands();

	//handler.Register(const char* commandType, const char* exceptionType, void (*)(iCommand&, std::exception&)) {}
	void Register(const char* commandType, const char* exceptionType, void (cExceptionsHandler::* action)(iCommand&, std::exception&))
	{
		handler.Register(commandType, exceptionType, action);
	}

	void push_back(iCommand* command)
	{
		commands.push_back(command);
	}


protected:
	cExceptionsHandler handler;
	cCommandsDeque commands;

};

#endif //#ifndef SOLIDANDEXCEPTIONS_HPP
