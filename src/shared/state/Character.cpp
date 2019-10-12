#include "Character.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <memory>
#include <vector>

using namespace std;
using namespace state;

Character::Character(CharacterTypeID id, bool newIsInBase, std::string newNom, int newY, int newX)
{
    typeID = id;
    isInBase = newIsInBase;
    status = AVAILABLE;
    name = newNom;
    position.setX(newX);
    position.setY(newY);

    if (id == DISTANCE)
    {
        characterMove = 3;
        characterAttack = 2;
        stats.setHealth(100);
        stats.setAttack(50);
        stats.setDefense(30);
        stats.setMana(50);
    }

    else if (id == STRENGHT)
    {
        characterMove = 1;
        characterAttack = 1;
        stats.setHealth(100);
        stats.setAttack(50);
        stats.setDefense(80);
        stats.setMana(20);
    }

    else if (id == MAGICIAN)
    {
        characterMove = 2;
        characterAttack = 2;
        stats.setHealth(100);
        stats.setAttack(70);
        stats.setDefense(10);
        stats.setMana(100);
    }
}

int Character::getCharacterMove(){
    return characterMove;
}

bool Character::getIsInBase(){
    return isInBase;
}
CharacterStatusID Character::getStatus(){
    return status;
}

int Character::getCharacterAttack(){
    return characterAttack;
}
CharacterTypeID Character::getType(){
    return typeID;
}
void Character::setStatus(CharacterStatusID newStatus){
    this->status = newStatus;
}
void Character::setCharacterMove(int newCharacterMove){
    this->characterMove = newCharacterMove;
}

bool Character::isMapCell(){
    return false;
}

int Character::getBoosted(){
    return boosted;
}

void Character::setBoosted(int numberOfTurns){
    this->boosted = numberOfTurns; // number of turns being boosted
}

Stats &Character::getStats(){
    Stats & refStats = stats;
    return refStats;
}
// Setters and Getters
void Character::setIsInBase(bool isInBase){
    this->isInBase = isInBase;
}

void Character::setCharacterAttack(int characterAttack){
    this->characterAttack = characterAttack;
}

CharacterTypeID Character::getTypeID() const{
    return typeID;
}
void Character::setTypeID(CharacterTypeID typeID){
    this->typeID = typeID;
}
void Character::setStats(const Stats &stats){
    this->stats = stats;
}