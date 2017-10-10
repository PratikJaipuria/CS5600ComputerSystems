#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
  size_t size1 = 12;
  size_t size2 = 15;
  size_t size3 = 105;
  size_t size4 = 235;
  // size_t size5 = 367;
  size_t size5 = 3930;

  void *mem1 = malloc(size1);
  void *mem2 = malloc(size2);
  void *mem3 = malloc(size3);
  void *mem4 = malloc(size4);
  void *mem5 = malloc(size5);
  printf("Successfully malloc'd %zu bytes at addr %p\n", size1, mem1);
  printf("Successfully malloc'd %zu bytes at addr %p\n", size2, mem2);
  printf("Successfully malloc'd %zu bytes at addr %p\n", size3, mem3);
  printf("Successfully malloc'd %zu bytes at addr %p\n", size4, mem4);
  printf("Successfully malloc'd %zu bytes at addr %p\n", size5, mem5);
  assert(mem1 != NULL);
  assert(mem2 != NULL);
  assert(mem3 != NULL);
  assert(mem4 != NULL);
  assert(mem5 != NULL);
  
  // free(mem);
  // printf("Successfully free'd %zu bytes from addr %p\n", size, mem);
  return 0;
}
