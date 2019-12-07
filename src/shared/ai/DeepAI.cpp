#include "DeepAI.h"
#include "state.h"
#include "engine.h"
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <queue>
#include <list>

using namespace ai;
using namespace state;
using namespace engine;
using namespace std;

DeepAI::DeepAI(Engine& engine, int pn){
    this->playerNumber = pn;
    initMapNodes(engine.getState());
    initDeepNodes(engine.getState());
}

bool DeepAI::initDeepNodes(State& state){
    return true;
}

void DeepAI::run(engine::Engine &engine){
    cout << "run heuristic ia" << endl;
    updateMapNodes(engine.getState());
    int selectedIndex = advancedSelectCharacter(engine.getState());

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
        size_t movesLeft = selectedChar.getCharacterMove();
        size_t nextPosInPath = 0;

        // until this character has 0 moves, he will try to get closer to an specific enemy character
        
        // selected target to get closer
        int targetIndex = advancedSelectTarget(engine.getState(), selectedIndex);
        // Character &targetToGetCloser = *engine.getState().getCharacters()[targetIndex];
        
        // localize source and target mapnodes
        MapNode &source = mapNodes[findMapNodeIndex(engine.getState(), selectedIndex)];
        MapNode &target = mapNodes[findMapNodeIndex(engine.getState(), targetIndex)];

        // call algorithm to choose shortest path
        vector<MapNode> path = callShortestPath(source, target);

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

int DeepAI::advancedSelectCharacter(state::State &state){
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
bool DeepAI::initMapNodes(state::State &state){
    // raw init
    int k = 0;
    for(unsigned int i = 0; i < state.getMap().size(); i++){
        for(unsigned j = 0; j < state.getMap()[i].size(); j++){
            mapNodes.push_back(MapNode{state.getMap()[i][j]->getPosition().getX(), 
                                state.getMap()[i][j]->getPosition().getY(), k, !state.getMap()[i][j]->isSpace()});
            if(state.getMap()[i][j]->isOccupied(state) != -1)
                mapNodes.back().setOcuppied(true);
            k++;
        }
    }

    // nears (can be optimized but like this it takes miliseconds and just one time.)
    for (auto &mn : mapNodes)
        for (auto &line : state.getMap())
            for (auto &mc : line)
                // match MapNode with MapCell
                if (mn.getX() == mc->getPosition().getX() && mn.getY() == mc->getPosition().getY())
                {
                    for (auto &nearPos : mc->getPosition().getNearPositions())
                        // if nearPos is within mapNodes
                        for (auto &mn2 : mapNodes)
                            // I can not be a near, continue
                            if (mn2.getX() == mn.getX() && mn2.getY() == mn.getY())
                                continue;
                            else if (mn2.getX() == nearPos.getX() && mn2.getY() == nearPos.getY())
                                mn.addNear(&mn2);
                }
    
    return true;
}

// update occupied state from each MapNode to use it in shortestPath
// the idea is that if a mapCell is occupied, it's like a momentary obstacle.
void DeepAI::updateMapNodes(State &state){
    for(auto &mapNode : mapNodes){
        mapNode.setOcuppied(false);
        for(auto &charac : state.getCharacters())
            if(charac->getPosition().getX() == mapNode.getX()
            && charac->getPosition().getY() == mapNode.getY())
                mapNode.setOcuppied(true);
    }
}

int DeepAI::advancedSelectTarget(State& state, int selectedCharacIndex){
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

int DeepAI::findMapNodeIndex(State& state, int characterIndex){
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
list<MapNode> DeepAI::shortestPath(MapNode &source, MapNode &target)
{
    list<MapNode> explored;
    
    queue<list<MapNode>> paths;
    list<MapNode> first;
    first.push_back(source);
    paths.push(first);

    // check if source == target

    // bfs
    while(paths.size() > 0){
        //pop
        list<MapNode> path = paths.front();
        paths.pop();

        MapNode node = path.back();
        bool alreadyVisited = false;
        for(auto &visited : explored)
            if(node.id == visited.id)
                alreadyVisited = true;

        if(!alreadyVisited && !node.isObstacle){
            vector<MapNode *> nears = node.getNears();
            for(auto &near : nears){
                bool nearVisited = false;
                for(auto &visited : explored)
                    if(visited.id == near->id)
                        nearVisited = true;
                
                if(!nearVisited && !near->isObstacle){
                    list<MapNode> new_path(path);
                    new_path.push_back(*near);
                    paths.push(new_path);

                    // if near is target
                    if(near->id == target.id)
                        return new_path;
                }
            }
            explored.push_back(node);
        }
    }
    return first;
}

// this IS NOT the algorithm, just some parsing and stuff.
std::vector<MapNode> DeepAI::callShortestPath(MapNode &source, MapNode &target)
{
    list<MapNode> path = shortestPath(source,target);
    vector<MapNode> result;
    
    for(auto &node : path){
        // continue if its first or last position of path
        if(node.id == path.front().id || node.id == path.back().id)
            continue;
        MapNode n{node.getX(), node.getY(), node.id, node.isObstacle};
        result.push_back(n);
    }
    return result;
}



// int DeepAI::evaluate(engine::Engine& engine){
// 	int returnValue;
// 	if(engine.getState().getEnd()){
//         // atention
// 		if(engine.getState().getTurnOwner()){
// 			returnValue=100-engine.getState().getTurn();
// 		}
// 		else{
// 			returnValue=-100+engine.getState().getTurn();
// 		}
// 	}
// 	else{
// 		int totalPV=0, totalPVEnnemy=0, nbCharacterAlive=0, nbCharaterAliveEnnemy=0;
// 		for(size_t i=0; i<engine.getState().getCharacters().size(); i++){
//             // atention
// 			if(engine.getState().getCharacters()[i]->getPlayerOwner()){
// 				totalPVEnnemy+=engine.getState().getCharacters()[i]->getStats().getHealth();
// 				nbCharaterAliveEnnemy+=1;
// 			}
// 			else{
// 				totalPV+=engine.getState().getCharacters()[i]->getStats().getHealth();
// 				nbCharacterAlive+=1;
// 			}	
// 		}
// 		returnValue=totalPV-totalPVEnnemy+100*nbCharacterAlive-100*nbCharaterAliveEnnemy;
// 	}
// 	return returnValue;
// }