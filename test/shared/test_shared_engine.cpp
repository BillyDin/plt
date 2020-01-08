
#include <boost/test/unit_test.hpp>
#include <vector>
#include "../../src/shared/engine.h"
#include "../../src/client/render.h"

using namespace ::engine;
using namespace ::render;
using namespace std;
BOOST_AUTO_TEST_CASE(TestStaticAssert)
{
    BOOST_CHECK(1);
}

BOOST_AUTO_TEST_CASE(TestEngineNamespace)
{
    {
        Engine ngine{};
        ngine.getState().initializeCharacters();
        ngine.getState().initializeMapCell();

        unique_ptr<engine::Command> ptr_sc(new engine::SelectCharacterCommand(*ngine.getState().getCharacters()[0]));
        ngine.addCommand(move(ptr_sc));
        ptr_sc->serialize();

        unique_ptr<engine::Command> ptr_ac1(new engine::AttackCommand(*ngine.getState().getCharacters()[0], *ngine.getState().getCharacters()[1]));
        ngine.addCommand(move(ptr_ac1));
        ptr_ac1->serialize();

        unique_ptr<engine::Command> ptr_fc(new engine::FinishTurnCommand());
        ngine.addCommand(move(ptr_fc));
        ptr_fc->serialize();

        ngine.update();
    }
}

/* vim: set sw=2 sts=2 et : */
