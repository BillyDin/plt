
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
		c.setStatus(CharacterStatusID::SELECTED);
		BOOST_CHECK_NE(c.getStatus(), CharacterStatusID::AVAILABLE);
		BOOST_CHECK_GT(c.getCharacterMove(), 0);
		BOOST_CHECK_GT(c.getCharacterAttack(), 0);
		c.setCharacterMove(10);
		c.getStats();

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

		// inherited equal method from Element
		Character c1{STRENGHT, true, "Testy", 0, 0};
		Character c1identical{STRENGHT, true, "Testy", 0, 0};
		BOOST_CHECK_EQUAL(c1.equals(c1identical), true);
	}

	// Cursor
	{
		Position nextPos{0, 0};
		Cursor c{1, 1};
		BOOST_CHECK_EQUAL(c.isMapCell(), false);
		c.move(nextPos);
		BOOST_CHECK_EQUAL(c.getPosition().equals(nextPos), true);
	}

	// State
	{
		State s;
		s.initializeCharacters();
		s.initializeMapCell();

		// Looking the map and character distribution, it must be one cell free.
		// because there are 4 map cells and only 3 characters.
		// To the future, we know that always must be at least a free cell in the game to have sense.
		bool mapCellFree = false;

		for (auto &row : s.getMap())
		{
			for (auto &cell : row)
			{
				if ((mapCellFree = cell.get()->isOccupied(s)))
					break;
			}
		}

		BOOST_CHECK_EQUAL(mapCellFree, true);
		s.setEnd(false);
		BOOST_CHECK_EQUAL(s.getEnd(), false);

		BOOST_CHECK_GT(s.getMap().size(), 0);		 // Greater than equl
		BOOST_CHECK_GT(s.getCharacters().size(), 0); // Greater than equl

		s.setTurn(2);
		BOOST_CHECK_EQUAL(s.getTurn(), 2);
		BOOST_CHECK_EQUAL(s.getCursor().isMapCell(), false);
	}

	// StateEvent
	{
		StateEvent se{ALLCHANGED};
		se.setStateEventID(CHARACTERCHANGED);
		BOOST_CHECK_NE(se.stateEventID, ALLCHANGED);
	}

	// Observable
	{
		class DummyObserver : IObserver
		{
		private:
			bool notified = false;
		public:
			void stateChanged(const StateEvent &e, State &s)
			{
				notified = true;
			}
			bool getNotified(){ return notified; }
		};

		DummyObserver * dummyObs = new DummyObserver();
		BOOST_CHECK_EQUAL(dummyObs->getNotified(), false);
		State s;
		StateEvent se{StateEventID::ALLCHANGED};
		s.registerObserver((IObserver *)dummyObs);
		s.notifyObservers(se, s);
		BOOST_CHECK_EQUAL(dummyObs->getNotified(), true);
	}

	// Stats
	{
		Stats stats;
		stats.setAttack(10);
		stats.setDefense(10);
		stats.setHealth(10);
		stats.setMana(100);
		BOOST_CHECK_EQUAL((stats.getAttack() == 10 && stats.getDefense() == 10), (stats.getHealth() < stats.getMana()));
	}

	// MapCells
	{
		int x = 1, y = 2;
		SpaceMapCell smp{SpaceMapCellID::CONCRETE, x, y};

		BOOST_CHECK_EQUAL(smp.getSpaceMapCellID(), SpaceMapCellID::CONCRETE);
		smp.setIsBoost(true);
		BOOST_CHECK_EQUAL(smp.getIsBoost(), true);
		BOOST_CHECK_EQUAL(smp.isMapCell(), true);

		ObstacleMapCell omp{ObstacleMapCellID::FIRE, x, y};
		BOOST_CHECK_EQUAL(omp.getObstacleMapCellID(), ObstacleMapCellID::FIRE);
	}
}

/* vim: set sw=2 sts=2 et : */
