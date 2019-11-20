#include "CheckWinnerCommand.h"
// to use console
#include <iostream>

using namespace engine;
using namespace state;
using namespace std;

CheckWinnerCommand::CheckWinnerCommand()
{
    id = CHECK_WINNER;
    // maybe this constructor will be useful
    // i dont know right now
}

void CheckWinnerCommand::execute(state::State &state)
{
    // for each player, we have to check if his characters are all deaths.
    // winner == 1 -> player 1 wins
    // winner == 2 -> player 2 wins
    // winner == 0 -> still there're alive characters in both sides.

    int winner = 0;
    bool p1StillInGame = false;
    bool p2StillInGame = false;
    // we will iterate looking for a character alive for each player.
    for (auto &charac : state.getCharacters())
    {
        // player 1
        if (charac->getPlayerOwner() == 1)
        {
            if (charac->getStatus() != DEATH)
            {
                p1StillInGame = true;
                break;
            }
        }
    }
    // since we are using break, We have to iterate 2 times.
    // posibly refactorable, but my brain can't think in another way right now.
    for (auto &charac : state.getCharacters())
    {
        // player 2
        if (charac->getPlayerOwner() == 2)
        {
            if (charac->getStatus() != DEATH)
            {
                p2StillInGame = true;
                break;
            }
        }
    }
    // both in game? 0. p1 in game? then he's the winner. if not, p2.
    winner = (p1StillInGame && p2StillInGame) ? 0 : (p1StillInGame) ? 1 : 2;
    if(winner){
        state.setEnd(true);
        state.setWinner(winner);
        cout << endl << "ATENTION!!! THE WINNER IS THE PLAYER " << winner << endl;
        cout << endl << "All the opponent's characters are dead" << endl;
    }
}