#include "ObstacleMapCell.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <memory>
#include <vector>

using namespace std;
using namespace state;

ObstacleMapCell::ObstacleMapCell(ObstacleMapCellID id, int newX, int newY, int p_tileCode)
{
    this->typeID = id;
    this->position.setX(newX);
    this->position.setY(newY);
    tileCode = p_tileCode;
}

ObstacleMapCellID ObstacleMapCell::getObstacleMapCellID()
{
    return this->typeID;
}