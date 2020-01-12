#include "PlayerService.h"
#include "ServiceException.h"
#include <memory>

using namespace server;
using namespace std;

PlayerService::PlayerService(Game &game) : AbstractService("/player"), game(game)
{
}

HttpStatus PlayerService::get(Json::Value &out, int id)
{
    if(id == -1){
        Json::Value playersArray(Json::arrayValue);
        for(auto& p : game.getPlayers()){
            Json::Value inArray(Json::arrayValue);
            inArray.append(p.first);
            inArray.append(p.second->name);
            playersArray.append(inArray);
        }
        out["players"] = playersArray;
        return HttpStatus::OK;
    }
    Player *player = game.getPlayerById(id);
    if (!player)
        throw ServiceException(HttpStatus::NOT_FOUND, "Invalid player id");
    out["name"] = player->name;
    out["free"] = player->free;
    out["playerNumber"] = (game.getPlayers().begin()->first == id) ? 1 : 2;
    return HttpStatus::OK;
}

HttpStatus PlayerService::put(Json::Value &in, int id)
{
    Player *player = game.getPlayerById(id);
    if (!player)
        throw ServiceException(HttpStatus::NOT_FOUND, "Invalid player id");
    
    unique_ptr<Player> playermod(new Player(*player));
    if (in.isMember("name"))
        playermod->name = in["name"].asString();
    if (in.isMember("free"))
        playermod->free = in["free"].asBool();
    game.modifyPlayer(id, std::move(playermod));
    return HttpStatus::OK;
}

HttpStatus PlayerService::post(Json::Value &out, Json::Value &in)
{
    if (game.getPlayers().size() >= 2)
        throw ServiceException(HttpStatus::OUT_OF_RESOURCES, "Without free places to join");
    
    string name = in["name"].asString();
    bool free = in["free"].asBool();
    Player new_player(name, free);
    std::unique_ptr<Player> ptr_player(new Player(new_player));
    out["id"] = game.addPlayer(move(ptr_player));
    return HttpStatus::CREATED;
}

HttpStatus PlayerService::remove(int id)
{
    Player *player = game.getPlayerById(id);
    
    if (!player)
        throw ServiceException(HttpStatus::NOT_FOUND, "Invalid player id");
    
    game.removePlayer(id);
    return HttpStatus::OK;
}
