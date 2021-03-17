#!/bin/bash

# Stolen from goodboy advanced

OUTPATH="source/assets"
ASSETS="${PWD}/assets"

PWD_OLD=$(pwd)

function gen_png {
	echo "pnging $1"
	for i in $(find $1 -type f -name "*.psd"); do
		convert -quiet "${i%.*}.psd" -flatten "${i%.*}.png"
	done
}

OBJECTS=""

function add_objects {
	for i in $(find $1 -type f -name "*.png"); do
		TMP="${i%.*}.png"
		OBJECTS="$OBJECTS $PWD/${TMP:2} "
	done
}

BACKGROUNDS=""

function add_background {
	for i in $(find $1 -type f -name "*.png"); do
		BACKGROUNDS="$BACKGROUNDS ${i%.*}.png "
	done
}


gen_png "./assets/background"
add_background "./assets/background"

colour-agg.exe ./assets/backgrounds_out.png $BACKGROUNDS

gen_png "./assets/whale"
add_objects "./assets/whale"

gen_png "./assets/text"
add_objects "./assets/text"

gen_png "./assets/misc"
add_objects "./assets/misc"

gen_png "./assets/weapons"
add_objects "./assets/weapons"

gen_png "./assets/enemy"
add_objects "./assets/enemy"

gen_png "./assets/obstacles"
add_objects "./assets/obstacles"

gen_png "./assets/game_intro"


OBJECTS="$OBJECTS $PWD/assets/text/lifeTitle.png "

# colour-agg.exe ./assets/objects_out.png $OBJECTS

rm -rf $OUTPATH
mkdir -p $OUTPATH

cd $OUTPATH

SP_OPTIONS=""
SP_OPTIONS="$SP_OPTIONS -ftc"
SP_OPTIONS="$SP_OPTIONS -gt"        			# output tiled graphics
SP_OPTIONS="$SP_OPTIONS -gT ff00f7" 			# RGB 24 BIT
SP_OPTIONS="$SP_OPTIONS -gB8"       			# output 4bpp graphics
SP_OPTIONS="$SP_OPTIONS -gu32"       			# output data as byte array
SP_OPTIONS="$SP_OPTIONS -pS" 					# Share pallet
SP_OPTIONS="$SP_OPTIONS -O spriteShared"		# Shared pallet name
# SP_OPTIONS="$SP_OPTIONS -Mw 2 -Mh 2"			# SPRITE_16x16

echo "Creating object tiles / pal / map"
grit \
	$OBJECTS \
	$SP_OPTIONS

BG_OPTIONS=""
BG_OPTIONS="$BG_OPTIONS -ftc"					# Create C file
BG_OPTIONS="$BG_OPTIONS -gT ff00f7" 			# RGB 24 BIT
BG_OPTIONS="$BG_OPTIONS -gB8"					# Bit depth 8
BG_OPTIONS="$BG_OPTIONS -gu16" 					# use short
BG_OPTIONS="$BG_OPTIONS -m"						# Export map
BG_OPTIONS="$BG_OPTIONS -mR8"					# Create Map
BG_OPTIONS="$BG_OPTIONS -mLs"					# Map 16 Bit
BG_OPTIONS="$BG_OPTIONS -pS" 					# Share pallet
BG_OPTIONS="$BG_OPTIONS -O mainGameShared"	# Shared pallet name

echo "Creating background tiles for main game / pal / map"
grit \
	$ASSETS/background/fog.png \
	$ASSETS/background/backgroundCity.png \
	$ASSETS/background/building0TileSet.png \
	$ASSETS/background/building1TileSet.png \
	$ASSETS/background/building2TileSet.png \
	$ASSETS/background/building3TileSet.png \
	$ASSETS/background/lava0TileSet.png \
	$ASSETS/background/buildingtileset.png \
	$BG_OPTIONS


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

echo "Creating background tiles for title screen / pal / map"
grit \
	$ASSETS/backgrounds_out.png \
	$ASSETS/title_screen/title_text.png \
	$ASSETS/background/cloud.png \
	$ASSETS/background/backgroundSky.png $BG_OPTIONS


BG_OPTIONS=""
BG_OPTIONS="$BG_OPTIONS -ftc"					# Create C file
BG_OPTIONS="$BG_OPTIONS -gT ff00f7" 			# RGB 24 BIT
BG_OPTIONS="$BG_OPTIONS -gB8"					# Bit depth 8
BG_OPTIONS="$BG_OPTIONS -gu16" 					# use short
BG_OPTIONS="$BG_OPTIONS -ga 240" 				# start pallet at 240
BG_OPTIONS="$BG_OPTIONS -m"						# Export map
BG_OPTIONS="$BG_OPTIONS -ma 450"				# Tiles start at 450
BG_OPTIONS="$BG_OPTIONS -mLs"					# Map 16 Bit
BG_OPTIONS="$BG_OPTIONS -pS" 					# Share pallet
BG_OPTIONS="$BG_OPTIONS -ps 240" 				# start pallet at 240
BG_OPTIONS="$BG_OPTIONS -ps 249" 				# start pallet at 240
BG_OPTIONS="$BG_OPTIONS -pn 15" 				# start pallet at 240
BG_OPTIONS="$BG_OPTIONS -gS"					# Share tiles
BG_OPTIONS="$BG_OPTIONS -O giBackgroundShared"	# Shared pallet name

echo "Creating reg background for game intro tiles / pal / map"
grit \
	$ASSETS/game_intro/giEmpty.png \
	$ASSETS/game_intro/ready.png \
	$ASSETS/game_intro/set.png \
	$ASSETS/game_intro/go.png \
	$ASSETS/game_intro/giSky.png $BG_OPTIONS

BG_OPTIONS=""
BG_OPTIONS="$BG_OPTIONS -ftc"						# Create C file
BG_OPTIONS="$BG_OPTIONS -gT ff00f7" 				# RGB 24 BIT
BG_OPTIONS="$BG_OPTIONS -gB8"						# Bit depth 8
BG_OPTIONS="$BG_OPTIONS -gu16" 						# use short
BG_OPTIONS="$BG_OPTIONS -m"							# Export map
BG_OPTIONS="$BG_OPTIONS -mLa"						# Map Affine
BG_OPTIONS="$BG_OPTIONS -mRa"						# Tile reudciton needs this to stop from that stupid flip tile reduciotn
BG_OPTIONS="$BG_OPTIONS -pS" 						# Share pallet
BG_OPTIONS="$BG_OPTIONS -gS"						# Share tiles
BG_OPTIONS="$BG_OPTIONS -O giBackgroundAffShared"	# Shared pallet name

echo "Creating affine background for game intro tiles / pal / map"
grit \
	$ASSETS/game_intro/giEmpty.png \
	$ASSETS/game_intro/giCityTop.png \
	$BG_OPTIONS

SP_OPTIONS=""
SP_OPTIONS="$SP_OPTIONS -ftc"
SP_OPTIONS="$SP_OPTIONS -gt"        			# output tiled graphics
SP_OPTIONS="$SP_OPTIONS -gT ff00f7" 			# RGB 24 BIT
SP_OPTIONS="$SP_OPTIONS -gB8"       			# output 4bpp graphics
SP_OPTIONS="$SP_OPTIONS -gu32"       			# output data as byte array
SP_OPTIONS="$SP_OPTIONS -pS" 					# Share pallet
SP_OPTIONS="$SP_OPTIONS -O giSpriteShared"		# Shared pallet name
# SP_OPTIONS="$SP_OPTIONS -Mw 2 -Mh 2"			# SPRITE_16x16

echo "Creating objects for game_intro"
grit \
	$ASSETS/game_intro/whaleLarge.png \
	$SP_OPTIONS


echo "compelte creating assets"
cd ${PWD_OLD}