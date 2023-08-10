
#include <stdio.h>
#include <stdlib.h>

#define EMPTY 0
#define WALL 1
#define MOUSE 2
#define CHEESE 3

typedef struct mouse {
  int x, y;
  int mazeH, mazeW;
} mouse;

void buildMazeFromFile(size_t h, size_t w, int maze[h][w], char* fileName) {
  FILE* fptr = fopen(fileName, "r");
  char line[w];

  int y = 0;
  if (!fptr) return;
  while(fgets(line, w + 2, fptr)) {
    if (y >= h) break;
    printf("%s", line);
    for (int i = 0; i < w; i++) {
      if (line[i] == '#') {
        maze[y][i] = WALL;
      }
      else if (line[i] == '%') {
        maze[y][i] = CHEESE;
      }
      else if (line[i] == 'M') {
        maze[y][i] = MOUSE;
      }
    }
    y++;
  }
  fclose(fptr);
}

void nullMaze(size_t h, size_t w, int maze[h][w]) {
  for (int i = 0; i < h; i++) {
    for (int j = 0; j < w; j++) {
      maze[i][j] = 0;
    }
  }
}

int main(int argc, char** argv) {
  if (argc <= 2) {
    printf("Maze and maze size arguments needed");
    return 1;
  }
  size_t h = atoi(argv[2]);
  size_t w = atoi(argv[2]);
  int maze[h][w];
  nullMaze(h, w, maze);
  buildMazeFromFile(h, w, maze, argv[1]);

  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
        printf("%d ", maze[y][x]);
    }
    printf("\n");
  }
  return 0;
}
