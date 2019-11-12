
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
        TileSet ts1{TileSetID::CHARACTER};
        BOOST_CHECK_EQUAL(ts1.getCellHeight(), ts1.getCellWidth()); // square
        BOOST_CHECK_EQUAL(ts1.getImageFile(), "res/characters.png");
        ts1.getTexture();
        BOOST_CHECK_EQUAL(ts1.getTileSetID(), CHARACTER);

        TileSet ts2{TileSetID::MAPCELL};
        BOOST_CHECK_EQUAL(ts2.getCellHeight(), ts2.getCellWidth()); // square
        BOOST_CHECK_EQUAL(ts2.getImageFile(), "res/32x32map.png");
        ts2.getTexture();
        BOOST_CHECK_EQUAL(ts2.getTileSetID(),MAPCELL);


        TileSet ts3{TileSetID::HUD};
        BOOST_CHECK_EQUAL(ts3.getCellHeight(), ts3.getCellWidth()); // square
        BOOST_CHECK_EQUAL(ts3.getImageFile(), "res/32x32map.png");
        ts3.getTexture();
        BOOST_CHECK_EQUAL(ts3.getTileSetID(),HUD);

        TileSet ts4{TileSetID::CURSOR};
        BOOST_CHECK_EQUAL(ts4.getCellHeight(), ts4.getCellWidth()); // square
        BOOST_CHECK_EQUAL(ts4.getImageFile(), "res/cursor.png");
        ts4.getTexture();
        BOOST_CHECK_EQUAL(ts4.getTileSetID(),CURSOR);


    

        //StateLayer
        // engine::Engine ngine{"game"};
        // State st{"Render"};
        // sf::RenderWindow window(sf::VideoMode(ngine.getState().getMap()[0].size() * 32 + 256, ngine.getState().getMap().size() * 32 + 32, 32), "map");
        // StateLayer sl{st,window};


    }
}

/* vim: set sw=2 sts=2 et : */
