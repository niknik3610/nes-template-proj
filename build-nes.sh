PATH=$PATH:./cc65
export CC65_HOME=./cc65
NAME="main"
TARGET="bin/main.nes"

cl65 -t nes -Oi $1 -o $TARGET

