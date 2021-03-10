#!/bin/bash

chmod +x make_assets.sh
if [ $1 = "build" ]; then
	bash ./make_assets.sh
fi
rm log/log.log
make $1