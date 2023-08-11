#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include "stack.h"

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

stack queue;

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
  clock_t start;
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
        } else {
        //else if (maze[y][x] == EMPTY) {
          printf("%c%c", ' ', ' ');
        }
      }
    }
    printf("\n");
  }
}

void printFaceDir(mouse* m) {
  if (m->faceDir == U) printf("Facing: UP\n");
  else if (m->faceDir == D) printf("Facing: DOWN\n");
  else if (m->faceDir == L) printf("Facing: LEFT\n");
  else if (m->faceDir == R) printf("Facing: RIGHT\n");
}

double getTime(mouse* m) {
  clock_t now = clock();
  double time_taken = ((double)(now - m->start) / CLOCKS_PER_SEC); //Seconds
  return time_taken;
}

void printMouseStat(mouse* m) {
  printf("Pos (x,y): (%d,%d)\n", m->pos.x, m->pos.y);
  printFaceDir(m);
  printf("Time: %f s\n", getTime(m));
}

mouse* createMouse(size_t mH, size_t mW) {
  mouse* m = (mouse*)malloc(sizeof(mouse));
  m->mazeH = mH;
  m->mazeW = mW;
  m->pos = (pos){-1,-1};
  m->startPos = (pos){-1,-1};
  m->targetPos = (pos){-1,-1};
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
  m->floodAr[wallPos.y][wallPos.x] = -1;
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

void getMinOfNBs(mouse* m, pos sP, int x, int y, int* min) {
  int value = m->floodAr[y][x];

  if (value < *min && !equal(sP, (pos){x,y}) && m->maze[y][x] != WALL) {
    *min = value;
  }

  if (equal(sP, (pos){x,y})) getMinOfNBs(m, sP, x + 1, y, min);
  if (equal(sP, (pos){x,y})) getMinOfNBs(m, sP, x - 1, y, min);
  if (equal(sP, (pos){x,y})) getMinOfNBs(m, sP, x, y + 1, min);
  if (equal(sP, (pos){x,y})) getMinOfNBs(m, sP, x, y - 1, min);
}

void addAllAccNBsToQ(mouse* m, pos sP, int x, int y) {
  if (!equal(sP, (pos){x,y}) && m->maze[y][x] != WALL) {
    push(&queue, &(pos){x,y});
  }

  if (equal(sP, (pos){x,y})) addAllAccNBsToQ(m, sP, x + 1, y);
  if (equal(sP, (pos){x,y})) addAllAccNBsToQ(m, sP, x - 1, y);
  if (equal(sP, (pos){x,y})) addAllAccNBsToQ(m, sP, x, y + 1);
  if (equal(sP, (pos){x,y})) addAllAccNBsToQ(m, sP, x, y - 1);
}

void updateFloodAr(mouse* m, int x, int y) {
  if (x >= m->mazeW || y >= m->mazeH || m->maze[y][x] == WALL) return;
  push(&queue, &(pos){x, y});
  while(!empty(&queue)) {
    pos* cur = pop(&queue);
    int curCellVal = m->floodAr[cur->y][cur->x];
    int min = 1000000;
    getMinOfNBs(m, *cur, cur->x, cur->y, &min);
    if (curCellVal <= min) {
      m->floodAr[cur->y][cur->x] = min + 1;
      addAllAccNBsToQ(m, *cur, cur->x, cur->y);
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
      continue;
    }
    int bpD = m->floodAr[bestPos.y][bestPos.x];
    int curD = m->floodAr[m->lfr[i].y][m->lfr[i].x];
    if (curD < bpD) {
      bestPos = m->lfr[i];
      bpi = i;
    }
  }
  return bpi;
}

void checkForTarget(mouse* m) {
  if (equal(m->pos, m->targetPos)) {
    m->targetFound = true;
  }
}

void render(mouse* m, int** maze) {
  system("clear");
  printArray2FS(m->mazeH, m->mazeW, m->floodAr);
  printMaze(m->mazeH, m->mazeW, maze, m);
  printMouseStat(m);
  usleep(10*1000);
}

bool noShorterPath(mouse* m) {
  int cD = m->floodAr[m->pos.y][m->pos.x];
  int nD;
  bool wall;
  for (int i = 0; i < 3; i++) {
    nD = m->floodAr[m->lfr[i].y][m->lfr[i].x];
    wall = isWall(m->lfr[i], m->maze);
    if (nD < cD && !wall) return false;
  }
  return true;
}

void floodFillSearch(mouse* m, int** maze) {
  m->pos = m->startPos;
  int bpi;
  m->start = clock();
  while(!m->targetFound) {
    getSurround(m, maze);
    if (noShorterPath(m)) {
      updateFloodAr(m, m->pos.x, m->pos.y);
    }
    bpi = detectBestMove(m);
    moveTo(m, bpi);
    checkForTarget(m);
    render(m, maze);
  }
  printf("Target found in %f s\n", getTime(m));
}

void returnToStart(mouse* m, int** maze) {

}

void findSPTP(mouse* m, int** maze) {
  for (int y = 0; y < m->mazeH; y++) {
    for (int x = 0; x < m->mazeH; x++) {
      if (maze[y][x] == MOUSE) {
        m->startPos = (pos){x,y};
      }
      else if (maze[y][x] == TARGET) {
        m->targetPos = (pos){x,y};
      }
    }
  }
  if (isNull(m->startPos)) {
    printf("Mouse not found!\n");
    exit(1);
  }
  else if (isNull(m->targetPos)) {
    printf("Target not found!\n");
    exit(1);
  }
}

void buildMazeFromFile(size_t h, size_t w, int** maze, char* fileName) {
  FILE* fptr = fopen(fileName, "r");
  char line[w];

  int y = 0;
  if (!fptr) {
      return;
  }
  while(fgets(line, w + 2, fptr)) {
    if (y >= h) break;
    for (int x = 0; x < w; x++) {
      if (line[x] == '#') {
        maze[y][x] = WALL;
      }
      else if (line[x] == '%') {
        maze[y][x] = TARGET;
      }
      else if (line[x] == 'M') {
        maze[y][x] = MOUSE;
      }
    }
    y++;
  }
  fclose(fptr);
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
  buildMazeFromFile(h, w, maze, mz);

  mouse* m = createMouse(h, w);
  findSPTP(m, maze);
  m->faceDir = U;

  initStack(&queue);
  initializeFloodArray(m);

  //m->startPos = (pos){1,1};
  floodFillSearch(m, maze);

  return 0;
}
