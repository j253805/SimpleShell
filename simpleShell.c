#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/_types.h>
#include <unistd.h>

// Functions that might be used

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
      return 1;
    }

    // Henter hostname
    gethostname(hostName, sizeof(hostName));

    // Newline etter hver prompt
    printf("\n");
  } while (1);

  return 0;
}
