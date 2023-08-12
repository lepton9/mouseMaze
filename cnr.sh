#/bin/bash

gcc -g ./src/mouseMaze.c ./src/stack.c ./src/lList.c -I./include/ -o ./bin/mm
./bin/mm
