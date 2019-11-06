#include <iostream>
#include <string.h>
// Les lignes suivantes ne servent qu'à vérifier que la compilation avec SFML fonctionne
#include <SFML/Graphics.hpp>

void testSFML()
{
    sf::Texture texture;
}
// Fin test SFML

#include <state.h>
#include "render.h"
#include "engine.h"
#include <iostream>
#include <unistd.h>
using namespace std;
using namespace state;
using namespace render;

int main()
{
    cout << "--- Engine du jeu ---" << endl;
    engine::Engine ngine;
    cout << "--- engine object created ---" << endl;

    ngine.getState().initializeMapCell();
    cout << "--- state map initialized ---" << endl;

    ngine.getState().initializeCharacters();
    cout << "--- characters initialized ---" << endl;

    //-----------------------------

    sf::RenderWindow window(sf::VideoMode(ngine.getState().getMap()[0].size() * 32, ngine.getState().getMap().size() * 32 + 32, 32), "map");
    StateLayer layer(ngine.getState(), window);

    layer.initSurfaces(ngine.getState());
    cout << "--- game window initialized ---" << endl;

    StateLayer stateLayer(ngine.getState(), window);
    cout << "--- state layer initialized with the state and the window ---" << endl;

    stateLayer.initSurfaces(ngine.getState());
    cout << "--- state layer's surfaces initialized ---" << endl;

    // Registering observer
    StateLayer *ptr_stateLayer = &stateLayer;
    ngine.getState().registerObserver(ptr_stateLayer);
    bool once = true;
    int turns2go = 6;
    while (window.isOpen())
    {
        sf::Event event;

        if (once)
        {
            stateLayer.draw(window);
            once = false;
            cout << "ATENTION!! push some key to trigger a turn..." << endl;
        }

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }

            else if (event.type == sf::Event::KeyPressed)
            {
                cout << "key pressed !" << endl;

                if(turns2go == 6 || turns2go == 4){
                    int priority = 0;
                    cout << "turn number: " << -(turns2go - 7) << endl;
                    // harcoding now to 3 the chances to move for each round,
                    // to meet quickly in the center of the map and have some fight.
                    ngine.getState().getCharacters()[0]->setCharacterMove(3);

                    int initialXP1 = ngine.getState().getCharacters()[0]->getPosition().getX();
                    int initialYP1 = ngine.getState().getCharacters()[0]->getPosition().getY();
                    cout << "initial character [Player 1] pos " << ngine.getState().getCharacters()[0]->getPosition().getX() << " " << ngine.getState().getCharacters()[0]->getPosition().getY() << endl;

                    Position pos1{initialXP1, ++initialYP1};
                    unique_ptr<engine::Command> ptr_mc1(new engine::MoveCommand(*ngine.getState().getCharacters()[0], pos1));
                    ngine.addCommand(priority++, move(ptr_mc1));

                    Position pos2{initialXP1, ++initialYP1};
                    unique_ptr<engine::Command> ptr_mc2(new engine::MoveCommand(*ngine.getState().getCharacters()[0], pos2));
                    ngine.addCommand(priority++, move(ptr_mc2));

                    Position pos3{initialXP1, ++initialYP1};
                    unique_ptr<engine::Command> ptr_mc3(new engine::MoveCommand(*ngine.getState().getCharacters()[0], pos3));
                    ngine.addCommand(priority++, move(ptr_mc3));

                    ngine.update();
                    turns2go--;
                }
                else if(turns2go == 5 || turns2go == 3){
                    int priority = 0;
                    cout << "turn number: " << -(turns2go - 7) << endl;
                    // harcoding now to 3 the chances to move for each round,
                    // to meet quickly in the center of the map and have some fight.
                    ngine.getState().getCharacters()[1]->setCharacterMove(3);

                    int initialXP2 = ngine.getState().getCharacters()[1]->getPosition().getX();
                    int initialYP2 = ngine.getState().getCharacters()[1]->getPosition().getY();
                    cout << "initial character [Player 2] pos " << ngine.getState().getCharacters()[1]->getPosition().getX() << " " << ngine.getState().getCharacters()[0]->getPosition().getY() << endl;

                    Position pos1{initialXP2, --initialYP2};
                    unique_ptr<engine::Command> ptr_mc1(new engine::MoveCommand(*ngine.getState().getCharacters()[1], pos1));
                    ngine.addCommand(priority++, move(ptr_mc1));

                    Position pos2{initialXP2, --initialYP2};
                    unique_ptr<engine::Command> ptr_mc2(new engine::MoveCommand(*ngine.getState().getCharacters()[1], pos2));
                    ngine.addCommand(priority++, move(ptr_mc2));

                    Position pos3{initialXP2, --initialYP2};
                    unique_ptr<engine::Command> ptr_mc3(new engine::MoveCommand(*ngine.getState().getCharacters()[1], pos3));
                    ngine.addCommand(priority++, move(ptr_mc3));

                    ngine.update();
                    turns2go--;
                }
                else if(turns2go == 2){
                    int priority = 0;
                    cout << "turn number: " << -(turns2go - 7) << endl;
                    // harcoding now to 3 the chances to move for each round,
                    // to meet quickly in the center of the map and have some fight.
                    ngine.getState().getCharacters()[0]->setCharacterMove(3);
                    int initialXP1 = ngine.getState().getCharacters()[0]->getPosition().getX();
                    int initialYP1 = ngine.getState().getCharacters()[0]->getPosition().getY();
                    cout << "initial character [Player 1] pos " << ngine.getState().getCharacters()[0]->getPosition().getX() << " " << ngine.getState().getCharacters()[0]->getPosition().getY() << endl;

                    Position pos1{initialXP1, ++initialYP1};
                    unique_ptr<engine::Command> ptr_mc1(new engine::MoveCommand(*ngine.getState().getCharacters()[0], pos1));
                    ngine.addCommand(priority++, move(ptr_mc1));

                    Position pos2{initialXP1, ++initialYP1};
                    unique_ptr<engine::Command> ptr_mc2(new engine::MoveCommand(*ngine.getState().getCharacters()[0], pos2));
                    ngine.addCommand(priority++, move(ptr_mc2));

                    unique_ptr<engine::Command> ptr_ac1(new engine::AttackCommand(*ngine.getState().getCharacters()[0], *ngine.getState().getCharacters()[1]));
                    ngine.addCommand(priority++, move(ptr_ac1));

                    ngine.update();
                    turns2go--;
                }
                else if (turns2go == 1){
                    int priority = 0;
                    cout << "turn number: " << -(turns2go - 7) << endl;
                    // harcoding now to 3 the chances to move for each round,
                    // to meet quickly in the center of the map and have some fight.
                    ngine.getState().getCharacters()[1]->setCharacterMove(3);

                    int initialXP2 = ngine.getState().getCharacters()[1]->getPosition().getX();
                    int initialYP2 = ngine.getState().getCharacters()[1]->getPosition().getY();
                    cout << "initial character [Player 2] pos " << ngine.getState().getCharacters()[1]->getPosition().getX() << " " << ngine.getState().getCharacters()[0]->getPosition().getY() << endl;

                    Position pos1{initialXP2, ++initialYP2};
                    unique_ptr<engine::Command> ptr_mc1(new engine::MoveCommand(*ngine.getState().getCharacters()[1], pos1));
                    ngine.addCommand(priority++, move(ptr_mc1));

                    unique_ptr<engine::Command> ptr_ac1(new engine::AttackCommand(*ngine.getState().getCharacters()[0], *ngine.getState().getCharacters()[1]));
                    ngine.addCommand(priority++, move(ptr_ac1));

                    ngine.update();
                    turns2go--;
                }
                else if (turns2go == 0){
                    sleep(2);
                    cout << "ENGINE SHOW finished, closing window" << endl;
                    window.close();
                }
            }
        }
    }
    return 0;
}
//         }
// if (argc > 1)
// {
//     if (strcmp(argv[1], "hello") == 0)
//         cout << "Bonjour " << ((argv[2]) ? argv[2] : "tout le monde") << endl;

//     else if (strcmp(argv[1], "render") == 0)
//     {
//         cout << "--- Render ---" << endl;
//         //
//         State state;
//         state.initializeMapCell();
//         state.initializeCharacters();
//         sf::RenderWindow window(sf::VideoMode(state.getMap()[0].size() * 32, (state.getMap().size() * 32) + 32), "Map");
//         StateLayer layer(state, window);
//         layer.initSurfaces(state);
//         while (window.isOpen())
//         {
//             sf::Event event;
//             while (window.pollEvent(event))
//             {
//                 if (event.type == sf::Event::Closed)
//                     window.close();
//             }
//             // render
//             layer.draw(window);
//         }
//         return 0;
//     }

//     else if (strcmp(argv[1], "engine") == 0)
//     {

//     }
// }