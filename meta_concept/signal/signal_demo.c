#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

static void child_signal_handler()
{
    
}

static void signal_handler(int32_t sig)
{
    printf("In signal handler, %d\n", sig);    
}

void * signal_set(int32_t signo, void (*func) (void *, int), void *param)
{
    struct sigaction sig;
    struct sigaction osig;
    sigset_t sset;
    int32_t ret;

    if ((void *)SIG_IGN == func || (void *)SIG_DFL == func) {
        /*不捕捉信号,忽略或按默认动作处理信息*/
        sig.sa_handler = (void *)func;
        func = NULL;
        param = NULL;
    } else {
        sig.sa_handler = signal_handler;    
    }

    sigemptyset(&sig.sa_mask);
    sig.sa_flags = 0;
    sig.sa_flags |= SA_RESTART;

    /*设置信号处理函数时，阻塞信信号*/
    if (NULL != func) {
        sigemptyset(&sset);
        sigaddset(&sset, signo);
        sigprocmask(SIG_BLOCK, &sset, NULL);
    }

    ret = sigaction(signo, &sig, &osig);
    if (ret < 0) {
        return SIG_ERR;
    }

    /*解除信号阻塞*/
    if (NULL != func) {
        sigprocmask(SIG_UNBLOCK, &sset, NULL);    
    }
    
    return ((osig.sa_flags & SA_SIGINFO) ? (void * )osig.sa_sigaction : (void *)osig.sa_handler);
}

void launch_schedule(void)
{
    signal_set(SIGCHLD, child_signal_handler, NULL);
    while(1) {
        
    }    
}

void parent_process(int32_t child_pid)
{
    printf("Create child process %d success\n", child_pid);
    launch_schedule();
}

void child_process(void)
{
    printf("Child process here\n");    
    launch_schedule();
}

int32_t main(void)
{
    pid_t pid;

    pid = fork();
    if (pid) {
        parent_process(pid);
    } else if (pid < 0) {
        printf("Create child process fail %d\n", pid);    
        return pid;
    }

    child_process();
}

