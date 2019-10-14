
#include <boost/test/unit_test.hpp>

#include "../../src/shared/state.h"

using namespace ::state;

BOOST_AUTO_TEST_CASE(TestStaticAssert)
{
  BOOST_CHECK(1);
}

BOOST_AUTO_TEST_CASE(TestCharacter)
{
  {
    // Character
    Character c {STRENGHT, true, "Testy", 0, 0};
    BOOST_CHECK_EQUAL(c.getPosition().getY(), 0);
    BOOST_CHECK_EQUAL(c.getPosition().getX(), 0);
    BOOST_CHECK_EQUAL(c.getType(), STRENGHT);
    BOOST_CHECK_EQUAL(c.getName(), "Testy");
    BOOST_CHECK_EQUAL(c.isMapCell(), false);
    
    Position p {10,10};
    c.setPosition(p);
    BOOST_CHECK_EQUAL(c.getPosition().equals(p), true);

  }

  // State
  {
    State s;
    s.initializeCharacters();
    s.initializeMapCell();
    BOOST_CHECK_LE(1, 32); // Less than equal
    BOOST_CHECK_GT(22, 11); // Greater than equl
  }

  {
    BOOST_CHECK_LE(1, 32); // Less than equal
    BOOST_CHECK_GT(22, 11); // Greater than equl
  }
}

/* vim: set sw=2 sts=2 et : */
