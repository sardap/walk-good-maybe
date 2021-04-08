#!/bin/bash

# Stolen from goodboy advanced

OUTPATH="source/assets"
ASSETS="${PWD}/assets"

PWD_OLD=$(pwd)

function gen_png {
	echo "pnging $1"
	for i in $(find $1 -type f -name "*.psd"); do
		convert -quiet "${i%.*}.psd" -flatten "${i%.*}.png"
		gba-colourer "${i%.*}.png"
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

gen_png "./assets/effects"
add_objects "./assets/effects"

gen_png "./assets/game_intro"

gen_png "./assets/title_screen"

gen_png "./assets/credits"

gen_png "./assets/sound_test"


OBJECTS="$OBJECTS $PWD/assets/text/lifeTitle.png "

version-img-gen ./assets/title_screen/tsTitleText.png

colour-agg.exe ./assets/title_screen/tsBackgroundAgg.png \
	./assets/title_screen/tsEmpty.png \
	./assets/title_screen/tsWater.png \
	./assets/title_screen/tsLava.png \
	./assets/title_screen/tsCity.png \
	./assets/title_screen/tsBeach.png \
	./assets/title_screen/tsTitleText.png \
	./assets/title_screen/tsSoundTestText.png


colour-agg.exe ./assets/credits/crColourAgg.png \
	./assets/credits/crEmpty.png \
	./assets/credits/crPaulFace.png \
	./assets/credits/crSpace.png 

rm -rf $OUTPATH
mkdir -p $OUTPATH

cd $OUTPATH

SP_OPTIONS=""
SP_OPTIONS="$SP_OPTIONS -ftc"
SP_OPTIONS="$SP_OPTIONS -gt"        			# output tiled graphics
SP_OPTIONS="$SP_OPTIONS -gT ff00f7" 			# RGB 24 BIT
SP_OPTIONS="$SP_OPTIONS -gB8"       			# output 8bpp graphics
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
BG_OPTIONS="$BG_OPTIONS -O mainGameCityShared"	# Shared pallet name

echo "Creating background tiles for city main game / pal / map"
grit \
	$ASSETS/background/fog.png \
	$ASSETS/background/backgroundCity.png \
	$ASSETS/background/building0TileSet.png \
	$ASSETS/background/building1TileSet.png \
	$ASSETS/background/building2TileSet.png \
	$ASSETS/background/building3TileSet.png \
	$ASSETS/background/building4TileSet.png \
	$ASSETS/background/building5TileSet.png \
	$ASSETS/background/building6TileSet.png \
	$ASSETS/background/lava0TileSet.png \
	$ASSETS/background/buildingtileset.png \
	$ASSETS/background/mgPauseCity.png \
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
BG_OPTIONS="$BG_OPTIONS -O mainGameBeachShared"	# Shared pallet name

echo "Creating background tiles for beach main game / pal / map"
grit \
	$ASSETS/title_screen/tsEmpty.png \
	$ASSETS/title_screen/tsWater.png \
	$ASSETS/title_screen/tsLava.png \
	$ASSETS/background/mgPauseBeach.png \
	$ASSETS/background/mgBeach.png \
	$ASSETS/background/mgBeachWaterFog00.png \
	$ASSETS/background/mgBeachWaterFog01.png \
	$ASSETS/background/mgBeachIsland00.png \
	$BG_OPTIONS


BG_OPTIONS=""
BG_OPTIONS="$BG_OPTIONS -ftc"					# Create C file
BG_OPTIONS="$BG_OPTIONS -gT ff00f7" 			# RGB 24 BIT
BG_OPTIONS="$BG_OPTIONS -gB8"					# Bit depth 8
BG_OPTIONS="$BG_OPTIONS -gzl" 					# LZ77 compressed
BG_OPTIONS="$BG_OPTIONS -m"						# Export map
BG_OPTIONS="$BG_OPTIONS -mR8"					# Create Map
BG_OPTIONS="$BG_OPTIONS -mLs"					# Map 16 Bit
BG_OPTIONS="$BG_OPTIONS -pS" 					# Share pallet
BG_OPTIONS="$BG_OPTIONS -gS"					# Share tiles
BG_OPTIONS="$BG_OPTIONS -O titleScreenShared"	# Shared pallet name

echo "Creating background tiles for title screen / pal / map"
grit \
	$ASSETS/title_screen/tsEmpty.png \
	$ASSETS/title_screen/tsWater.png \
	$ASSETS/title_screen/tsLava.png \
	$ASSETS/title_screen/tsCity.png \
	$ASSETS/title_screen/tsBeach.png \
	$ASSETS/title_screen/tsBeachGameText.png \
	$ASSETS/title_screen/tsCityGameText.png \
	$ASSETS/title_screen/tsTitleText.png \
	$ASSETS/title_screen/tsCredits.png \
	$ASSETS/title_screen/tsSoundTestText.png \
	$BG_OPTIONS

SP_OPTIONS=""
SP_OPTIONS="$SP_OPTIONS -ftc"
SP_OPTIONS="$SP_OPTIONS -gt"        			# output tiled graphics
SP_OPTIONS="$SP_OPTIONS -gT ff00f7" 			# RGB 24 BIT
SP_OPTIONS="$SP_OPTIONS -gB8"       			# output 8bpp graphics
SP_OPTIONS="$SP_OPTIONS -pS" 					# Share pallet
SP_OPTIONS="$SP_OPTIONS -O tsSpirteShared"		# Shared pallet name

echo "Creating objects for title screen"
grit \
	$ASSETS/title_screen/tsArrow.png \
	$ASSETS/title_screen/tsArrowRed.png \
	$SP_OPTIONS

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
BG_OPTIONS="$BG_OPTIONS -gzl" 						# LZ77 compressed
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
SP_OPTIONS="$SP_OPTIONS -gB8"       			# output 8bpp graphics
SP_OPTIONS="$SP_OPTIONS -gu32"       			# output data as byte array
SP_OPTIONS="$SP_OPTIONS -pS" 					# Share pallet
SP_OPTIONS="$SP_OPTIONS -O giSpriteShared"		# Shared pallet name
# SP_OPTIONS="$SP_OPTIONS -Mw 2 -Mh 2"			# SPRITE_16x16

echo "Creating objects for game_intro"
grit \
	$ASSETS/game_intro/giWhale_air_0.png \
	$ASSETS/game_intro/giWhale_air_1.png \
	$ASSETS/game_intro/giWhale_air_2.png \
	$ASSETS/game_intro/giWhale_air_3.png \
	$SP_OPTIONS

BG_OPTIONS=""
BG_OPTIONS="$BG_OPTIONS -ftc"					# Create C file
BG_OPTIONS="$BG_OPTIONS -gT ff00f7" 			# RGB 24 BIT
BG_OPTIONS="$BG_OPTIONS -gB8"					# Bit depth 8
BG_OPTIONS="$BG_OPTIONS -gu8"					# export as bytes
BG_OPTIONS="$BG_OPTIONS -m"						# Export map
BG_OPTIONS="$BG_OPTIONS -ma 600"				# Tiles start at 600
BG_OPTIONS="$BG_OPTIONS -mR8"					# Create Map
BG_OPTIONS="$BG_OPTIONS -mLs"					# Map 16 Bit
BG_OPTIONS="$BG_OPTIONS -p" 					# exculde pallet output
BG_OPTIONS="$BG_OPTIONS -pe 10" 				# export up until 10

echo "Creating space background for credits / pal / map"
grit \
	$ASSETS/credits/crSpace.png $BG_OPTIONS


BG_OPTIONS=""
BG_OPTIONS="$BG_OPTIONS -ftc"					# Create C file
BG_OPTIONS="$BG_OPTIONS -gT ff00f7" 			# RGB 24 BIT
BG_OPTIONS="$BG_OPTIONS -gB8"					# Bit depth 8
BG_OPTIONS="$BG_OPTIONS -m"						# Export map
BG_OPTIONS="$BG_OPTIONS -ma 0"					# Tiles start at 0
BG_OPTIONS="$BG_OPTIONS -mR8"					# Create Map
BG_OPTIONS="$BG_OPTIONS -mLs"					# Map 16 Bit
BG_OPTIONS="$BG_OPTIONS -p" 					# exculde pallet output
BG_OPTIONS="$BG_OPTIONS -pe 120" 				# export up until 100
BG_OPTIONS="$BG_OPTIONS -Zl" 					# compress all with lz77

echo "Creating background tiles for credits / pal / map"
grit \
	$ASSETS/credits/crPaulFace.png \
	$ASSETS/credits/crWhaleLargeStanding.png \
	$ASSETS/credits/crBiscut.png \
	$ASSETS/credits/crPaulMic.png \
	$ASSETS/credits/crBuildings.png \
	$ASSETS/credits/crRealBuildings.png \
	$ASSETS/credits/crTunaPasta.png \
	$ASSETS/credits/crEmpty.png \
	$ASSETS/credits/crPatrickFace.png $BG_OPTIONS

BG_OPTIONS=""
BG_OPTIONS="$BG_OPTIONS -ftc"					# Create C file
BG_OPTIONS="$BG_OPTIONS -gT ff00f7" 			# RGB 24 BIT
BG_OPTIONS="$BG_OPTIONS -gB8"					# Bit depth 8
BG_OPTIONS="$BG_OPTIONS -gu8"					# export as bytes
BG_OPTIONS="$BG_OPTIONS -m"						# Export map
BG_OPTIONS="$BG_OPTIONS -mR8"					# Create Map
BG_OPTIONS="$BG_OPTIONS -mLs"					# Map 16 Bit
BG_OPTIONS="$BG_OPTIONS -pS" 					# Share pallet
BG_OPTIONS="$BG_OPTIONS -gS"					# Share tiles
BG_OPTIONS="$BG_OPTIONS -O stSharedBackground"	# Shared pallet name
BG_OPTIONS="$BG_OPTIONS -Zl" 					# compress all with lz77

echo "Creating background tiles for sound test / pal / map"
grit \
	$ASSETS/sound_test/stBackground.png \
	$BG_OPTIONS

SP_OPTIONS=""
SP_OPTIONS="$SP_OPTIONS -ftc"
SP_OPTIONS="$SP_OPTIONS -gt"        			# output tiled graphics
SP_OPTIONS="$SP_OPTIONS -gT ff00f7" 			# RGB 24 BIT
SP_OPTIONS="$SP_OPTIONS -gB8"       			# output 8bpp graphics
SP_OPTIONS="$SP_OPTIONS -pS" 					# Share pallet
SP_OPTIONS="$SP_OPTIONS -O stSpirteShared"		# Shared pallet name

echo "Creating objects for sound test"
grit \
	$ASSETS/sound_test/stArrow.png \
	$ASSETS/sound_test/stArrowRed.png \
	$SP_OPTIONS


echo "compelte creating assets"
cd ${PWD_OLD}