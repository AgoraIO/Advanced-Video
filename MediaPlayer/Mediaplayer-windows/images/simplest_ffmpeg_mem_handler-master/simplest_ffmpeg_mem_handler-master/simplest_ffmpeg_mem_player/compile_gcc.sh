#! /bin/sh
gcc simplest_ffmpeg_mem_player.cpp -g -o simplest_ffmpeg_mem_player.out -lstdc++ \
-I /usr/local/include -L /usr/local/lib -lSDLmain -lSDL -lavformat -lavcodec -lavutil -lswscale

