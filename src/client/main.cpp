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
using namespace std;
using namespace state;
using namespace render;

int main(int argc, char *argv[])
{
    if (argc > 1)
    {
        if (strcmp(argv[1], "hello") == 0)
        {
            cout << "Bonjour " << ((argv[2]) ? argv[2] : "tout le monde") << endl;
        }
        else if (strcmp(argv[1], "render") == 0)
        {
            cout << "--- Render ---" << endl;
            // 
            State state;
            state.initializeMapCell();
            state.initializeCharacters();
            sf::RenderWindow window(sf::VideoMode(state.getMap()[0].size() * 32, (state.getMap().size() * 32) + 32), "Map");
            StateLayer layer(state, window);
            layer.initSurfaces(state);
            while (window.isOpen())
            {
                sf::Event event;
                while (window.pollEvent(event))
                {
                    if (event.type == sf::Event::Closed)
                        window.close();
                }
                // render
                layer.draw(window);
            }
        }
    }
    return 0;
}
