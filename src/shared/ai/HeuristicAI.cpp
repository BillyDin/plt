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

    // we do the best choice basing us in the distance between characters.
    Character &selectedChar = *engine.getState().getCharacters()[selectCharacter(engine.getState())];
    unique_ptr<Command> selectCommand(new SelectCharacterCommand(selectedChar));
    engine.addCommand(move(selectCommand));

    // can attack?
    if (selectedChar.allowedTargetsToAttack(engine.getState()).size() > 0){
        // can attack.
    } else {
        // can't attack. let's move until attack or moves chances == 0
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

// TODO
// std::vector<MapNode> HeuristicAI::shortestPath(MapNode &source, MapNode &target){}