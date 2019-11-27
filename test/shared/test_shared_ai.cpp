
#include <boost/test/unit_test.hpp>
#include <vector>
#include "../../src/shared/ai.h"
#include "../../src/shared/state.h"
#include "../../src/shared/engine.h"

using namespace ::ai;
using namespace ::engine;
using namespace ::state;
using namespace std;
BOOST_AUTO_TEST_CASE(TestStaticAssert)
{
    BOOST_CHECK(1);
}

BOOST_AUTO_TEST_CASE(TestEngineNamespace)
{
    {
        srand(time(0));
        engine::Engine ngine{"game"};

        ngine.getState().initializeMapCell("../../../res/map_v0.txt");
        ngine.getState().initializeCharacters();

        RandomAI rai;
        BOOST_CHECK_EQUAL(2, rai.getPlayerNumber());
        rai.setPlayerNumber(1);

        HeuristicAI hai;
        hai.setPlayerNumber(2);
        BOOST_CHECK_EQUAL(2, hai.getPlayerNumber());

        hai.initMapNodes(ngine.getState());

        rai.run(ngine);
        hai.run(ngine);
    }
    {
        MapNode mn1{1,1,1,0};
        MapNode mn2{1,2,2,1};

        mn1.addNear(&mn2);
        mn2.addNear(&mn1);

        mn1.setOcuppied(true);
        BOOST_CHECK_NE(mn1.getOccupied(), mn2.getOccupied());

        BOOST_CHECK_EQUAL(mn1.getX(), mn2.getX());
        BOOST_CHECK_NE(mn1.getY(), mn2.getY());

        mn1.setX(1);
        mn2.setX(1);
        mn1.setY(1);
        BOOST_CHECK_GT(mn1.getNears().size(), 0);
    }
}

/* vim: set sw=2 sts=2 et : */
