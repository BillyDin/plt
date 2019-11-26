#include "HeuristicAI.h"
#include "state.h"
#include "engine.h"
#include <iostream>
#include <stdlib.h>
#include <unistd.h>

using namespace ai;
using namespace state;
using namespace engine;
using namespace std;

void HeuristicAI::run(engine::Engine &engine){
    cout << "run heuristic ia" << endl;
    int selectedIndex = selectCharacter(engine.getState());

    // we do the best choice basing us in the distance between characters.
    Character &selectedChar = *engine.getState().getCharacters()[selectedIndex];
    unique_ptr<Command> selectCommand(new SelectCharacterCommand(selectedChar));
    engine.addCommand(move(selectCommand));

    // can attack?
    if (selectedChar.allowedTargetsToAttack(engine.getState()).size() > 0){
        // can attack.
        // will attack the first option for now...
        Character &targetToAttack = *engine.getState().getCharacters()[selectedChar.allowedTargetsToAttack(engine.getState())[0]];
        unique_ptr<Command> atkCmd(new AttackCommand(selectedChar, targetToAttack));
        engine.addCommand(move(atkCmd));
        engine.update();

        unique_ptr<Command> finTurnCmd(new FinishTurnCommand());
        engine.addCommand(move(finTurnCmd));
        engine.update();
        return;
    } else {
        // can't attack. let's move until attack or moves chances == 0
        int movesLeft = selectedChar.getCharacterMove();
        int nextPosInPath = 0;

        // until this character has 0 moves, he will try to get closer to an specific enemy character
        
        // selected target to get closer
        int targetIndex = selectTarget(engine.getState(), selectedIndex);
        Character &targetToGetCloser = *engine.getState().getCharacters()[targetIndex];
        
        // localize source and target mapnodes
        MapNode &source = mapNodes[findMapNodeIndex(engine.getState(), selectedIndex)];
        MapNode &target = mapNodes[findMapNodeIndex(engine.getState(), targetIndex)];

        // call algorithm to choose shortest path
        vector<MapNode> path = shortestPath(source, target);

        while (movesLeft > 0 && nextPosInPath < path.size()){
            // select position to go from shortestPath
            MapNode mnToGo = path[nextPosInPath];
            Position p{mnToGo.getX(), mnToGo.getY()};

            // move this character
            unique_ptr<Command> mvCmd(new MoveCommand(selectedChar, p));
            engine.addCommand(move(mvCmd));
            engine.update();

            // decrement local moves variable, incrementNextPosition.
            movesLeft--;
            nextPosInPath++;

            // can attack?
            if (selectedChar.allowedTargetsToAttack(engine.getState()).size() > 0)
            {
                // can attack.
                // will attack the first option for now...
                Character &targetToAttack = *engine.getState().getCharacters()[selectedChar.allowedTargetsToAttack(engine.getState())[0]];
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

void HeuristicAI::setPlayerNumber(int np){
    if(np == 1 || np == 2)
        this->playerNumber = np;
}

int HeuristicAI::getPlayerNumber(){
    return playerNumber;
}

int HeuristicAI::selectCharacter(state::State &state){
    int index = -1;
    int globalMinDist = INT32_MAX;
    
    // for each alive character of mine
    for (unsigned int i = 0; i < state.getCharacters().size(); i++)
        // just my characters
        if(state.getCharacters()[i]->getPlayerOwner() == playerNumber && state.getCharacters()[i]->getStatus() != DEATH){
            int minimalDistance = INT32_MAX;
            // for each enemy character
            for (auto &enemyCharacter : state.getCharacters())
                // just enemies
                if(enemyCharacter->getPlayerOwner() != playerNumber && enemyCharacter->getStatus() != DEATH)
                    if(state.getCharacters()[i]->getPosition().distance(enemyCharacter->getPosition()) < minimalDistance)
                        minimalDistance = state.getCharacters()[i]->getPosition().distance(enemyCharacter->getPosition());
                
            if(minimalDistance < globalMinDist){
                globalMinDist = minimalDistance;
                index = i;
            }
        }
    return index;
}

// initializes mapNodes vector, which is a grid of all cells where a character can 
// walk on a given moment. So, if a cell its obstacle, isnt added to this vector.
// even if a cell its occuped, isnt added to this vector.
bool HeuristicAI::initMapNodes(state::State &state){
    // raw init
    for (auto &line : state.getMap())
        for (auto &mc : line)
            if (mc->isSpace())
                mapNodes.push_back(MapNode{mc->getPosition().getX(), mc->getPosition().getY()});

    // nears (can be optimized but like this it takes miliseconds and just one time.)
    for (auto &mn : mapNodes)
        for (auto &line : state.getMap())
            for (auto &mc : line)
                // match MapNode with MapCell
                if (mn.getX() == mc->getPosition().getX() && mn.getY() == mc->getPosition().getY())
                    for (auto &nearPos : mc->getPosition().getNearPositions())
                        // if nearPos is within mapNodes
                        for (auto &mn2 : mapNodes)
                            // I can not be a near, continue
                            if (mn2.getX() == mn.getX() && mn2.getY() == mn.getY())
                                continue;
                            else if (mn2.getX() == nearPos.getX() && mn2.getY() == nearPos.getY())
                                mn.addNear(&mn2);
    
    return true;
}

// update occupied state from each MapNode to use it in shortestPath
// the idea is that if a mapCell is occupied, it's like a momentary obstacle.
void HeuristicAI::updateMapNodes(State &state){
    for(auto &mapNode : mapNodes){
        mapNode.setOcuppied(false);
        for(auto &charac : state.getCharacters())
            if(charac->getPosition().getX() == mapNode.getX()
            && charac->getPosition().getY() == mapNode.getY())
                mapNode.setOcuppied(true);
    }
}

int HeuristicAI::selectTarget(State& state, int selectedCharacIndex){
    Character &selectedChar = *state.getCharacters()[selectedCharacIndex];
    int index = -1;
    int minimalDist = INT32_MAX;
    // iterate enemies and choose the nearest enemy
    for(unsigned int i = 0; i < state.getCharacters().size(); i++){
        if(state.getCharacters()[i]->getPlayerOwner() != playerNumber 
        && state.getCharacters()[i]->getStatus() != DEATH
        && selectedChar.getPosition().distance(state.getCharacters()[i]->getPosition()) < minimalDist){
            index = i;
            minimalDist = selectedChar.getPosition().distance(state.getCharacters()[i]->getPosition());
        }
    }
    return index;
}

int HeuristicAI::findMapNodeIndex(State& state, int characterIndex){
    Character &selectedChar = *state.getCharacters()[characterIndex];

    for(unsigned int i = 0; i < mapNodes.size(); i++){
        if(selectedChar.getPosition().getX() == mapNodes[i].getX()
        && selectedChar.getPosition().getY() == mapNodes[i].getY()){
            return i;
        }
    }

    // not found
    return -1;
}

// TODO
std::vector<MapNode> HeuristicAI::shortestPath(MapNode &source, MapNode &target){
    // do the magic
    vector<MapNode> result;
    return result;
}