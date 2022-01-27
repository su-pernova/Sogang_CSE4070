#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bitmap.h"
#include "debug.h"
#include "hash.h"
#include "hex_dump.h"
#include "limits.h"
#include "list.h"
#include "round.h"

#include "main.h"

int main(){
	char input[50];
	bitmap_node_head = NULL;
	hash_node_head = NULL;
	list_node_head = NULL;

	while(1){
		// get standard input data
		fgets(input, 50, stdin);
		input[strlen(input)-1]='\0';

		// make proper action
		if(!strncmp(input, "quit", 4)) exit(1);

		else if(!strncmp(input, "create", 6)) create_cmd(input);
		else if(!strncmp(input, "dumpdata", 8)) dumpdata_cmd(input);
		else if(!strncmp(input, "delete", 6)) delete_cmd(input);

		else if(!strncmp(input, "bitmap", 6)) bitmap_cmd(input);
		else if(!strncmp(input, "hash", 4)) hash_cmd(input);
		else if(!strncmp(input, "list", 4)) list_cmd(input);
	}
}

/* ------------ create function ------------ */
void create_cmd(char *input){
	char input_create[4][10] = {'\0', };
	char *ptr = strtok(input, " ");

	// save input command in array 'input_create'
	int i = 0;
	while(ptr != NULL){
		strcpy(input_create[i],ptr);
		ptr = strtok(NULL, " ");
		i++;
	}

	// 1. bitmap creation
	if(!strcmp(input_create[1], "bitmap")){
		struct bitmap_node *new_node;
		new_node = (struct bitmap_node*)malloc(sizeof(struct bitmap_node));

		strcpy(new_node->title,input_create[2]);
		new_node->bitmap = bitmap_create(atoi(input_create[3]));
		new_node->next = NULL;

		if(bitmap_node_head == NULL) bitmap_node_head = new_node;
		else{
			struct bitmap_node *temp = bitmap_node_head;
			while(temp->next != NULL) temp = temp->next;
			temp->next = new_node;
		}
	}

	// 2. hashtable creation
	else if(!strcmp(input_create[1], "hashtable")){
		struct hash_node *new_node;
		new_node = (struct hash_node*)malloc(sizeof(struct hash_node));

		strcpy(new_node->title,input_create[2]);
		new_node->hash = (struct hash*)malloc(sizeof(struct hash));
		hash_init(new_node->hash, my_hash_function, hash_less, NULL);
		new_node->next = NULL;

		if(hash_node_head == NULL) hash_node_head = new_node;
		else{
			struct hash_node *temp = hash_node_head;
			while(temp->next!=NULL) temp = temp->next;
			temp->next = new_node;
		}
	}

	// 3. list creation
	else if(!strcmp(input_create[1], "list")){
		struct list_node *new_node;
		new_node = (struct list_node*)malloc(sizeof(struct list_node));

		strcpy(new_node->title, input_create[2]);
		new_node->list = (struct list*)malloc(sizeof(struct list));
		list_init(new_node->list);
		new_node->next = NULL;

		if(list_node_head == NULL) list_node_head = new_node;
		else{
			struct list_node *temp = list_node_head;
			while(temp->next!=NULL) temp = temp->next;
			temp->next = new_node;
		}
	}
	return;
}

/* ------------ dumpdata function ------------ */
void dumpdata_cmd(char *input){
	char input_dumpdata[2][10] = {'\0', };
	char *ptr = strtok(input, " ");
	struct bitmap_node *temp_bitmap = bitmap_node_head;
	struct hash_node *temp_hash = hash_node_head;
	struct list_node *temp_list = list_node_head;

	// save input command in array 'input_dump'
	int i = 0;
	while(ptr!=NULL){
		strcpy(input_dumpdata[i], ptr);
		ptr = strtok(NULL, " ");
		i++;
	}

	// 1. bitmap dumpdata
	while(1){
		if(temp_bitmap == NULL) break;
		if(!strcmp(temp_bitmap->title, input_dumpdata[1])){
			my_bitmap_dump(temp_bitmap->bitmap);
			return;
		}
		else temp_bitmap = temp_bitmap->next;
	}

	// 2. hashtable dumpdata
	while(1){
		if(temp_hash == NULL) break;
		if(!strcmp(temp_hash->title, input_dumpdata[1])){
			my_hash_dump(temp_hash->hash);
			return;
		}
		else temp_hash = temp_hash->next;
	}

	// 3. list dumpdata
	while(1){
		if(temp_list == NULL) break;
		if(!strcmp(temp_list->title, input_dumpdata[1])){
			my_list_dump(temp_list->list);
			return;
		}
		else temp_list = temp_list->next;
	}


}

/* ------------ delete function ------------ */
void delete_cmd(char *input){
	char input_delete[2][10] = {'\0', };
	char *ptr = strtok(input, " ");
	struct bitmap_node *temp_bitmap = bitmap_node_head;
	struct hash_node *temp_hash = hash_node_head;
	struct list_node *temp_list = list_node_head;

	// save input command in array 'input_delete'
	int i = 0;
	while(ptr != NULL){
		strcpy(input_delete[i], ptr);
		ptr = strtok(NULL, " ");
		i++;
	}

	// 1. bitmap delete
	while(1){
		if(temp_bitmap == NULL) break;
		if(!strcmp(temp_bitmap->title, input_delete[1])){
			bitmap_destroy(temp_bitmap->bitmap);
			struct bitmap_node *temp = bitmap_node_head;
			if(temp_bitmap == bitmap_node_head){
				bitmap_node_head = NULL;
				free(temp_bitmap);
				return;
			}
			while(temp->next != temp_bitmap) temp = temp->next;
			temp->next = temp->next->next;
			free(temp_bitmap);
			return;
		}
		else temp_bitmap = temp_bitmap->next;
	}

	// 2. hashtable delete
	while(1){
		if(temp_hash == NULL) break;
		if(!strcmp(temp_hash->title, input_delete[1])){
			hash_destroy(temp_hash->hash, hash_free);
			struct hash_node *temp = hash_node_head;
			if(temp_hash == hash_node_head){
				free(temp_hash);
				hash_node_head = NULL;
				return;
			}
			while(temp->next != temp_hash) temp = temp->next;
			temp->next = temp->next->next;
			free(temp_hash);
			return;
		}
		else temp_hash = temp_hash->next;
	}

	// 3. list delete
	while(1){
		if(temp_list == NULL) break;
		if(!strcmp(temp_list->title, input_delete[1])){
			struct list_elem *listtemp = list_begin(temp_list->list);
			while(list_empty(temp_list->list) == false){
				listtemp = list_pop_front(temp_list->list);
				struct list_item *item = list_entry(listtemp, struct list_item, elem);
				free(item);
				listtemp = list_begin(temp_list->list);
			}
			struct list_node *temp = list_node_head;
			if(temp_list == list_node_head){
				free(temp_list);
				list_node_head = NULL;
				return;
			}
			while(temp->next != temp_list) temp = temp->next;
			temp->next = temp->next->next;
			free(temp_list);
			return;
		}
		else temp_list = temp_list->next;
	}
}

/* ------------ dump functions for dumpdata function ------------ */
void my_bitmap_dump(struct bitmap *bitmap){
	for(int i=0; i<bitmap_size(bitmap); i++){
		if(bitmap_test(bitmap, i) == true) printf("1");
		else printf("0");
	} printf("\n");
	return;
}

void my_hash_dump(struct hash *hash){
	if(hash_empty(hash) == 1) return;
	struct hash_iterator temp;
	hash_first(&temp, hash);
	hash_next(&temp);
	while(1){
		struct my_hash_elem *elem = hash_entry(hash_cur(&temp), struct my_hash_elem, hash_elem);
		printf("%d ", elem->data);
		if(!hash_next(&temp)) break;
	} printf("\n");
	return;
}

void my_list_dump(struct list *list){
	if(list_empty(list)) return;
	struct list_elem *elem = list_begin(list);
	while(list_end(list) != elem){
		struct list_item *item = list_entry(elem, struct list_item, elem);
		printf("%d ", item->data);
		elem = list_next(elem);
	} printf("\n");
	return;
}

/* ------------ bitmap function ------------ */
void bitmap_cmd(char *input){
	char input_bitmap[8][10] = {'\0', };
	char *ptr = strtok(input, " _");
	struct bitmap_node *temp;

	// save input command in array 'input_bitmap'
	int i = 0;
	while(ptr != NULL){
		strcpy(input_bitmap[i],ptr);
		ptr = strtok(NULL, " _");
		i++;
	}
	
	// 1. bitmap_mark 
	if(!strcmp(input_bitmap[1],"mark")){
		temp = my_bitmap_find(input_bitmap[2]);
		bitmap_mark(temp->bitmap, atoi(input_bitmap[3]));
	}

	// 2. bitmap_all 
	else if(!strcmp(input_bitmap[1], "all")){
		temp = my_bitmap_find(input_bitmap[2]);
		if(bitmap_all(temp->bitmap, atoi(input_bitmap[3]), atoi(input_bitmap[4])) == true) printf("true\n");
		else printf("false\n");
	}

	// 3. bitmap_any 
	else if(!strcmp(input_bitmap[1], "any")){
		temp=my_bitmap_find(input_bitmap[2]);
		if(bitmap_any(temp->bitmap, atoi(input_bitmap[3]), atoi(input_bitmap[4])) == true) printf("true\n");
		else printf("false\n");
	}

	// 4. bitmap_contains 
	else if(!strcmp(input_bitmap[1], "contains")){
		temp = my_bitmap_find(input_bitmap[2]);
		bool value;
		if(!strcmp(input_bitmap[5],"true")) value = true;
		else value = false;
		if(bitmap_contains(temp->bitmap, atoi(input_bitmap[3]),atoi(input_bitmap[4]), value)==true) printf("true\n");
		else printf("false\n");
	}

	// 5. bitmap_count 
	else if(!strcmp(input_bitmap[1], "count")){
		temp = my_bitmap_find(input_bitmap[2]);
		bool value;
		if(!strcmp(input_bitmap[5], "true")) value = true;
		else value = false;
		printf("%zu\n", bitmap_count(temp->bitmap, atoi(input_bitmap[3]), atoi(input_bitmap[4]),value));
	}

	// 6. bitmap_dump 
	else if (!strcmp(input_bitmap[1], "dump")){
		temp = my_bitmap_find(input_bitmap[2]);
		bitmap_dump(temp->bitmap);
	}

	// 7. bitmap_expand 
	else if(!strcmp(input_bitmap[1], "expand")){
		temp = my_bitmap_find(input_bitmap[2]);
		temp->bitmap = bitmap_expand(temp->bitmap, atoi(input_bitmap[3]));
	}

	// 8. bitmap_set_all 
	else if( !strcmp(input_bitmap[1],"set")&& !(strcmp(input_bitmap[2], "all"))){
		temp = my_bitmap_find(input_bitmap[3]);
		bool value;
		if(!strcmp(input_bitmap[4],"true")) value = true;
		else value = false;
		bitmap_set_all(temp->bitmap, value);
	}

	// 9. bitmap_set_multiple
	else if(!strcmp(input_bitmap[1], "set")&& !(strcmp(input_bitmap[2], "multiple"))){
		temp = my_bitmap_find(input_bitmap[3]);
		bool value;
		if(!strcmp(input_bitmap[6], "true")) value = true;
		else value = false;
		bitmap_set_multiple(temp->bitmap, atoi(input_bitmap[4]), atoi(input_bitmap[5]), value);
	}

	// 10. bitmap_set
	else if(!strcmp(input_bitmap[1], "set")){
		temp = my_bitmap_find(input_bitmap[2]);
		bool value;
		if(!strcmp(input_bitmap[4], "true")) value = true;
		else value = false;
		bitmap_set(temp->bitmap, atoi(input_bitmap[3]), value);
	}

	// 11. bitmap_flip 
	else if(!strcmp(input_bitmap[1], "flip")){
		temp = my_bitmap_find(input_bitmap[2]);
		bitmap_flip(temp->bitmap, atoi(input_bitmap[3]));
	}

	// 12. bitmap_none 
	else if(!strcmp(input_bitmap[1], "none")){
		temp = my_bitmap_find(input_bitmap[2]);
		if(bitmap_none(temp->bitmap, atoi(input_bitmap[3]), atoi(input_bitmap[4])) == true) printf("true\n");
		else printf("false\n");
	}

	// 13. bitmap_reset
	else if(!strcmp(input_bitmap[1], "reset")){
		temp = my_bitmap_find(input_bitmap[2]);
		bitmap_reset(temp->bitmap, atoi(input_bitmap[3]));
	}

	// 14. bitmap_scan_and_flip
	else if(!strcmp(input_bitmap[1], "scan") && !(strcmp(input_bitmap[2], "and"))){
		temp = my_bitmap_find(input_bitmap[4]);
		bool value;
		if(!strcmp(input_bitmap[7],"true")) value = true;
		else value = false;
		printf("%zu\n", bitmap_scan_and_flip(temp->bitmap, atoi(input_bitmap[5]), atoi(input_bitmap[6]), value));
	}

	// 15. bitmap_scan 
	else if(!strcmp(input_bitmap[1], "scan")){
		temp = my_bitmap_find(input_bitmap[2]);
		bool value;
		if(!strcmp(input_bitmap[5],"true")) value = true;
		else value = false;
		printf("%zu\n",bitmap_scan(temp->bitmap, atoi(input_bitmap[3]), atoi(input_bitmap[4]),value));
	}

	// 16. bitmap_size 
	else if(!strcmp(input_bitmap[1], "size")){
		temp = my_bitmap_find(input_bitmap[2]);
		printf("%zu\n", bitmap_size(temp->bitmap));
	}

	// 17. bitmap_test 
	else if(!strcmp(input_bitmap[1], "test")){
		temp = my_bitmap_find(input_bitmap[2]);
		if(bitmap_test(temp->bitmap, atoi(input_bitmap[3])) == true) printf("true\n");
		else printf("false\n");
	}
}

/* ------------ bitmap find function ------------ */
struct bitmap_node *my_bitmap_find(char *name){
	struct bitmap_node *temp_find = bitmap_node_head;
	while(1){
		if(!strcmp(temp_find->title, name)) return temp_find;
		else temp_find = temp_find->next;
	}
}

/* ------------ hashtable function ------------ */
void hash_cmd(char *input){
	char input_hash[4][10] = {'\0',};
	char *ptr = strtok(input, " _");
	struct hash_node *temp;

	// save input command in array 'input_hash'
	int i = 0;
	while(ptr != NULL){
		strcpy(input_hash[i], ptr);
		ptr = strtok(NULL, " _");
		i++;
	}
	
	// 1. hash_insert 
	if(!strcmp(input_hash[1], "insert")){
		temp = my_hash_find(input_hash[2]);
		struct my_hash_elem *new_hash = (struct my_hash_elem*)malloc(sizeof(struct my_hash_elem));
		new_hash->data = atoi(input_hash[3]);
		hash_insert(temp->hash, &new_hash->hash_elem);
	}

	// 2. hash_apply 
	else if(!strcmp(input_hash[1], "apply")){
		temp = my_hash_find(input_hash[2]);
		if(!strcmp(input_hash[3], "square")) hash_apply(temp->hash, hash_square);
		else if(!strcmp(input_hash[3], "triple")) hash_apply(temp->hash, hash_triple);	
	}

	// 3. hash_delete 
	else if(!strcmp(input_hash[1], "delete")){
		temp = my_hash_find(input_hash[2]);
		struct my_hash_elem *del = (struct my_hash_elem*)malloc(sizeof(struct my_hash_elem));
		del->data = atoi(input_hash[3]);
		hash_delete(temp->hash, &del->hash_elem);
	}

	// 4. hash_empty 
	else if(!strcmp(input_hash[1], "empty")){
		temp = my_hash_find(input_hash[2]);
		if(hash_empty(temp->hash) == true) printf("true\n");
		else printf("false\n");
	}

	// 5. hash_size
	else if(!strcmp(input_hash[1], "size")){
		temp = my_hash_find(input_hash[2]);
		printf("%zu\n", hash_size(temp->hash));

	}

	// 6. hash_clear 
	else if(!strcmp(input_hash[1], "clear")){
		temp = my_hash_find(input_hash[2]);
		hash_clear(temp->hash, NULL);
	}

	// 7. hash_find 
	else if(!strcmp(input_hash[1], "find")){
		temp = my_hash_find(input_hash[2]);
		struct my_hash_elem *elem = (struct my_hash_elem*)malloc(sizeof(struct my_hash_elem));
		elem->data = atoi(input_hash[3]);
		if(hash_find(temp->hash, &elem->hash_elem)) printf("%d\n", elem->data);
	}

	// 8. hash_replace
	else if(!strcmp(input_hash[1], "replace")){
		temp = my_hash_find(input_hash[2]);
		struct my_hash_elem *elem = (struct my_hash_elem*)malloc(sizeof(struct my_hash_elem));
		elem->data = atoi(input_hash[3]);
		hash_replace(temp->hash, &elem->hash_elem);
	}

}

/* ------------ hashtable find function ------------ */
struct hash_node *my_hash_find(char *name){
	struct hash_node *temp_find = hash_node_head;
	while(1){
		if(!strcmp(temp_find->title, name)) return temp_find;
		else temp_find = temp_find->next;
	}
}

/* ------------ list function ------------ */
void list_cmd(char *input){
	char input_list[7][10] = {'\0',};
	char *ptr = strtok(input, " _");
	struct list_node *temp;

	// save input command in array 'input_list'
	int i = 0;
	while(ptr != NULL){
		strcpy(input_list[i], ptr);
		ptr = strtok(NULL, " _");
		i++;
	}

	// 1. list_push_front
	if(!strcmp(input_list[1], "push") && !strcmp(input_list[2], "front")){
		temp = my_list_find(input_list[3]);
		struct list_item *new = (struct list_item*)malloc(sizeof(struct list_item));
		new->data = atoi(input_list[4]);
		list_push_front(temp->list, &new->elem);
	}

	// 2. list_push_back
	else if(!strcmp(input_list[1], "push") && !strcmp(input_list[2], "back")){
		temp = my_list_find(input_list[3]);
		struct list_item *new = (struct list_item*)malloc(sizeof(struct list_item));
		new->data = atoi(input_list[4]);
		list_push_back(temp->list, &new->elem);
	}

	// 3. list_pop_front
	else if(!strcmp(input_list[1], "pop") && !strcmp(input_list[2],"front")){
		temp = my_list_find(input_list[3]);
		list_pop_front(temp->list);
	}

	// 4. list_pop_back 
	else if(!strcmp(input_list[1], "pop") && !strcmp(input_list[2], "back")){
		temp = my_list_find(input_list[3]);
		list_pop_back(temp->list);
	}

	// 5. list_front
	else if(!strcmp(input_list[1], "front")){
		temp = my_list_find(input_list[2]);
		struct list_item *new_item;
		struct list_elem *new_elem;
		new_elem = list_front(temp->list);
		new_item = list_entry(new_elem, struct list_item, elem);
		printf("%d\n", new_item->data);
	}

	// 6. list_back
	else if(!strcmp(input_list[1], "back")){
		temp = my_list_find(input_list[2]);
		struct list_item *new_item;
		struct list_elem *new_elem;
		new_elem = list_back(temp->list);
		new_item = list_entry(new_elem, struct list_item, elem);
		printf("%d\n", new_item->data);
	}

	// 7. list_insert_ordered 
	else if(!strcmp(input_list[1], "insert") && !(strcmp(input_list[2], "ordered"))){
		temp = my_list_find(input_list[3]);
		struct list_item *new = (struct list_item*)malloc(sizeof(struct list_item));
		new->data = atoi(input_list[4]);
		list_insert_ordered(temp->list, &new->elem, list_less, NULL);
	}

	// 8. list_insert 
	else if(!strcmp(input_list[1], "insert")){
		temp = my_list_find(input_list[2]);
		struct list_item *new = (struct list_item*)malloc(sizeof(struct list_item));
		new->data = atoi(input_list[4]);
		int count = 0;
		struct list_elem *original = list_head(temp->list);
		while(count <= atoi(input_list[3])){
			original = original->next;
			count++;
		} list_insert(original, &new->elem);
	}

	// 9. list_empty 
	else if(!strcmp(input_list[1], "empty")){
		temp = my_list_find(input_list[2]);
		if(list_empty(temp->list) == true) printf("true\n");
		else printf("false\n");
	}

	// 10. list_size 
	else if(!strcmp(input_list[1], "size")){
		temp = my_list_find(input_list[2]);
		printf("%zu\n", list_size(temp->list));
	}

	// 11. list_max 
	else if(!strcmp(input_list[1], "max")){
		temp = my_list_find(input_list[2]);
		struct list_elem *new_elem = (struct list_elem*)malloc(sizeof(struct list_elem));
		new_elem = list_max(temp->list, list_less, NULL);
		struct list_item *new_item = (struct list_item*)malloc(sizeof(struct list_item));
		new_item = list_entry(new_elem, struct list_item, elem);
		printf("%d\n", new_item->data);
	}

	// 12. list_min
	else if(!strcmp(input_list[1], "min")){
		temp = my_list_find(input_list[2]);
		struct list_elem *new_elem = (struct list_elem*)malloc(sizeof(struct list_elem));
		new_elem = list_min(temp->list, list_less, NULL);
		struct list_item *new_item = (struct list_item*)malloc(sizeof(struct list_item));
		new_item = list_entry(new_elem, struct list_item, elem);
		printf("%d\n", new_item->data);
	}

	// 13. list_remove
	else if(!strcmp(input_list[1], "remove")){
		temp = my_list_find(input_list[2]);
		struct list_elem *ptr = list_head(temp->list);
		int count = 0;
		while(count <= atoi(input_list[3])){
			ptr = ptr->next;
			count++;
		} list_remove(ptr);
	}

	// 14. list_reverse 
	else if(!strcmp(input_list[1], "reverse")){
		temp = my_list_find(input_list[2]);
		list_reverse(temp->list);
	}

	// 15. list_shuffle
	else if(!strcmp(input_list[1], "shuffle")){
		temp = my_list_find(input_list[2]);
		list_shuffle(temp->list);
	}

	// 16. list_sort
	else if(!strcmp(input_list[1], "sort")){
		temp = my_list_find(input_list[2]);
		list_sort(temp->list, list_less, NULL);
	}

	// 17. list_splice
	else if(!strcmp(input_list[1], "splice")){
		temp = my_list_find(input_list[2]);
		struct list_node *new_node = my_list_find(input_list[4]);
		int count = 0;
		struct list_elem* original = list_head(temp->list);
		while(count <= atoi(input_list[3])){
			original = original->next;
			count++;
		}
		struct list_elem *first_elem = list_head(new_node->list);
		struct list_elem *last_elem = list_head(new_node->list);
		count = 0;
		while(count <= atoi(input_list[6])){
			last_elem = last_elem->next;
			count++;
		} count = 0;
		while(count <= atoi(input_list[5])){
			first_elem = first_elem->next;
			count++;
		} list_splice(original, first_elem, last_elem);
	}

	// 18. list_swap
	else if(!strcmp(input_list[1], "swap")){
		temp = my_list_find(input_list[2]);
		struct list_elem *a = list_head(temp->list);
		struct list_elem *b = list_head(temp->list);
		int count = 0;
		while(count <= atoi(input_list[3])){
			a = a->next;
			count++;
		} count = 0;
		while(count <= atoi(input_list[4])){
			b = b->next;
			count++;
		} list_swap(a, b);
	}
	
	// 19. list_unique 
	else if(!strcmp(input_list[1], "unique")){
		temp = my_list_find(input_list[2]);
		if(input_list[3][0] == '\0') list_unique(temp->list, NULL, list_less, NULL);
		else{
			struct list_node *new_node = my_list_find(input_list[3]);
			list_unique(temp->list, new_node->list, list_less, NULL);
		}
	}
}

/* ------------ list find function ------------ */
struct list_node *my_list_find(char* name){
	struct list_node *temp_find = list_node_head;
	while(1){
		if(!strcmp(temp_find->title, name)) return temp_find;
		else temp_find = temp_find->next;
		if(temp_find == NULL) return NULL;
	}
}