
#include "VersionService.h"

using namespace server;

VersionService::VersionService () : AbstractService("/version") {
    
}

HttpStatus VersionService::get (Json::Value& out, int id) {
    out["version"] = 0;
    return HttpStatus::OK;
}
