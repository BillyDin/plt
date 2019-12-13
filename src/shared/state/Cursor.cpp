#include "Cursor.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <memory>
#include <vector>

using namespace std;
using namespace state;

Cursor::Cursor(int x, int y, int p_tileCode)
{
    this->lastPosition.setX(x);
    this->lastPosition.setY(y);
    this->position.setX(x);
    this->position.setY(y);
    tileCode = p_tileCode;
    visible = false;
}

Cursor* Cursor::clone(){
    return new Cursor(*this);
}

bool Cursor::isMapCell()
{
    return false;
}

state::Position& Cursor::getLastPosition(){
    return this->lastPosition;
}

void Cursor::move(Position &destination)
{
    this->lastPosition = position;
    this->position = destination;
}
bool Cursor::getVisible (){
    return visible;
}
void Cursor::setVisible (bool visibility){
    this->visible = visibility;
}
// Setters and Getters