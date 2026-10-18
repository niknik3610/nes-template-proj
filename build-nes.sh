PATH=$PATH:./cc65
export CC65_HOME=./cc65
FILE_NAME="main.c"
TARGET="bin/main.nes"
INCLUDES="./include"

cl65 -t nes -C $INCLUDES/nrom_32k_vert.cfg neslib/crt0.s -Oi $FILE_NAME  -o $TARGET
rm ./*.o

