
#include <boost/test/unit_test.hpp>
#include <vector>
#include "../../src/shared/state.h"

using namespace ::state;
using namespace std;
BOOST_AUTO_TEST_CASE(TestStaticAssert)
{
	BOOST_CHECK(1);
}

BOOST_AUTO_TEST_CASE(TestStateNamespace)
{
	{
		// Character
		Character c{STRENGHT, true, "Testy", 0, 0};
		BOOST_CHECK_EQUAL(c.getPosition().getY(), 0);
		BOOST_CHECK_EQUAL(c.getPosition().getX(), 0);
		BOOST_CHECK_EQUAL(c.getType(), STRENGHT);
		BOOST_CHECK_EQUAL(c.getName(), "Testy");
		BOOST_CHECK_EQUAL(c.isMapCell(), false);
		c.setBoosted(2);
		BOOST_CHECK_GT(c.getBoosted(), 0);

		BOOST_CHECK_EQUAL(c.getIsInBase(), true);

		Position p{10, 10};
		c.setPosition(p);
		BOOST_CHECK_EQUAL(c.getPosition().equals(p), true);
		c.setTypeID(MAGICIAN);
		BOOST_CHECK_EQUAL(c.getType(), MAGICIAN);

		Position p2{-12, -32};
		BOOST_CHECK_GT(p.distance(p2), 0); // distance returns a positive int.

		Character c2{DISTANCE, true, "Shaker", 10, 10};
		BOOST_CHECK_EQUAL(c.getPosition().equals(c2.getPosition()), true);
	}

	// State
	{
		State s;
		s.initializeCharacters();
		s.initializeMapCell();
		// Looking the map and character distribution, it must be one cell free.
		// because there are 4 map cells and only 3 characters.
		bool mapCellFree = false;
		for (auto &row : s.getMap())
		{
			for (auto &cell : row)
			{
				if((mapCellFree = cell.get()->isOccupied(s))) break;
			}
		}
		BOOST_CHECK_EQUAL(mapCellFree, true);
		s.setEnd(false);
		BOOST_CHECK_EQUAL(s.getEnd(), false);

		BOOST_CHECK_GT(s.getMap().size(), 0);		 // Greater than equl
		BOOST_CHECK_GT(s.getCharacters().size(), 0); // Greater than equl

		StateEvent se{ALLCHANGED};
		se.setStateEventID(CHARACTERCHANGED);
		BOOST_CHECK_NE(se.stateEventID, ALLCHANGED);

		Stats stats;
		stats.setAttack(10);
		stats.setDefense(10);
		stats.setHealth(10);
		stats.setMana(100);
		BOOST_CHECK_EQUAL((stats.getAttack() == 10 && stats.getDefense() == 10), (stats.getHealth() < stats.getMana()));
	}

	{
		BOOST_CHECK_LE(1, 32);  // Less than equal
		BOOST_CHECK_GT(22, 11); // Greater than equl
	}
}

/* vim: set sw=2 sts=2 et : */