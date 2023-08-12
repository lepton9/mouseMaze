#ifndef LLIST_H
#define LLIST_H

typedef struct node {
  void* data;
  struct node* next;
} node;

typedef struct lList {
  int size;
  node* head;
} lList;

lList* create_list();
int get_size(lList* list);
node* copy_nodes(node* head);
void copy_list(lList* listTo, lList* listFrom);
void add_to_begin(lList* list, void* data);
void add_to_end(lList* list, void* data);
void* pop_front(lList* list);
void* pop_last(lList* list);
int is_empty(lList* list);
int list_size(lList* list);
void list_clear(lList* list);

#endif
