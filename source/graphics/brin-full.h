
//{{BLOCK(brin_full)

//======================================================================
//
//	brin_full, 512x256@4, 
//	+ palette 256 entries, not compressed
//	+ 31 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 64x32 
//	Total size: 512 + 992 + 4096 = 5600
//
//	Time-stamp: 2020-09-17, 17:15:21
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.3
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_BRIN_FULL_H
#define GRIT_BRIN_FULL_H

#define brin_fullTilesLen 992
extern const unsigned short brin_fullTiles[496];

#define brin_fullMapLen 4096
extern const unsigned short brin_fullMap[2048];

#define brin_fullPalLen 512
extern const unsigned short brin_fullPal[256];

#endif // GRIT_BRIN_FULL_H

//}}BLOCK(brin_full)
