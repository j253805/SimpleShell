#include <stdio.h>
#include <stdlib.h>
#include <sys/_types.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

// ############### General shell functions ############
char *readLine(void);
char **parseLine(char *line);
void exitShell(void);
int executeLine(char **tokenizedLine);
void freeTokens(char **tokens);

// Globale variabler, refaktorerer kanskje program slik at disse ikke blir nødvendige
int numberOfArgs = 0;
int exitStatus = 1;

// ############### General shell functions ############

// --------------------------------------------------------

// ############### History functions ##################
#define HISTORY_SIZE 100

void addToHistory(char **history, char *line, uint16_t *historyAdditionCount);
void printHistory(char **history, uint16_t historyAdditionCount);
void deleteHistory(char **history, uint16_t *historyAdditionCount);

char *history[HISTORY_SIZE];
uint16_t historyAdditionCount = 0;
// ############### History functions ##################

int main(void)
{

  // Strings for shell-prompten
  char cwd[1024];
  char *userName = getlogin();
  char hostName[1024];
  gethostname(hostName, sizeof(hostName));

  char *line;
  char **tokens;

  do
  {
    // Henter nåværende arbeidskatalog
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
      printf("%s@%s %s\n> $ ", userName, hostName, cwd);
      line = readLine();
      addToHistory(history, line, &historyAdditionCount);
      tokens = parseLine(line);

      // Sjekker om tokens != NULL og tokens[0] ikke er tom
      if (tokens && tokens[0])
      {
        executeLine(tokens);
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

  // int tokenIndex = 0;
  currentToken = strtok(line, " ");
  while (currentToken != NULL)
  {

    if (position >= bufSize)
    {
      bufSize += 64;
      buffer = realloc(buffer, bufSize);
      if (buffer == NULL)
      {
        fprintf(stderr, "Error allocating memory");
        exit(EXIT_FAILURE);
      }
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

void exitShell(void)
{
  exitStatus = 0;
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
    deleteHistory(history, &historyAdditionCount);
    printf("history deleted\n");
    return 1;
  }
  // Om program skal avsluttes
  if (strcmp(tokenizedLine[0], "exit") == 0)
  {
    exitShell();
    return 1;
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
    if (waitpid(childPID, &status, WUNTRACED) == -1)
    {
      perror("Error wating for child");
      exit(EXIT_FAILURE);
    }
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

// TODO: Historie funker nesten godt nå, men når den printes printes også \n karakterer, dette må fikses

// En funksjon som legger til kommandoer i en historietabell
// MaksInnslag == HISTORY_SIZE, tømmer dersom lengden overskrider dette
void addToHistory(char **history, char *line, uint16_t *historyAdditionCount)
{

  if (*historyAdditionCount < HISTORY_SIZE)
  {
    if ((history[*historyAdditionCount] = strdup(line)) == NULL)
    {
      fprintf(stderr, "Error copying string to history");
      exit(EXIT_FAILURE);
    }
    (*historyAdditionCount)++;
  }
  else
  {
    // Shifter tabellen for å gjøre plass til den overflødige kommando
    for (int i = 0; i < HISTORY_SIZE - 1; i++)
    {
      free(history[i]);
      history[i] = history[i + 1];
    }
    if ((history[HISTORY_SIZE - 1] = strdup(line)) == NULL)
    {
      fprintf(stderr, "Error copying string to history");
      exit(EXIT_FAILURE);
    }
    (*historyAdditionCount) -= 1;
  }
}

void deleteHistory(char **history, uint16_t *historyAdditionCount)
{
  for (int i = 0; i < *historyAdditionCount; i++)
  {
    free(history[i]);
  }
  *historyAdditionCount = 0;
}

void printHistory(char **history, uint16_t historyAdditionCount)
{

  for (int i = 0; i < historyAdditionCount; i++)
  {
    printf("%s\n", history[i]);
  }
  printf("\n");
}
