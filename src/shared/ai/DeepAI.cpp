#include "DeepAI.h"
#include "state.h"
#include "engine.h"
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <queue>
#include <list>
#include <limits>

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
    
    Node root{-1};
    deepNodes.push_back(root);

    for (size_t i = 0; i < state.getCharacters().size(); i++)
    {
        //mine
        if(state.getCharacters()[i]->getPlayerOwner() == playerNumber){
            Node node{i};
            node.setParent(root);
            root.addAdjacent(node);
            deepNodes.push_back(node);
        }
    }

    for (auto &n : deepNodes)
    {
        if(n.getIndex() == -1) continue;

        for (size_t i = 0; i < state.getCharacters().size(); i++)
        {
            //enemies
            if (state.getCharacters()[i]->getPlayerOwner() != playerNumber)
            {
                Node node{i};
                node.setParent(n);
                n.addAdjacent(node);
                deepNodes.push_back(node);
            }
        }
    }
    return true;
}

void DeepAI::run(engine::Engine &engine){



    cout << "run deep ia" << endl;
    updateMapNodes(engine.getState());
    int dangerSituationIndex = evaluateEscape(engine.getState());
    if (dangerSituationIndex == -1)
    {
        int selectedIndex = evaluateCharacter(engine.getState());

        // we do the best choice basing us in the distance between characters.
        Character &selectedChar = *engine.getState().getCharacters()[selectedIndex];
        unique_ptr<Command> selectCommand(new SelectCharacterCommand(selectedChar));
        engine.addCommand(move(selectCommand));

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
        else
        {
            // can't attack. let's move until attack or moves chances == 0
            size_t movesLeft = selectedChar.getCharacterMove();
            size_t nextPosInPath = 0;

            // until this character has 0 moves, he will try to get closer to an specific enemy character

            // selected target to get closer
            int targetIndex = evaluateTarget(engine.getState(), selectedIndex);
            // Character &targetToGetCloser = *engine.getState().getCharacters()[targetIndex];

            // localize source and target mapnodes
            MapNode &source = mapNodes[findMapNodeIndex(engine.getState(), selectedIndex)];
            MapNode &target = mapNodes[findMapNodeIndex(engine.getState(), targetIndex)];

            // call algorithm to choose shortest path
            vector<MapNode> path = callShortestPath(source, target);

            while (movesLeft > 0 && nextPosInPath < path.size())
            {
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
    else{
        Character &selectedChar = *engine.getState().getCharacters()[dangerSituationIndex];
        unique_ptr<Command> selectCommand(new SelectCharacterCommand(selectedChar));
        engine.addCommand(move(selectCommand));

        // can't attack. let's move until attack or moves chances == 0
            size_t movesLeft = selectedChar.getCharacterMove();
            size_t nextPosInPath = 0;

            // until this character has 0 moves, he will try to get closer to an specific enemy character

            // selected target to get closer
            int targetIndex = findBestAlly(engine.getState(), dangerSituationIndex);
            // Character &targetToGetCloser = *engine.getState().getCharacters()[targetIndex];

            // localize source and target mapnodes
            MapNode &source = mapNodes[findMapNodeIndex(engine.getState(), dangerSituationIndex)];
            MapNode &target = mapNodes[findMapNodeIndex(engine.getState(), targetIndex)];

            // call algorithm to choose shortest path
            vector<MapNode> path = callShortestPath(source, target);

            while (movesLeft > 0 && nextPosInPath < path.size())
            {
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
            }
            unique_ptr<Command> finTurnCmd(new FinishTurnCommand());
            engine.addCommand(move(finTurnCmd));
            engine.update();
            return;
    }
}

int DeepAI::findBestAlly (state::State& state, int index){
    int bestChoiceIndex = -1;
    float bestChoice = 0;
    for (size_t i = 0; i < state.getCharacters().size(); i++)
    {
        if(index != i && state.getCharacters()[i]->getPlayerOwner() == playerNumber && state.getCharacters()[i]->getStatus() != DEATH){
            if(state.getCharacters()[i]->getStats().getHealthWithDefense() > bestChoice){
                bestChoiceIndex = i;
                bestChoice = state.getCharacters()[i]->getStats().getHealthWithDefense();
            }
        }
    }
    return bestChoiceIndex;
}

int DeepAI::evaluateEscape(state::State &state){
    int charactersAlive = 0;

    for(auto& charac : state.getCharacters()){
        if(charac->getPlayerOwner() == playerNumber && charac->getStatus() != DEATH) charactersAlive++;
    }

    vector<int> dangerSituations;
    for (size_t i = 0; i < state.getCharacters().size(); i++)
    {
        float realHealth = state.getCharacters()[i]->getStats().getHealthWithDefense();
        //my chars
        if(state.getCharacters()[i]->getPlayerOwner() == playerNumber && state.getCharacters()[i]->getStatus() != DEATH){
            for(auto &enemy : state.getCharacters()){
                if(enemy->getPlayerOwner() != playerNumber){
                    if(enemy->getStatus() != DEATH)
                        if((realHealth + (5 * state.getCharacters()[i]->getPosition().distance(enemy->getPosition()))) < DANGER_SITUATION)
                            dangerSituations.push_back(i);
                }
            }
        }
    }
    if(!dangerSituations.empty() && charactersAlive > 1) return dangerSituations[0];
    else return -1;
}

int DeepAI::evaluateCharacter(state::State &state){
    int index = -1;
    float globalValue = 0;

    // evaluation
    for (size_t i = 0; i < state.getCharacters().size(); i++)
    {
        if(state.getCharacters()[i]->getPlayerOwner() == playerNumber && state.getCharacters()[i]->getStatus() != DEATH){
            const float realHealth = state.getCharacters()[i]->getStats().getHealthWithDefense();
            float myBestChoice = realHealth;
            for(auto& enemy : state.getCharacters()){
                if(enemy->getPlayerOwner() != playerNumber){
                    float distance = 0.5 * state.getCharacters()[i]->getPosition().distance(enemy->getPosition());
                    if((realHealth - distance) > myBestChoice){
                        myBestChoice = realHealth - distance;
                    }
                }
            }
            // here we have the best value

            if(myBestChoice > globalValue){
                globalValue = myBestChoice;
                index = i;
            }
        }
    }
    
    return index;
}

int DeepAI::evaluateTarget(State& state, int selectedCharacIndex){
    int enemiesQuantity = 0;
    for(auto& enemy : state.getCharacters()){
        if(enemy->getPlayerOwner() != playerNumber && enemy->getStatus() != DEATH) enemiesQuantity++;
    }

    const float myRealHealth = state.getCharacters()[selectedCharacIndex]->getStats().getHealthWithDefense();
    float bestChoice = numeric_limits<float>::max();
    int indexEnemy = -1;
    for (size_t i = 0; i < state.getCharacters().size(); i++)
    {
        // each enemy
        if(state.getCharacters()[i]->getPlayerOwner() != playerNumber && state.getCharacters()[i]->getStatus() != DEATH){

            if(enemiesQuantity > 1 
                && myRealHealth < state.getCharacters()[i]->getStats().getHealthWithDefense()) 
                continue;

            if(state.getCharacters()[i]->getStats().getHealthWithDefense() < bestChoice){
                bestChoice = state.getCharacters()[i]->getStats().getHealthWithDefense();
                indexEnemy = i;
            }
        }
    }
    
    return indexEnemy;
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