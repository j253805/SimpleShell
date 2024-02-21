#ifndef UTILITY_AND_STATUS_H
#define UTILITY_AND_STATUS_H

#include <stdlib.h>
#include <stdio.h>

// Function prototypes
void exitShell(int *exitStatus);

void exitShell(int *exitStatus)
{
    *exitStatus = 0;
}

#endif /* UTILITY_AND_STATUS_H */