#!/bin/bash

# Stolen from goodboy advanced

OUTPATH="source/assets"
ASSETS="../../assets"

for i in $(find ./assets/whale -type f -name "*.psd"); do
    psd $i -c
done

for i in $(find ./assets/text -type f -name "*.psd"); do
    psd $i -c
done

for i in $(find ./assets/weapons -type f -name "*.psd"); do
    psd $i -c
done

go run tools/colour-agg/main.go \
	./assets/out.png \
	./assets/weapons/gun_0_bullet.png \
	./assets/text/numbersfont.png \
	./assets/whale/whale_small.png \
	./assets/whale/whale_small_jump_0.png \
	./assets/whale/whale_small_jump_1.png 

rm -rf $OUTPATH
mkdir -p $OUTPATH

cd $OUTPATH

SPRITE_8x8=""
SPRITE_8x8="$SPRITE_8x8 -ftc"       # output to C files
SPRITE_8x8="$SPRITE_8x8 -gt"        # output tiled graphics
SPRITE_8x8="$SPRITE_8x8 -gB4"       # output 4bpp graphics
SPRITE_8x8="$SPRITE_8x8 -gu32"       # output data as byte array
SPRITE_8x8="$SPRITE_8x8 -gT ff00f7" # transparent colour
# SPRITE_8x8="$SPRITE_8x8 -pe 16"     # up to 16 colours in the palette

SPRITE_16x16="$SPRITE_8x8 -Mw 2 -Mh 2"
SPRITE_32x32="$SPRITE_8x8 -Mw 4 -Mh 4"
SPRITE_32x64="$SPRITE_8x8 -Mw 4 -Mh 8"
SPRITE_64x64="$SPRITE_8x8 -Mw 8 -Mh 8"

# grit \
# 	$ASSETS/whale/whale_small.png \
# 	$ASSETS/whale/whale_small_jump_0.png \
# 	$ASSETS/whale/whale_small_jump_1.png $SPRITE_16x16

# grit \
# 	$ASSETS/text/numbersfont.png $SPRITE_8x8

SP_OPTIONS=""
SP_OPTIONS="$SP_OPTIONS -ftc"
SP_OPTIONS="$SP_OPTIONS -gt"        			# output tiled graphics
SP_OPTIONS="$SP_OPTIONS -gT ff00f7" 			# RGB 24 BIT
SP_OPTIONS="$SP_OPTIONS -gB4"       			# output 4bpp graphics
SP_OPTIONS="$SP_OPTIONS -gu32"       			# output data as byte array
SP_OPTIONS="$SP_OPTIONS -pS" 					# Share pallet
SP_OPTIONS="$SP_OPTIONS -O spriteShared"		# Shared pallet name
# SP_OPTIONS="$SP_OPTIONS -Mw 2 -Mh 2"			# SPRITE_16x16

grit \
	$ASSETS/out.png \
	$ASSETS/weapons/gun_0_bullet.png \
	$ASSETS/text/numbersfont.png \
	$ASSETS/whale/whale_small.png \
	$ASSETS/whale/whale_small_jump_0.png \
	$ASSETS/whale/whale_small_jump_1.png $SP_OPTIONS

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