#include "StateEvent.h"
#include <iostream>

using namespace state;

StateEvent::StateEvent(StateEventID e) : stateEventID(e){}

void StateEvent::setStateEventID(StateEventID newID)
{
    stateEventID = newID;
}
