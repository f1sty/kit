#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

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
  FILE *fp;
  struct termios term;
} buffer_t;

void move(uint line, uint column) { printf("\033[%u;%uH", line, column); }

void move_home() { move(0, 0); }

void clear() { printf("\033[2J\033[H"); }

void process_key_insert(buffer_t *buffer, char ch) {
  switch (ch) {
  case 27:
    buffer->term.c_lflag &= ~ICANON;
    buffer->term.c_lflag |= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &buffer->term);
    buffer->mode = MODE_NORMAL;
    break;
  default:
    break;
  }
}

void process_key_normal(buffer_t *buffer, char ch) {
  switch (ch) {
  case 'q':
    buffer->term.c_lflag &= ICANON;
    buffer->term.c_lflag |= ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &buffer->term);
    exit(EXIT_SUCCESS);
    break;
  case 'i':
    buffer->term.c_lflag &= ICANON;
    buffer->term.c_lflag |= ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &buffer->term);
    buffer->mode = MODE_INSERT;
    break;
  default:
    break;
  }
}

int main(int argc, char *argv[]) {
  struct stat stats;
  content_t content = {0};
  buffer_t buffer = {.mode = MODE_NORMAL};

  if (argc == 2) {
    FILE *fp = fopen(argv[1], "rb+");
    if (!fp) {
      perror("fopen");
      return EXIT_FAILURE;
    }
    if (fstat(fileno(fp), &stats) != 0) {
      perror("fstat");
      return EXIT_FAILURE;
    }
    content.size = stats.st_size;
    content.capacity = content.size + BUFFER_SIZE;
    content.data = calloc(content.capacity, sizeof(char));
    fread(content.data, sizeof(char), content.capacity, fp);
    buffer.content = content;
    buffer.fp = fp;
  } else {
    content.size = 0;
    content.capacity = BUFFER_SIZE;
    content.data = calloc(BUFFER_SIZE, sizeof(char));
    buffer.content = content;
    FILE *fp = fmemopen(buffer.content.data, BUFFER_SIZE, "rb+");
    buffer.fp = fp;
  }

  clear();
  printf("%s", buffer.content.data);
  move_home();
  fflush(stdout);

  tcgetattr(STDIN_FILENO, &buffer.term);
  buffer.term.c_lflag &= ~ICANON;
  buffer.term.c_lflag |= ~ECHO;
  tcsetattr(STDIN_FILENO, TCSANOW, &buffer.term);

  char ch;
  while ((read(STDIN_FILENO, &ch, 1)) == 1) {
    switch (buffer.mode) {
    case MODE_NORMAL:
      process_key_normal(&buffer, ch);
      break;
    case MODE_INSERT:
      process_key_insert(&buffer, ch);
      break;
    default:
      break;
    }
  }

  return 0;
}
