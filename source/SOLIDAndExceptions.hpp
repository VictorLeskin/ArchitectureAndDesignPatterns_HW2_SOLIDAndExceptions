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

class cCommandsDeque
{
public:
	cCommandsDeque() {}

	bool empty() const { return commands.empty(); }
	std::unique_ptr< iCommand> pop_front()
	{
		std::unique_ptr< iCommand> ret = std::move(commands.front());
		commands.pop_front();
		return ret;
	}
	void push_back(std::unique_ptr < iCommand > &command)
	{
		commands.push_back( std::move(command) );
	}

protected:	
	std::deque< std::unique_ptr<iCommand> > commands;
};

class cException : public std::exception
{
public:
  cException(const char* sz) : szWhat(sz) {}

  const char* what() const override { return szWhat; }

protected:
  const char* szWhat;
};


class iLogger // interface class of logger
{
public:
	virtual void WriteEvent(iCommand& command, cException& exc) = 0;
};



// commands 
class cCommandWriteToLogger : public iCommand
{
public:
	cCommandWriteToLogger(iLogger &l, std::unique_ptr< iCommand>& c, cException& e)
		: logger(&l), command( std::move(c) ), exc(e) {}

	virtual void Execute() { logger->WriteEvent( *command, exc ); }
	virtual const char* Type() { return "Logger"; };

protected:
	iLogger* logger;
	std::unique_ptr< iCommand> command;
	cException exc;
};

class cRepeatCommand : public iCommand // interface class of command
{
public:
	cRepeatCommand(std::unique_ptr<iCommand>& c) 
		: command(std::move(c))
		, name( std::string("Repeater of ") + command->Type() )
	{
	}

	virtual void Execute() { ++executionCount; command->Execute(); }
	virtual const char* Type() { return name.c_str(); };
	int ExecutionCount() const { return executionCount; }

protected:		
	int executionCount = 0;
	std::unique_ptr<iCommand> command;
	std::string name;

};

class cExceptionsHandler
{
public:
	using exceptionProcessor = void (*)(cExceptionsHandler&, std::unique_ptr< iCommand>&, cException&);

protected:
	using key = std::tuple<std::string,std::string>; // command, exception

public:
	cExceptionsHandler() {}
	
	cExceptionsHandler::exceptionProcessor Handle(std::unique_ptr< iCommand>& command, cException& e);

	static void writeToLogger(cExceptionsHandler& handler, std::unique_ptr< iCommand>& command, cException &e)
	{
		handler.logger->WriteEvent(*command, e);
	}

	static void	addCommandWriteToLogger(cExceptionsHandler& handler, std::unique_ptr< iCommand>& command, cException& e)
	{
		std::unique_ptr<iCommand> r(new cCommandWriteToLogger(*handler.logger, command, e));
		handler.commandsDeque->push_back( r );
	}

	static void	repeatCommand(cExceptionsHandler& handler, std::unique_ptr< iCommand>& command, cException& e)
	{
		std::unique_ptr<iCommand> r(new cRepeatCommand(command));
		handler.commandsDeque->push_back(r);
	}

	static void	repeatAndWriteToLogger(cExceptionsHandler& handler, std::unique_ptr< iCommand>& command, cException& e)
	{
		repeatCommand(handler, command, e);
		addCommandWriteToLogger(handler, command, e);
	}

	static void	repeatTwiceAndWriteToLogger(cExceptionsHandler& handler, std::unique_ptr< iCommand>& command, cException& e);

	static void	skipException(cExceptionsHandler& handler, std::unique_ptr< iCommand>& command, cException& e)
	{
	}



	void Register(const char* commandType, const char* exceptionType, exceptionProcessor procesor);
	std::optional< exceptionProcessor > get(const char* commandType, const char* exceptionType);

	void setLogger(iLogger &l) { logger = &l; }
	void setCommandsDeque(cCommandsDeque& c) { commandsDeque = &c; }

protected:
	iLogger* logger = nullptr;
	cCommandsDeque* commandsDeque = nullptr;

	std::map<key, exceptionProcessor> exceptionActions;
};

class SOLIDAndExceptions
{
public:
    void run();

	void push_back(std::unique_ptr<iCommand> &command)
	{
		commands.push_back(command);
	}

	cCommandsDeque& getCommandsDeque() { return commands; }

	void set(cExceptionsHandler* h) { handler = h;  };

protected:
	cExceptionsHandler *handler = nullptr;
	cCommandsDeque commands;

};

#endif //#ifndef SOLIDANDEXCEPTIONS_HPP
