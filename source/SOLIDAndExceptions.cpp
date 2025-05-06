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
        catch (const std::exception&)
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


void cExceptionsHandler::Register(const char* commandType, const char* exceptionType, exceptionProcessor procesor)
{
    exceptionActions.insert(std::make_pair(std::tuple(commandType, exceptionType), procesor));
}

std::optional<cExceptionsHandler::exceptionProcessor> cExceptionsHandler::get(const char* commandType, const char* exceptionType) const
{
    if (auto f = exceptionActions.find(std::tuple(commandType, exceptionType) ); 
        exceptionActions.end() != f )
        return std::optional<exceptionProcessor>(f->second);
    else
        return std::optional<exceptionProcessor>();
}
