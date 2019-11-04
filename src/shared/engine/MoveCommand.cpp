#include "MoveCommand.h"
// to use console
#include <iostream>

using namespace engine;
using namespace state;
using namespace std;

MoveCommand::MoveCommand(state::Character &characterTarget, state::Position &positionTarger) : characterTarget(characterTarget), positionTarget(positionTarget)
{
    id = MOVE;
}

void MoveCommand::execute(state::State &state)
{
    if (characterTarget.getStatus() == SELECTED)
    {
        // if the character has moves to do
        if ( true /*characterTarget.getCharacterMove() > 0*/)
        {
            // TODO: Verificate legal moves
            // Moving
            characterTarget.getPosition().setX(positionTarget.getX());
            characterTarget.getPosition().setY(positionTarget.getY());
            characterTarget.setCharacterMove(characterTarget.getCharacterMove() - 1);
            cout << "The character " <<  characterTarget.getName() << " has been moved to [" << positionTarget.getX() << ", " << positionTarget.getY() << "]" << endl;
        }
        else
            cout << "Without move chances" << endl;
    }
    cout << "\n";
}
