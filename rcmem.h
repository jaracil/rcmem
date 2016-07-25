/*
 * rcmem.h
 *
 *  Created on: 25 de jul. de 2016
 *      Author: Jose Luis Aracil (pepe.aracil.gomez@gmail.com)
 */

#ifndef RCMEM_H_
#define RCMEM_H_

#include <pthread.h>
#include <stdlib.h>
#include <stdint.h>

typedef void (*destructor_t)(void *p);

typedef struct rcmem_s {
	size_t count;
	size_t size;
	destructor_t des;
	pthread_mutex_t mux;
} rcmem_t;

void *rcmem_alloc(size_t sz, destructor_t des);
void *rcmem_zalloc(size_t sz, destructor_t des);
void *rcmem_malloc(size_t sz);
void *rcmem_calloc(size_t nmemb, size_t size);
void *rcmem_copy(void *src);
void *rcmem_alloc_str(const char *str);
void *rcmem_alloc_buf(void *buf, size_t sz);
void *rcmem_ref(void *p);
void rcmem_unref(void *p);
void rcmem_free(void *p);
size_t rcmem_count(void *p);
size_t rcmem_size(void *p);
destructor_t rcmem_destructor(void *p);
pthread_mutex_t *rcmem_mux(void *p);



#endif /* RCMEM_H_ */
