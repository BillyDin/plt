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
    this->position.setX(x);
    this->position.setY(y);
    tileCode = p_tileCode;
}

bool Cursor::isMapCell()
{
    return false;
}

void Cursor::move(Position &destination)
{
    this->position = destination;
}
// Setters and Getters