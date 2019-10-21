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

	else if (id == TileSetID::CHARACTER_MAGE)
	{
		cellWidth = 32;
		cellHeight = 32;
		resourceFile = "res/mage.png";
	}

	else if (id == TileSetID::CHARACTER_SOLDIER)
	{
		cellWidth = 32;
		cellHeight = 32;
		resourceFile = "res/soldier.png";
	}

	else if (id == TileSetID::CHARACTER_DISTANCE)
	{
		cellWidth = 32;
		cellHeight = 32;
		resourceFile = "res/distance.png";
	}

	else if (id == TileSetID::MAPCELL)
	{
		cellWidth = 32;
		cellHeight = 32;
		resourceFile = "res/32x32map.png";
	}

	// else if (id == TileSetID::CURSOR)
	// {
	// 	cellWidth = 16;
	// 	cellHeight = 16;
	// 	resourceFile = "res/a_cursor_tileset.png";
	// }

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
