#include <string>
#include "../render.h"

using namespace render;

TileSet::TileSet(TileSetID newID)
{

	id = newID;

	if (id == TileSetID::HUD)
	{
		cellWidth = 32;
		cellHeight = 32;
		resourceFile = "res/32x32map.png";
	}

	else if (id == TileSetID::CHARACTER)
	{
		cellWidth = 32;
		cellHeight = 32;
		resourceFile = "res/characters.png";
	}

	else if (id == TileSetID::MAPCELL)
	{
		cellWidth = 32;
		cellHeight = 32;
		resourceFile = "res/32x32map.png";
	}

	else if (id == TileSetID::CURSOR)
	{
		cellWidth = 32;
		cellHeight = 32;
		resourceFile = "res/cursor.png";
	}

	textureTileset.loadFromFile(resourceFile);
}

const int TileSet::getTileSetID()
{
	return id;
}

const int TileSet::getCellWidth()
{
	return cellWidth;
}

const int TileSet::getCellHeight()
{
	return cellHeight;
}

const std::string TileSet::getImageFile()
{
	return resourceFile;
}

sf::Texture &TileSet::getTexture()
{
	sf::Texture &refTexture = textureTileset;
	return refTexture;
}
