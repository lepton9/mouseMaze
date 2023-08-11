#ifndef STACK_H
#define STACK_H

#include <stdio.h>

#define MAX_SIZE 100
typedef struct {
  void* data[MAX_SIZE];
  int front;
  int back;
  int size;
} stack;

void initStack(stack* stack);
int full(stack* stack);
int empty(stack* stack);
void push(stack* stack, void* value);
void* pop(stack* stack);

#endif
