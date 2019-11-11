#include "Engine.h"
#include "engine.h"
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
    int priority = 0;
    // if currentCommands isnt empty
    if (currentCommands.size() > 0)
        // find largest priority
        priority = currentCommands.rbegin()->first + 1;

    // passive commands...
    unique_ptr<Command> ptr_cw(new CheckWinnerCommand());
    addCommand(move(ptr_cw), priority++);
    bool addFinishTurn = false;
    for (size_t i = 0; i < currentCommands.size(); i++)
    {
        if (currentCommands[i]->getCommandID() == ATTACK)
        {
            addFinishTurn = true;
            break;
        }
    }
    if(addFinishTurn){
        cout << "finishing because you attacked" << endl;
        unique_ptr<Command> ptr_ft(new FinishTurnCommand());
        addCommand(move(ptr_ft), priority++);
    }
}

void Engine::addCommand(std::unique_ptr<Command> ptr_cmd, int priority)
{
    if (priority == -1){
        // find largest priority
        if(currentCommands.size() > 0)
            priority = currentCommands.rbegin()->first + 1;
        else{
            priority = 0;
        }
    }
    currentCommands[priority] = move(ptr_cmd);
}

void Engine::update()
{
    if (!currentState.getEnd())
    {
        cout << "Adding passive commands ..." << endl;
        addPassiveCommands();
        cout << "Executing commands from turn " << currentState.getTurn() << endl;
        //default event
        StateEvent stateEvent(ALLCHANGED);
        bool endTurn = false;
        for (size_t i = 0; i < currentCommands.size(); i++)
        {
            stateEvent.setStateEventID(CHARACTERCHANGED);

            if (endTurn == false && currentCommands[i]->getCommandID() == ATTACK || currentCommands[i]->getCommandID() == FINISH_TURN)
{

            currentState.setTurn(currentState.getTurn() + 1);
                endTurn = true;
}

            // TODO: Execute only the player active's commands.
            currentCommands[i]->execute(currentState);
            currentState.notifyObservers(stateEvent, currentState);
            usleep(200 * 1000);
        }
        // clean using iterator
        map<int, std::unique_ptr<Command>>::iterator iterator;
        for (iterator = currentCommands.begin(); iterator != currentCommands.end(); iterator++)
        {
            currentCommands.erase(iterator);
        }
        
    }
    else
    {
        cout << "The game is ended, we have a winner" << endl;
    }
}
