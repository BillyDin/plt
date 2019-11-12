#include "Position.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <memory>
#include <vector>

using namespace std;
using namespace state;

Position::Position(){
	this->x=0;
	this->y=0;
}

Position::Position(int x, int y){
	this->x=x;
	this->y=y;
}

int Position::getX(){
    return this->x;
}

int Position::getY(){
    return this->y;
}

void Position::setX(int newX){
    this->x = newX;
}

void Position::setY(int newY){
    this->y = newY;
}

bool Position::equals(Position& other){
	return (this->x == other.getX() && this->y == other.getY()); 
}

int Position::distance(Position& other){
    // |x - otherX| + |y - otherY|
	return abs(x - other.getX()) + abs(y - other.getY());
}

std::vector<Position> Position::getNearPositions(){
	// 4 positions, near
	// here we doesnt validate if he is within the map.
	vector<Position> result;
	Position north{this->getX(), this->getY() + 1};
	Position south{this->getX(), this->getY() - 1};
	Position west{this->getX() - 1, this->getY()};
	Position east{this->getX() + 1, this->getY()};
	result.push_back(move(north));
	result.push_back(move(south));
	result.push_back(move(west));
	result.push_back(move(east));

	return result;
}