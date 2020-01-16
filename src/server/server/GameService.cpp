
#include "GameService.h"
#include "ServiceException.h"
#include <unistd.h>

using namespace server;

GameService::GameService (Game& game) : AbstractService("/game"), game(game) {
    
}

HttpStatus GameService::get (Json::Value& out, int id) {
    if(id == 1){
        bool can = 1;
        for(auto& p : game.getPlayers()){
            if(p.second->free == true) {
                can = 0;
                break;
            }
        }
        if(can) sleep(3);
        out["canStart"] = can;
    }
    else {
        out["status"] = game.getStatus(); // 1 => CREATING, 2 => RUNNING
    }
    // TODO: think what  to do here
    return HttpStatus::OK;
}
