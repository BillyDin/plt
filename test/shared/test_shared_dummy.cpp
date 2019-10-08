
#include <boost/test/unit_test.hpp>

#include "../../src/shared/state/State.h"

using namespace ::state;

BOOST_AUTO_TEST_CASE(TestStaticAssert)
{
  BOOST_CHECK(1);
}

BOOST_AUTO_TEST_CASE(TestExemple)
{
  {
    BOOST_CHECK_EQUAL(0, 0);
    BOOST_CHECK_EQUAL(21, 21);
  }

  {
    BOOST_CHECK_LE(1, 32); // Less than equal
    BOOST_CHECK_GT(22, 11); // Greater than equl
  }
}

/* vim: set sw=2 sts=2 et : */
