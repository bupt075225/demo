#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <errno.h>

#include "lookup_intf.h"


/*
 * Create netlink socket
 */
int32_t netlink_socket(nl_handle_t *nl)
{
    struct sockaddr_nl nl_src;
    socklen_t addr_len;
    int32_t ret = 0;

    /* Create socket */
    nl->fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if(nl->fd < 0) {
        printf("Create netlink socket failed,%s\n", strerror(errno));
        return -1;
    }

    /* Bind socket */
    nl_src.nl_family = AF_NETLINK;
    nl_src.nl_pid = getpid();
    ret = bind(nl->fd, (struct sockaddr *)&nl_src, sizeof(nl_src));
    if(ret < 0) {
        printf("Bind netlink src address failed, %s\n", strerror(errno));
        close(nl->fd);
        return -1;
    }

    /* Get socket address */
    ret = getsockname(nl->fd, (struct sockaddr *)&nl_src, &addr_len);
    if(ret < 0 || addr_len != sizeof(nl_src)) {
        printf("Get netlink socket address failed, %s\n", strerror(errno));
        close(nl->fd);
        return -1;
    }

    if (AF_NETLINK != nl_src.nl_family) {
        printf("Wrong address family %d\n", nl_src.nl_family);    
        close(nl->fd);
        return -1;
    }

    printf("Netlink portid:%d\n", nl_src.nl_pid);
    nl->nl_pid = nl_src.nl_pid;

    return 0;
}


/*
 * Send netlink message to kernel
 *
 * Netlink header:struct nlmsghdr
 *
 * |------------------------------|
 * |        nlmsg_len             |
 * |------------------------------|
 * |  nlmsg_type  |  nlmsg_flags  |
 * |------------------------------|
 * |        nlmsg_seq             |
 * |------------------------------|
 * |        nlmsg_pid             |
 * |------------------------------|
 * |        payload               |
 * |------------------------------|
 *
 */
int32_t netlink_request(nl_handle_t *nl, uint8_t family, uint16_t msg_type)
{
    int32_t ret = 0;
    struct sockaddr_nl nl_dst;
    /* Define netlink message structure include header and payload*/
    struct nl_req {
        struct nlmsghdr hdr;
        struct rtgenmsg gen;
    } req;
    struct iovec iov;
    struct msghdr msg;

    /* Send message to kernel */
    memset(&req, 0, sizeof(req));
    /* Prepare netlink message header */
    req.hdr.nlmsg_len = NLMSG_LENGTH(sizeof(struct rtgenmsg));
    req.hdr.nlmsg_type = msg_type;
    /* Flag NLM_F_REQUEST requireing an answer */
    req.hdr.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
    req.hdr.nlmsg_seq = 1;
    req.hdr.nlmsg_pid = nl->nl_pid;
    /* Prepare netlink message payload,AF_PACKET specify which
     * address family we are interested in*/
    req.gen.rtgen_family = family;

    /* Prepare message destination address */
    memset(&nl_dst, 0, sizeof(nl_dst));
    nl_dst.nl_family = AF_NETLINK;

    iov.iov_base = &req;
    iov.iov_len = req.hdr.nlmsg_len;

    /* Prepare message for system call sendmsg */
    memset(&msg, 0, sizeof(msg));
    msg.msg_name = (void *)&nl_dst;
    msg.msg_namelen = sizeof(nl_dst);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;


    printf("Send netlink message to kernel\n");
    ret = sendmsg(nl->fd, &msg, 0);
    if(ret < 0) {
        printf("Send netlink message failed, %s\n", strerror(errno));
        return -1;
    }
    
    return 0;
}

/*
 * RTA stands for route attributes.Think of route information here,
 * routes have lots of attribute, and so do links.
 *
 * struct nlmsghdr + struct ifinfomsg + struct rtattr + attribute
 *
 * ---------------------
 * | struct nlmsghdr   |
 * ---------------------
 * | sturct ifinfomsg  |
 * ---------------------
 * | rta_len | rta_type|
 * ---------------------
 * |   attribute       |
 * ---------------------
 * | rta_len | rta_type|
 * ---------------------
 * |   attribute       |
 * ---------------------
 * |   .....           | 
 * ---------------------
 */
void parse_rtattr(struct rtattr **tb, int max, 
                  struct rtattr *rta, size_t len)
{
    while(RTA_OK(rta, len)) {
        if(rta->rta_type <= max) {
            tb[rta->rta_type] = rta;
        }
        rta = RTA_NEXT(rta, len);
    }
}

int32_t netlink_if_link_filter(struct nlmsghdr *hdr)
{
    struct ifinfomsg *iface;
    struct rtattr *attribute;
    int32_t len;
    struct rtattr *tb[IFLA_MAX + 1];

    if(RTM_NEWLINK != hdr->nlmsg_type) {
        return 0;
    }

    if(hdr->nlmsg_len < sizeof(struct ifinfomsg)) {
        return -1;
    }

    iface = NLMSG_DATA(hdr);
    len = hdr->nlmsg_len - NLMSG_LENGTH(sizeof(*iface));

    memset(tb, 0, sizeof(tb));
    parse_rtattr(tb, IFLA_MAX, IFLA_RTA(iface), len);

    /* Interface index and name */
    printf("Interface %d :", iface->ifi_index); 
    if(NULL==tb[IFLA_IFNAME]) {
        return -1;
    }
    printf("%s\n", (char *)RTA_DATA(tb[IFLA_IFNAME]));

    /* Return if loopback */
    if(ARPHRD_LOOPBACK==iface->ifi_type) {
        return 0;
    }

    /* Interface MTU and flags */
    printf("\tMTU:%d, flags:0x%x\n", *(uint32_t *)RTA_DATA(tb[IFLA_MTU]),
            iface->ifi_flags);

    /* Interface hardware address(MAC) */
    if(tb[IFLA_ADDRESS]) {
        size_t hw_addr_len = RTA_PAYLOAD(tb[IFLA_ADDRESS]);
        if(hw_addr_len > IF_HWADDR_MAX) {
            printf("MAC address for %s is too long:%zu", 
                    tb[IFLA_IFNAME], hw_addr_len);
            return -1;
        }
    }

    return 0;
}

int32_t netlink_response(nl_handle_t *nl)
{
    struct sockaddr_nl nl_src;
    struct nlmsghdr *hdr;
    struct msghdr reply;
    struct iovec iov;
    char buf[NLMSG_BUF_SIZE];
    int32_t end = 0;
    int32_t rsp_len = 0;
    
    /* End flag that will be updated upon receiveing 
     * the message that defines an end of answer */
    while(!end) {
        memset(&reply, 0, sizeof(reply));
        memset(&iov, 0, sizeof(iov));
        iov.iov_base = buf;
        iov.iov_len = sizeof(buf);

        reply.msg_name = &nl_src;
        reply.msg_namelen = sizeof(nl_src);
        reply.msg_iov = &iov;
        reply.msg_iovlen = 1;

        rsp_len = recvmsg(nl->fd, &reply, 0);
        if(rsp_len > 0) {
            //printf("Received total message length:%d\n", rsp_len);
            /* This netlink message itself can have several 
             * message in it's payload */
            for(hdr = (struct nlmsghdr *)buf; NLMSG_OK(hdr, rsp_len); 
                hdr = NLMSG_NEXT(hdr, rsp_len)) {
                //printf("Netlink message type:%d, length:%d\n", 
                        //hdr->nlmsg_type, hdr->nlmsg_len);
                if(RTM_NEWLINK==hdr->nlmsg_type) {
                    netlink_if_link_filter(hdr);
                }
                if(NLMSG_DONE==hdr->nlmsg_type) {
                    printf("Finish of reading\n");
                    end++;
                }

                if(NLMSG_ERROR==hdr->nlmsg_type) {
                    printf("Error handling\n");
                }
            }
        } else {
            printf("Receive netlink message failed, %s\n", strerror(errno));    
            return -1;
        }
    }
}

