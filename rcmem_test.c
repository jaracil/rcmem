/*
 * rcmem_test.c
 *
 *  Created on: 25 de jul. de 2016
 *      Author: pepe
 */

#include "rcmem.h"
#include <assert.h>

static int des_called = 0;
static void *des_ptr = NULL;

void destructor(void *p){
	des_called = 1;
	des_ptr = p;
}

void rcmem_test(){
	void *p ,*p1;

	des_called = 0;
	des_ptr = NULL;
	assert((p = rcmem_alloc(16, &destructor))!=NULL);
	assert(rcmem_size(p) == 16);
	assert(rcmem_count(p) == 1);
	assert(rcmem_ref(p) == p);
	assert(rcmem_count(p) == 2);
	rcmem_unref(p);
	assert(rcmem_count(p) == 1);
	assert(des_called == 0);
	rcmem_unref(p);
	assert(des_called == 1);
	assert(des_ptr == p);
	des_called = 0;
	des_ptr = NULL;
	assert((p = rcmem_alloc_str("Hello world!"))!=NULL);
	assert(strcmp(p, "Hello world!") == 0);
	assert((p1 = rcmem_copy(p))!=NULL);
	rcmem_unref(p);
	assert(des_called == 0);
	assert(rcmem_count(p1) == 1);
	assert(strcmp(p1, "Hello world!") == 0);
	rcmem_unref(p1);
}

int main(int argc, char* argv[]){
	rcmem_test();
	puts("Test passed.");
	return 0;
}
