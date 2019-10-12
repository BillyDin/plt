#include "ObstacleMapCell.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <memory>
#include <vector>

using namespace std;
using namespace state;

ObstacleMapCell::ObstacleMapCell(ObstacleMapCellID id, int newX, int newY){
    this->typeID = id;
    this->position.setX(newX);
    this->position.setY(newY);
}

ObstacleMapCellID ObstacleMapCell::getObstacleMapCellID(){
    return this->typeID;
}