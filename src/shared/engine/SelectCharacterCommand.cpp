#include "SelectCharacterCommand.h"
// to use console
#include <iostream>

using namespace engine;
using namespace state;
using namespace std;

SelectCharacterCommand::SelectCharacterCommand(state::Character &pTarget) : target(pTarget)
{
}

void SelectCharacterCommand::execute(state::State &state)
{
    target.setStatus(SELECTED);
}