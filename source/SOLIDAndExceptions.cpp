///************************* ITELMA SP ****************************************

#include "SOLIDAndExceptions.hpp"
#include <cassert>

void SOLIDAndExceptions::run()
{
    bool stop = false;
    while (!stop)
    {
        if (true == commands.empty())
            break;
        std::unique_ptr< iCommand> cmd = commands.pop_front();
        try 
        {
            cmd->Execute();
        }
        catch (cException &e) 
        {
            cExceptionsHandler::exceptionProcessor p = handler->Handle(cmd, e);
            (*p)(*handler, cmd, e);
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


cExceptionsHandler::exceptionProcessor cExceptionsHandler::Handle(std::unique_ptr< iCommand>& command, cException& e)
{
    auto action = get(command->Type(), e.what());
    if (action.has_value())
        return *action;
    throw(cException( "There is not action for this command and type" ));
}

void cExceptionsHandler::repeatTwiceAndWriteToLogger(cExceptionsHandler& handler, std::unique_ptr<iCommand>& command, cException& e)
{
    iCommand *p = command.get();
    if( cRepeatCommand *p1 = dynamic_cast<cRepeatCommand *>(p); p1 != nullptr )
    {
        if( p1->ExecutionCount() == 1 )
            repeatCommand(handler, command, e);
        else if (p1->ExecutionCount() == 2)
            addCommandWriteToLogger(handler, command, e);
    }
}

void cExceptionsHandler::Register(const char* commandType, const char* exceptionType, exceptionProcessor procesor)
{
    exceptionActions.insert(std::make_pair(std::tuple(commandType, exceptionType), procesor));
}

std::optional<cExceptionsHandler::exceptionProcessor> cExceptionsHandler::get(const char* commandType, const char* exceptionType)
{
    if (auto f = exceptionActions.find(std::tuple(commandType, exceptionType) ); 
        exceptionActions.end() != f )
        return std::optional<exceptionProcessor>(f->second);
    else
        return std::optional<exceptionProcessor>();
}
