#include "../include/stack.h"

void initialize(stack* stack) {
  stack->front = 0;
  stack->back = 0;
  stack->size = 0;
}

int full(stack* stack) {
  return (stack->size == MAX_SIZE);
}

int empty(stack* stack) {
  return (stack->size == 0);
}

void push(stack* stack, void* value) {
  if (full(stack)) {
    printf("Stack is full -> %d\n", stack->size);
    return;
  }
  if (empty(stack)) {
    stack->front = 0;
    stack->back = 0;
  }
  stack->data[stack->back] = value;
  stack->size++;
  stack->back++;
  if (stack->back == MAX_SIZE) stack->back = 0;
}

void* pop(stack* stack) {
  if (empty(stack)) {
    return NULL;
  }
  void* value = stack->data[stack->front];
  stack->front++;
  if (stack->front == MAX_SIZE) stack->front = 0;
  stack->size--;
  return value;
}

void clear(stack* stack) {
  while(!empty(stack)) {
    pop(stack);
  }
  stack->front = 0;
  stack->back = 0;
}

