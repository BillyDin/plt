#include "MoveCommand.h"
// to use console
#include <iostream>

using namespace engine;
using namespace state;
using namespace std;

MoveCommand::MoveCommand(state::Character &characterTarget, state::Position &positionTarger) : characterTarget(characterTarget), positionTarget(positionTarger)
{
    id = MOVE;
}

void MoveCommand::execute(state::State &state)
{
    cout << "Trying to move a character" << endl;
    bool allowed = false;
    if (characterTarget.getStatus() == SELECTED)
    {
        // if the character has moves to do
        cout << "Move chances left " << characterTarget.getCharacterMove() << endl;
        if (characterTarget.getCharacterMove() > 0)
        {
            for (auto &i : characterTarget.allowedPosToMove(state))
            {
                if (i.equals(positionTarget))
                {
                    allowed = true;
                    break;
                }
            }
            if (allowed)
            {
                // Moving
                characterTarget.getPosition().setX(positionTarget.getX());
                characterTarget.getPosition().setY(positionTarget.getY());
                characterTarget.setCharacterMove(characterTarget.getCharacterMove() - 1);

                cout << "The character " << characterTarget.getName() << " has been moved to [" << positionTarget.getX() << ", " << positionTarget.getY() << "]" << endl;
                cout << "Decreasing by 1 moving chances, now the character has " << characterTarget.getCharacterMove() << " move chances left" << endl;
            }
            else
                cout << "The character " << characterTarget.getName() << " can't move to that position" << endl;
        }
        else
            cout << "Without move chances" << endl;
    }
    cout << "\n";
}
