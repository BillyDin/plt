#include "ai.h"
#include "engine.h"
#include "state.h"
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

using namespace ai;
using namespace engine;
using namespace state;
using namespace std;


void RandomAI::run (engine::Engine& engine){
}


std::unique_ptr<state::Character> selectCharacter (state::State& state, int playerNumber){
    
    int randomNumber;
    std::vector<std::unique_ptr<Character>> possibleCharacters;
    srand(time(NULL));

    for( auto &character : state.getCharacters()){
        if(playerNumber== character->getPlayerOwner() && character->getStatus() != DEATH )
            possibleCharacters.push_back(move(character));
    }
    randomNumber = rand() % possibleCharacters.size(); 
    return possibleCharacters[randomNumber];
}
 
state::Character& chooseTarget (state::State& state){


}