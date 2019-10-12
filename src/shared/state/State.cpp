#include "State.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <memory>
#include <vector>

using namespace std;
using namespace state;

// Operations
State::State()
{
    std::cout << "Creating an state object\n";
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

void State::setTurn(int newTurn){
    this->turn = newTurn;
}

void State::setEnd(bool result){
    this->end = result;
}

// Setters and Getters