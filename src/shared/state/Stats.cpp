#include "Stats.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <memory>
#include <vector>

using namespace std;
using namespace state;

int Stats::getHealth()
{
    return this->health;
}
int Stats::getAttack()
{
    return this->attack;
}
int Stats::getDefense()
{
    return this->defense;
}
int Stats::getMana()
{
    return this->mana;
}
void Stats::setHealth(int newHealth)
{
    this->health = newHealth;
}
void Stats::setAttack(int newAttack)
{
    this->attack = newAttack;
}
void Stats::setDefense(int newDefense)
{
    this->defense = newDefense;
}
void Stats::setMana(int newMana)
{
    this->mana = newMana;
}