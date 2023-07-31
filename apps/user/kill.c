/*
 * (C) 2022, Cornell University
 * All rights reserved.
 */

/* Author: Mubtasim Ahmed Rakheen
 * Description: User application to kill a process with the given PID
 */

#include "app.h"
#include "servers.h"
#include <stdlib.h>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: kill <pid>\n");
        return -1;
    }

    int pid = atoi(argv[1]);
    int status = kill_process(pid);

    if (status == 0)
        printf("Process with PID %d killed successfully.\n", pid);
    else
        printf("Failed to kill process with PID %d.\n", pid);

    return status;
}


