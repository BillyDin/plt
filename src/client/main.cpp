#include <iostream>

// Les lignes suivantes ne servent qu'à vérifier que la compilation avec SFML fonctionne
#include <SFML/Graphics.hpp>

void testSFML() {
    sf::Texture texture;
}

// Fin test SFML

#include <state.h>

using namespace std;
using namespace state;

int main(int argc,char* argv[])
{

    if(argv[1] != NULL)
        cout << "Bonjour " << (argv[1]) << endl;
    else cout << "Bonjour " << endl;

    return 0;
}
