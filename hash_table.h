#ifndef __my_hash_table_h__
#define __my_hash_table_h__

typedef struct list_node {
    void* key;
    void* val;
    struct list_node* next;
    struct list_node* prev;
}list_node;

typedef int (*equal_fp)(void* key1, void* key2);
typedef unsigned int (*hash_fp)(void* key);
typedef void (*free_key_fp)(void* key);
typedef void (*free_val_fp)(void* val);

typedef struct hash_table {
    list_node** table;
    int bucket;
    int size;
    equal_fp equal;
    hash_fp hash;
    free_key_fp free_key;
    free_val_fp free_val;
} hash_table;


typedef struct ht_iter{
    hash_table* ht;
    int slot;
    list_node *curr;
} ht_iter;

extern hash_table* ht_new(equal_fp equal, hash_fp hash, free_key_fp free_key, free_val_fp free_val);
extern void ht_release(hash_table* ht);
extern void ht_set(hash_table* table, void* key, void* val);
extern void* ht_get(hash_table* table, void* key);
extern void ht_remove(hash_table* table, void* key);

extern ht_iter* new_ht_iter(hash_table* ht);
extern ht_iter* next_ht_iter(ht_iter* it);
extern void* get_ht_iter_key(ht_iter* it);
extern void* get_ht_iter_val(ht_iter* it);

#endif