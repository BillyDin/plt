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
    cout << "ATENTION! " << attacker.getName() << " is trying to attack " << target.getName() << endl;
    // check distance
    int maxDistance = attacker.getCharacterAttackDistance() + 1;
    // if the target is within the max distance of the attacker
    cout << attacker.getPosition().distance(target.getPosition()) << " <= " << maxDistance << endl;
    if (attacker.getPosition().distance(target.getPosition()) <= maxDistance)
    {
        // attacking
        cout << "Actual health of " << target.getName() << ": " << target.getStats().getHealth() << endl;

        // easier to read.
        int baseDamage = attacker.getStats().getAttack();

        int targetHealth = target.getStats().getHealth();

        // we'll work the defense as a coeficient to reduce the final damage
        float targetDefense = target.getStats().getDefense() * 0.01;
        cout << "Defense coef. of the target (" << target.getName() << ") is: " << targetDefense << endl;
        cout << "Base damage of the attacker (" << attacker.getName() << ") is: " << baseDamage << endl;
        int finalDamage = (1 - targetDefense) * baseDamage;
        cout << "Final given damage: " << finalDamage <<  endl;        

        int finalHealth = targetHealth - finalDamage;

        target.getStats().setHealth(finalHealth);
        if (target.getStats().getHealth() < 1)
            target.setStatus(DEATH);

        cout << "After HIT, the health of " << target.getName() << " descends to: " << target.getStats().getHealth() << endl;
    }
    else{
        cout << "the distances is not good" << endl;
    }
}