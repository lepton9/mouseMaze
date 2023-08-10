#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EMPTY 0
#define WALL 1
#define MOUSE 2
#define TARGET 3

typedef struct pos {
  int x, y;
} pos;

typedef struct mouse {
  int x, y;
  int startX, startY;
  int mazeH, mazeW;
  int targetX, targetY;
  int** floodFAr;
} mouse;

int** allocArray(size_t h, size_t w) {
  int** ar = (int**)malloc(sizeof(int*) * h);
  for (int i = 0; i < h; i++) {
    ar[i] = (int*)malloc(sizeof(int) * w);
  }
  return ar;
}

void nullArray(size_t h, size_t w, int** maze) {
  for (size_t i = 0; i < h; i++) {
    memset(maze[i], 0, w * sizeof(int));
  }
}

void printArray(size_t h, size_t w, int** maze) {
  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      printf("%d ", maze[y][x]);
    }
    printf("\n");
  }
}

void printArray2FS(size_t h, size_t w, int** maze) {
  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      printf("%2d ", maze[y][x]);
    }
    printf("\n");
  }
}

mouse* createMouse(size_t mH, size_t mW, int sX, int sY) {
  mouse* m = (mouse*)malloc(sizeof(mouse));
  m->x = sX;
  m->y = sY;
  m->mazeH = mH;
  m->mazeW = mW;
  m->targetX = -1;
  m->targetY = -1;
  m->floodFAr = allocArray(mH, mW);
  nullArray(m->mazeH, m->mazeW, m->floodFAr);
  return m;
}


int distance(int x1, int y1, int x2, int y2) {
  return abs(x1 - x2) + abs(y1 - y2);
}

void initializeFloodArray(size_t h, size_t w, int** maze, mouse* m) {
  int d;
  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      d = distance(m->targetX, m->targetY, x, y);
      m->floodFAr[y][x] = d;
    }
  }
}

void floodFillSearch() {

}

void findTarget() {

}

pos buildMazeFromFile(size_t h, size_t w, int** maze, char* fileName) {
  FILE* fptr = fopen(fileName, "r");
  char line[w];

  int y = 0;
  pos tarPos;
  if (!fptr) {
      tarPos.x = -1;
      tarPos.y = -1;
      return tarPos;
  }
  while(fgets(line, w + 2, fptr)) {
    if (y >= h) break;
    printf("%s", line);
    for (int x = 0; x < w; x++) {
      if (line[x] == '#') {
        maze[y][x] = WALL;
      }
      else if (line[x] == '%') {
        maze[y][x] = TARGET;
        tarPos.x = x;
        tarPos.y = y;
      }
      else if (line[x] == 'M') {
        maze[y][x] = MOUSE;
      }
    }
    y++;
  }
  fclose(fptr);
  return tarPos;
}

int main(int argc, char** argv) {
  char* mz;
  char* mS;
  if (argc <= 2) {
    char str[100];
    printf("Give maze and maze size: ");
    fgets(str, sizeof(str), stdin);
    char* ptr = strtok(str, " ");
    mz = ptr;
    ptr = strtok(NULL, " ");
    mS = ptr;
  } else {
    mz = argv[1];
    mS = argv[2];
  }
  size_t h = atoi(mS);
  size_t w = atoi(mS);
  int** maze = allocArray(h, w);
  nullArray(h, w, maze);
  pos targetPos = buildMazeFromFile(h, w, maze, mz);

  mouse* m = createMouse(h, w, 0, 0);
  m->targetX = targetPos.x;
  m->targetY = targetPos.y;

  initializeFloodArray(h, w, maze, m);
  printArray2FS(h, w, m->floodFAr);

  return 0;
}
