#include "HeuristicAI.h"
#include "state.h"
#include "engine.h"
#include <iostream>
#include <stdlib.h>
#include <unistd.h>

using namespace ai;
using namespace state;
using namespace std;

void HeuristicAI::run(engine::Engine &engine){
    cout << "run heuristic ia" << endl;
    if(initMapNodes(engine.getState())){
        // code ...
    }
}

void HeuristicAI::setPlayerNumber(int np){
    if(np == 1 || np == 2)
        this->playerNumber = np;
}

int HeuristicAI::getPlayerNumber(){
    return playerNumber;
}

// by now, we are reusing the selectCharacter function of RandomAI
int HeuristicAI::selectCharacter(state::State &state){
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