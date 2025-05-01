///************************* ITELMA SP ****************************************
#ifndef SOLIDANDEXCEPTIONS_HPP
#define SOLIDANDEXCEPTIONS_HPP

#include <deque>
#include <string>
#include <memory>
#include <optional>
#include <cassert>
#include <map>

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

// commands 
class cCommandWriteToLogger : public iCommand
{
public:
	cCommandWriteToLogger(iLogger &l, iCommand& c, std::exception& e) 
		: logger(&l), command(&c), exc(&e) {}

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
	cRepeatCommand()
	{
	}

	virtual void Execute();
	virtual const char* Type() { return "Repeater"; };
};


class cExceptionsHandler
{
public:
	using exceptionProcessor = void (*)(iCommand&, std::exception&);

protected:
	using key = std::tuple<std::string,std::string>; // command, exception

public:
	cExceptionsHandler() {}
	
	static iCommand& Handle(iCommand& command, std::exception& e);

	static void defaultProcessor(iCommand&, std::exception&)
	{
		assert(0);
	}

	static void repeatCommand(iCommand& command, std::exception&)
	{
		commandsDeque->push_back(&command);
	}

	static void writeToLogger(iCommand& command, std::exception &e)
	{
		logger->WriteEvent(command, e);
	}

	void Register(const char* commandType, const char* exceptionType, void (*procesor)(iCommand&, std::exception&));
	std::optional< exceptionProcessor > get(const char* commandType, const char* exceptionType);

	static void setDeque(std::deque<iCommand*>& c) { commandsDeque = &c; }
	static void setLogger(iLogger* l) { logger = l; }
	 
protected:
	static std::deque<iCommand*> *commandsDeque;
	static iLogger* logger;

	std::map<key, exceptionProcessor> exceptionActions;

};

class SOLIDAndExceptions
{
public:
    void run();

	void push_back(iCommand* command)
	{
		commands.push_back(command);
	}

protected:
	cExceptionsHandler handler;
	cCommandsDeque commands;

};

#endif //#ifndef SOLIDANDEXCEPTIONS_HPP
