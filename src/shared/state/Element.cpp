#include "Element.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <memory>
#include <vector>

using namespace std;
using namespace state;

Element::Element()
{
    name = "";
    position.setX(0);
    position.setY(0);
}

Element::~Element(){
}

std::string Element::getName()
{
    return name;
}

Position &Element::getPosition()
{
    Position &refPosition = position;
    return refPosition;
}

bool Element::equals(Element& other){
	return (this->name == other.getName() && this->position.equals(other.getPosition()));
}

// Setters and Getters

void Element::setPosition(const Position &position)
{
    this->position = position;
}
