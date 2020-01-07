#include "Game.h"
#include <memory>
#include <map>
#include <vector>
#include <set>

using namespace server;
using namespace std;
using std::unique_ptr;
using std::map;
using std::vector;


Player& Game::getPlayerById (int id){};

const std::vector<Player>& Game ::getPlayers() const {
    return players;
}

void Game::setPlayers(const std::vector<Player>& players){
    this->players = players;
}

