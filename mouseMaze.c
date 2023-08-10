#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define U -1
#define D -2
#define L -3
#define R -4

#define EMPTY 0
#define WALL 1
#define MOUSE 2
#define TARGET 3

#define TURN_TIME_MS 10

typedef struct pos {
  int x, y;
} pos;

typedef struct mouse {
  pos pos;
  pos startPos;
  int mazeH, mazeW;
  pos targetPos;
  bool targetFound;
  int faceDir;
  int** maze;
  int** floodAr;
  pos* lfr;
  clock_t time;
} mouse;

bool isNull(pos p) {
  return (p.x < 0 || p.y < 0);
}

bool equal(pos p1, pos p2) {
  return (p1.x == p2.x) && (p1.y == p2.y);
}

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

void printMaze(size_t h, size_t w, int** maze, mouse* m) {
  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      if (equal(m->pos, (pos){x, y})) {
        printf("%c%c", '(', ')');
      }
      else if (equal(m->targetPos, (pos){x, y})) {
        printf("%c%c", '%', '%');
      } else {
        if (maze[y][x] == WALL) {
          printf("%c%c", '#', '#');
        }
        else if (maze[y][x] == EMPTY) {
          printf("%c%c", ' ', ' ');
        }
      }
    }
    printf("\n");
  }
}

mouse* createMouse(size_t mH, size_t mW, int sX, int sY) {
  mouse* m = (mouse*)malloc(sizeof(mouse));
  m->startPos.x = sX;
  m->startPos.y = sY;
  m->pos.x = sX;
  m->pos.y = sY;
  m->mazeH = mH;
  m->mazeW = mW;
  m->targetPos.x = -1;
  m->targetPos.y = -1;
  m->targetFound = false;
  m->floodAr = allocArray(mH, mW);
  nullArray(m->mazeH, m->mazeW, m->floodAr);
  m->maze = allocArray(mH, mW);
  nullArray(m->mazeH, m->mazeW, m->maze);
  m->lfr = malloc(sizeof(pos) * 3);
  return m;
}

pos posFront(const int faceDir, const pos p) {
  pos pL = p;
  if (faceDir == U) pL.y--;
  else if (faceDir == D) pL.y++;
  else if (faceDir == L) pL.x--;
  else if (faceDir == R) pL.x++;
  return pL;
}
pos posLeft(const int faceDir, const pos p) {
  pos pL = p;
  if (faceDir == U) pL.x--;
  else if (faceDir == D) pL.x++;
  else if (faceDir == L) pL.y++;
  else if (faceDir == R) pL.y--;
  return pL;
}
pos posRight(const int faceDir, const pos p) {
  pos pL = p;
  if (faceDir == U) pL.x++;
  else if (faceDir == D) pL.x--;
  else if (faceDir == L) pL.y--;
  else if (faceDir == R) pL.y++;
  return pL;
}
pos posBack(const int faceDir, const pos p) {
  pos pB = p;
  if (faceDir == U) pB.y++;
  else if (faceDir == D) pB.y--;
  else if (faceDir == L) pB.x++;
  else if (faceDir == R) pB.x--;
  return pB;
}

bool isWall(pos p, int** maze) {
  return maze[p.y][p.x] == WALL;
}

void insertWall(mouse* m, pos wallPos) {
  m->maze[wallPos.y][wallPos.x] = WALL;
}

void getSurround(mouse* m, int** maze) {
  pos pL = posLeft(m->faceDir, m->pos);
  pos pF = posFront(m->faceDir, m->pos);
  pos pR = posRight(m->faceDir, m->pos);
  if (isWall(pL, maze)) insertWall(m, pL);
  if (isWall(pF, maze)) insertWall(m, pF);
  if (isWall(pR, maze)) insertWall(m, pR);
  m->lfr[0] = pL;
  m->lfr[1] = pF;
  m->lfr[2] = pR;
}

void turnLeft(mouse* m) {
  if (m->faceDir == U) m->faceDir = L;
  else if (m->faceDir == D) m->faceDir = R;
  else if (m->faceDir == L) m->faceDir = D;
  else if (m->faceDir == R) m->faceDir = U;
}
void turnRight(mouse* m) {
  if (m->faceDir == U) m->faceDir = R;
  else if (m->faceDir == D) m->faceDir = L;
  else if (m->faceDir == L) m->faceDir = U;
  else if (m->faceDir == R) m->faceDir = D;
}
void turnAround(mouse* m) {
  if (m->faceDir == U) m->faceDir = D;
  else if (m->faceDir == D) m->faceDir = U;
  else if (m->faceDir == L) m->faceDir = R;
  else if (m->faceDir == R) m->faceDir = L;
}

void moveTo(mouse* m, int ilfr) {
  if (ilfr < 0) {
    m->pos = posBack(m->faceDir, m->pos);
    turnAround(m);
  } else {
    m->pos = m->lfr[ilfr];
    if (ilfr == 0) {
      turnLeft(m);
    }
    else if (ilfr == 2) {
      turnRight(m);
    }
  }
}

int distance(int x1, int y1, int x2, int y2) {
  return abs(x1 - x2) + abs(y1 - y2);
}

void initializeFloodArray(mouse* m) {
  if (m->targetPos.x == -1 || m->targetPos.y == -1) return;
  int d;
  for (int y = 0; y < m->mazeH; y++) {
    for (int x = 0; x < m->mazeW; x++) {
      d = distance(m->targetPos.x, m->targetPos.y, x, y);
      m->floodAr[y][x] = d;
    }
  }
}

void updateFloodArDist(mouse* m) {
  int d;
  for (int y = 0; y < m->mazeH; y++) {
    for (int x = 0; x < m->mazeW; x++) {

    }
  }
}

int detectBestMove(mouse* m) {
  pos bestPos = {-1, -1};
  int bpi = -1;
  for (int i = 0; i < 3; i++) {
    if (m->maze[m->lfr[i].y][m->lfr[i].x] == WALL) continue;
    if (isNull(bestPos)) {
      bestPos = m->lfr[i];
      bpi = i;
    }
    else if (m->floodAr[m->lfr[i].y][m->lfr[i].x] < m->floodAr[bestPos.y][bestPos.x]) {
      bestPos = m->lfr[i];
      bpi = i;
    }
  }
  return bpi;
}


void checkForTarget(mouse* m) {
  if (equal(m->pos, m->targetPos)) m->targetFound = true;
}

void floodFillSearch(mouse* m, int** maze) {
  int bpi;
  clock_t timeStart = clock();
  while(!m->targetFound) {
    getSurround(m, maze);
    updateFloodArDist(m);
    bpi = detectBestMove(m);
    moveTo(m, bpi);
    checkForTarget(m);
  }
  m->time = clock() - timeStart;
  printf("Target found in %d ms", (int)(m->time * 1000 / CLOCKS_PER_SEC));
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

  mouse* m = createMouse(h, w, 1, h - 2);
  m->targetPos = targetPos;
  m->faceDir = U;

  initializeFloodArray(m);
  printMaze(h, w, maze, m);

  return 0;
}
