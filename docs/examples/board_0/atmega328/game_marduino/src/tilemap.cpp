

#include "tilemap.h"
#include "tilesetbitmap.h"
#include "levels.h"

  TileMap::TileMap() {
        _tileMapWidth = 1;
        _tileMapHeight = 1;
        _tileWidth = 8;
        _tileHeight = 8;
        _tileMapID = NULL;
  }

	void TileMap::getTileIndexByPos(int coordx, int coordy, int * pickedTile) {
		pickedTile[0] = coordx / _tileWidth;
		pickedTile[1] = coordy / _tileHeight;
	}

	void TileMap::getPosByTileIndex(int tilex, int tiley, int * result) {
		result[0] = tilex*_tileWidth;
		result[1] = tiley*_tileHeight;
	}

  unsigned int TileMap::getMapWidth() {
          return _tileMapWidth;
  }
  
  unsigned int TileMap::getMapHeight() {
          return _tileMapHeight; 
  }
  
  unsigned int TileMap::getTileWidth() {
          return _tileWidth;        
  }
  
  unsigned int TileMap::getTileHeight() {
          return _tileHeight;
  }

	void TileMap::setDisplayPointer(Adafruit_PCD8544 * disp) {
		_display = disp;
	}

	void TileMap::setTileSize(unsigned int w, unsigned int h) {
		_tileWidth = w;
		_tileHeight = h;
	}

	void TileMap::setMapSize(unsigned int w, unsigned int h) {
		_tileMapWidth = w;
		_tileMapHeight = h;
	}

	void TileMap::setTileMap(TILEMAPSET * tilemaparray) {
		_tileMapID = tilemaparray;		
	}

	void TileMap::drawTile(const unsigned int id, int x, int y) {

		BMP * tileToDraw = NULL;

		switch(id) {

			case 2: {tileToDraw = tileset002; break;}
			case 3: {tileToDraw = tileset003; break;}
			case 4: {tileToDraw = tileset004; break;}
      case 11: {tileToDraw = tileset011; break;}
      case 14: {tileToDraw = tileset014; break;}
      case 55: {tileToDraw = tileset055; break;}
      case 56: {tileToDraw = tileset056; break;}
      case 71: {tileToDraw = tileset071; break;}
      case 72: {tileToDraw = tileset072; break;}
			case 76: {tileToDraw = tileset076; break;}
			case 77: {tileToDraw = tileset077; break;}
			case 78: {tileToDraw = tileset078; break;}
			case 79: {tileToDraw = tileset079; break;}
			case 80: {tileToDraw = tileset080; break;}
      case 102: {tileToDraw = tileset102; break;}
      case 103: {tileToDraw = tileset103; break;}
      case 118: {tileToDraw = tileset118; break;}
      case 119: {tileToDraw = tileset119; break;}
      case 145: {tileToDraw = tileset145; break;}
      case 146: {tileToDraw = tileset146; break;}
      case 162: {tileToDraw = tileset162; break;}
      case 161: {tileToDraw = tileset080; break;}
      case 163: {tileToDraw = tileset163; break;}
		}

		if (tileToDraw != NULL) 
			_display->drawBitmap(x, y, tileToDraw, _tileWidth,_tileHeight,1);

	}
	
	void TileMap::drawMap(int xcamera, int ycamera) {

		//define tile x and y index iterator
		int it_start[2] = {0,0};
		int it_end[2] = {1,1};
		getTileIndexByPos(xcamera, ycamera, it_start);
		getTileIndexByPos(xcamera+_display->width()+_tileWidth, ycamera+_display->height(), it_end);

    // reverse camera movement
     xcamera *= -1;

		for (int yy = it_start[1]; yy < it_end[1]; yy++) {
		for (int xx = it_start[0]; xx < it_end[0]; xx++) {

                        
			unsigned int tileIndex = xx + (yy*_tileMapWidth);
      tileIndex = pgm_read_word_near(&_tileMapID[tileIndex]);
      
      if (tileIndex > 0)  
		    drawTile(tileIndex,xcamera+(xx*_tileWidth),ycamera+(yy*_tileHeight));
                        //Serial.print(test);
                        //Serial.print("\n");    // prints a tab

		}
		}		
	
	}

 /*void TileMap::createCollisionMap() {
  
   //_mapCollider = new Rect[_tileMapWidth*_tileMapHeight];

                for (int yy = 0; yy < _tileMapHeight; yy++) {
    for (int xx = 0; xx < _tileMapWidth; xx++) {
  
                        unsigned int tileIndex = xx + (yy * _tileMapWidth);
                        tileIndex = pgm_read_word_near(&_tileMapID[tileIndex]);
                        
                        if (tileIndex == 0 || tileIndex == 1 || tileIndex == 144 || tileIndex == 145 || tileIndex == 161 || tileIndex == 162 || tileIndex == 163) {
                        
                             _mapCollider[tileIndex].x = -1;
                             _mapCollider[tileIndex].y = -1;
                             _mapCollider[tileIndex].width = -1;
                             _mapCollider[tileIndex].height = -1;
                        
                        } else {
                 
                             _mapCollider[tileIndex].x = xx*_tileWidth;
                             _mapCollider[tileIndex].y = yy*_tileHeight;
                             _mapCollider[tileIndex].width = _tileWidth;
                             _mapCollider[tileIndex].height = _tileHeight;                        
                        
                        }
  
                }
                }

  };*/
