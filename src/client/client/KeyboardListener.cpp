#include <string>
#include "../client.h"
#include "../shared/state.h"
#include "../shared/engine.h"
#include <iostream>
#include <unistd.h>
#include <queue>
using namespace client;
using namespace state;
using namespace engine;
using namespace std;

KeyboardListener::KeyboardListener(engine::Engine &engine)
{
    initQueues(engine.getState());
}

void KeyboardListener::initQueues(state::State &state)
{
    for (size_t i = 0; i < state.getCharacters().size(); i++)
    {
        if (state.getCharacters()[i]->getPlayerOwner() == 1)
            queuePlayer1.push(i);
        else
            queuePlayer2.push(i);
    }
}

int KeyboardListener::nearestEnemy(state::State &state, int playerNum)
{
    // getting selected index
    int characIndex = -1;
    for (size_t i = 0; i < state.getCharacters().size(); i++)
    {
        if(state.getCharacters()[i]->getPlayerOwner() == playerNum && state.getCharacters()[i]->getStatus() == SELECTED){
            characIndex = i;
            break;
        }
    }

    Character &selectedChar = *state.getCharacters()[characIndex];
    int index = -1;
    int minimalDist = INT32_MAX;
    // iterate enemies and choose the nearest enemy
    for (unsigned int i = 0; i < state.getCharacters().size(); i++)
    {
        if (state.getCharacters()[i]->getPlayerOwner() != playerNum && state.getCharacters()[i]->getStatus() != DEATH && selectedChar.getPosition().distance(state.getCharacters()[i]->getPosition()) < minimalDist)
        {
            index = i;
            minimalDist = selectedChar.getPosition().distance(state.getCharacters()[i]->getPosition());
        }
    }
    return index;
}

void KeyboardListener::verifyDead(state::State &state)
{
    queue<size_t> p1;
    queue<size_t> p2;
    while (queuePlayer1.size() > 0)
    {
        if (state.getCharacters()[queuePlayer1.front()]->getStatus() != DEATH)
            p1.push(queuePlayer1.front());
        queuePlayer1.pop();
    }
    queuePlayer1 = p1;

    while (queuePlayer2.size() > 0)
    {
        if (state.getCharacters()[queuePlayer2.front()]->getStatus() != DEATH)
            p2.push(queuePlayer2.front());
        queuePlayer2.pop();
    }
    queuePlayer2 = p2;
}

void KeyboardListener::infiniteSelecting(state::State &state, int playerNum)
{
    if (playerNum == 1)
    {
        state.getCursor().move(state.getCharacters()[queuePlayer1.front()]->getPosition());
        size_t enque = queuePlayer1.front();
        queuePlayer1.pop();
        queuePlayer1.push(enque);
    }
    else
    {
        state.getCursor().move(state.getCharacters()[queuePlayer2.front()]->getPosition());
        size_t enque = queuePlayer2.front();
        queuePlayer2.pop();
        queuePlayer2.push(enque);
    }
}

void KeyboardListener::infiniteSelectMoving(state::State &state, int playerNum)
{
    // getting selected index
    int characIndex = -1;
    for (size_t i = 0; i < state.getCharacters().size(); i++)
    {
        if(state.getCharacters()[i]->getPlayerOwner() == playerNum && state.getCharacters()[i]->getStatus() == SELECTED){
            characIndex = i;
            break;
        }
    }
    if(state.getCharacters()[characIndex]->getCharacterMove() > 0){
        // we are gonna use pop many times. so we have to check if the queue its empty
        if (toVisitQueue.empty())
        {
            for (auto &pos : state.getCharacters()[characIndex]->allowedPosToMove(state))
            {
                toVisitQueue.push(pos);
            }
        }

        state.getCursor().move(toVisitQueue.front());

        if (!toVisitQueue.empty())
            toVisitQueue.pop();
    }
}

void KeyboardListener::cursorAction(state::State &state, int playerNum)
{
    if (state.getActualAction() == IDLE)
    {
        infiniteSelecting(state, playerNum);
    }
    else if (state.getActualAction() == MOVING)
    {
        infiniteSelectMoving(state, playerNum);
    }
    else if (state.getActualAction() == ATTACKING)
    {
        int enemy = (playerNum == 1) ? 2 : 1;
        infiniteSelecting(state, enemy);
    }
}

bool KeyboardListener::triggerAction(engine::Engine &engine, KeyID key)
{
    int actualPlayer = engine.getState().getTurnOwner();
    verifyDead(engine.getState());
    switch (key)
    {
    case LEFT:
    {
        cursorAction(engine.getState(), actualPlayer);
        StateEvent se{StateEventID::CURSOR_CHANGED};
        engine.getState().notifyObservers(se, engine.getState());
        break;
    }
    case RIGHT:
    {
        cursorAction(engine.getState(), actualPlayer);
        StateEvent se{StateEventID::CURSOR_CHANGED};
        engine.getState().notifyObservers(se, engine.getState());
        break;
    }
    case TOP:
    {
        cursorAction(engine.getState(), actualPlayer);
        StateEvent se{StateEventID::CURSOR_CHANGED};
        engine.getState().notifyObservers(se, engine.getState());
        break;
    }
    case DOWN:
    {
        cursorAction(engine.getState(), actualPlayer);
        StateEvent se{StateEventID::CURSOR_CHANGED};
        engine.getState().notifyObservers(se, engine.getState());
        break;
    }
    case MOVE: // press M
    {
        bool selected = false;
        for (auto &charac : engine.getState().getCharacters())
        {
            if (charac->getStatus() == SELECTED && charac->getPlayerOwner() == actualPlayer)
            {
                selected = true;
                while(!toVisitQueue.empty()) toVisitQueue.pop();

                for (size_t i = 1; i < charac->allowedPosToMove(engine.getState()).size(); i++)
                    toVisitQueue.push(charac->allowedPosToMove(engine.getState())[i]);
                
                engine.getState().getCursor().move(charac->allowedPosToMove(engine.getState())[0]);
                break;
            }
        }
        if (selected)
        {
            engine.getState().setActualAction(MOVING);
            engine.getState().getCursor().setVisible(true);
            engine.getState().getCursor().setTileCode(2);
            StateEvent se{StateEventID::CURSOR_CHANGED};
            engine.getState().notifyObservers(se, engine.getState());
        }
        else if (!selected)
        {
            StateEvent se{StateEventID::ALERT};
            se.text = "First you have to select a character with ENTER";
            engine.getState().notifyObservers(se, engine.getState());
        }
        break;
    }
    case ATTACK: // press A
    {
        bool selected = false;
        for (size_t i = 0; i < engine.getState().getCharacters().size(); i++)
        {
            /* code */
            Character &charac = *engine.getState().getCharacters()[i];
            if (charac.getStatus() == SELECTED && charac.getPlayerOwner() == actualPlayer)
            {
                // define enemy
                int enemy = (actualPlayer == 1) ? 2 : 1;
                if (enemy == 1)
                    engine.getState().getCursor().move(
                        engine.getState().getCharacters()[nearestEnemy(engine.getState(), actualPlayer)]->getPosition());
                else
                    engine.getState().getCursor().move(
                        engine.getState().getCharacters()[nearestEnemy(engine.getState(), actualPlayer)]->getPosition());

                selected = true;
                break;
            }
        }

        if (selected && (engine.getState().getActualAction() == IDLE || engine.getState().getActualAction() == MOVING))
        {
            engine.getState().setActualAction(ATTACKING);
            engine.getState().getCursor().setVisible(true);
            engine.getState().getCursor().setTileCode(1);
            StateEvent se{StateEventID::CURSOR_CHANGED};
            engine.getState().notifyObservers(se, engine.getState());
        }
        else if (!selected)
        {
            StateEvent se{StateEventID::ALERT};
            se.text = "First you have to select a character with ENTER";
            engine.getState().notifyObservers(se, engine.getState());
        }
        break;
    }
    case PASS_TURN:
    {
        std::unique_ptr<engine::Command> ptr_fc(new engine::FinishTurnCommand());
        engine.addCommand(move(ptr_fc));
        engine.update();
        break;
    }
    case SELECT:
    {

        switch (engine.getState().getActualAction())
        {
        case IDLE:
        {
            cout << "trying to select a character " << endl;
            // try to select a character
            for (auto &charac : engine.getState().getCharacters())
            {
                if (charac->getPlayerOwner() == actualPlayer && charac->getStatus() != DEATH)
                {
                    if (engine.getState().getCursor().getPosition().equals(charac->getPosition()))
                    {
                        unique_ptr<engine::Command> ptr_sc(new engine::SelectCharacterCommand(*charac));
                        engine.addCommand(move(ptr_sc));
                        cout << charac->getName() << " selected" << endl;
                        engine.update();
                    }
                    else
                        cout << "Nobody is in that map cell" << endl;
                }
            }
            break;
        }
        case MOVING:
        {
            while(!toVisitQueue.empty()) toVisitQueue.pop();
            cout << "trying to move a character " << endl;
            for (auto &charac : engine.getState().getCharacters())
            {
                if (charac->getPlayerOwner() == actualPlayer && charac->getStatus() == SELECTED)
                {
                    bool allowed = false;
                    for (auto &allowPos : charac->allowedPosToMove(engine.getState()))
                    {
                        if (allowPos.equals(engine.getState().getCursor().getPosition()))
                        {
                            allowed = true;
                            break;
                        }
                    }
                    if (allowed)
                    {
                        Position pos1{engine.getState().getCursor().getPosition().getX(), engine.getState().getCursor().getPosition().getY()};
                        unique_ptr<engine::Command> ptr_mc1(new engine::MoveCommand(*charac, pos1));
                        engine.addCommand(move(ptr_mc1));
                        engine.update();
                    }
                    else
                    {
                        cout << " not allowed to move overe there " << endl;
                    }
                }
            }
            break;
        }
        case ATTACKING:
        {
            int victimPlayer = actualPlayer == 1 ? 2 : 1;
            cout << "trying to attack a target " << endl;

            for (auto &target : engine.getState().getCharacters())
            {
                // finding the target
                if (target->getPlayerOwner() == victimPlayer)
                {
                    // if the cursor is placed in one enemy position.
                    if (target->getPosition().equals(engine.getState().getCursor().getPosition()))
                    {
                        for (auto &attacker : engine.getState().getCharacters())
                        {
                            if (attacker->getPlayerOwner() == actualPlayer && attacker->getStatus() == SELECTED)
                            {
                                int maxDistance = attacker->getCharacterAttackDistance() + 1;
                                // if the target is within the max distance of the attacker
                                cout << attacker->getPosition().distance(target->getPosition()) << " <= " << maxDistance << endl;
                                if (attacker->getPosition().distance(target->getPosition()) <= maxDistance)
                                {
                                    unique_ptr<engine::Command> ptr_ac1(new engine::AttackCommand(*attacker, *target));
                                    engine.addCommand(move(ptr_ac1));

                                    unique_ptr<engine::Command> ptr_ftc(new engine::FinishTurnCommand());
                                    engine.addCommand(move(ptr_ftc));
                                    engine.update();
                                    break;
                                }
                            }
                            else
                            {
                                cout << "the distances is not good" << endl;
                            }
                        }
                    }
                    else
                    {
                        cout << " not attack to move overe there " << endl;
                    }
                }
            }
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
    return true;
}
