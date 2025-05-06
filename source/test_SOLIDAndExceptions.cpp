///************************* ITELMA SP ****************************************

#include <gtest/gtest.h>

#include "SOLIDAndExceptions.hpp"

// clang-format off

// gTest grouping class
class test_iLogger : public ::testing::Test
{
public:
    // additional class to access to member of tested class
    class Test_iLogger : public iLogger
    {
    public:
        // add here members for free access.
        using iLogger::iLogger; // delegate constructors
        std::ostringstream strm;

        void WriteEvent(iCommand& command, std::exception& exc) override
        {
            strm << "Command: " 
                 << "'" << command.Type() << "'" 
                << " Exception: "
                << "'" << exc.what() << "'" << std::endl;
        }
    };

};

// gTest grouping class
class test_SOLIDAndExceptions : public ::testing::Test
{
public:
  // additional class to access to member of tested class
  class Test_SOLIDAndExceptions : public SOLIDAndExceptions
  {
  public:
    // add here members for free access.
    using SOLIDAndExceptions::SOLIDAndExceptions; // delegate constructors

    using SOLIDAndExceptions::handler;
    using SOLIDAndExceptions::commands;

  };

};

// gTest grouping class
class test_cExceptionsHandler : public ::testing::Test
{
public:
    // additional class to access to member of tested class
    class Test_cExceptionsHandler : public cExceptionsHandler
    {
    public:
        // add here members for free access.
        using cExceptionsHandler::cExceptionsHandler; // delegate constructors

        static void process(cExceptionsHandler &h, std::unique_ptr<iCommand>& command, std::exception& e)
        {
        }
    };

};

class cTestCommand1 : public iCommand
{
public:

    virtual void Execute() override
    {
        throw(std::exception("Execution throw this"));
    }

    virtual const char* Type() override { return "cTestCommand1"; }
};


class cTestCommandExecuteTwice : public iCommand
{
public:
    virtual void Execute() override
    {
        ++executeCnt;
        if( executeCnt == 1 )
            throw(std::exception("Execution throw this"));
        else if (executeCnt == 2)
            throw(std::exception("Second execution throw this"));
        else
            throw(std::exception("Ups"));
    }

    virtual const char* Type() override { return "cTestCommandExecuteTwice"; }

    int executeCnt = 0;
};



TEST_F(test_cExceptionsHandler, test_Register )
{
    Test_cExceptionsHandler t;

    t.Register("Command", "Exception", Test_cExceptionsHandler::process);
    
    auto res = t.get("Command", "Exception");
    ASSERT_TRUE(res);
    EXPECT_TRUE(res.has_value());
    EXPECT_TRUE(Test_cExceptionsHandler::process  == *res);

    EXPECT_TRUE(t.get("Command", "Exception"));
    EXPECT_FALSE(t.get("Command", "exception"));
    EXPECT_FALSE(t.get("command", "Exception"));
}

TEST_F(test_cExceptionsHandler, test_get)
{
    Test_cExceptionsHandler t;

    t.Register("Command", "Exception", Test_cExceptionsHandler::process);

    auto res = t.get("Command", "Exception");
    ASSERT_TRUE(res);
    EXPECT_TRUE(Test_cExceptionsHandler::process == *res);

    EXPECT_FALSE(t.get("Command", "exception"));
    EXPECT_FALSE(t.get("command", "Exception"));
}

TEST_F(test_SOLIDAndExceptions, test_runCommandWriteToLogger)
{
    Test_SOLIDAndExceptions t;
    test_iLogger::Test_iLogger t3;

    cExceptionsHandler h;
    h.setLogger(t3);
    h.setCommandsDeque(t.getCommandsDeque());

    h.Register("cTestCommand1", "Execution throw this", cExceptionsHandler::addCommandWriteToLogger );

    t.set(&h);
   
    std::unique_ptr<iCommand> t1( new cTestCommand1 );

    t.push_back(t1);

    t.run();

    EXPECT_EQ("Command: 'cTestCommand1' Exception: 'Execution throw this'\n", t3.strm.str());
}

TEST_F(test_SOLIDAndExceptions, test_runRepeatCommand)
{
    Test_SOLIDAndExceptions t;
    test_iLogger::Test_iLogger t3;

    cExceptionsHandler h;
    h.setLogger(t3);
    h.setCommandsDeque(t.getCommandsDeque());

    h.Register("cTestCommandExecuteTwice", "Execution throw this", cExceptionsHandler::repeatCommand);
    h.Register("Repeator of cTestCommandExecuteTwice", "Second execution throw this", cExceptionsHandler::skipException);

    t.set(&h);

    cTestCommandExecuteTwice* p = new cTestCommandExecuteTwice;
    EXPECT_EQ(0, p->executeCnt);

    std::unique_ptr<iCommand> t1(p);
    
    t.push_back(t1);

    const auto s0 = t3.strm.str();

    t.run();

    EXPECT_EQ(2, p->executeCnt );
}



