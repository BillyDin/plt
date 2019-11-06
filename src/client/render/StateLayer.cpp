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

StateLayer::StateLayer(state::State &state, sf::RenderWindow &window) : window(window)
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

bool StateLayer::showText()
{
    sf::Text text;
    text.setPosition(32.f, window.getSize().y - 32.f);
    // select the font
    text.setFont(font); // font is a sf::Font
    // set the string to display
    text.setString("Hello, welcome to our game! Enjoy.");

    // set the character size
    text.setCharacterSize(24); // in pixels, not points!

    // set the color
    text.setFillColor(sf::Color::White);

    // set the text style

    // inside the main loop, between window.clear() and window.display()
    window.draw(text);
    return true;
}

void StateLayer::initSurfaces(state::State &state)
{
    render::Surface surfMap;
    surfMap.loadTextures(state, tilesets[0]->getTexture(), sf::Vector2u(tilesets[0]->getCellWidth(), tilesets[0]->getCellHeight()), state.getMap().size(), state.getMap()[0].size());
    std::unique_ptr<Surface> ptrsurfMap (new Surface(surfMap));
    
    Surface characters;
    characters.loadCharacters(state, tilesets[1]->getTexture(), sf::Vector2u(tilesets[1]->getCellWidth(), tilesets[1]->getCellHeight()), state.getCharacters().size(), 1);
    std::unique_ptr<Surface> ptrsurfChar1 (new Surface(characters));

	if(surface.size()!=0){  
		while(surface.size()!=0){
			surface.pop_back();
		}
	}
	
	surface.push_back(move(ptrsurfMap));
    surface.push_back(move(ptrsurfChar1));
}

void StateLayer::stateChanged(const state::StateEvent &e, state::State &state)
{
    if (e.stateEventID == StateEventID::ALLCHANGED){
        std::cout << "STATE CHANGED EVENT: all changed" << endl;
		initSurfaces(state);
		draw(window);
	}
	else if (e.stateEventID == StateEventID::TEXTCHANGED){
		draw(window);
        std::cout << "STATE CHANGED EVENT: text changed" << endl;
	}
	else if (e.stateEventID == StateEventID::CHARACTERCHANGED){
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

    // draw text
    showText();
    
	window.display();
}
