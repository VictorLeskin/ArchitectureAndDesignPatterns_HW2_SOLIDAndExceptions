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

        void WriteEvent(iCommand& command, cException& exc) override
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

        static void process(cExceptionsHandler &h, std::unique_ptr<iCommand>& command, cException& e)
        {
        }
    };

};

class cTestCommand1 : public iCommand
{
public:

    virtual void Execute() override
    {
        throw(cException("Execution throw this"));
    }

    virtual const char* Type() override { return "cTestCommand1"; }
};


class cTestCommandRepeat : public iCommand
{
public:
  virtual void Execute() override
  {
    ++executeCnt;
    if (executeCnt == 1)
      throw(cException("Execution throw this"));
    // not 
  }

  virtual const char* Type() override { return "cTestCommandRepeat"; }

  static int executeCnt;
};
int cTestCommandRepeat::executeCnt;


class cTestCommandThrowTwice : public iCommand
{
public:
    virtual void Execute() override
    {
        ++executeCnt;
        if( executeCnt == 1 )
            throw(cException("Execution throw this"));
        else if (executeCnt == 2)
            throw(cException("Second execution throw this"));
        else
            throw(cException("Ups"));
    }

    virtual const char* Type() override { return "cTestCommandExecuteTwice"; }

    static int executeCnt;
};
int cTestCommandThrowTwice::executeCnt;

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

TEST_F(test_cExceptionsHandler, test_Handle)
{
  Test_cExceptionsHandler t;
  std::unique_ptr<iCommand> t1(new cTestCommand1);
  cException t2("I am big");

  t.Register(t1->Type(), "I am big", Test_cExceptionsHandler::process);

  auto res = t.Handle(t1, t2);
  EXPECT_TRUE(res == Test_cExceptionsHandler::process);

  cException t3("I am second");

  try 
  {
    t.Handle(t1, t3);
    FAIL();
  }
  catch (const std::exception& expected) 
  {
    ASSERT_STREQ("There is not action for this command and type", expected.what());
  }
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

    // check that exception message appears in the logger
    EXPECT_EQ("Command: 'cTestCommand1' Exception: 'Execution throw this'\n", t3.strm.str());
}

TEST_F(test_SOLIDAndExceptions, test_runRepeatCommand)
{
  Test_SOLIDAndExceptions t;
  test_iLogger::Test_iLogger t3;

  cExceptionsHandler h;
  h.setLogger(t3);
  h.setCommandsDeque(t.getCommandsDeque());

  h.Register("cTestCommandRepeat", "Execution throw this", cExceptionsHandler::repeatCommand);

  t.set(&h);

  // test command will throw twice.
  EXPECT_EQ(0, cTestCommandRepeat::executeCnt);

  std::unique_ptr<iCommand> t1(new cTestCommandRepeat);

  t.push_back(t1);
  t.run();

  EXPECT_EQ(2, cTestCommandRepeat::executeCnt);
}

TEST_F(test_SOLIDAndExceptions, test_runRepeatAndWriteToLogger)
{
    Test_SOLIDAndExceptions t;
    test_iLogger::Test_iLogger t3;

    cExceptionsHandler h;
    h.setLogger(t3);
    h.setCommandsDeque(t.getCommandsDeque());

    h.Register("cTestCommandExecuteTwice", "Execution throw this", cExceptionsHandler::repeatCommand);
    h.Register("Repeater of cTestCommandExecuteTwice", "Second execution throw this", cExceptionsHandler::skipException);

    t.set(&h);

    // test command will throw twice.
    cTestCommandThrowTwice::executeCnt = 0;

    std::unique_ptr<iCommand> t1(new cTestCommandThrowTwice);
    
    t.push_back(t1);
    t.run();

    EXPECT_EQ(2, cTestCommandThrowTwice::executeCnt);
}


TEST_F(test_SOLIDAndExceptions, test_runRepeatTwiceAndWriteToLogger)
{
  Test_SOLIDAndExceptions t;
  test_iLogger::Test_iLogger t3;

  cExceptionsHandler h;
  h.setLogger(t3);
  h.setCommandsDeque(t.getCommandsDeque());

  h.Register("cTestCommandExecuteTwice", "Execution throw this", cExceptionsHandler::repeatCommand);
  h.Register("Repeater of cTestCommandExecuteTwice", "Second execution throw this", cExceptionsHandler::addCommandWriteToLogger);

  t.set(&h);

  // test command will throw twice.
  cTestCommandThrowTwice::executeCnt = 0;

  std::unique_ptr<iCommand> t1(new cTestCommandThrowTwice );

  t.push_back(t1);
  t.run();

  EXPECT_EQ(2, cTestCommandThrowTwice::executeCnt);
  EXPECT_EQ("Command: 'Repeater of cTestCommandExecuteTwice' Exception: 'Second execution throw this'\n", t3.strm.str());
}
