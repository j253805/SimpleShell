#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/_types.h>
#include <unistd.h>
#include <string.h>

// Functions that might be used
char *readLine(void);
char **parseLine(char *line);
void exitShell(void);
int executeLine(char **tokenizedLine);

int numberOfArgs = 0;
int exitStatus = 1;

int main(void)
{

  // Strings for shell-prompten
  char cwd[1024];
  char *userName = getlogin();
  char hostName[1024];
  char *line;
  char **tokens;
  gethostname(hostName, sizeof(hostName));

  do
  {
    // Henter nåværende arbeidskatalog
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
      printf("%s@%s %s\n> $ ", userName, hostName, cwd);
      line = readLine();
      tokens = parseLine(line);
      executeLine(tokens);
    }
    else
    {
      perror("getcwd() error");
      exit(EXIT_FAILURE);
    }
    // Henter hostname
    gethostname(hostName, sizeof(hostName));

    // Newline etter hver prompt
    printf("\n");

    // Må settes til 0, etter hver iterasjon for riktig tokenizing
    numberOfArgs = 0;
  } while (exitStatus);

  return 0;
}

char *readLine(void)
{

  int bufSize = 1024;
  char *buffer = malloc(bufSize * sizeof(char));
  int position = 0;

  if (buffer == NULL)
  {
    fprintf(stderr, "Error allocating memory\n");
    exit(EXIT_FAILURE);
  }

  // While-løkke som leser tegn for tegn
  int ch;
  while ((ch = getchar()) != EOF && ch != '\n')
  {

    buffer[position] = (char)ch;
    position++;

    if (position > bufSize)
    {
      bufSize += 1024;
      buffer = realloc(buffer, bufSize);
      if (buffer == NULL)
      {
        fprintf(stderr, "Error reallocating memory\n");
        exit(EXIT_FAILURE);
      }
    }
  }

  // Null terminerer buffer
  buffer[position] = '\0';

  return buffer;
}

char **parseLine(char *line)
{
  char **tokens = NULL;
  char *currentToken;

  tokens = malloc(sizeof(char *));
  if (tokens == NULL)
  {
    fprintf(stderr, "Error allocating memory\n");
    exit(EXIT_FAILURE);
  }

  // int tokenIndex = 0;
  currentToken = strtok(line, " ");
  while (currentToken != NULL)
  {
    // Allokerer minne for currentToken
    // Pluss 1 for '\0'
    tokens[numberOfArgs] = malloc((strlen(currentToken) + 1) * sizeof(char));
    if (tokens[numberOfArgs] == NULL)
    {
      fprintf(stderr, "Error allocating memory\n");
      exit(EXIT_FAILURE);
    }

    // Kopierer token inn til tokens-array
    strcpy(tokens[numberOfArgs], currentToken);
    numberOfArgs++;

    tokens = realloc(tokens, (numberOfArgs + 1) * sizeof(char *));
    if (tokens == NULL)
    {
      fprintf(stderr, "Error allocating memory\n");
      exit(EXIT_FAILURE);
    }

    currentToken = strtok(NULL, " ");
  }

  tokens[numberOfArgs] = NULL;
  return tokens;
}

void exitShell(void)
{
  exitStatus = 0;
}

int executeLine(char **tokenizedLine)
{
  // Om program skal avsluttes
  if (strcmp(tokenizedLine[0], "exit") == 0)
  {
    exitShell();
  }

  pid_t childPID;
  int status;

  childPID = fork();
  // I barneprosess, vi bryr oss ikke om forelder
  if (childPID == 0)
  {
    execvp(tokenizedLine[0], tokenizedLine);
  }
  else if (childPID < 0)
  {
    perror("Error on fork");
    exit(EXIT_FAILURE);
  }
  else
  {
    waitpid(childPID, &status, WUNTRACED);
  }

  return 1;
}
