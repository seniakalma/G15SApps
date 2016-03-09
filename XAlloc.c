
#include "XAlloc.h"
//#include "RichString.h"

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <err.h>
#include <stdlib.h>
#include <string.h>

/*{
#include <stdlib.h>
}*/

void* xMalloc(size_t size) {
   void* data = malloc(size);
   if (!data && size > 0) {
      assert(0);
   }
   return data;
}

void* xCalloc(size_t nmemb, size_t size) {
   void* data = calloc(nmemb, size);
   if (!data) {
	   assert(0);
   }
   return data;
}

void* xRealloc(void* ptr, size_t size) {
   void* data = realloc(ptr, size);
   if (!data && size > 0) {
	   assert(0);
   }
   return data;
}

char* xStrdup(const char* str) {
   char* data = strdup(str);
   if (!data && str) {
	   assert(0);
   }
   return data;
}
