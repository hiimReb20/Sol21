#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "icl_hash.h"

#include <limits.h>

#define BITS_IN_int (sizeof(int) * CHAR_BIT)
#define THREE_QUARTERS ((int) ((BITS_IN_int * 3) /4))
#define ONE_EIGHTH ((int) (BITS_IN_int / 8))
#define HIGH_BITS ( ~((unsigned int)(~0) >> ONE_EIGHTH))

/*
*A simple string hash
*param[in] key -- the string to be hashed
*returns the hash index
*/
unsigned int hash_pjw(char* key){
    char *datum = (char *)key;
    unsigned int hash_value, i;
    
    if(!datum) return 0;

    for(hash_value=0; *datum;++datum){
        hash_value=(hash_value << ONE_EIGHTH) + *datum;
        if((i=hash_value & HIGH_BITS)!=0)
            hash_value=(hash_value ^ (i >> THREE_QUARTERS)) & ~HIGH_BITS;
    }
    return (hash_value);
}

int string_cmp(char* a, char* b){
    return (strcmp((char*)a, (char*)b)==0);
}


/*
* Create a new hash table.
*nbuckets -- number of buckets to create
*hash_function -- pointer to the hashing function to be used
*hash_key_compare -- pointer to the hash key comparison function to be used
*returns pointer to new hash table.
 */
myhash *hash_create(int nbuckets, unsigned int (*hash_function)(char*), int (*hash_key_compare)(char*, char*), int max_file, long max_dim){
    myhash *ht;
    int i;
    ht=(myhash*)malloc(sizeof(myhash));
    if(!ht) return NULL;
    
    ht->nentries=0;
    ht->min=0;
    ht->max=0;
    ht->key_min=NULL;
    ht->key_max=NULL;
    ht->nfile=0;
    ht->dim=0;
    ht->max_dim=max_dim;
    ht->max_file=max_file;
    ht->buckets=(entry**)malloc(nbuckets*sizeof(entry*));
    if(!ht->buckets) return NULL;

    ht->nbuckets=nbuckets;
    for(i=0; i<ht->nbuckets; i++)
        ht->buckets[i]=NULL;
    
    ht->hash_function=hash_function ? hash_function : hash_pjw;
    ht->hash_key_compare=hash_key_compare ? hash_key_compare : string_cmp;
    return ht;
}


/*
*search for an entry in a has table
*ht -- the hash table to be searched
*key -- the key of the item to search for
*returns pointer to te data corresponding to the key or null
*/
char *hash_find(myhash *ht, char* key){
    entry* curr;
    unsigned int hash_val;

    if(!ht || !key) return NULL;
    
    hash_val=(*ht->hash_function)(key) % ht->nbuckets;

    for(curr=ht->buckets[hash_val]; curr!=NULL; curr=curr->next)
        if(ht->hash_key_compare(curr->k, key))
        return(curr->k);
    
    return NULL;
}


char *hash_ret_data(myhash *ht, char* key){
    entry* curr;
    unsigned int hash_val;

    if(!ht || !key) return NULL;
    
    hash_val=(*ht->hash_function)(key) % ht->nbuckets;

    for(curr=ht->buckets[hash_val]; curr!=NULL; curr=curr->next)
        if(ht->hash_key_compare(curr->k, key))
        return(curr->data);
    
    return NULL;
}


/*
* Insert an item into the hash table.
* @param ht -- the hash table
* @param key -- the key of the new item
* @param data -- pointer to the new item's data
* @returns pointer to the new item.  Returns NULL on error.
*/
entry *hash_insert(myhash *ht, char* key, char* data){
    entry *curr;
    unsigned int hash_val;
    if(!ht || !key) return NULL;

    hash_val=(*ht->hash_function)(key) %ht->nbuckets;
    for(curr=ht->buckets[hash_val]; curr!=NULL; curr=curr->next)
        if(ht->hash_key_compare(curr->k, key))
            return NULL;

    curr=(entry*)malloc(sizeof(entry));
    if(!curr) return NULL;
    int l=strlen(key);
    
    if((curr->k=malloc((l+1)*sizeof(char)))==NULL){
        perror("hash_insert: malloc key");
        return NULL;
    }
    strncpy(curr->k, key, l+1);
    curr->data=data;
    curr->dim=0;
    curr->last_op_was_open=1;
    int rc = pthread_mutex_init(&curr->m, NULL); assert(rc == 0); 
    rc = pthread_cond_init(&curr->c, NULL); assert(rc == 0);
    curr->next=ht->buckets[hash_val]; //inserito in testa
    ht->buckets[hash_val]=curr;
    ht->nentries++;

    if(ht->key_min==NULL){
        ht->key_min=curr->k;
        ht->key_max=curr->k;
        ht->min=1;
        ht->max=1;
        curr->n=1;
    }
    else{
        ht->max++;
        curr->n=ht->max;
        ht->key_max=curr->k;
        
    }
    return curr;
}


/*
 * Replace entry in hash table with the given entry.
 * @param ht -- the hash table
 * @param key -- the key of the new item
 * @param data -- pointer to the new item's data
 * @param olddata -- pointer to the old item's data (set upon return)
 * @returns pointer to the new item.  Returns NULL on error.
 */
entry *hash_update_insert(myhash *ht, char* key, char* data){
    entry *curr, *prev;
    unsigned int hash_val;
    int stop=0;

    if(!ht || !key) return NULL;
    hash_val=(* ht->hash_function)(key) % ht->nbuckets;
    prev=NULL;
    curr=ht->buckets[hash_val];
    
	
    while((curr!=NULL) && (stop==0)){
        if(ht->hash_key_compare(curr->k, key)){
            if(data!=NULL){
            if(curr->data==NULL){
                curr->data=malloc((strlen(data)+1)*sizeof(char));
                if(curr->data==NULL){
                    perror("hash_update_insert: malloc curr->data");
                    return NULL;
                }

                strncpy(curr->data,data,strlen(data)+1);
            }
            else{
                int l=0, o=0;
                char *olddata;
                o=strlen(curr->data);
                olddata=malloc((o+1)*sizeof(char));
                strncpy(olddata,curr->data, o+1);
                ht->dim-=strlen(curr->data)+1;
                free(curr->data);
                l=o+strlen(data)+1;
                curr->data=malloc(l*sizeof(char));
                if(curr->data==NULL){
                    perror("hash_update_insert: malloc curr->data");
                    return NULL;
                }
                strncpy(curr->data,olddata,strlen(olddata)+1);
                strncat(curr->data,data,strlen(data)+1);
                free(olddata);
            }
            stop=1;
            curr->dim=strlen(curr->data)+1;
            ht->dim+=strlen(curr->data)+1;
            if(hash_check_w(ht, curr->k)==1)ht->nfile++;
            curr->last_op_was_open=0;
            }
            else{
            stop=1;
            if(hash_check_w(ht, curr->k)==1)ht->nfile++;
            curr->last_op_was_open=0;
            }
        }
        else{
        	prev=curr, curr=curr->next;
        }
    }
    return curr;
}


/**
 * Free one hash table entry located by key (key and data are freed using functions).
 *ht -- the hash table to be freed
 *key -- the key of the new item
 *free_key -- pointer to function that frees the key
 * free_data -- pointer to function that frees the data
 *
 *returns 0 on success, -1 on failure.
 */
int hash_delete(myhash *ht, char* key, void (*free_key)(void*), void (*free_data) (void*)){
    entry *curr, *prev;
    unsigned int hash_val;
    int stop=0, era_min=0, era_max=0;

    if(!ht || !key) return -1;
    hash_val=(* ht->hash_function)(key) % ht->nbuckets;

    prev=NULL;
    curr=ht->buckets[hash_val];
    while((curr!=NULL) && (stop==0)){
        if(ht->hash_key_compare(curr->k, key)){
        		if(ht->hash_key_compare(ht->key_max, key)){
        			era_max=1;
        		}
        		if(ht->hash_key_compare(ht->key_min, key)){
        			era_min=1;
        		}
            if(prev==NULL)
                ht->buckets[hash_val]=curr->next;
            else
                prev->next=curr->next;
            if(*free_key && curr->k) (*free_key) (curr->k);
            if(*free_data && curr->data) (*free_data)(curr->data);
            ht->nentries--;
            ht->dim-=curr->dim;
            free(curr);
            stop=1;
        }
        else{
		     prev=curr;
		     curr=curr->next;
        }
    }
    if(curr==NULL) return -1;
    
    if(era_min && era_max){
    	ht->min=0;
    	ht->max=0;
    	ht->key_min=NULL;
    	ht->key_max=NULL;
    }
    else if(era_min && !era_max){
        int min=ht->max+1;
        for(int i=0; i<ht->nbuckets; i++){
            for(curr=ht->buckets[i]; curr!=NULL;curr=curr->next){
                if(curr->n<min){
                    min=curr->n;
                    ht->key_min=curr->k;

                }
            }
        }
        ht->min=min;
    }
    
    else if(era_max && !era_min){
        int max=ht->min-1;
        for(int i=0; i<ht->nbuckets; i++){
        		curr=ht->buckets[i];
            while(curr!=NULL){
                if(curr->n>max){
                    max=curr->n;
                    ht->key_max=curr->k;

                }
                curr=curr->next;
            }
        }
        ht->max=max;
    }
    ht->nfile--;
    return 0;
}


/* * Free hash table structures (key and data are freed using functions).
 *
 *ht -- the hash table to be freed
 *free_key -- pointer to function that frees the key
 * free_data -- pointer to function that frees the data
 *
 * @returns 0 on success, -1 on failure.
 */
int hash_destroy(myhash *ht, void (*free_key)(void*), void (*free_data)(void*)){
    entry *bucket, *curr, *next;
    int i;
    if(!ht) return -1;
    for(i=0; i<ht->nbuckets; i++){
        bucket=ht->buckets[i];
        for(curr=bucket; curr!=NULL; ){
            next=curr->next;
            if(*free_key && curr->k) (*free_key)((void*)curr->k);
            if(*free_data && curr->data) (*free_data)((void*)curr->data);
            free(curr);
            curr=next;
        }
    }

    if(ht->buckets) free(ht->buckets);
    if(ht) free(ht);
    return 0;
}


/*
 * Dump the hash table's contents to the given file pointer.
stream -- the file to which the hash table should be dumped
ht -- the hash table to be dumped
returns 0 on success, -1 on failure.
 */

int hash_dump(FILE *stream, myhash* ht){
    entry *bucket, *curr;
    int i;

    if(!ht) return -1;

    for(i=0; i<ht->nbuckets; i++){
        bucket=ht->buckets[i];
        for(curr=bucket; curr!=NULL; ){
            if(curr->k)
                fprintf(stream, "hash_dump: %s: %p\n", (char*)curr->k, curr->data);
            curr=curr->next;
        }
    }
    return 0;
}


int hash_check_w(myhash *ht, char* key){
    entry* curr;
    unsigned int hash_val;

    if(!ht || !key) return -1;
    
    hash_val=(*ht->hash_function)(key) % ht->nbuckets;

    for(curr=ht->buckets[hash_val]; curr!=NULL; curr=curr->next)
        if(ht->hash_key_compare(curr->k, key)){
            return curr->last_op_was_open;
        }
        
    
    return -1;
}

int hash_set_open(myhash *ht, char* key){
    entry* curr;
    unsigned int hash_val;

    if(!ht || !key) return -1;
    
    hash_val=(*ht->hash_function)(key) % ht->nbuckets;

    for(curr=ht->buckets[hash_val]; curr!=NULL; curr=curr->next)
        if(ht->hash_key_compare(curr->k, key)){
            curr->last_op_was_open=0;
            return curr->last_op_was_open;
        }
        
    
    return -1;
}

void print(myhash *ht){
	if(ht->key_min!=NULL && ht->key_max!=NULL)
	printf("\nbuck=%d, nent=%d, min=%d, max=%d, key_min=%s, key_max=%s, dim=%ld, nfile=%d\n", ht->nbuckets, ht->nentries, ht->min, ht->max, ht->key_min, ht->key_max, ht->dim, ht->nfile);
	else if(ht->key_min==NULL && ht->key_max!=NULL)
		printf("\nbuck=%d, nent=%d, min=%d, max=%d, key_max=%s, dim=%ld, nfile=%d\n", ht->nbuckets, ht->nentries, ht->min, ht->max, ht->key_max, ht->dim, ht->nfile);
	else if(ht->key_min!=NULL && ht->key_max==NULL)
		printf("\nbuck=%d, nent=%d, min=%d, max=%d, key_min=%s, dim=%ld, nfile=%d\n", ht->nbuckets, ht->nentries, ht->min, ht->max, ht->key_min, ht->dim, ht->nfile);
	else if(ht->key_min==NULL && ht->key_max==NULL)
		printf("\nbuck=%d, nent=%d, min=%d, max=%d\n, dim=%ld, nfile=%d", ht->nbuckets, ht->nentries, ht->min, ht->max, ht->dim, ht->nfile);
	for(int i=0; i<ht->nbuckets; i++){
        entry *curr, *prev;
        for(prev=NULL, curr=ht->buckets[i]; curr!=NULL; prev=curr, curr=curr->next){
            if(curr->data==NULL)
                printf("key=%s, data=NULL, hash_val=%d, n=%d, dim=%ld, flag_op=%d\n", curr->k, i, curr->n, curr->dim, curr->last_op_was_open);
            else
                printf("key=%s, data=%s, hash_val=%d, n=%d, dim=%ld, flag_op=%d\n", curr->k, curr->data, i, curr->n, curr->dim, curr->last_op_was_open);

        }
    }
}
