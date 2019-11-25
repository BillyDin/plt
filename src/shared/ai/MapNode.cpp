#include "MapNode.h"
#include <iostream>
#include <stdlib.h>
#include <unistd.h>

using namespace ai;
using namespace std;

MapNode::MapNode(int x, int y){
    this->x = x;
    this->y = y;
}

vector<MapNode *> MapNode::getNears(){
    return nears;
}

bool MapNode::addNear (MapNode* near){
    nears.push_back(near);
    return true;
}

int MapNode::getX(){
    return x;
}

bool MapNode::getOccupied (){
    return occupied;
}

void MapNode::setOcuppied (bool newOccup){
    this->occupied = newOccup;
}

int MapNode::getY(){
    return y;
}

void MapNode::setX(int newX){
    this->x = newX;
}

void MapNode::setY(int newY){
    this->y = newY;
}