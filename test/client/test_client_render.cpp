
#include <boost/test/unit_test.hpp>
#include <vector>
#include "../../src/shared/state.h"
#include "../../src/client/render.h"
#include "../../src/shared/engine.h"

using namespace ::state;
using namespace ::render;
using namespace std;
using namespace ::engine;

BOOST_AUTO_TEST_CASE(TestStaticAssert)
{
    BOOST_CHECK(1);
}


BOOST_AUTO_TEST_CASE(TestRenderNamespace)
{
    {
        //TileSet
        TileSet ts1{TileSetID::CHARACTER, "test"};
        BOOST_CHECK_EQUAL(ts1.getCellHeight(), ts1.getCellWidth()); // square
        ts1.getTexture();
        BOOST_CHECK_EQUAL(ts1.getTileSetID(), CHARACTER);

        TileSet ts2{TileSetID::MAPCELL, "test"};
        BOOST_CHECK_EQUAL(ts2.getCellHeight(), ts2.getCellWidth()); // square
        ts2.getTexture();
        BOOST_CHECK_EQUAL(ts2.getTileSetID(),MAPCELL);

        TileSet ts4{TileSetID::CURSOR, "test"};
        BOOST_CHECK_EQUAL(ts4.getCellHeight(), ts4.getCellWidth()); // square
        ts4.getTexture();
        BOOST_CHECK_EQUAL(ts4.getTileSetID(),CURSOR);

        State state{"render"};
        // if this passes, we have an error opening files
        state.initializeMapCell( "../../../res/map_v0.txt");
        state.initializeCharacters();
        sf::RenderWindow window(sf::VideoMode(state.getMap()[0].size() * 32 + 256, state.getMap().size() * 32 + 32, 32), "map");
        StateLayer layer(state, window, "test");
        layer.initSurfaces(state);
        layer.draw(window);
        StateEvent se{StateEventID::CHARACTERCHANGED};
        layer.stateChanged(se, state);
        BOOST_CHECK_GT(layer.getTilesets().size(), 0);
        BOOST_CHECK_GT(layer.getSurfaces().size(), 0);
    }
}

/* vim: set sw=2 sts=2 et : */
