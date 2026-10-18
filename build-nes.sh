PATH=$PATH:./cc65
export CC65_HOME=./cc65
FILE_NAME="main"
TARGET="bin/main.nes"

cl65 -t nes -Oi $FILE_NAME -o $TARGET

