
#include <boost/test/unit_test.hpp>
#include <vector>
#include "../../src/server/server.h"

using namespace ::server;
using namespace std;


BOOST_AUTO_TEST_CASE(TestStaticAssert)
{
    BOOST_CHECK(1);
}

BOOST_AUTO_TEST_CASE(TestServerNamespace)
{
    {
        Game game;
        ServicesManager sm;
        std::unique_ptr<AbstractService> upVs(new VersionService());
        Player p1{"fede", true};
        Player p2{"billy", true};
        std::unique_ptr<AbstractService> upPs(new PlayerService(game));
        sm.registerService(move(upVs));
        sm.registerService(move(upPs));
        auto found = sm.findService("/version");
        BOOST_CHECK_EQUAL(found->getPattern(), "/version");
        Json::Value out;
        HttpStatus status = found->get(out, -1);
        BOOST_CHECK_EQUAL(status, HttpStatus::OK);

        std::unique_ptr<Player> upP1(new Player(p1));
        std::unique_ptr<Player> upP2(new Player(p2));
        BOOST_CHECK_EQUAL(1, game.addPlayer(move(upP1))); // first player with ID == 1
        BOOST_CHECK_EQUAL(2, game.addPlayer(move(upP2)));
        BOOST_CHECK_EQUAL(nullptr, game.getPlayerById(-1)); // notfound
        
        std::unique_ptr<Player> newPlayer(new Player("modified", false));
        game.modifyPlayer(1, move(newPlayer));
        game.removePlayer(1);

        found = sm.findService("/player");
        found->get(out, 2);
        Json::Value jsonIn;
        jsonIn["name"] = "waldo";
        jsonIn["free"] = true;
        found->post(out, jsonIn);
        found->put(jsonIn, 2);
        found->remove(2);
        found->get(out, -1); // all players

        Json::Value badJson;
        badJson["somethingUnsupported"] = 123;
        badJson["anotherThingUnsupported"] = "asd";
        found->put(badJson, 3);
        string empty = "";
        sm.queryService(empty, empty, "/version", "GET");

    }
}

/* vim: set sw=2 sts=2 et : */
