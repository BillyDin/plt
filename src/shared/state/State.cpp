#include "State.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <memory>
#include <vector>
#include "SpaceMapCell.h"
#include "ObstacleMapCell.h"
#include <map>

using namespace std;
using namespace state;

// Operations
State::State()
{
    Cursor c{0, 0};
    this->cursor = c;
    std::cout << "Creating an state object\n";
}

State::~State()
{
}

std::vector<std::unique_ptr<Character>> &State::getCharacters()
{
    vector<unique_ptr<Character>> &refCharacters = characters;
    return refCharacters;
}

std::vector<std::vector<std::unique_ptr<MapCell>>> &State::getMap()
{
    vector<vector<unique_ptr<MapCell>>> &refMapCell = map;
    return refMapCell;
}

int State::getTurn()
{
    return turn;
}

bool State::getEnd()
{
    return end;
}

Cursor &State::getCursor()
{
    Cursor &refCursor = cursor;
    return refCursor;
}

void State::initializeCharacters()
{
    std::unique_ptr<Character> ptrC1(new Character(STRENGHT, true, "Soldier", 0, 0, 24));
    characters.push_back(move(ptrC1));

    std::unique_ptr<Character> ptrC2(new Character(DISTANCE, true, "Miss Ranger", 10, 10, 12));
    characters.push_back(move(ptrC2));

    std::unique_ptr<Character> ptrC3(new Character(MAGICIAN, true, "Witch Doctor", 0, 1, 0));
    characters.push_back(move(ptrC3));
    cout << "finished\n";
}

void State::initializeMapCell()
{
    int i, j, k = 0;
    // mapping (this will be dependent on the choosed resource)
    std::map<int, SpaceMapCellID> mapp_spaces;
    std::map<int, ObstacleMapCellID> mapp_obstacles;
    
    mapp_spaces[118] = SAND;
    mapp_spaces[119] = SAND;
    mapp_spaces[120] = SAND;
    mapp_spaces[141] = SAND;
    mapp_spaces[142] = SAND;
    mapp_spaces[143] = SAND;
    mapp_spaces[164] = SAND;
    mapp_spaces[165] = SAND;
    mapp_spaces[166] = SAND;
    mapp_spaces[520] = SAND;
    mapp_spaces[566] = SAND;
    mapp_spaces[39] = CONCRETE;
    mapp_spaces[40] = CONCRETE;
    mapp_spaces[43] = CONCRETE;
    mapp_spaces[44] = CONCRETE; // red
    mapp_spaces[31] = CONCRETE; // actually is wood
    mapp_spaces[54] = CONCRETE; // actually is wood
    mapp_spaces[55] = CONCRETE; // actually is wood
    mapp_spaces[77] = CONCRETE; // actually is wood

    mapp_obstacles[496] = WATER;
    mapp_obstacles[497] = WATER;

    int map_tile[] = {
118,	119,	119,	119,	119,	119,	119,	119,	119,	119,	119,	119,	119,	119,	119,	119,	119,	119,	119,	119,	119,	119,	119,	119,	120,
141,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	143,
141,	142,	142,	142,	39,	    142,	142,	142,	142,	142,	142,	142,	39,	    142,	142,	142,	142,	142,	142,	142,	39,	    142,	142,	142,	143,
141,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	143,
141,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	143,
141,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	143,
141,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	143,
141,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	143,
141,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	143,
520,	31,     31,	    520,	520,	520,	520,	520,	520,	520,	54,	    54,	    31,	    31,	    31,	    520,	520,	520,	520,	520,	520,	520,	31,	    31,	    520,
496,	54,     54,	    496,	496,	496,	496,	496,	496,	496,	54,	    54,	    54,	    54,	    54,	    496,	496,	496,	496,	496,	496,	496,	54,	    54,	    496,
566,	77,     77,	    566,	566,	566,	566,	566,	566,	566,	54,	    54,	    77,	    77,	    77,	    566,	566,	566,	566,	566,	566,	566,	77,	    77,	    566,
141,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	143,
141,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	143,
141,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	143,
141,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	143,
141,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	143,
141,	142,	142,	142,	43,	    142,	142,	142,	142,	142,	142,	142,	43,	    142,	142,	142,	142,	142,	142,	142,	43,	    142,	142,	142,	143,
141,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	142,	143,
164,	165,	165,	165,	165,	165,	165,	165,	165,	165,	165,	165,	165,	165,	165,	165,	165,	165,	165,	165,	165,	165,	165,	165,	166
    };
    for (i = 0; i < 20; i++)
    {
        std::vector<std::unique_ptr<MapCell>> newLigne;
        for (j = 0; j < 25; j++)
        {
            if (map_tile[k] >= 0 && map_tile[k])
            {
                if (mapp_spaces.find(map_tile[k]) != mapp_spaces.end())
                {
                    std::unique_ptr<SpaceMapCell> spc(new SpaceMapCell(mapp_spaces[map_tile[k]], i, j, map_tile[k]));
                    newLigne.push_back(move(spc));
                }
                else if (mapp_obstacles.find(map_tile[k]) != mapp_obstacles.end())
                {
                    std::unique_ptr<ObstacleMapCell> obs(new ObstacleMapCell(mapp_obstacles[map_tile[k]], i, j, map_tile[k]));
                    newLigne.push_back(move(obs));
                } else {
                    std::unique_ptr<SpaceMapCell> spc(new SpaceMapCell(mapp_spaces[map_tile[118]], i, j, map_tile[118]));
                    newLigne.push_back(move(spc));
                }
            }
            k++;
        }
        map.push_back(move(newLigne));
    }
}

void State::setTurn(int newTurn)
{
    this->turn = newTurn;
}

void State::setEnd(bool result)
{
    this->end = result;
}

// Setters and Getters