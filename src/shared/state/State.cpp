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
    std::unique_ptr<Character> ptrC1(new Character(c1));
    characters.push_back(ptrC1);

    Character c2 (DISTANCE, true, "Miss Ranger", 1, 1);
    std::unique_ptr<Character> ptrC2(new Character(c2));
    characters.push_back(ptrC2);

    Character c3 (MAGICIAN, true, "Witch Doctor", 2, 2);
    std::unique_ptr<Character> ptrC3(new Character(c3));
    characters.push_back(ptrC3);
}

void State::initializeMapCell(){
    
}

void State::setTurn(int newTurn){
    this->turn = newTurn;
}

void State::setEnd(bool result){
    this->end = result;
}

// Setters and Getters