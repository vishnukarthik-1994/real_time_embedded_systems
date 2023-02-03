#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#define KB 1024
#define MB (1024 * KB)

static void copy_data(void *dest, void *src, size_t size) {
  for (int i = 0; i < size; i++) {
    *((char *)dest + i) = *((char *)src + i);
  }
}

static void startRAMTest() {
  char *src, *dest;
  struct timespec start, end;
  long elapsed_time_ns;

  // Allocate memory for source and destination buffers
  src = (char *)malloc(MB);
  dest = (char *)malloc(MB);

  assert(src != NULL);
  assert(dest != NULL);

  // Measure time taken to copy 1 KB of data
  clock_gettime(CLOCK_MONOTONIC, &start);
  copy_data(dest, src, KB);
  clock_gettime(CLOCK_MONOTONIC, &end);

  elapsed_time_ns = (end.tv_sec - start.tv_sec) * 1000000000 + (end.tv_nsec - start.tv_nsec);
  printf("Time taken to copy 1 KB of data: %ld ns\n", elapsed_time_ns);

  // Measure time taken to copy 1 MB of data
  clock_gettime(CLOCK_MONOTONIC, &start);
  copy_data(dest, src, MB);
  clock_gettime(CLOCK_MONOTONIC, &end);

  elapsed_time_ns = (end.tv_sec - start.tv_sec) * 1000000000 + (end.tv_nsec - start.tv_nsec);
  printf("Time taken to copy 1 MB of data: %ld ns\n", elapsed_time_ns);

  // Deallocate memory
  free(src);
  free(dest);
  return;
}

static void startFileSystemTest() {
  char *src, *dest;
  struct timespec start, end;
  long elapsed_time_ns;
  FILE *src_file, *dest_file;

  // Allocate memory for source and destination buffers
  src = (char *)malloc(MB);
  system("truncate -s 1M src.dat");

  assert(src != NULL);
  assert(dest != NULL);

  // Open source file for reading
  src_file = fopen("src.dat", "rb");
  if (src_file == NULL) {
    perror("Failed to open source file");
    return;
  }

  // Open destination file for writing
  dest_file = fopen("dest.dat", "wb");
  if (dest_file == NULL) {
    perror("Failed to open destination file");
    return;
  }

  // Measure time taken to copy 1 KB of data
  clock_gettime(CLOCK_MONOTONIC, &start);
  fread(src, KB, 1, src_file);
  fwrite(src, KB, 1, dest_file);
  clock_gettime(CLOCK_MONOTONIC, &end);

  elapsed_time_ns = (end.tv_sec - start.tv_sec) * 1000000000 + (end.tv_nsec - start.tv_nsec);
  printf("Time taken to copy 1 KB of data: %ld ns\n", elapsed_time_ns);

  // Measure time taken to copy 1 MB of data
  clock_gettime(CLOCK_MONOTONIC, &start);
  fread(src, MB, 1, src_file);
  fwrite(src, MB, 1, dest_file);
  clock_gettime(CLOCK_MONOTONIC, &end);

  elapsed_time_ns = (end.tv_sec - start.tv_sec) * 1000000000 + (end.tv_nsec - start.tv_nsec);
  printf("Time taken to copy 1 MB of data: %ld ns\n", elapsed_time_ns);

  // Close source and destination files
  fclose(src_file);
  fclose(dest_file);

  // Deallocate memory
  free(src);

  return;
}


int main() {
  printf("Measure time taken to copy RAM memory \n");
  startRAMTest();
  printf("\nMeasure time taken to copy file system memory \n");
  startFileSystemTest();
  return 0;
}