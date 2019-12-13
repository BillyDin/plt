#include "AI.h"
#include "state.h"
#include <stdlib.h>
#include <unistd.h>

using namespace ai;
using namespace state;
using namespace std;

AI::AI(int pn){
    this->playerNumber = pn;
}

int AI::getPlayerNumber (){
    return playerNumber;
}

void AI::setPlayerNumber (int pn){
    if(pn == 1 || pn == 2){
        playerNumber = pn;
    }
}