#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

#define BUFFER_SIZE 1024

typedef enum { MODE_INSERT, MODE_NORMAL } buffer_mode_t;

typedef struct {
  size_t capacity;
  size_t size;
  char *data;
} content_t;

typedef struct {
  content_t content;
  buffer_mode_t mode;
  FILE fd;
} buffer_t;

void move(uint line, uint column) {
  printf("\033[%u;%uH", line, column);
}

void move_home() { move(0, 0); }

void clear() { printf("\033[2J\033[H"); }

int main(int argc, char *argv[]) {
  struct stat fd_stat;
  content_t content = {};
  buffer_t buffer = {.mode = MODE_INSERT};

  if (argc == 2) {
    FILE *fp = fopen(argv[1], "rb+");
    if (!fp) {
      perror("fopen");
      return EXIT_FAILURE;
    }
    if (fstat(fileno(fp), &fd_stat) != 0) {
      perror("fstat");
      return EXIT_FAILURE;
    }
    content.size = fd_stat.st_size;
    content.capacity = content.size + BUFFER_SIZE;
    content.data = calloc(content.capacity, sizeof(char));
    fread(content.data, sizeof(char), content.capacity, fp);
    buffer.content = content;
  } else {
    content.size = 0;
    content.capacity = BUFFER_SIZE;
    content.data = calloc(BUFFER_SIZE, sizeof(char));
    buffer.content = content;
  }
  clear();
  printf("%s", buffer.content.data);
  move_home();

  int ch;
  while ((ch = getchar())) {
    putchar(ch);
  }

  return 0;
}
