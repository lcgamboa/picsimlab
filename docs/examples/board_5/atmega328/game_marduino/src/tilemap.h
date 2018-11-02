
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

#include "marduinotypes.h"

#ifndef TILEMAP_H
#define TILEMAP_H

class TileMap {

private:

	TILEMAPSET * _tileMapID;
	unsigned int _tileWidth;
	unsigned int _tileHeight;
	unsigned int _tileMapWidth;
	unsigned int _tileMapHeight;
	Adafruit_PCD8544 * _display; //pointer to display 

public:

	TileMap();
	void getTileIndexByPos(int, int, int *);
	void getPosByTileIndex(int, int, int *);
        unsigned int getMapWidth();
        unsigned int getMapHeight();
        unsigned int getTileWidth();
        unsigned int getTileHeight();
	void setDisplayPointer(Adafruit_PCD8544 *); //using Adafruit display
	void setMapSize(unsigned int, unsigned int);
	void setTileSize(unsigned int, unsigned int);
	void setTileMap(TILEMAPSET *);
	void drawTile(const unsigned int, int, int);
	void drawMap(int, int);

};

#endif
