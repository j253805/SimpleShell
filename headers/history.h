#ifndef HISTORY_H
#define HISTORY_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HISTORY_SIZE 10000

// Function prototypes
void addToHistory(char ***history, char *line, uint16_t *historyAdditionCount);
void emptyHistory(char **history, uint16_t *historyAdditionCount);
void printHistory(char **history, uint16_t historyAdditionCount);

void addToHistory(char ***history, char *line, uint16_t *historyAdditionCount)
{
    int lineLength = strlen(line);

    if (!(lineLength <= 1 || line[0] == '\n' || line[1] == '\0'))
    {
        // Reallocate memory for the history array
        *history = (char **)realloc(*history, (*historyAdditionCount + 1) * sizeof(char *));
        if (*history == NULL)
        {
            fprintf(stderr, "Error reallocating memory\n");
            exit(EXIT_FAILURE);
        }

        // Allocate memory for the line in the array
        (*history)[*historyAdditionCount] = malloc((lineLength + 1) * sizeof(char));
        if ((*history)[*historyAdditionCount] == NULL)
        {
            fprintf(stderr, "Error allocating memory\n");
            exit(EXIT_FAILURE);
        }

        // Copy the line into the array
        strcpy((*history)[*historyAdditionCount], line);
        (*historyAdditionCount)++;
    }
}

void emptyHistory(char **history, uint16_t *historyAdditionCount)
{
    for (int i = 0; i < *historyAdditionCount; i++)
    {
        free(history[i]);
        history[i] = NULL;
    }
    *historyAdditionCount = 0;
}

void printHistory(char **history, uint16_t historyAdditionCount)
{
    // Off-set dropper å skrive ut nyeste kommando, som alltids vil være "history"
    for (int i = 0; i < historyAdditionCount - 1; i++)
    {
        // (i+1) er posisjon 1 indexert
        printf("%d  %s\n", (i + 1), history[i]);
    }
    printf("\n");
}

#endif /* HISTORY_H */
