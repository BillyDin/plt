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
    //TODO check for allowed attacks
    // attacking
    cout << "ATENTION! " << attacker.getName() << " is trying to attack " << target.getName() << endl;
    cout << "Actual health of " << target.getName() << ": " << target.getStats().getHealth() << endl;
    target.getStats().setHealth(target.getStats().getHealth() - 20);
    cout << "After HIT, the health of " << target.getName() << " descends to: " << target.getStats().getHealth() << endl;
}