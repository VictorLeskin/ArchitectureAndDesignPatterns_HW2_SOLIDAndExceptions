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
            handler->Handle(cmd, e)(*handler, cmd, e);
        }
        catch (const std::exception& ex)
        {
          assert(nullptr=="Not processed operation");
        }
    }
}


cExceptionsHandler::exceptionProcessor cExceptionsHandler::Handle(std::unique_ptr< iCommand>& command, cException& e)
{
    auto action = get(command->Type(), e.what());
    if (action.has_value())
        return *action;

    throw(std::exception( "There is not action for this command and type" ));
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
