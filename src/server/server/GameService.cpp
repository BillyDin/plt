
#include "GameService.h"
#include "ServiceException.h"

using namespace server;

GameService::GameService (Game& game) : AbstractService("/game"), game(game) {
    
}

HttpStatus GameService::get (Json::Value& out, int id) {
    // TODO: think what  to do here
    out["status"] = game.getStatus(); // 1 => CREATING, 2 => RUNNING
    return HttpStatus::OK;
}
