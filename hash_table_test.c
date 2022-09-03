#include "hash_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


int to_int(void* p) {
    return *(int*)p;
}
// for int
int int_equal(void* a, void* b) {
    return *(int*)a == *(int*)b;
}

unsigned int int_hash(void* val) {
    return *(int*)val;
}

void* int_alloc() {
    return malloc(sizeof(int));
}

void int_free(void* key) {
    free(key);
}

void* build_key(int a) {
    int* p = (int*)malloc(sizeof(int));
    *p = a;
    return p;
}

void* build_val(int val) {
    return build_key(val);
}

void test_int_hash_set_get_remove() {
    hash_table* m = ht_new(int_equal, int_hash, int_free, int_free);
    int  bucket = 0;
    for(int i = 1; i <= 100; i++) {
        ht_set(m, build_key(i), build_val(i * i));
        // print_int_table(&m);
        void* val = ht_get(m, build_key(i));
        int v = to_int(val);
        printf("val: %d\n", v);
        if(m->bucket != bucket) {
            printf("val: %d\n", m->bucket);
            bucket = m->bucket;
        }
    }
    for(int i=102; i >=1; i--) {
        ht_remove(m, build_key(i));
        void *val = ht_get(m, build_key(i));
        assert(val == 0);
        if(val == 0) {
            printf("%d not exist or delete successfully\n", i);
        } else{
            printf("%d not exist or delete successfully\n", i);
        }
        if(m->bucket != bucket) {
            printf("val: %d\n", m->bucket);
            bucket = m->bucket;
        }
    }
    ht_release(m);

}

void test_int_ht_iter() {
    hash_table* m = ht_new(int_equal, int_hash, int_free, int_free);
    int  bucket = 0;
    for(int i = 1; i <= 100; i++) {
        ht_set(m, build_key(i), build_val(i * i));
        // print_int_table(&m);
        void* val = ht_get(m, build_key(i));
        printf("val: %d\n", to_int(val));
        if(m->bucket != bucket) {
            printf("val: %d\n", m->bucket);
            bucket = m->bucket;
        }
    }

    for(ht_iter* it = new_ht_iter(m); it != 0; it = next_ht_iter(it)) {
        void *key = get_ht_iter_key(it);
        void *val = get_ht_iter_val(it);
        int k = to_int(key);
        printf("key: %d\n", k);
        printf("val: %d\n", to_int(val));
    }
    ht_release(m);
}

void my_test_ht() {
    test_int_hash_set_get_remove();
    test_int_ht_iter();
}