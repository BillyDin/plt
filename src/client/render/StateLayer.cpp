#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>
#include "../render.h"
#include <iostream>
#include <fstream>
#include <memory>
#include <sstream>
#include <unistd.h>

using namespace render;
using namespace std;
using namespace state;

StateLayer::StateLayer(state::State &state, sf::RenderWindow &window) : currentState(state), window(window)
{
    font.loadFromFile("res/typographie.ttf");

    std::unique_ptr<TileSet> tilesetMapCell(new TileSet(TileSetID::MAPCELL));
    tilesets.push_back(move(tilesetMapCell));

    std::unique_ptr<TileSet> tilesetCharacterMage(new TileSet(TileSetID::CHARACTER));
    tilesets.push_back(move(tilesetCharacterMage));
}

std::vector<std::unique_ptr<TileSet>> &StateLayer::getTilesets()
{
    std::vector<std::unique_ptr<TileSet>> &refTileSets = tilesets;
    return refTileSets;
}

std::vector<std::unique_ptr<Surface>> &StateLayer::getSurfaces()
{
    std::vector<std::unique_ptr<Surface>> &refSurfaces = surface;
    return refSurfaces;
}

void StateLayer::initSurfaces(state::State &state)
{
    render::Surface surfMap;
    surfMap.loadTextures(state, tilesets[0]->getTexture(), sf::Vector2u(tilesets[0]->getCellWidth(), tilesets[0]->getCellHeight()), state.getMap().size(), state.getMap()[0].size());
    std::unique_ptr<Surface> ptrsurfMap(new Surface(surfMap));

    Surface characters;
    characters.loadCharacters(state, tilesets[1]->getTexture(), sf::Vector2u(tilesets[1]->getCellWidth(), tilesets[1]->getCellHeight()), state.getCharacters().size(), 1);
    std::unique_ptr<Surface> ptrsurfChar1(new Surface(characters));

    if (surface.size() != 0)
    {
        while (surface.size() != 0)
        {
            surface.pop_back();
        }
    }

    surface.push_back(move(ptrsurfMap));
    surface.push_back(move(ptrsurfChar1));
}

void StateLayer::stateChanged(const state::StateEvent &e, state::State &state)
{
    if (e.stateEventID == StateEventID::ALLCHANGED)
    {
        std::cout << "STATE CHANGED EVENT: all changed" << endl;
        initSurfaces(state);
        draw(window);
    }
    else if (e.stateEventID == StateEventID::TEXTCHANGED)
    {
        draw(window);
        std::cout << "STATE CHANGED EVENT: text changed" << endl;
    }
    else if (e.stateEventID == StateEventID::CHARACTERCHANGED)
    {
        initSurfaces(state);
        draw(window);
        std::cout << "STATE CHANGED EVENT: character changed" << endl;
    }
}

void StateLayer::draw(sf::RenderWindow &window)
{
    window.clear();
    // draw mapcells
    window.draw(*surface[0]);

    // draw characters
    window.draw(*surface[1]);
    printText();
    window.display();
}

bool StateLayer::printText()
{
    std::vector<sf::Text> texts;

    sf::Text player1;
    player1.setPosition(window.getSize().x - 240.f, 0.f);
    player1.setFont(font);
    player1.setString("Player 1");
    player1.setFillColor(sf::Color::White);
    texts.push_back(move(player1));

    sf::Text player2;
    player2.setPosition(window.getSize().x - 240.f, (window.getSize().y / 2));
    player2.setFont(font);
    player2.setString("Player 2");
    player2.setFillColor(sf::Color::White);
    texts.push_back(move(player2));

    float playerOneBasePos = player1.getPosition().y + 32.f;
    float playerTwoBasePos = player2.getPosition().y + 32.f;

    for (auto &charac : currentState.getCharacters())
    {
        if (charac->getPlayerOwner() == 1)
        {
            sf::Text textStats;
            textStats.setPosition(window.getSize().x - 240.f, playerOneBasePos);
            textStats.setFont(font);
            std::string str = charac->getName() + "\n";
            str += "Health: " + std::to_string(charac->getStats().getHealth()) + "\n";
            str += "Moves: " + std::to_string(charac->getCharacterMove()) + "\n";
            str += "Attack dist.: " + std::to_string(charac->getCharacterAttackDistance());
            textStats.setString(str);
            textStats.setFillColor(sf::Color::White);
            textStats.setCharacterSize(15); // in pixels, not points!

            playerOneBasePos += 32.f;
            texts.push_back(move(textStats));
        }
        else
        {
            sf::Text textStats;
            textStats.setPosition(window.getSize().x - 240.f, playerTwoBasePos);
            textStats.setFont(font);
            std::string str = charac->getName() + "\n";
            str += "Health: " + std::to_string(charac->getStats().getHealth()) + "\n";
            str += "Moves: " + std::to_string(charac->getCharacterMove()) + "\n";
            str += "Attack dist.: " + std::to_string(charac->getCharacterAttackDistance());
            textStats.setString(str);
            textStats.setFillColor(sf::Color::White);
            textStats.setCharacterSize(15); // in pixels, not points!

            playerTwoBasePos += 32.f;
            texts.push_back(move(textStats));
        }
    }

    sf::Text turnInfo;
    turnInfo.setPosition(1.f, window.getSize().y - 32.f);
    turnInfo.setFont(font); // font is a sf::Font

    std::string str = "Turn: " + std::to_string(currentState.getTurn());
    turnInfo.setString(str);
    turnInfo.setCharacterSize(24); // in pixels, not points!
    turnInfo.setFillColor(sf::Color::White);

    texts.push_back(move(turnInfo));

    sf::Text turnInfo2;
    turnInfo2.setPosition((window.getSize().x - 270.f), window.getSize().y - 32.f);
    turnInfo2.setFont(font); // font is a sf::Font

    std::string str2 = "Turn owner: Player " + std::to_string(currentState.getTurnOwner());

    turnInfo2.setString(str2);
    turnInfo2.setCharacterSize(24); // in pixels, not points!
    turnInfo2.setFillColor(sf::Color::White);

    texts.push_back(move(turnInfo2));

    for (auto &i : texts)
        window.draw(i);
    texts.clear();
    return true;
}