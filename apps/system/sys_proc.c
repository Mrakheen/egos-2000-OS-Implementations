/*
 * (C) 2022, Cornell University
 * All rights reserved.
 */

/* Author: Yunhao Zhang
 * Description: system process for spawning and killing other processes
 */

#include "elf.h"
#include "app.h"
#include "disk.h"
#include <string.h>
#include <stdlib.h>

static int app_ino, app_pid;
static void sys_spawn(int base);
static int app_spawn(struct proc_request *req);
static void app_kill(int pid);

int main() {
    SUCCESS("Enter kernel process GPID_PROCESS");    

    int sender, shell_waiting;
    char buf[SYSCALL_MSG_LEN];

    sys_spawn(SYS_FILE_EXEC_START);
    grass->sys_recv(NULL, buf, SYSCALL_MSG_LEN);
    INFO("sys_proc receives: %s", buf);

    sys_spawn(SYS_DIR_EXEC_START);
    grass->sys_recv(NULL, buf, SYSCALL_MSG_LEN);
    INFO("sys_proc receives: %s", buf);

    sys_spawn(SYS_SHELL_EXEC_START);
    
    while (1) {
        struct proc_request *req = (void*)buf;
        struct proc_reply *reply = (void*)buf;
        grass->sys_recv(&sender, buf, SYSCALL_MSG_LEN);

        switch (req->type) {
        case PROC_SPAWN:
            reply->type = app_spawn(req) < 0 ? CMD_ERROR : CMD_OK;

            /* Handling background processes */
            shell_waiting = (req->argv[req->argc - 1][0] != '&');
            if (!shell_waiting && app_pid > 0)
                INFO("process %d running in the background", app_pid);
            grass->sys_send(GPID_SHELL, (void*)reply, sizeof(reply));
            break;
        case PROC_EXIT:
            grass->proc_free(sender);

            if (shell_waiting && app_pid == sender)
                grass->sys_send(GPID_SHELL, (void*)reply, sizeof(reply));
            else
                INFO("background process %d terminated", sender);
            break;
        case PROC_KILLALL:
            grass->proc_free(-1); break;
           // case PROC_SETPRIO:
            /*
              Student's code goes here (priority / scheduler). 
              Set the priority of the process.  The req should
              have the pid and priority.
              Send a reply signifying CMD_OK on success and
              CMD_ERROR on failure
            */  
            break;
        case PROC_KILL:
            app_kill(atoi(req->argv[0]));
            reply->type = CMD_OK;
            grass->sys_send(sender, (void*)reply, sizeof(reply));
            break;
        default:
            FATAL("sys_proc: invalid request %d", req->type);
        }
    }
}

static int app_read(int off, char* dst) { file_read(app_ino, off, dst); }

static int app_spawn(struct proc_request *req) {
    int bin_ino = dir_lookup(0, "bin/");
    if ((app_ino = dir_lookup(bin_ino, req->argv[0])) < 0) return -1;

    app_pid = grass->proc_alloc();
    int argc = req->argv[req->argc - 1][0] == '&'? req->argc - 1 : req->argc;

    elf_load(app_pid, app_read, argc, (void**)req->argv);
    grass->proc_set_ready(app_pid);

    grass->proc_set_prio(app_pid,2);

    return 0;
}

static void app_kill(int pid) {
    grass->proc_free(pid);
}

static int sys_proc_base;
char* sysproc_names[] = {"sys_proc", "sys_file", "sys_dir", "sys_shell"};

static int sys_proc_read(int block_no, char* dst) {
    return earth->disk_read(sys_proc_base + block_no, 1, dst);
}

static void sys_spawn(int base) {
    int pid = grass->proc_alloc();
    INFO("Load kernel process #%d: %s", pid, sysproc_names[pid - 1]);

    sys_proc_base = base;
    elf_load(pid, sys_proc_read, 0, NULL);
    grass->proc_set_ready(pid);
    grass->proc_set_prio(pid, 2);

}