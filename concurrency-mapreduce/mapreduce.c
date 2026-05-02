#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>
#include "mapreduce.h"

typedef struct {
    char *key;
    char *value;
} KeyValue;

typedef struct {
    KeyValue *pairs;
    int size;
    int capacity;
    int current_idx; // For get_next iteration
    pthread_mutex_t lock;
} Partition;

Partition *partitions;
int num_partitions_global;
Partitioner partition_func_global;

char **files;
int num_files;
int current_file_idx;
pthread_mutex_t file_lock;

void MR_Emit(char *key, char *value) {
    unsigned long p_idx = partition_func_global(key, num_partitions_global);
    Partition *p = &partitions[p_idx];

    pthread_mutex_lock(&p->lock);
    if (p->size == p->capacity) {
        p->capacity = p->capacity == 0 ? 1024 : p->capacity * 2;
        p->pairs = realloc(p->pairs, p->capacity * sizeof(KeyValue));
        assert(p->pairs != NULL);
    }
    p->pairs[p->size].key = strdup(key);
    p->pairs[p->size].value = strdup(value);
    p->size++;
    pthread_mutex_unlock(&p->lock);
}

unsigned long MR_DefaultHashPartition(char *key, int num_partitions) {
    unsigned long hash = 5381;
    int c;
    while ((c = *key++) != '\0')
        hash = hash * 33 + c;
    return hash % num_partitions;
}

int compare_kv(const void *a, const void *b) {
    KeyValue *kv1 = (KeyValue *)a;
    KeyValue *kv2 = (KeyValue *)b;
    int cmp = strcmp(kv1->key, kv2->key);
    if (cmp == 0) {
        return strcmp(kv1->value, kv2->value);
    }
    return cmp;
}

char *get_next(char *key, int partition_number) {
    Partition *p = &partitions[partition_number];
    if (p->current_idx < p->size && strcmp(p->pairs[p->current_idx].key, key) == 0) {
        return p->pairs[p->current_idx++].value;
    }
    return NULL;
}

void *mapper_wrapper(void *arg) {
    Mapper map = (Mapper)arg;
    while (1) {
        pthread_mutex_lock(&file_lock);
        if (current_file_idx >= num_files) {
            pthread_mutex_unlock(&file_lock);
            break;
        }
        char *file = files[current_file_idx++];
        pthread_mutex_unlock(&file_lock);
        map(file);
    }
    return NULL;
}

// Redefining reducer wrapper to take a struct
typedef struct {
    Reducer reduce;
    int p_idx;
} ReducerArgs;

void *reducer_thread_func(void *arg) {
    ReducerArgs *args = (ReducerArgs *)arg;
    Partition *p = &partitions[args->p_idx];
    
    p->current_idx = 0;
    while (p->current_idx < p->size) {
        char *key = p->pairs[p->current_idx].key;
        args->reduce(key, get_next, args->p_idx);
    }
    
    free(args);
    return NULL;
}

void MR_Run(int argc, char *argv[], 
	    Mapper map, int num_mappers, 
	    Reducer reduce, int num_reducers, 
	    Partitioner partition) {
    
    num_partitions_global = num_reducers;
    partition_func_global = partition;
    
    partitions = calloc(num_reducers, sizeof(Partition));
    for (int i = 0; i < num_reducers; i++) {
        pthread_mutex_init(&partitions[i].lock, NULL);
    }
    
    files = &argv[1];
    num_files = argc - 1;
    current_file_idx = 0;
    pthread_mutex_init(&file_lock, NULL);
    
    pthread_t *mappers = malloc(num_mappers * sizeof(pthread_t));
    for (int i = 0; i < num_mappers; i++) {
        pthread_create(&mappers[i], NULL, mapper_wrapper, map);
    }
    
    for (int i = 0; i < num_mappers; i++) {
        pthread_join(mappers[i], NULL);
    }
    free(mappers);
    
    // Sorting
    for (int i = 0; i < num_reducers; i++) {
        qsort(partitions[i].pairs, partitions[i].size, sizeof(KeyValue), compare_kv);
    }
    
    pthread_t *reducers = malloc(num_reducers * sizeof(pthread_t));
    for (int i = 0; i < num_reducers; i++) {
        ReducerArgs *args = malloc(sizeof(ReducerArgs));
        args->reduce = reduce;
        args->p_idx = i;
        pthread_create(&reducers[i], NULL, reducer_thread_func, args);
    }
    
    for (int i = 0; i < num_reducers; i++) {
        pthread_join(reducers[i], NULL);
    }
    free(reducers);
    
    // Cleanup
    for (int i = 0; i < num_reducers; i++) {
        for (int j = 0; j < partitions[i].size; j++) {
            free(partitions[i].pairs[j].key);
            free(partitions[i].pairs[j].value);
        }
        free(partitions[i].pairs);
        pthread_mutex_destroy(&partitions[i].lock);
    }
    free(partitions);
    pthread_mutex_destroy(&file_lock);
}
