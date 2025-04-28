///************************* ITELMA SP ****************************************
#ifndef SOLIDANDEXCEPTIONS_HPP
#define SOLIDANDEXCEPTIONS_HPP

#include <deque>
#include <string>

class iCommand // interface class of command
{
public:
	virtual void Execute() = 0;
	virtual const char* Type() = 0;
};

class iLogger // interface class of logger
{
public:
	virtual void WriteEvent(iCommand& command, std::exception &exc) = 0;
};

class cWriteToLog : public iCommand // interface class of command
{
public:
	cWriteToLog(iLogger* l) : logger(l) {}

	virtual void Execute() { logger->WriteEvent( ) }
	virtual const char* Type() { return "Logger"; };

protected:
	iLogger* logger;
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

	void writeLog(iCommand& command, std::exception &e)
	{
		logger->WriteEvent(command, e);
	}

	void Register(const char *commandType, const char *exceptionType, void (*)(iCommand &,std::exception &)) {}

	void setLogger(iLogger* l) { logger = l; }

protected:
	std::deque<iCommand*> *commandsDeque;
	iLogger* logger;
};


class SOLIDAndExceptions
{
public:
    void run();

protected:
	std::deque<iCommand*> commands;

};

#endif //#ifndef SOLIDANDEXCEPTIONS_HPP
