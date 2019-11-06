#include "AttackCommand.h"
// to use console
#include <iostream>

using namespace engine;
using namespace state;
using namespace std;

AttackCommand::AttackCommand(state::Character &newAttacker, state::Character &newTarget) : attacker(newAttacker), target(newTarget)
{
    id = ATTACK;
}

void AttackCommand::execute(state::State &state){
    // attacking
    cout << "ATENTION! " << attacker.getName() << " is trying to attack " << target.getName() << endl;

}