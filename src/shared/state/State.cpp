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
	delete ptr_cursor;
	ptr_cursor=nullptr;
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

void State::initializeCharacters(){
    Character c1 (STRENGHT, true, "Shaker", 0, 0);
    unique_ptr<Character> ptrC1(new Character(c1));
    characters.push_back(move(ptrC1));

    Character c2 (DISTANCE, true, "Miss Ranger", 1, 0);
    unique_ptr<Character> ptrC2(new Character(c2));
    characters.push_back(move(ptrC2));

    Character c3 (MAGICIAN, true, "Witch Doctor", 0, 1);
    unique_ptr<Character> ptrC3(new Character(c3));
    characters.push_back(move(ptrC3));
}

void State::initializeMapCell(){
    // for tests purposes
    // a 2 lines map. s => space; o => obstacle
    /* 
        s s
        s o 
    */
    vector<unique_ptr<MapCell>> line1;

    SpaceMapCell smp1{SpaceMapCellID::SAND, 0, 0};
    unique_ptr<SpaceMapCell> ptrSmp1(new SpaceMapCell(smp1));
    line1.push_back(move(ptrSmp1));

    SpaceMapCell smp2{SpaceMapCellID::CONCRETE, 1, 0};
    unique_ptr<SpaceMapCell> ptrSmp2(new SpaceMapCell(smp2));
    line1.push_back(move(ptrSmp2));

    vector<unique_ptr<MapCell>> line2;

    SpaceMapCell smp3{SpaceMapCellID::SAND, 0, 1};
    unique_ptr<SpaceMapCell> ptrSmp3(new SpaceMapCell(smp3));
    line1.push_back(move(ptrSmp3));

    ObstacleMapCell omp1{ObstacleMapCellID::TREE, 1, 1};
    unique_ptr<ObstacleMapCell> ptrOmp1(new ObstacleMapCell(omp1));
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