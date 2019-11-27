#include "ai.h"
#include "engine.h"
#include "state.h"
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

using namespace ai;
using namespace engine;
using namespace state;
using namespace std;

void RandomAI::run(engine::Engine &engine)
{
    int randomCharSelected = selectCharacter(engine.getState());
    // always select someone
    Character &selectedChar = *engine.getState().getCharacters()[randomCharSelected];
    unique_ptr<Command> selectCommand(new SelectCharacterCommand(selectedChar));
    engine.addCommand(move(selectCommand));

    // can attack?
    if (selectedChar.allowedTargetsToAttack(engine.getState()).size() > 0)
    {
        // can attack
        cout << "first if can attack ? true " << endl;
        int moves = selectedChar.getCharacterMove();
        while (selectedChar.allowedTargetsToAttack(engine.getState()).size() > 0 && moves > 0)
        {
            int random = selectedChar.allowedTargetsToAttack(engine.getState())[(rand() % (selectedChar.allowedTargetsToAttack(engine.getState()).size()))];
            Character &targetToAttack = *engine.getState().getCharacters()[random];
            // choose to attack or to move (0 move, 1 attack)
            if ((rand() % 2))
            {
                // attack
                unique_ptr<Command> atkCmd(new AttackCommand(selectedChar, targetToAttack));
                engine.addCommand(move(atkCmd));
                engine.update();

                unique_ptr<Command> finTurnCmd(new FinishTurnCommand());
                engine.addCommand(move(finTurnCmd));
                engine.update();
                return;
            }
            else
            {
                // move
                int randomMove = (rand() % selectedChar.allowedPosToMove(engine.getState()).size());
                Position &p = selectedChar.allowedPosToMove(engine.getState())[randomMove];
                unique_ptr<Command> mvCmd(new MoveCommand(selectedChar, p));
                engine.addCommand(move(mvCmd));
                engine.update();
                moves--;
            }
        }
        unique_ptr<Command> finTurnCmd(new FinishTurnCommand());
        engine.addCommand(move(finTurnCmd));
        engine.update();
    }
    else
    {
        int moves = selectedChar.getCharacterMove();
        while ( moves > 0)
        {
            // can NOT attack, JUST MOVE.
            int randomMove = (rand() % selectedChar.allowedPosToMove(engine.getState()).size());
            Position p{selectedChar.allowedPosToMove(engine.getState())[randomMove].getX(), selectedChar.allowedPosToMove(engine.getState())[randomMove].getY()};
            unique_ptr<Command> mvCmd(new MoveCommand(selectedChar, p));
            engine.addCommand(move(mvCmd));
            engine.update();
            moves--;
            cout << " move executed " << endl;

            // now i was deplaced, can attack?
            if (selectedChar.allowedTargetsToAttack(engine.getState()).size())
            {
                // just attack
                int random = selectedChar.allowedTargetsToAttack(engine.getState())[(rand() % (selectedChar.allowedTargetsToAttack(engine.getState()).size()))];
                Character &targetToAttack = *engine.getState().getCharacters()[random];                
                unique_ptr<Command> atkCmd(new AttackCommand(selectedChar, targetToAttack));
                engine.addCommand(move(atkCmd));
                engine.update();

                unique_ptr<Command> finTurnCmd(new FinishTurnCommand());
                engine.addCommand(move(finTurnCmd));
                engine.update();
                return;
            }
        }
        unique_ptr<Command> finTurnCmd(new FinishTurnCommand());
        engine.addCommand(move(finTurnCmd));
        engine.update();
        return;
    }
}

int RandomAI::selectCharacter (state::State& state){
    std::vector<int> posibleIndex;

    for(unsigned int i = 0; i < state.getCharacters().size(); i++){
        Character &charac = *state.getCharacters()[i];
        if(charac.getPlayerOwner() == playerNumber && charac.getStatus() != DEATH)
            posibleIndex.push_back(i);
    }

    int randomNumber = rand() % posibleIndex.size();
    cout << "[";
    for(auto &index : posibleIndex){
        cout << index << ", ";
    }
    cout << "]" << endl;
    return posibleIndex[randomNumber];
}

int RandomAI::getPlayerNumber (){
    return playerNumber;
}

void RandomAI::setPlayerNumber (int pn){
    if(pn == 1 || pn == 2){
        playerNumber = pn;
    }
}