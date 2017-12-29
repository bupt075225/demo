#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <errno.h>

#define MAX_PAYLOAD  1024
#define NETLINK_TEST  25

int32_t netlink_quick_start(void)
{
    struct sockaddr_nl src_addr, dst_addr;
    struct nlmsghdr *nlh = NULL;
    struct iovec iov;
    struct msghdr msg;
    int32_t sock_fd, ret;
    int32_t state_smg = 0;
    int32_t state;

    /* Create socket */
    sock_fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_TEST);
    if (-1 == sock_fd) {
        printf("Error getting socket:%s.\nPlease insmod your private kernel module\n",
                strerror(errno));
        return -1;
    }

    /* Binding */
    memset(&src_addr, 0, sizeof(src_addr));
    src_addr.nl_family = AF_NETLINK;
    src_addr.nl_pid = 100;
    src_addr.nl_groups = 0;

    ret = bind(sock_fd, (struct sockaddr *)&src_addr, sizeof(src_addr));
    if (ret < 0) {
        printf("Bind failed:%s", strerror(errno));
        close(sock_fd);
        return ret;
    }
    
    /* Create netlink message */
    nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
    if (!nlh) {
        printf("Malloc nlmsghdr error\n");
        close(sock_fd);
        return -1;
    }

    memset(&dst_addr, 0, sizeof(dst_addr));
    dst_addr.nl_family = AF_NETLINK;
    dst_addr.nl_pid = 0;
    dst_addr.nl_groups = 0;

    nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
    nlh->nlmsg_pid = 100;
    nlh->nlmsg_flags = 0;
    strcpy(NLMSG_DATA(nlh), "Hello kernel");
    iov.iov_base = (void *)nlh;
    iov.iov_len = NLMSG_SPACE(MAX_PAYLOAD);

    memset(&msg, 0, sizeof(msg));
    msg.msg_name = (void *)&dst_addr;
    msg.msg_namelen = sizeof(dst_addr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    /* Send message */
    printf("state_smg\n");
    state_smg = sendmsg(sock_fd, &msg, 0);
    if (-1 == state_smg) {
        printf("Get error sendmsg = %s\n", strerror(errno));
    }

    /* Receive message */
    printf("waiting received\n");
    while(1) {
        printf("In while recvmsg\n");    
        state = recvmsg(sock_fd, &msg, 0);
        if (-1==state) {
            printf("Receive message failed\n");
        }
        printf("Received message:%s\n", (char *)NLMSG_DATA(nlh));
    }
    close(sock_fd);
    free(nlh);
    return 0;
}
