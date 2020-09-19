
//{{BLOCK(backgroundSky)

//======================================================================
//
//	backgroundSky, 256x256@4, 
//	+ palette 256 entries, not compressed
//	+ 8 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 32x32 
//	Total size: 512 + 256 + 2048 = 2816
//
//	Time-stamp: 2020-09-17, 17:21:27
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.3
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_BACKGROUNDSKY_H
#define GRIT_BACKGROUNDSKY_H

#define backgroundSkyTilesLen 256
extern const unsigned short backgroundSkyTiles[128];

#define backgroundSkyMapLen 2048
extern const unsigned short backgroundSkyMap[1024];

#define backgroundSkyPalLen 512
extern const unsigned short backgroundSkyPal[256];

#endif // GRIT_BACKGROUNDSKY_H

//}}BLOCK(backgroundSky)
