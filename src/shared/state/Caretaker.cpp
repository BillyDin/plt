#include "Caretaker.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <memory>
#include <vector>

using namespace std;
using namespace state;

void Caretaker::addMemento(MementoState& memento){
    this->savedStates.push_back(memento);
}

MementoState& Caretaker::getMemento(int index){
    if (index != -1 && index < savedStates.size()){
        auto it1 = std::next(savedStates.begin(), index);
        return *it1;
    }
    return savedStates.back();
}