/*
 * rcmem.c
 *
 *  Created on: 25 de jul. de 2016
 *      Author: Jose Luis Aracil (pepe.aracil.gomez@gmail.com)
 */

#include "rcmem.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>


static void *_rcmem_generic_alloc(size_t sz, destructor_t des, bool zfill){
	rcmem_t *m;

	if (zfill){
		m = calloc(1, sizeof(*m) + sz);
	} else {
		m = malloc(sizeof(*m) + sz);
	}
	if (m == NULL) {
		return NULL;
	}
	m->count = 1;
	m->size = sz;
	m->des = des;
	pthread_mutex_init(&m->mux, NULL);
	return (void *)(m + 1);

}

void *rcmem_alloc(size_t sz, destructor_t des){
	return _rcmem_generic_alloc(sz, des, false);
}

void *rcmem_zalloc(size_t sz, destructor_t des){
	return _rcmem_generic_alloc(sz, des, true);
}

void *rcmem_malloc(size_t sz){
	return rcmem_alloc(sz, NULL);
}

void *rcmem_calloc(size_t nmemb, size_t size){
	return rcmem_zalloc(nmemb * size, NULL);
}

void *rcmem_copy(void *src){
	size_t sz = rcmem_size(src);
	void *tar = rcmem_alloc(sz, rcmem_destructor(src));
	if (tar == NULL){
		return NULL;
	}
	return memcpy(tar, src, sz);
}

void *rcmem_alloc_str(const char *str){
	size_t sz = strlen(str) + 1;
	void *tar = rcmem_alloc(sz, NULL);
	if (tar == NULL){
		return NULL;
	}
	return memcpy(tar, str, sz);
}

void *rcmem_alloc_buf(void *buf, size_t sz){
	void *tar = rcmem_alloc(sz, NULL);
	if (tar == NULL){
		return NULL;
	}
	return memcpy(tar, buf, sz);
}

void *rcmem_ref(void *p){
	rcmem_t *m = ((rcmem_t *)p) - 1;

	pthread_mutex_lock(&m->mux);
	m->count ++;
	pthread_mutex_unlock(&m->mux);
	return p;
}

void rcmem_unref(void *p){
	rcmem_t *m = ((rcmem_t *)p) - 1;

	pthread_mutex_lock(&m->mux);
	m->count --;
	if (m->count > 0) {
		goto ex;
	}
	if (m->des != NULL){
		pthread_mutex_unlock(&m->mux);
		m->des(p);
		pthread_mutex_lock(&m->mux);
		if (m->count > 0) {  // incremented ref counter in destructor ???
			goto ex;
		}
	}
	pthread_mutex_unlock(&m->mux);
	pthread_mutex_destroy(&m->mux);
	free(m);
	return;
ex:
	pthread_mutex_unlock(&m->mux);
	return;
}

void rcmem_free(void *p){
	return rcmem_unref(p);
}

size_t rcmem_count(void *p){
	rcmem_t *m = ((rcmem_t *)p) - 1;
	size_t count;

	pthread_mutex_lock(&m->mux);
	count = m->count;
	pthread_mutex_unlock(&m->mux);
	return count;
}

size_t rcmem_size(void *p){
	rcmem_t *m = ((rcmem_t *)p) - 1;
	size_t size;

	pthread_mutex_lock(&m->mux);
	size = m->size;
	pthread_mutex_unlock(&m->mux);
	return size;
}

destructor_t rcmem_destructor(void *p){
	rcmem_t *m = ((rcmem_t *)p) - 1;
	destructor_t des;

	pthread_mutex_lock(&m->mux);
	des = m->des;
	pthread_mutex_unlock(&m->mux);
	return des;

}

pthread_mutex_t *rcmem_mux(void *p){
	rcmem_t *m = ((rcmem_t *)p) - 1;
	return &m->mux;
}

