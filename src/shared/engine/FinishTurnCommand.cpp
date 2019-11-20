#include "FinishTurnCommand.h"
// to use console
#include <iostream>

using namespace engine;
using namespace state;
using namespace std;

FinishTurnCommand::FinishTurnCommand (){
    id = FINISH_TURN;
    // maybe this constructor will be useful
    // i dont know right now
}

void FinishTurnCommand::execute (state::State& state){
    state.setTurnOwner((state.getTurnOwner() == 1) ? 2 : 1);
    // at this point, the turn's owner has changed.
    state.setActualAction(IDLE);
    state.getCursor().setTileCode(2);
    bool cursorPositionAssigned = false;
    state.setTurn(state.getTurn() + 1);
    // other things?
    for(auto &charac : state.getCharacters()){
        if(charac->getStatus() != DEATH){
            charac->setCharacterMove(charac->getBaseCharacterMove());
            // if it's your turn, your characters will become avaiables.
            if(charac->getPlayerOwner() == state.getTurnOwner()){
                if (!cursorPositionAssigned)
                {
                    state.getCursor().setPosition(charac->getPosition());
                    cursorPositionAssigned = true;
                }
                charac->setStatus(AVAILABLE);
            }
            // if it's not your turn, then your alive characters will wait.
            else
                charac->setStatus(WAITING);
        }
    }
}