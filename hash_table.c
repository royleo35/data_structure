#include "hash_table.h"
#include <stdlib.h>


hash_table* ht_new(equal_fp equal, hash_fp hash, free_key_fp free_key, free_val_fp free_val){
    hash_table* ht = (hash_table*)malloc(sizeof(hash_table));
    ht->equal = equal;
    ht->hash = hash;
    ht->free_key = free_key;
    ht->free_val = free_val;
    return ht;
}

static void remove_node(hash_table *table, list_node* node, int slot) {
    if(node->prev->key == 0) { // first node
        table->table[slot] = node->next;
    }
    node->prev->next = node->next;
    node->next->prev = node->prev;
    table->free_key(node->key);
    table->free_val(node->val);
    free(node);
    table->size--;
}

static list_node* new_node(void *key, void *val)  {
    list_node *node  = (list_node*)malloc(sizeof(list_node));
    node->prev = node->next = node;
    node->key = key;
    node->val = val;
    return node;
}

static void insert_node_aux(list_node** list, list_node *node) {
    if (*list == 0) {
        *list = new_node(0, 0);
    }
    list_node* head = *list;
    head->prev->next = node;
    node->prev = head->prev;
    node->next = head;
    head->prev = node;
    *list = node; // new head
}

static void insert_node(list_node** list, void* key, void* val) {
    list_node* node = new_node(key, val);
    insert_node_aux(list, node);
}

static void free_table(hash_table *table) {
    for(int i = 0; i< table->bucket; i++) {
        list_node *node = table->table[i];
        if(node == 0) {
            continue;
        }
        while(node->key != 0) {
            list_node *n = node->next;
            table->free_key(node->key);
            table->free_val(node->val);
            free(node);
            node = n;
        }
        free(node);
    }
    free(table->table);
}

void ht_release(hash_table* ht) {
    free_table(ht);
    free(ht);
}

static void rehash(hash_table *table, int new_bucket) {
    list_node** new_table = (list_node**)calloc(1, sizeof(list_node*) * new_bucket);
    for(int i = 0; i < table->bucket; i++){
        list_node *node = table->table[i];
        if(node == 0) {
            continue;
        } else if(node->key == 0) {//delete dummy
            free(node);
        }
        for( ; node->key != 0; ){
            list_node *next = node->next;
            unsigned int slot = table->hash(node->key) & (new_bucket-1);
            insert_node_aux(&new_table[slot], node);
            node = next;
        }
    }
    // free array only, can't free list because list node has been linked into new table
    free(table->table);
    table->bucket = new_bucket;
    table->table = new_table;
}

static void grow_up(hash_table *table) {
    if(table->bucket == 0) {
        table->bucket = 2;
        table->table = (list_node**)calloc(1, sizeof(list_node*)*2);
        return;
    }
    if ((float)(table->size) / (float)(table->bucket) < 1.25) {
        return;
    }
    // rehash
    rehash(table, table->bucket << 1);
}

static void grow_down(hash_table* table) {
    if(table->bucket == 2) { // can't grow_down again
        return;
    }
    if ((float)(table->size) / (float)(table->bucket) > 0.1) {
        return;
    }
    // rehash
    rehash(table, table->bucket >> 1);
}


static list_node *get_node(hash_table *table, void* key, int* slot) {
    if(table->bucket == 0) {
        return 0;
    }
    int s = (int)(table->hash(key) & (table->bucket-1));
    list_node * node = table->table[s];
    if(node== 0) {
        return 0;
    }
    for ( ; node->key != 0; node = node->next) {
        if(table->equal(node->key, key)) {
            if(slot != 0) {
                *slot = s;
            }
            return node;
        }
    }
    return 0;
}

void ht_set(hash_table* table, void* key, void* val) {
    list_node *node = get_node(table, key, 0);
    if(node != 0) { // already exist
        table->free_val(node->val);
        node->val = val;
        return;
    }
    grow_up(table);
    unsigned int slot = table->hash(key) & (table->bucket -1);
    insert_node(&table->table[slot], key, val);
    table->size++;
}


void* ht_get(hash_table* table, void* key) {
    list_node *node = get_node(table, key, 0);
    return node == 0? 0:node->val;
}

void ht_remove(hash_table* table, void* key){
    int slot = -1;
    list_node *node = get_node(table, key, &slot);
    if(node == 0) {
        return;
    }
    remove_node(table, node, slot);
    grow_down(table);
}

static list_node* get_one_node(hash_table* ht, list_node* curr, int* slot) {
    // find curr list
    if(curr != 0 && curr->next->key != 0) {
        return curr->next;
    }
    if(curr == 0) {// first search
        *slot = 0;
    } else { // next slot
        (*slot)++;
    }
    for(int i = *slot; i< ht->bucket; i++) {
        list_node* l = ht->table[i];
        if(l != 0 && l->key != 0) {
            *slot = i;
            return l;
        }
    }
    return 0;
}

ht_iter* next_ht_iter(ht_iter* it) {
    list_node *next = get_one_node(it->ht, it->curr, &it->slot);
    if(next == 0) {
        free(it);
        return 0;
    }
    it->curr = next;
    return it;
}

ht_iter* new_ht_iter(hash_table* ht) {
    if(ht->size == 0) {
        return 0;
    }
    ht_iter *it = (ht_iter*)malloc(sizeof(ht_iter));
    it->slot = 0;
    it->curr = 0;
    it->ht = ht;
    ht_iter *n = next_ht_iter(it);
    if(n == 0) {
        free(it);
    }
    return n;
}

void* get_ht_iter_key(ht_iter* it) {
    return it->curr->key;
}

void* get_ht_iter_val(ht_iter* it) {
    return it->curr->val;
}

