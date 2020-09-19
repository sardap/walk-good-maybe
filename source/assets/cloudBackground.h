
//{{BLOCK(cloudBackground)

//======================================================================
//
//	cloudBackground, 256x256@4, 
//	+ palette 256 entries, not compressed
//	+ 9 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 32x32 
//	Total size: 512 + 288 + 2048 = 2848
//
//	Time-stamp: 2020-09-17, 19:12:55
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.3
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_CLOUDBACKGROUND_H
#define GRIT_CLOUDBACKGROUND_H

#define cloudBackgroundTilesLen 288
extern const unsigned short cloudBackgroundTiles[144];

#define cloudBackgroundMapLen 2048
extern const unsigned short cloudBackgroundMap[1024];

#define cloudBackgroundPalLen 512
extern const unsigned short cloudBackgroundPal[256];

#endif // GRIT_CLOUDBACKGROUND_H

//}}BLOCK(cloudBackground)
