#include "ServiceException.h"

using namespace server;
using namespace std;

ServiceException::ServiceException(HttpStatus status, string msg)
    : httpStatus(status), msg(msg)
{
}

HttpStatus ServiceException::status()
{
    return httpStatus;
}
