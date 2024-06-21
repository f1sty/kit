#include <stdio.h>
#include <sys/types.h>

#define BUFSIZE 1024

void move(uint line, uint column) {
  char escape_code[32] = { 0 };
  snprintf(escape_code, 32, "\033[%u;%uH", line, column);
  puts(escape_code);
}

void move_home() {
  move(0, 0);
}

void clear_scr() {
  puts("\033[2J");
}

int main(int argc, char *argv[])
{
  move_home();
  clear_scr();
  move(46, 0);
  printf("%s", argv[0]);
  move_home();
  
  char input[BUFSIZE] = { 0 };
  scanf("%s", input);

  return 0;
}
