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
  };

};
 
TEST_F(test_SOLIDAndExceptions, test_run )
{
  Test_SOLIDAndExceptions t;

  t.Register( "Logger", "Loggin failure", )
}

