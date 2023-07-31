#include "egos.h"

void set_prio(int pid, int prio) 
{ 
    return grass->proc_set_prio(pid, prio);
} 
