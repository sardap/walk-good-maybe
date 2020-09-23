#!/bin/bash

# Stolen from goodboy advanced

OUTPATH="source/assets"
ASSETS="../../assets"

rm -rf $OUTPATH
mkdir -p $OUTPATH

cd $OUTPATH

SPRITE_8x8=""
SPRITE_8x8="$SPRITE_8x8 -ftc"       # output to C files
SPRITE_8x8="$SPRITE_8x8 -gt"        # output tiled graphics
SPRITE_8x8="$SPRITE_8x8 -gB4"       # output 4bpp graphics
SPRITE_8x8="$SPRITE_8x8 -gu32"       # output data as byte array
SPRITE_8x8="$SPRITE_8x8 -gT ff00f7" # transparent colour
SPRITE_8x8="$SPRITE_8x8 -pe 16"     # up to 16 colours in the palette

SPRITE_16x16="$SPRITE_8x8 -Mw 2 -Mh 2"
SPRITE_32x32="$SPRITE_8x8 -Mw 4 -Mh 4"
SPRITE_32x64="$SPRITE_8x8 -Mw 4 -Mh 8"
SPRITE_64x64="$SPRITE_8x8 -Mw 8 -Mh 8"

grit $ASSETS/whale/whale.png $SPRITE_32x32

BG_OPTIONS=""
BG_OPTIONS="$BG_OPTIONS -ftc"					# Create C file
BG_OPTIONS="$BG_OPTIONS -gT ff00f7" 			# RGB 24 BIT
BG_OPTIONS="$BG_OPTIONS -gB8"					# Bit depth 8
BG_OPTIONS="$BG_OPTIONS -gu16" 					# use short
BG_OPTIONS="$BG_OPTIONS -m"						# Export map
BG_OPTIONS="$BG_OPTIONS -mR8"					# Create Map
BG_OPTIONS="$BG_OPTIONS -mLs"					# Map 16 Bit
BG_OPTIONS="$BG_OPTIONS -pS" 					# Share pallet
BG_OPTIONS="$BG_OPTIONS -gS"					# Share tiles
BG_OPTIONS="$BG_OPTIONS -O titleScreenShared"	# Shared pallet name

grit \
	$ASSETS/title_screen/title_text.png \
	$ASSETS/background/backgroundSky.png \
	$ASSETS/background/build_tile_set.png $BG_OPTIONS