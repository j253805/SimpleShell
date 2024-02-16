#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/_types.h>
#include <unistd.h>

// Functions that might be used
char *readLine();

int main(void) {

  // Strings for shell-prompten
  char cwd[1024];
  char *userName = getlogin();
  char hostName[1024];
  gethostname(hostName, sizeof(hostName));

  do {

    // Henter nåværende arbeidskatalog
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
      printf("%s@%s %s\n> $ ", userName, hostName, cwd);
    } else {
      perror("getcwd() error");
      exit(EXIT_FAILURE);
    }
    // Henter hostname
    gethostname(hostName, sizeof(hostName));

    // Newline etter hver prompt
    printf("\n");

  } while (1);

  return 0;
}

char *readLine() {

  int bufSize = 1024;
  char *buffer = malloc(bufSize * sizeof(char));
  int position = 0;

  if (buffer == NULL) {
    fprintf(stderr, "Error allocating memory\n");
    exit(EXIT_FAILURE);
  }

  // While-løkke som leser tegn for tegn
  int ch;
  while ((ch = getchar()) != EOF && ch != '\n') {

    buffer[position] = (char)ch;
    position++;

    if (position > bufSize) {
      bufSize += 1024;
      buffer = realloc(buffer, bufSize);
      if (buffer == NULL) {
        fprintf(stderr, "Error reallocating memory\n");
        exit(EXIT_FAILURE);
      }
    }
  }

  // Null terminerer buffer
  buffer[position] = '\0';

  return buffer;
}
