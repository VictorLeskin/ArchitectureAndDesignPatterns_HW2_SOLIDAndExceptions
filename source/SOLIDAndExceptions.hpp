#ifndef SOLIDANDEXCEPTIONS_HPP
#define SOLIDANDEXCEPTIONS_HPP

#include <deque>
#include <string>
#include <memory>
#include <optional>
#include <cassert>
#include <map>

// interface class of command
class iCommand
{
public:
	virtual ~iCommand() = default;

	virtual void Execute() = 0;
	virtual const char* Type() = 0;
};

// A class of the deque of commands. It is just a wrapper of std::deque with pointers to commands.
// std::unique_ptr  helps us avoid problems with a memory allocation/freeing.
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

// base class of exception used in task. It has name.
class cException : public std::exception
{
public:
  cException(const char* sz) : szWhat(sz) {}

	const char* what() const noexcept { return szWhat; }

protected:
  const char* szWhat;
};

// interface class of logger
class iLogger 
{
public:
	virtual void WriteEvent(iCommand& command, cException& exc) = 0;
};



// commands

// command to write to logger 
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

// command to repeat command
class cRepeatCommand : public iCommand // interface class of command
{
public:
	cRepeatCommand(std::unique_ptr<iCommand>& c) 
		: command(std::move(c))
		, name( std::string("Repeater of ") + command->Type() )
	{
	}

	virtual void Execute() { command->Execute(); }
	virtual const char* Type() { return name.c_str(); };

protected:		
	std::unique_ptr<iCommand> command;
	std::string name;
};

class cExceptionsHandler
{
public:
	using exceptionProcessor = void (*)(cExceptionsHandler&, std::unique_ptr< iCommand>&, cException&);
	using key = std::tuple<std::string,std::string>; // command, exception

public:
	cExceptionsHandler() {}
	
	cExceptionsHandler::exceptionProcessor Handle(std::unique_ptr< iCommand>& command, cException& e);

  void Register(const char* commandType, const char* exceptionType, exceptionProcessor procesor);
  std::optional< exceptionProcessor > get(const char* commandType, const char* exceptionType) const;

  void setLogger(iLogger& l) { logger = &l; }
  void setCommandsDeque(cCommandsDeque& c) { commandsDeque = &c; }

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
	
	static void	skipException(cExceptionsHandler& handler, std::unique_ptr< iCommand>& command, cException& e)
	{
	}

protected:
	iLogger* logger = nullptr;
	cCommandsDeque* commandsDeque = nullptr;

	std::map<key, exceptionProcessor> exceptionActions;
};

class SOLIDAndExceptions
{
public:
  void run();

	// interface 
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
