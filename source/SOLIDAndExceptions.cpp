///************************* ITELMA SP ****************************************

#include "SOLIDAndExceptions.hpp"
#include <cassert>

void SOLIDAndExceptions::run()
{
    bool stop = false;
    cExceptionsHandler handler(commands);

    while (!stop) 
    {
        iCommand* cmd = commands.front();
        commands.pop_front();

        try 
        {
            cmd->Execute();
        }
        catch (std::exception &e) 
        {
            handler.Handle(*cmd, e).Execute();
        }
    }

}

class cEndOfUniverse : public iCommand
{
    virtual void Execute() override;
    virtual const char* Type() { return "End of Universe"; }
};

iCommand& cExceptionsHandler::Handle(iCommand& command, std::exception& e)
{
    static cEndOfUniverse endOfEverything;
    return endOfEverything;
}

void cEndOfUniverse::Execute()
{
    assert(false);
}
