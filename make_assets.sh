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

TITLE_SCREEN_OPTIONS=""
TITLE_SCREEN_OPTIONS="$TITLE_SCREEN_OPTIONS -ftc"					# Create C file
TITLE_SCREEN_OPTIONS="$TITLE_SCREEN_OPTIONS -gT ff00f7" 			# RGB 24 BIT
TITLE_SCREEN_OPTIONS="$TITLE_SCREEN_OPTIONS -gB8"					# Bit depth 8
TITLE_SCREEN_OPTIONS="$TITLE_SCREEN_OPTIONS -gu16" 					# use short
TITLE_SCREEN_OPTIONS="$TITLE_SCREEN_OPTIONS -m"						# Export map
TITLE_SCREEN_OPTIONS="$TITLE_SCREEN_OPTIONS -mR8"					# Create Map
TITLE_SCREEN_OPTIONS="$TITLE_SCREEN_OPTIONS -mLs"					# Map 16 Bit
TITLE_SCREEN_OPTIONS="$TITLE_SCREEN_OPTIONS -pS" 					# Share pallet
TITLE_SCREEN_OPTIONS="$TITLE_SCREEN_OPTIONS -gS"					# Share tiles
TITLE_SCREEN_OPTIONS="$TITLE_SCREEN_OPTIONS -O titleScreenShared"	# Shared pallet name

grit \
	$ASSETS/title_screen/title_text.png \
	$ASSETS/title_screen/backgroundSky.png $TITLE_SCREEN_OPTIONS