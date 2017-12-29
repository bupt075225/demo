#include <stdlib.h>
#include <stdio.h>

#include "create_process.h"

static void launch_schedule(void)
{
    int32_t a = MAX;

    printf("Current process pid %d\n", getpid());
    printf("Current process group id %d\n", getpgrp());
    /*while(1) {
        
    }*/    
}
static void child_process(void)
{
    printf("Child process %d here\n", getpid());    
    launch_schedule();
}

static void parent_process(void)
{
    printf("Parent process %d here\n", getpid());
    launch_schedule();
}

int32_t main(void)
{
    pid_t pid;

    pid = fork();
    if (pid > 0) {
        parent_process();
    } else if (0 == pid) {
        child_process();
    } else {
        printf("Create child process fail %d\n", pid);    
        return pid;
    }

    exit(0);
}

