
#include <boost/test/unit_test.hpp>
#include <vector>
#include "../../src/shared/state.h"
#include "../../src/client/render.h"

using namespace ::state;
using namespace ::render;
using namespace std;
BOOST_AUTO_TEST_CASE(TestStaticAssert)
{
    BOOST_CHECK(1);
}

BOOST_AUTO_TEST_CASE(TestRenderNamespace)
{
    {
        TileSet ts{TileSetID::CHARACTER};
        BOOST_CHECK_EQUAL(ts.getCellHeight(), ts.getCellWidth()); // square
        BOOST_CHECK_EQUAL(ts.getImageFile(), "res/characters.png");
        ts.getTexture();
        BOOST_CHECK_EQUAL(ts.getTileSetID(), CHARACTER);
    }
}

/* vim: set sw=2 sts=2 et : */
