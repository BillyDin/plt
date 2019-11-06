#include "../render.h"
#include <iostream>

#include <cstdlib>
using namespace std;
using namespace sf;
using namespace state;
using namespace render;

bool Surface::loadCharacters(state::State &state, sf::Texture &textureTileset, sf::Vector2u tileSize, unsigned int width, unsigned int height)
{
    // on redimensionne le tableau de vertex pour qu'il puisse contenir tout le niveau
    quads.setPrimitiveType(sf::Quads);
    quads.resize(width * height * 4);
    // on remplit le tableau de vertex, avec un quad par tuile
    for (unsigned int i = 0; i < width; ++i)
    {
        // only showing not death characters
        if (state.getCharacters()[i]->getStatus() != DEATH)
        {
            int tileNumber = state.getCharacters()[i]->getTileCode();
            texture = textureTileset;

            // on en déduit sa position dans la texture du tileset
            int tu = tileNumber % (texture.getSize().x / tileSize.x);
            int tv = tileNumber / (texture.getSize().x / tileSize.x);

            // on récupère un pointeur vers le quad à définir dans le tableau de vertex
            sf::Vertex *quad = &quads[i * 4];

            // on définit ses quatre coins
            quad[0].position = sf::Vector2f(state.getCharacters()[i]->getPosition().getX() * tileSize.x, state.getCharacters()[i]->getPosition().getY() * tileSize.y);
            quad[1].position = sf::Vector2f((state.getCharacters()[i]->getPosition().getX() + 1) * tileSize.x, state.getCharacters()[i]->getPosition().getY() * tileSize.y);
            quad[2].position = sf::Vector2f((state.getCharacters()[i]->getPosition().getX() + 1) * tileSize.x, (state.getCharacters()[i]->getPosition().getY() + 1) * tileSize.y);
            quad[3].position = sf::Vector2f(state.getCharacters()[i]->getPosition().getX() * tileSize.x, (state.getCharacters()[i]->getPosition().getY() + 1) * tileSize.y);

            // on définit ses quatre coordonnées de texture
            quad[0].texCoords = sf::Vector2f(tu * tileSize.x, tv * tileSize.y);
            quad[1].texCoords = sf::Vector2f((tu + 1) * tileSize.x, tv * tileSize.y);
            quad[2].texCoords = sf::Vector2f((tu + 1) * tileSize.x, (tv + 1) * tileSize.y);
            quad[3].texCoords = sf::Vector2f(tu * tileSize.x, (tv + 1) * tileSize.y);
        }
    }

    return true;
}

bool Surface::loadTextures(state::State &state, sf::Texture &textureTileset, sf::Vector2u tileSize, unsigned int width, unsigned int height)
{
    texture = textureTileset;

    // on redimensionne le tableau de vertex pour qu'il puisse contenir tout le niveau
    quads.setPrimitiveType(sf::Quads);
    quads.resize(width * height * 4);

    // on remplit le tableau de vertex, avec un quad par tuile
    for (unsigned int i = 0; i < state.getMap().size(); ++i)
    {
        for (unsigned int j = 0; j < state.getMap()[0].size(); j++)
        {
            // get the current tile number
            int tileNumber = state.getMap()[i][j]->getTileCode();
            // on en déduit sa position dans la texture du tileset
            int tu = tileNumber % (texture.getSize().x / tileSize.x);
            int tv = tileNumber / (texture.getSize().x / tileSize.x);

            // on récupère un pointeur vers le quad à définir dans le tableau de vertex
            sf::Vertex *quad = &quads[(i + j * width) * 4];

            // define its 4 corners
            quad[0].position = sf::Vector2f(j * tileSize.x, i * tileSize.y);
            quad[1].position = sf::Vector2f((j + 1) * tileSize.x, i * tileSize.y);
            quad[2].position = sf::Vector2f((j + 1) * tileSize.x, (i + 1) * tileSize.y);
            quad[3].position = sf::Vector2f(j * tileSize.x, (i + 1) * tileSize.y);

            // define its 4 texture coordinates
            quad[0].texCoords = sf::Vector2f(tu * tileSize.x, tv * tileSize.y);
            quad[1].texCoords = sf::Vector2f((tu + 1) * tileSize.x, tv * tileSize.y);
            quad[2].texCoords = sf::Vector2f((tu + 1) * tileSize.x, (tv + 1) * tileSize.y);
            quad[3].texCoords = sf::Vector2f(tu * tileSize.x, (tv + 1) * tileSize.y);
        }
    }
    return true;
}

void Surface::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    // apply the transform
    states.transform *= getTransform();

    // apply the tileset texture
    states.texture = &texture;

    // draw the vertex array
    target.draw(quads, states);
}