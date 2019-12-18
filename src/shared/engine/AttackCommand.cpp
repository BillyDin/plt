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
Json::Value AttackCommand::serialize (){
    Json::Value newCommand;
	newCommand["id"] = id;
	newCommand["player"] = attacker.getPlayerOwner();
	newCommand["attacker"] = attacker.getIndex();
	newCommand["target"] = target.getIndex();
	
	return newCommand;
    
}
void AttackCommand::execute(state::State &state){
    cout << "ATENTION! " << attacker.getName() << " is trying to attack " << target.getName() << endl;
    // check distance
    
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