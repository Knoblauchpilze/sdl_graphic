#!/bin/sh

CURR_DIR=$(dirname $0)

gdb --args ./bin/sdl_graphic_library_executable $(cat data/config/local)
