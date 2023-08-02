/*
 * (C) 2022, Cornell University
 * All rights reserved.
 */

/* Author: Yunhao Zhang
 * Description: helper functions for managing processes
 */

#include "egos.h"
#include "process.h"
#include "syscall.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>

void intr_entry(int id);

void excp_entry(int id) {
    /* Student's code goes here (handle memory exception). */

    /* If id is for system call, handle the system call and return */

    /* Otherwise, kill the process if curr_pid is a user application */

    /* Student's code ends here. */

    FATAL("excp_entry: kernel got exception %d", id);
}

void proc_init() {
    earth->intr_register(intr_entry);
    earth->excp_register(excp_entry);

    /* Student's code goes here (PMP memory protection). */

    /* Setup PMP TOR region 0x00000000 - 0x20000000 as r/w/x */

    /* Setup PMP NAPOT region 0x20400000 - 0x20800000 as r/-/x */

    /* Setup PMP NAPOT region 0x20800000 - 0x20C00000 as r/-/- */

    /* Setup PMP NAPOT region 0x80000000 - 0x80004000 as r/w/- */

    /* Student's code ends here. */

    /* The first process is currently running */
    proc_set_running(proc_alloc());
}
static void proc_set_status(int pid, int status) {
    for (int i = 0; i < MAX_NPROCESS; i++)
        if (proc_set[i].pid == pid) proc_set[i].status = status;
}

int proc_alloc() {
    static int proc_nprocs = 0;
    int random = 0;
    while(random <10){
        random = rand();
    }
    for (int i = 0; i < MAX_NPROCESS; i++) {
        if (proc_set[i].status == PROC_UNUSED) {
            proc_set[i].pid = ++proc_nprocs;
            proc_set[i].status = PROC_LOADING;

            // Assign default priority based on process type
            if (proc_nprocs <= 4) {
                proc_set[i].priLevel = 1;  // Kernel process
            } else {
                proc_set[i].priLevel = 2;  // User process
            }

            proc_set[i].num_of_Tickets = round(random/proc_set[i].priLevel);

            return proc_nprocs;
        }
    }

    FATAL("proc_alloc: reached the limit of %d processes", MAX_NPROCESS);
}


void proc_free(int pid) {
    if (pid != -1) {
        earth->mmu_free(pid);
        proc_set_status(pid, PROC_UNUSED);
        return;
    }

    /* Free all user applications */
    for (int i = 0; i < MAX_NPROCESS; i++)
        if (proc_set[i].pid >= GPID_USER_START && proc_set[i].status != PROC_UNUSED) {
            earth->mmu_free(proc_set[i].pid);
            proc_set[i].status = PROC_UNUSED;
        }
}

void proc_set_ready(int pid) { proc_set_status(pid, PROC_READY); }
void proc_set_running(int pid) { proc_set_status(pid, PROC_RUNNING); }
void proc_set_runnable(int pid) { proc_set_status(pid, PROC_RUNNABLE); }
int  proc_get_pid( ){ return curr_pid; }
struct process * proc_get_proc_set( ){ return &proc_set[0];}

//For Round Robin Scheduler
void proc_set_prio(int pid, int priority)
{
    if(pid >= 0 && pid < MAX_NPROCESS)
    {
        if(priority <= 10 && priority >= 1)
        {
            proc_set[pid].priLevel = priority;
            int random = 0;
            while(random <10){
                random = rand();
            }
            proc_set[pid].num_of_Tickets = round(random/proc_set[pid].priLevel);
        }
        else
        {
            printf("invalid priority level");
        }
    }
    else
    {
        printf("invalid pid");
    }     
} 

//For Lottery Scheduler
void proc_set_ticket(int pid, int num_of_tickets) {
    if (pid >= 0 && pid < MAX_NPROCESS) {
        proc_set[pid].num_of_Tickets = num_of_tickets;
    } else {
        printf("Invalid pid\n");
    }
}




