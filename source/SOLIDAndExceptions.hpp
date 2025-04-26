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

class cExceptionsHandler
{
public:
	cExceptionsHandler(std::deque<iCommand*>& c) : commandsDeque(&c) {}
	
	iCommand& Handle(iCommand& command, std::exception& e);

	std::deque<iCommand*> *commandsDeque;
};


class SOLIDAndExceptions
{
public:
    void run();

protected:
	std::deque<iCommand*> commands;

};

#endif //#ifndef SOLIDANDEXCEPTIONS_HPP
