#include "State.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <memory>
#include <vector>
#include "SpaceMapCell.h"
#include "ObstacleMapCell.h"
using namespace std;
using namespace state;

// Operations
State::State()
{

    std::cout << "Creating an state object\n";
}

State::~State(){

}

std::vector<std::unique_ptr<Character>> & State::getCharacters(){
    vector<unique_ptr<Character>> & refCharacters = characters;
	return refCharacters;
}

std::vector<std::vector<std::unique_ptr<MapCell>>>& State::getMap(){
    vector<vector<unique_ptr<MapCell>>> & refMapCell = map;
    return refMapCell;
}

int State::getTurn(){
    return turn;
}

bool State::getEnd(){
    return end;
}

Cursor * State::getCursor(){
    return ptr_cursor;
}

int State::initializeCharacters(){
    std::unique_ptr<Character> ptrC1(new Character(STRENGHT, true, "Shaker", 0, 0));
    characters.push_back(move(ptrC1));

    std::unique_ptr<Character> ptrC2(new Character(DISTANCE, true, "Miss Ranger", 1, 0));
    characters.push_back(move(ptrC2));

    std::unique_ptr<Character> ptrC3(new Character(MAGICIAN, true, "Witch Doctor", 0, 1));
    characters.push_back(move(ptrC3));
    cout << "finished\n";
    return 1;
}

void State::initializeMapCell(){
    // for tests purposes
    // a 2 lines map. s => space; o => obstacle
    /* 
        s s
        s o 
    */
    vector<unique_ptr<MapCell>> line1;

    unique_ptr<MapCell> ptrSmp1(new SpaceMapCell(SpaceMapCellID::SAND, 0, 0));
    line1.push_back(move(ptrSmp1));

    unique_ptr<MapCell> ptrSmp2(new SpaceMapCell(SpaceMapCellID::CONCRETE, 1, 0));
    line1.push_back(move(ptrSmp2));

    vector<unique_ptr<MapCell>> line2;

    unique_ptr<MapCell> ptrSmp3(new SpaceMapCell(SpaceMapCellID::SAND, 0, 1));
    line1.push_back(move(ptrSmp3));

    unique_ptr<MapCell> ptrOmp1(new ObstacleMapCell(ObstacleMapCellID::TREE, 1, 1));
    line1.push_back(move(ptrOmp1));

    map.push_back(move(line1));
    map.push_back(move(line2));
}

void State::setTurn(int newTurn){
    this->turn = newTurn;
}

void State::setEnd(bool result){
    this->end = result;
}

// Setters and Getters