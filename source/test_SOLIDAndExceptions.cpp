///************************* ITELMA SP ****************************************

#include <gtest/gtest.h>

#include "SOLIDAndExceptions.hpp"

// clang-format off

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

        static void process(iCommand& command, std::exception& e)
        {
        }
    };

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

 
TEST_F(test_SOLIDAndExceptions, test_run )
{
  Test_SOLIDAndExceptions t;

  //t.handler.Register("Command", "Exception", Test_cExceptionsHandler::process);

  //t.Register( "Logger", "Loggin failure", )
}

