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

void Engine::addCommand(int priority, std::unique_ptr<Command> ptr_cmd)
{
    currentCommands[priority] = move(ptr_cmd);
}

void Engine::update()
{
    map<int, std::unique_ptr<Command>>::iterator it;
    for (size_t i = 0; i < currentCommands.size(); i++)
    {
        // TODO: Execute only the player active's commands.
        currentCommands[i]->execute(currentState);
        sleep(2);
    }
    // clean
    for (it = currentCommands.begin(); it != currentCommands.end(); it++)
    {
        currentCommands.erase(it);
    }
}
