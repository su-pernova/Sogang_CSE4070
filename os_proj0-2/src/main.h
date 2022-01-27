#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "bitmap.h"
#include "debug.h"
#include "hash.h"
#include "hex_dump.h"
#include "limits.h"
#include "list.h"
#include "round.h"

struct bitmap_node {
	char title[10];
	struct bitmap *bitmap;
	struct bitmap_node *next;
}; struct bitmap_node *bitmap_node_head;

struct hash_node {
	char title[10];
	struct hash *hash;
	struct hash_node *next;
}; struct hash_node *hash_node_head;

struct list_node{
	char title[10];
	struct list *list;
	struct list_node *next;
}; struct list_node *list_node_head;

void create_cmd(char*);
void dumpdata_cmd(char*);
void delete_cmd(char*);

void my_bitmap_dump(struct bitmap*);
void my_hash_dump(struct hash*);
void my_list_dump(struct list*);

void bitmap_cmd(char*);
void hash_cmd(char*);
void list_cmd(char*);

struct bitmap_node *my_bitmap_find(char*);
struct hash_node *my_hash_find(char*);
struct list_node *my_list_find(char*);