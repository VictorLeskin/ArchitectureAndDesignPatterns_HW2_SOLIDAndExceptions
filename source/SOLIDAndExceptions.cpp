///************************* ITELMA SP ****************************************

#include "SOLIDAndExceptions.hpp"
#include <cassert>

std::deque<iCommand*>* cExceptionsHandler::commandsDeque;
iLogger* cExceptionsHandler::logger;

void SOLIDAndExceptions::run()
{
    bool stop = false;
    while (!stop)
    {
        if (true == commands.empty())
            break;
        iCommand &cmd = commands.pop_front();

        try 
        {
            cmd.Execute();
        }
        catch (std::exception &e) 
        {
            handler.Handle(cmd, e).Execute();
        }
    }
}


class cEndOfUniverse : public iCommand
{
    virtual void Execute() override
    {
        assert(false);
    }
    virtual const char* Type() { return "End of Universe"; }
};


iCommand& cExceptionsHandler::Handle(iCommand& command, std::exception& e)
{
    static cEndOfUniverse endOfEverything;
    return endOfEverything;
}

void cExceptionsHandler::Register(const char* commandType, const char* exceptionType, void(*procesor)(iCommand&, std::exception&))
{
    exceptionActions.insert(std::make_pair(std::tuple(commandType, exceptionType), procesor));
}

std::optional<cExceptionsHandler::exceptionProcessor> cExceptionsHandler::get(const char* commandType, const char* exceptionType)
{
    key k = std::tuple(commandType, exceptionType);
    if (auto f = exceptionActions.find(k); exceptionActions.end() != f )
    {
        return std::optional<exceptionProcessor>(f->second);
    }
    else
        return std::optional<exceptionProcessor>();
}
