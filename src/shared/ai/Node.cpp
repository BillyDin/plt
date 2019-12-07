#include "Node.h"
#include <iostream>
#include <stdlib.h>
#include <unistd.h>

using namespace ai;
using namespace std;


Node::Node(int index)
{
    this->index = index;
} 

int Node::getIndex()
{
    return index;
}

int Node::getValue()
{
    return value;
}

vector<Node *> Node::getAdjacents()
{
    return adjacents;
}

void Node::setValue(int newValue){
    this->value=newValue;
}

void Node::addAdjacent(Node* adj)
{
    adjacents.push_back(adj);
}
