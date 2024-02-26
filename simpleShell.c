#include <stdio.h>
#include <stdlib.h>
#include <sys/_types.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

// ########### Egendefinerte header filer #############
#include "history.h"
#include "utilityAndStatus.h"
// ########### Egendefinerte header filer #############

// ############### General shell functions ############
char *readLine(void);
char **parseLine(char *line);
int executeLine(char **tokenizedLine);
void freeTokens(char **tokens);

// Globale variabler, refaktorerer kanskje program slik at disse ikke blir nødvendige
int numberOfArgs = 0;
// ############### General shell functions ############

// --------------------------------------------------------

// ############### History variables ##################

// char *history[HISTORY_SIZE];
char **history = NULL;
uint16_t historyAdditionCount = 0;

// ############### History variables ##################

int main(void)
{

  // Strings for shell-prompten
  char cwd[1024];
  char *userName = getlogin();
  char hostName[1024];
  gethostname(hostName, sizeof(hostName));

  char *line;
  char **tokens;
  int status = 1;
  do
  {
    // Henter nåværende arbeidskatalog
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
      printf("%s@%s %s\n> $ ", userName, hostName, cwd);
      line = readLine();
      addToHistory(&history, line, &historyAdditionCount);
      tokens = parseLine(line);

      // Sjekker om tokens != NULL og tokens[0] ikke er tom
      if (tokens && tokens[0])
      {
        status = executeLine(tokens);
      }
    }
    else
    {
      perror("getcwd() error");
      exit(EXIT_FAILURE);
    }

    // Newline etter hver prompt
    printf("\n");

    // Må settes til 0, etter hver iterasjon for riktig tokenizing
    numberOfArgs = 0;

    // Frigjør minne
    free(line);
    freeTokens(tokens);
  } while (status);

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
    if (position >= bufSize)
    {
      bufSize += 1024;
      char *temp = realloc(buffer, bufSize * sizeof(char));
      if (temp == NULL)
      {
        fprintf(stderr, "Error reallocating memory\n");
        free(buffer);
        exit(EXIT_FAILURE);
      }
      buffer = temp;
    }

    buffer[position] = (char)ch;
    position++;
  }

  buffer[position] = '\0';

  return buffer;
}

char **parseLine(char *line)
{
  int bufSize = 64;
  char *currentToken;
  int position = 0;
  char **buffer = malloc(bufSize * sizeof(char *));

  if (buffer == NULL)
  {
    fprintf(stderr, "Error allocating memory");
    exit(EXIT_FAILURE);
  }

  currentToken = strtok(line, " ");
  while (currentToken != NULL)
  {

    if (position >= bufSize)
    {
      bufSize += 64;
      char **temp = realloc(buffer, bufSize);
      if (temp == NULL)
      {
        fprintf(stderr, "Error allocating memory");
        free(buffer);
        exit(EXIT_FAILURE);
      }
      buffer = temp;
    }

    // Allokerer minne for currentToken
    // Pluss 1 for '\0'
    buffer[position] = malloc((strlen(currentToken) + 1) * sizeof(char));
    if (buffer[position] == NULL)
    {
      fprintf(stderr, "Error allocating memory\n");
      exit(EXIT_FAILURE);
    }

    // Kopierer token inn til tokens-array
    strcpy(buffer[position], currentToken);
    position++;

    numberOfArgs++;

    // Neste token
    currentToken = strtok(NULL, " ");
  }

  buffer[position] = NULL;
  return buffer;
}

int executeLine(char **tokenizedLine)
{

  // Om histories skal printes
  if (strcmp(tokenizedLine[0], "history") == 0)
  {
    printHistory(history, historyAdditionCount);
    return 1;
    // Om historie skal slettes
  }
  else if (strcmp(tokenizedLine[0], "delete") == 0 && strcmp(tokenizedLine[1], "history") == 0)
  {
    emptyHistory(history, &historyAdditionCount);
    printf("history deleted\n");
    return 1;
  }
  // Om program skal avsluttes
  if (strcmp(tokenizedLine[0], "exit") == 0)
  {
    // Brukes i status variabel
    return 0;
  }

  // Håndterer cd
  if (strcmp(tokenizedLine[0], "cd") == 0)
  {

    if (tokenizedLine[1] == NULL)
    {
      fprintf(stderr, "cd: no directory specified");
      return 1;
    }
    else
    {

      if (chdir(tokenizedLine[1]) != 0)
      {
        perror("cd failed");
        exit(EXIT_FAILURE);
      }
    }
    return 1;
  }

  pid_t childPID;
  int status;

  childPID = fork();
  // I barneprosess
  if (childPID == 0)
  {
    if (execvp(tokenizedLine[0], tokenizedLine) == -1)
    {
      perror("Error executing execvp()");
      exit(EXIT_FAILURE);
    }
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

void freeTokens(char **tokens)
{
  if (tokens)
  {
    for (int i = 0; i < numberOfArgs; i++)
    {
      free(tokens[i]);
    }
    free(tokens);
  }
}