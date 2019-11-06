#include "Engine.h"
#include <iostream>
#include <unistd.h>

using namespace state;
using namespace engine;
using namespace std;

Engine::Engine() : currentState()
{
}

Engine::~Engine()
{
}

state::State &Engine::getState()
{
    state::State &refState = currentState;
    return refState;
}

void Engine::addPassiveCommands()
{
}

void Engine::addCommand(int prioriteratory, std::unique_ptr<Command> ptr_cmd)
{
    currentCommands[prioriteratory] = move(ptr_cmd);
}

void Engine::update()
{
    cout << "Executing commands from turn " << currentState.getTurn() << endl;
    //default event
    StateEvent stateEvent(ALLCHANGED);
    
    for (size_t i = 0; i < currentCommands.size(); i++)
    {
        if(currentCommands[i]->getCommandID() == MOVE)
            stateEvent.setStateEventID(CHARACTERCHANGED);
        // TODO: Execute only the player active's commands.
        currentCommands[i]->execute(currentState);
        currentState.notifyObservers(stateEvent , currentState);
        usleep(200*1000);
    }
    // clean using iterator
    map<int, std::unique_ptr<Command>>::iterator iterator;
    for (iterator = currentCommands.begin(); iterator != currentCommands.end(); iterator++)
    {
        currentCommands.erase(iterator);
    }

    // increasing turn
    currentState.setTurn(currentState.getTurn() + 1);
}
