#ifndef HISTORY_H
#define HISTORY_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HISTORY_SIZE 3

// Function prototypes
void addToHistory(char **history, char *line, uint16_t *historyAdditionCount);
void deleteHistory(char **history, uint16_t *historyAdditionCount);
void printHistory(char **history, uint16_t historyAdditionCount);

// TODO: BUG, dobbelfrigjøring av minne i "line", dette må fikses. Det frigjøres her dersom
// Historiegrensen overskredes, og i hovedløkka i simpleShell.c etter hver iterasjon
//  MaksInnslag == HISTORY_SIZE, tømmer dersom lengden overskrider dette
void addToHistory(char **history, char *line, uint16_t *historyAdditionCount)
{

    // Dette løste newline problemet, kanskje det skaper bungs senere, vær varsom
    // Altså ved å sjekke om lengden er over 1, hvis den er det, legg den til
    int lineLen = strlen(line);
    if (lineLen > 1)
    {

        if (*historyAdditionCount <= HISTORY_SIZE)
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
                if (history[i] != line && history[i] != NULL)
                {
                    free(history[i]);
                    history[i] = NULL;
                }
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
    // Off-set dropper å skrive ut nyeste kommando, som alltids vil være "history"
    for (int i = 0; i < historyAdditionCount - 1; i++)
    {
        // (i+1) er posisjon 1 indexert
        printf("%d  %s\n", (i + 1), history[i]);
    }
    printf("\n");
}

#endif /* HISTORY_H */
