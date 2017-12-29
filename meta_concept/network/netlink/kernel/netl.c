#include <linux/init.h>
#include <linux/module.h>
#include <net/sock.h>
#include <net/netlink.h>


#define NETLINK_TEST  25
#define MAX_MSGSIZE  1024


struct sock *nl_sk = NULL;
int flag = 0;

int string_length(char *s)
{
    int slen = 0;
    for(; *s; s++) {
        slen++;
    }
    return slen;
}

/* Send message to user space */
void sendnlmsg(char *message, int pid)
{
    struct sk_buff *skb_1;
    struct nlmsghdr *nlh;
    int len = NLMSG_SPACE(MAX_MSGSIZE);
    int slen = 0;

    if(!message || !nl_sk) {
        return;
    }
    printk("Pid:%d\n", pid);
    skb_1 = alloc_skb(len, GFP_KERNEL);
    if(!skb_1) {
        printk("allock_skb error\n");
        return;
    }
    slen = string_length(message);
    nlh = nlmsg_put(skb_1, 0, 0, 0, MAX_MSGSIZE, 0);
    NETLINK_CB(skb_1).portid = 0;
    NETLINK_CB(skb_1).dst_group = 0;
    memcpy(NLMSG_DATA(nlh), message, slen);
    printk("Send message '%s'\n", (char *)NLMSG_DATA(nlh));
    netlink_unicast(nl_sk, skb_1, pid, MSG_DONTWAIT);
}

/* Receive user space message */
void nl_data_ready(struct sk_buff *__skb)
{
    struct sk_buff *skb;
    struct nlmsghdr *nlh;
    char str[100];
    struct completion cmpl;
    int i = 10;
    int pid;
    printk("Begin data_ready\n");

    skb = skb_get(__skb);
    if (skb->len >= NLMSG_SPACE(0)) {
        nlh = nlmsg_hdr(skb);
        memcpy(str, NLMSG_DATA(nlh), sizeof(str));
        printk("Message received:%s\n", str);
        pid = nlh->nlmsg_pid;
        while(i--) {
            init_completion(&cmpl);
            /* Send message to user space per 3 seconds */
            wait_for_completion_timeout(&cmpl, 3*HZ);
            sendnlmsg("I am from kernel", pid);
        }
        flag = 1;
        kfree_skb(skb);
    }
    
}

int netlink_init(void)
{
    struct netlink_kernel_cfg cfg;

    cfg.groups = 1;
    cfg.input = nl_data_ready;
    cfg.cb_mutex = NULL;

    nl_sk = netlink_kernel_create(&init_net, NETLINK_TEST, &cfg); 
    if (!nl_sk) {
        printk("Create netlink socket error\n");
        return 1;
    }
    printk("Create netlink socket ok\n");
    
    return 0;
}

void netlink_exit(void)
{
    if (nl_sk != NULL) {
        sock_release(nl_sk->sk_socket);
    }    
    printk("Self module exited\n");
}

module_init(netlink_init);
module_exit(netlink_exit);
MODULE_LICENSE("GPL");
