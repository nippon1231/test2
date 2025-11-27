#include <genesis.h>
#include "game.h"
#include "resources.h"

Map* levelmap;




Map* level_init(u16 ind)
{

    VDP_loadTileSet(&bga_tileset,ind,DMA);
    levelmap=MAP_create(&bga_map,BG_A,TILE_ATTR_FULL(PAL1,false,false,false,ind));
    return levelmap;
}