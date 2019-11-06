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
        characterAttack = 2;
        stats.setHealth(100);
        stats.setAttack(50);
        stats.setDefense(30);
        stats.setMana(50);
    }

    else if (id == STRENGHT)
    {
        characterMove = 2;
        characterAttack = 1;
        stats.setHealth(100);
        stats.setAttack(50);
        stats.setDefense(80);
        stats.setMana(20);
    }

    else if (id == MAGICIAN)
    {
        characterMove = 1;
        characterAttack = 2;
        stats.setHealth(100);
        stats.setAttack(70);
        stats.setDefense(10);
        stats.setMana(100);
    }
}

// this algo will check for all the 
// tiles located in north, east, south and west of our character
std::vector<Position> Character::allowedPosToMove(State &state)
{
    std::vector<Position> canGoList;

    // xAxis: one tile west, one tile east
    for (int xAxis = position.getX() - 1; xAxis <= position.getX() + 1; xAxis++)
    {
        // yAxis: one tile north, one tile south
        for (int yAxis = position.getY() - 1; yAxis <= position.getY() + 1; yAxis++)
        {
            // gonna use absolute values,
            // reference https://www.programiz.com/cpp-programming/library-function/cstdlib/abs
            if (
                //only nears
                abs(xAxis - position.getX()) + abs(yAxis - position.getY()) <= 1
                
                //within the map top/left
                && xAxis >= 0 && yAxis >= 0

                //within the map bottom/right
                && abs(xAxis) < state.getMap().size() && abs(yAxis) < state.getMap()[xAxis].size())
            {
                if (state.getMap()[xAxis][yAxis]->isOccupied(state) == -1 && state.getMap()[xAxis][yAxis]->isSpace())
                {
                    Position posHelper;
                    posHelper.setX(xAxis);
                    posHelper.setY(yAxis);
                    canGoList.push_back(posHelper);
                }
            }
        }
    }
    return canGoList;
}

int Character::getCharacterMove()
{
    return characterMove;
}

bool Character::getIsInBase()
{
    return isInBase;
}
CharacterStatusID Character::getStatus()
{
    return status;
}

int Character::getCharacterAttack()
{
    return characterAttack;
}
CharacterTypeID Character::getType()
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

int Character::getBoosted()
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