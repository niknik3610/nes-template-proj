PATH=$PATH:./cc65
export CC65_HOME=./cc65
FILE_NAME="main.c"
TARGET="bin/main.nes"

# cl65 -t nes neslib/crt0.s -Oi $FILE_NAME  -o $TARGET
cl65 -t nes -Oi $FILE_NAME  -o $TARGET
rm ./*.o

