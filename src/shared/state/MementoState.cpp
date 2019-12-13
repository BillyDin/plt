#include "MementoState.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <memory>
#include <vector>

using namespace std;
using namespace state;

MementoState::MementoState(State& state) : state(state){
}

State& MementoState::recoverState(){
    return state;
}