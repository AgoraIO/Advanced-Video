#! /bin/sh
g++ simplest_ffmpeg_mem_player.cpp -g -o simplest_ffmpeg_mem_player.exe \
-I /usr/local/include -L /usr/local/lib \
-lmingw32 -lSDLmain -lSDL -lavformat -lavcodec -lavutil -lswscale
