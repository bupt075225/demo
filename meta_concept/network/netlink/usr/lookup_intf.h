#ifndef __LOOKUP_INTF_H__
#define __LOOKUP_INTF_H__

#include <stdint.h>
#include <linux/rtnetlink.h>
#include <net/if_arp.h>

#define NLMSG_BUF_SIZE  (3 * 1024)
#define IF_HWADDR_MAX   (20)

typedef struct _nl_handle {
    int32_t fd;
    uint32_t nl_pid;
} nl_handle_t;

int32_t netlink_socket(nl_handle_t *nl);
int32_t netlink_request(nl_handle_t *nl, uint8_t family, uint16_t msg_type);
int32_t netlink_response(nl_handle_t *nl);
#endif
