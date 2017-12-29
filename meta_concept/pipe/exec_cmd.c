#include <stdlib.h>
#include <stdio.h>

#include "exec_cmd.h"

int32_t main(void)
{
    FILE *fd_in = NULL;
    char *cmd = "./cmd.sh";
    char buffer[MAX_LINE];
    int32_t ret = 0;

    /*popen打开一个单工通信管道,然后调用fork创建子进程,exec载入
     * 命令并执行,父进程从管道读取命令输出*/
    fd_in = popen(cmd, "r");
    if (NULL == fd_in) {
        printf("Execute %s\n failed\n", cmd);
        exit(1);
    } else {
        while (fgets(buffer, MAX_LINE, fd_in) != NULL) {
            printf("%s\n", buffer); 
        }    
    }

    /*pclose等待命令执行完成,返回命令的退出状态,出错时返回-1*/
    ret = pclose(fd_in);
    if (-1 == ret) {
        printf("pclose error\n");
    } else {
        printf("cmd exit status %d\n", ret);
    }

    exit(0);
}
