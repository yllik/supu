#ifndef _HASHMAP_H
#define _HASHMAP_H

#include <unistd.h>

#define HASHMAP_SIZE 1024

typedef struct _hash_map_t* hash_map;

typedef void(*pfcb_hmap_value_free)(void* value);

extern void hmap_create(hash_map *hmap, int size);

extern void hmap_destroy(hash_map hmap, pfcb_hmap_value_free);

extern void hmap_set(hash_map hmap, const char* key, int key_len, void* value);

extern void hmap_get(hash_map hmap, const char* key);

#endif
