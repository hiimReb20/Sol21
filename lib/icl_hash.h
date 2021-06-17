#ifndef icl_hash_h
#define icl_hash_h

#include <stdio.h>
#include <pthread.h>

#if defined(c_plusplus) || defined(__cplusplus)
extern "C"{
#endif

typedef struct entry_s{
    char* k;
    char* data;
    int lock;
    int n;
    long dim;
    int last_op_was_open;
    struct entry_s* next;
    pthread_mutex_t m;
    pthread_cond_t c;
} entry;

typedef struct hash_s{
    int min;
    int max;
    char* key_min;
    char* key_max;
    int nbuckets;
    int nentries;
    int nfile;
    int max_file;
    long dim;
    long max_dim;
    entry** buckets;
    unsigned int (*hash_function) (char*);
    int (*hash_key_compare) (char*, char*);
}myhash;


myhash *hash_create(int nbuckets, unsigned int (*hash_function)(char*), int (*hash_key_compare)(char*, char*), int max_file, long max_dim);

char *hash_find(myhash *ht, char* key);
char *hash_ret_data(myhash *ht, char* key);

entry *hash_insert(myhash *ht, char* key, char* data);
entry *hash_update_insert(myhash *ht, char* key, char* data);

int hash_destroy(myhash *ht, void (*free_key)(void*), void (*free_data)(void*));
int hash_dump(FILE *, myhash *);
int hash_delete(myhash *ht, char* key, void (*free_key)(void*), void (*free_data) (void*));
int hash_check_w(myhash *ht, char* key);
int hash_set_open(myhash *ht, char* key);

unsigned int hash_pjw(char* key);

int string_cmp(char* a, char* b);


void print(myhash *ht);

#define hash_foreach(ht, tmpint, tmpent, kp, dp) \
    for(tmpint=0; tmpint<ht->nbuckets; tmpint++) \
        for(tmpent=ht->buckets[tmpint]; tmpent!=NULL && ((kp=tmpent->k)!=NULL) && ((dp=tmpent->data)!=NULL); tmpent=tmpent->next)


#if defined(c_plusplus) || defined(__cplusplus)
}
#endif

#endif
