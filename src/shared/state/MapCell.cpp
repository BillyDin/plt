#include "Character.h"
#include "MapCell.h"
#include "State.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <memory>
#include <vector>

using namespace std;
using namespace state;

MapCell::MapCell(int newX, int newY){
    this->position.setX(newX);
    this->position.setY(newY);
}

bool MapCell::isMapCell(){
    return true;
}

// Returns the position of a character in the characters list
// if is in this position. Else it returns -1
int MapCell::isOccupied(State& state){
    int res = -1;
    
    // iterate the character lists to check if one is in this mapcell
	vector<std::unique_ptr<Character>> & charactersList = state.getCharacters();
    for (size_t i = 0; i < charactersList.size(); i++)
    {
        res = (position.equals(charactersList[i]->getPosition())) ? i : -1;
        if (res != -1) break;
    }
}

// Setters and Getters