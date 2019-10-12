#include "SpaceMapCell.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <memory>
#include <vector>

using namespace std;
using namespace state;

SpaceMapCell::SpaceMapCell(SpaceMapCellID id, int newX, int newY){
    this->typeID = id;
    this->position.setX(newX);
    this->position.setY(newY);
}

SpaceMapCellID SpaceMapCell::getSpaceMapCellID(){
    return this->typeID;
}
bool SpaceMapCell::getIsBoost(){
    return this->isBoost;
}
void SpaceMapCell::setIsBoost(bool newIsBoost){
    this->isBoost = newIsBoost;
}
