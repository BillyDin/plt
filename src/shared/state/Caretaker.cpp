#include "Caretaker.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <memory>
#include <vector>

using namespace std;
using namespace state;

void Caretaker::addMemento(MementoState& memento){
    MementoState m{memento};
    savedStates.push_back(m);
}

MementoState& Caretaker::getMemento(int index){
    if (index != -1 && index < savedStates.size()){
        return savedStates[index];
    }
    return savedStates[savedStates.size() - 1];
}