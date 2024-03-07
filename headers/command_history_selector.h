#ifndef COMMAND_HISTORY_SELECTOR_H
#define COMMAND_HISTORY_SELECTOR_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *commandSelector(char ***history, uint16_t historyIndex, int upwardArrow, int downwardArrow);
int checkForUpwardsDirectionalArrow(char *line);
int checkForDownwardsDirectionalArrow(char *line);

/*
Ikke fått testa, men i jsh.c kanskje legg til "live" overvåkning av input utenom å sjekke line
Altså en getchar før selve innelsing, en enkel sjekk
Må da refaktorere de to nederste funksjonene
*/
char *commandSelector(char ***history, uint16_t historyIndex, int upwardArrow, int downwardArrow)
{
    char *selectedLine;

    uint16_t historyUpperLimit = historyIndex - 1;
    uint16_t historyLowerLimit = 0;
    if (*history != NULL)
    {
        if (upwardArrow && historyIndex > historyLowerLimit)
        {
            historyIndex--;
            selectedLine = (char *)malloc(strlen(*history[historyIndex]) + 1);
            if (selectedLine == NULL)
            {
                fprintf(stderr, "Error allocating memory\n");
                exit(EXIT_FAILURE);
            }
            strcpy(selectedLine, *history[historyIndex]);
        }
        else if (downwardArrow && historyIndex < historyUpperLimit)
        {
            historyIndex++;
            selectedLine = (char *)malloc(strlen(*history[historyIndex]) + 1);
            if (selectedLine == NULL)
            {
                fprintf(stderr, "Error allocating memory\n");
                exit(EXIT_FAILURE);
            }
            strcpy(selectedLine, *history[historyIndex]);
        }
    }

    return selectedLine;
}

int checkForUpwardsDirectionalArrow(char *line)
{
    int upwardsArrowDetected = 0;
    if (line[0] == '\033' && line[1] == '[' && line[2] == 'A')
    {
        upwardsArrowDetected = 1;
    }

    return upwardsArrowDetected;
}

int checkForDownwardsDirectionalArrow(char *line)
{
    int downwardsArrowDetected = 0;
    if (line[0] == '\033' && line[1] == '[' && line[2] == 'A')
    {
        downwardsArrowDetected = 1;
    }

    return downwardsArrowDetected;
}

#endif /* COMMAND_HISTORY_SELECTOR_H */
