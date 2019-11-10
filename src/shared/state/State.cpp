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
    int centerX = 12;
    // no more random pos
    std::unique_ptr<Character> ptrC1(new Character(STRENGHT, true, "Soldier", 2, centerX, 1, 24));
    characters.push_back(move(ptrC1));

    std::unique_ptr<Character> ptrC2(new Character(DISTANCE, true, "Miss Ranger", 17, centerX, 2, 12));
    characters.push_back(move(ptrC2));

    // std::unique_ptr<Character> ptrC3(new Character(MAGICIAN, true, "Witch Doctor", (rand() % 18 + 1), (rand() % 23 + 1), 0));
    // characters.push_back(move(ptrC3));
    cout << "characters inits finished\n";
}

void State::initializeMapCell()
{
    // Iteration helpers
    unsigned int i = 0, j = 0, k = 0;

    // mapping (this will be dependent on the choosed resource)
    // dictionary to signalize the type of each tileset by his id
    // (tile id defined by the position of the tile in de resource, we dont define it)
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
    mapp_spaces[44] = CONCRETE;
    mapp_spaces[31] = CONCRETE; // actually is wood
    mapp_spaces[54] = CONCRETE; // actually is wood
    mapp_spaces[55] = CONCRETE; // actually is wood
    mapp_spaces[77] = CONCRETE; // actually is wood

    mapp_obstacles[496] = WATER;
    mapp_obstacles[497] = WATER;

    std::ifstream file("res/map_v0.txt", ios::in);
    int map_tile[25 * 20];

    //this way we read the txt file.
    std::string content, line, tilecode;
    if (!file)
        return;
    while (getline(file, line))
    {
        line += ",";
        content += line;
    }
    cout << "--- file read succesfully ---" << endl;

    file.close();
    cout << "--- file closed succesfully ---" << endl;

    // from string to stream
    std::stringstream contentStream(content);
    cout << "--- from string to stream succesfully ---" << endl;

    // delimiting by comma
    while (std::getline(contentStream, tilecode, ','))
    {
        map_tile[i] = std::stoi(tilecode);
        i++;
    }
    cout << "--- building map_tile array succesfully ---" << endl;


    for (i = 0; i < 20; i++)
    {
        std::vector<std::unique_ptr<MapCell>> newline;
        for (j = 0; j < 25; j++)
        {
            cout << map_tile[k] << ",";
            if (map_tile[k] >= 0 && map_tile[k])
            {
                if (mapp_spaces.find(map_tile[k]) != mapp_spaces.end())
                {
                    std::unique_ptr<SpaceMapCell> spc(new SpaceMapCell(mapp_spaces[map_tile[k]], i, j, map_tile[k]));
                    newline.push_back(move(spc));
                }
                else if (mapp_obstacles.find(map_tile[k]) != mapp_obstacles.end())
                {
                    std::unique_ptr<ObstacleMapCell> obs(new ObstacleMapCell(mapp_obstacles[map_tile[k]], i, j, map_tile[k]));
                    newline.push_back(move(obs));
                }
                else
                {
                    std::unique_ptr<SpaceMapCell> spc(new SpaceMapCell(mapp_spaces[map_tile[118]], i, j, map_tile[118]));
                    newline.push_back(move(spc));
                }
            }
            k++;
        }
        cout << endl;
        map.push_back(move(newline));
    }
    cout << "--- map created succesfully ---" << endl;

}

void State::setTurn(int newTurn)
{
    this->turn = newTurn;
}

void State::setEnd(bool result)
{
    this->end = result;
}

int State::getTurnOwner (){
    return turnOwner;
}
void State::setTurnOwner (int newTurnOwner){
    if(newTurnOwner == 1 || newTurnOwner == 2)
        this->turnOwner = newTurnOwner;
    else
        cout << "error, State::setTurnOwner(int newTurnOwner) just allow 1 or 2 as param" << endl;
}

// Setters and Getters