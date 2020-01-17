
#include "GameService.h"
#include "ServiceException.h"
#include <unistd.h>

using namespace server;

GameService::GameService (Game& game) : AbstractService("/game"), game(game) {
    
}

HttpStatus GameService::get (Json::Value& out, int id) {
    if(id == 1){
        // 1 if player1 is ready, 0 if is not the case
        out["ready"] = (game.getPlayers().begin()->second->free == false) ? 1 : 0;
    }
    else if(id == 2){
        // 1 if player2 is ready, 0 if is not the case
        out["ready"] = (game.getPlayers().end()->second->free == false) ? 1 : 0;
    }
    else {
        out["status"] = game.getStatus(); // 1 => CREATING, 2 => RUNNING
    }
    // TODO: think what  to do here
    return HttpStatus::OK;
}
