# Font-Data-for-the-Pixel-Game-Engine
A little tool to upload your own font files, and create the correct datastrings for it to be copied into the PGE header file.

Inspired by and to be applied in combination with One Lone Coder's Pixel Game Engine (olcPGE) - see: https://github.com/OneLoneCoder/olcPixelGameEngine/tree/master
Much obliged to Javidx9!

This repo contains:
* an excel file to manually edit pixel patterns and copy them into a text editor. In the example I started from Javidx9's original nes font file, and created a more 'slim' variant;
* stored this variant as a text file "nesfont_slim.txt";
* this txt file can be read by the cpp program main.cpp, using the <L> option to load a txt file;
* after loading, you can output texture data using the T key, or spacing data using the S key;
These data strings can be used to replace the existing data strings in the olcPGE header file, thus creating your own font for the PGE;

If you don't wanna hack into the belly of the PGE header file, a nice alternative is availble in the extension by Gorbit: https://github.com/gorbit99/olcPGEX_TTF 

If you like hacking into the PGE - Have fun with it!

Joseph21

