#include "SelectCharacterCommand.h"
// to use console
#include <iostream>

using namespace engine;
using namespace state;
using namespace std;

SelectCharacterCommand::SelectCharacterCommand(state::Character &pTarget) : target(pTarget)
{
    id = SELECT_CHARACTER;
}

Json::Value SelectCharacterCommand::serialize (){
    
}

void SelectCharacterCommand::execute(state::State &state)
{
    for( auto & charac : state.getCharacters()){
        if(charac->getStatus() != DEATH && state.getTurnOwner() == charac->getPlayerOwner())
            charac->setStatus(AVAILABLE);
    }
    target.setStatus(SELECTED);
}