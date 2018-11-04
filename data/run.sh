#!/bin/sh

CURR_DIR=$(dirname $0)
echo $(cat data/config/local)
./bin/sdl_graphic_library_executable $(cat data/config/local)
