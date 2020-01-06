#include <iostream>
#include <fstream>
#include <string.h>
#include <sstream>
#include <map>
#include <memory>
#include <unistd.h>
#include <microhttpd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>

#include "state.h"
#include "render.h"
#include "engine.h"
#include "ai.h"

using namespace std;
using namespace state;
using namespace render;
using namespace engine;
using namespace ai;

int main(int argc, char const *argv[])
{
    if (argc > 1)
    {
        if (strcmp(argv[1], "hello") == 0)
            cout << "Bonjour " << ((argv[2]) ? argv[2] : "tout le monde") << endl;

        else if (strcmp(argv[1], "record") == 0)
        {

            std::string commands_file = "res/replay.txt";

            engine::Engine ngine{"game"};

            ngine.setEnableRecord(true);
            ngine.getState().initializeMapCell();
            ngine.getState().initializeCharacters();

            HeuristicAI hai1{ngine, 1};
            HeuristicAI hai2{ngine, 2};

            cout << "\t\t--- Record ---" << endl;
            cout << "On enregistre 5 tours de jeu dans le fichier " << commands_file << endl;
            cout << "Deux IA heuristiques jouent l'une contre l'autre" << endl;
            sleep(2);
            cout << "--> Début de l'enregistrement <--" << endl;

            // On simule 11 tours
            while (ngine.getState().getTurn() != 13)
            {
                // hai1 player1
                if (ngine.getState().getTurn() % 2 != 0)
                {
                    hai1.run(ngine);
                }
                //hai2 player2
                else
                {
                    hai2.run(ngine);
                }
            }
            cout << "--> Fin de l'enregistrement <--" << endl;

            cout << "--> Debut de l'enregistrement dans le fichier <--" << endl;
            // Ouverture du fichier en ecriture en effacant son contenu à l'ouverture
            std::ofstream written_file(commands_file, ios::out | ios::trunc);
            if (written_file)
            {

                Json::Value root = ngine.getRecord();
                cout << root << endl;

                // Ecriture dans le fichier du tableau de commandes de cette partie
                written_file << root;

                // Fermeture du fichier
                written_file.close();

                cout << "--> Fin de l'enregistrement dans le fichier <--" << endl;
            }
            else
            {
                cerr << "Impossible d'ouvrir le fichier des commandes enregistrées pour l'ecriture" << endl;
            }
        }
    }
    return 0;
}
