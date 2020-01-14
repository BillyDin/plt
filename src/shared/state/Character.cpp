#include "Character.h"
#include "State.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <memory>
#include <vector>

using namespace std;
using namespace state;

Character::Character(CharacterTypeID id, bool newIsInBase, std::string newNom, int newY, int newX, int newPlayerOwner, int p_tileCode)
{
    
    tileCode = p_tileCode;
    typeID = id;
    isInBase = newIsInBase;
    status = AVAILABLE;
    name = newNom;
    position.setX(newX);
    position.setY(newY);
    playerOwner = newPlayerOwner;

    if (id == DISTANCE)
    {
        characterMove = 3;
        characterAttackDistance = 2;
        stats.setHealth(100);
        stats.setAttack(50);
        stats.setDefense(30);
        stats.setMana(50);
    }

    else if (id == STRENGHT)
    {
        characterMove = 2;
        characterAttackDistance = 0;
        stats.setHealth(100);
        stats.setAttack(49);
        stats.setDefense(50);
        stats.setMana(20);
    }

    else if (id == MAGICIAN)
    {
        characterMove = 4;
        characterAttackDistance = 1;
        stats.setHealth(100);
        stats.setAttack(70);
        stats.setDefense(10);
        stats.setMana(100);
    }
    baseCharacterMove = characterMove;
    baseCharacterActackDistance = characterAttackDistance;
}

Character* Character::clone()
{
    return new Character(*this);
}

int Character::getIndex() const
{
    return index;
}

void Character::setIndex(int ind){
    this->index = ind;
}

int Character::getBaseCharacterMove(){
    return baseCharacterMove;
}

std::vector<int> Character::allowedTargetsToAttack(State &state){
    vector<int> posibleIndexes;
    for(unsigned int i = 0; i < state.getCharacters().size(); i++){
        Character& charac = *state.getCharacters()[i];
        if(charac.getPlayerOwner() != playerOwner && charac.getStatus() != DEATH){
            // check distances
            int maxDistance = characterAttackDistance + 1;
            if(position.distance(charac.getPosition()) <= maxDistance){
                posibleIndexes.push_back(i);
            }
        }
    }
    return posibleIndexes;
}

// this algo will check for all the
// tiles located in north, east, south and west of our character
std::vector<Position> Character::allowedPosToMove(State &state)
{
    std::vector<Position> canGoList;
    std::vector<Position> validNears;

    for (auto &nearPosition : position.getNearPositions())
        // if within map
        if (nearPosition.getY() >= 0 && nearPosition.getX() >= 0 
        && (unsigned int)nearPosition.getX() <= state.getMap()[0].size()
        && (unsigned int)nearPosition.getY() <= state.getMap().size())
            validNears.push_back(move(nearPosition));

    for (auto &validNear : validNears)
    {
        for (auto &line : state.getMap())
        {
            if(line[0]->getPosition().getY() != validNear.getY())
                continue;
            for (auto &mapcell : line)
            {
                if(mapcell->getPosition().getX() != validNear.getX())
                    continue;
                if (mapcell->getPosition().equals(validNear) && mapcell->isSpace() && mapcell->isOccupied(state) == -1)
                    canGoList.push_back(move(mapcell->getPosition()));
            }
        }
    }

    return canGoList;
    //return position.getNearPositions();
}

    // for(auto &line : state.getMap()){
    //     for(auto &mapcell : line){
    //         for(auto &near : position.getNearPositions()){
                
    //             if(mapcell->getPosition().getX() == near.getX() && 
    //                 mapcell->getPosition().getY() == near.getY())
    //                 canGoList.push_back(move(near));
    //         }
    //     }
    // }

int Character::getCharacterMove() const
{
    return characterMove;
}

bool Character::getIsInBase() const
{
    return isInBase;
}
CharacterStatusID Character::getStatus() const
{
    return status;
}

int Character::getCharacterAttackDistance() const
{
    return characterAttackDistance;
}
CharacterTypeID Character::getType() const
{
    return typeID;
}
void Character::setStatus(CharacterStatusID newStatus)
{
    this->status = newStatus;
}
void Character::setCharacterMove(int newCharacterMove)
{
    this->characterMove = newCharacterMove;
}

bool Character::isMapCell()
{
    return false;
}

int Character::getBoosted() const
{
    return boosted;
}

void Character::setBoosted(int numberOfTurns)
{
    this->boosted = numberOfTurns; // number of turns being boosted
}

Stats &Character::getStats() 
{
    Stats &refStats = stats;
    return refStats;
}
// Setters and Getters
void Character::setTypeID(CharacterTypeID typeID)
{
    this->typeID = typeID;
}

int Character::getPlayerOwner() const {
    return playerOwner;
}

void Character::setPlayerOwner(int pPlayerOwner){
    // verifing
    if(pPlayerOwner == 1 || pPlayerOwner == 2)
        this->playerOwner = pPlayerOwner;
    else
        cout << "Function Character::setPlayerOwner(int playerOwner): Player numbers 1 or 2." << endl;
}