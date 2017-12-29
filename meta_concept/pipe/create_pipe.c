#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_MSG_LEN   (32)

/*管道的读写描述符,写描述符的输出是读描述符的输入*/
static int32_t rw_pipe[2];

static void child_process(void)
{
    char message[MAX_MSG_LEN];
    
    /*构造父进程到子进程的管道数据流,子进程需要关闭写描述符*/
    close(rw_pipe[1]);
    printf("Child receive message from pipe\n");
    read(rw_pipe[0], message, MAX_MSG_LEN);
    printf("%s\n", message);
}

static void parent_process(void)
{
    int32_t ret = 0;
    char *message = "Hello world";
    
    printf("Parent write message to child by pipe\n");
    /*构造父进程到子进程的管道数据流,父进程需要关闭读描述符*/
    close(rw_pipe[0]);
    ret = write(rw_pipe[1], message, strlen(message));
    if (ret != strlen(message)) {
        printf("Write message to pipe failed\n");
    }
}

int32_t main(void)
{
    pid_t pid;

    /*创建半双工管道*/
    if (pipe(rw_pipe) < 0) {
         printf("Create pipe failed\n");        
    }

    pid = fork();
    if (pid > 0) {
        parent_process();
    } else if (0 == pid) {
        child_process();
    } else {
        printf("Create child process failed\n");    
    }

    exit(0);
}
