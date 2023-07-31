/*
 * (C) 2022, Cornell University
 * All rights reserved.
 */

/* Author: Yunhao Zhang
 * Description: user friendly interfaces of system processes
 * including GPID_PROCESS, GPID_FILE and GPID_DIR
 */

#include "egos.h"
#include "servers.h"
#include <string.h>

static int sender;
static char buf[SYSCALL_MSG_LEN];

void exit(int status) {
    grass->sys_exit(status);
    while(1);
}

int dir_lookup(int dir_ino, char* name) {
    struct dir_request req;
    req.type = DIR_LOOKUP;
    req.ino = dir_ino;
    strcpy(req.name, name);
    grass->sys_send(GPID_DIR, (void*)&req, sizeof(req));

    grass->sys_recv(&sender, buf, SYSCALL_MSG_LEN);
    if (sender != GPID_DIR) FATAL("dir_lookup: an error occurred");
    struct dir_reply *reply = (void*)buf;

    return reply->status == DIR_OK? reply->ino : -1;
}

int file_read(int file_ino, int offset, char* block) {
    struct file_request req;
    req.type = FILE_READ;
    req.ino = file_ino;
    req.offset = offset;
    grass->sys_send(GPID_FILE, (void*)&req, sizeof(req));

    grass->sys_recv(&sender, buf, SYSCALL_MSG_LEN);
    if (sender != GPID_FILE) FATAL("file_read: an error occurred");
    struct file_reply *reply = (void*)buf;
    memcpy(block, reply->block.bytes, BLOCK_SIZE);

    return reply->status == FILE_OK? 0 : -1;
}

/*int setprio( int pid, int priority )
{
  /* Student's code goes here to send a request to set a processes priority */
  /* and read and return the status */
//} */
int kill_process(int pid) {
    struct proc_request req;
    req.type = PROC_KILL;
    req.argc = 1;

    char pid_str[CMD_ARG_LEN];
    int i = 0;
    int temp = pid;

    // Convert pid to string representation
    if (temp == 0) {
        pid_str[i++] = '0';
    } else {
        if (temp < 0) {
            pid_str[i++] = '-';
            temp = -temp;
        }

        int num_digits = 0;
        int divisor = 1;
        while (temp / divisor > 0) {
            divisor *= 10;
            num_digits++;
        }

        divisor /= 10;

        while (divisor > 0) {
            int digit = temp / divisor;
            pid_str[i++] = digit + '0';
            temp %= divisor;
            divisor /= 10;
        }
    }

    pid_str[i] = '\0';

    // Copy pid_str to req.argv[0]
    strcpy(req.argv[0], pid_str);

    grass->sys_send(GPID_PROCESS, (void*)&req, sizeof(req));
    grass->sys_recv(&sender, buf, SYSCALL_MSG_LEN);

    if (sender != GPID_PROCESS)
        FATAL("kill_process: an error occurred");

    struct proc_reply *reply = (void*)buf;
    return reply->type == CMD_OK ? 0 : -1;
}


