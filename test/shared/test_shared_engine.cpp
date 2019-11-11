
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
        Engine ngine;
        ngine.getState().initializeCharacters();
        ngine.getState().initializeMapCell();

        unique_ptr<engine::Command> ptr_sc(new engine::SelectCharacterCommand(*ngine.getState().getCharacters()[0]));
        ngine.addCommand(move(ptr_sc));
        state::Position pos1{10, 10};
        unique_ptr<engine::Command> ptr_mc1(new engine::MoveCommand(*ngine.getState().getCharacters()[0], pos1));
        ngine.addCommand(move(ptr_mc1));

        unique_ptr<engine::Command> ptr_ac1(new engine::AttackCommand(*ngine.getState().getCharacters()[0], *ngine.getState().getCharacters()[1]));
        ngine.addCommand(move(ptr_ac1));

        unique_ptr<engine::Command> ptr_uh(new engine::UpdateHudCommand());
        ngine.addCommand(move(ptr_uh));

        unique_ptr<engine::Command> ptr_fc(new engine::FinishTurnCommand());
        ngine.addCommand(move(ptr_fc));
        ngine.update();
    }
}

/* vim: set sw=2 sts=2 et : */
