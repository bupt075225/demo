#include <stdlib.h>
#include <stdio.h>

#include "lookup_intf.h"

int32_t main(int32_t argc, char *argv[])
{
    int32_t ret = 0;
    nl_handle_t nl;

    /* Netlink quick start demo */
    printf(">>>>>>>Runing quick start demo\n");
    netlink_quick_start();

    /* Get interface info begin */
    printf("\n>>>>>>>Runing get interface demo\n");
    ret = netlink_socket(&nl);
    if(0!=ret) {
        return -1;
    }

    ret = netlink_request(&nl, AF_PACKET, RTM_GETLINK);
    if(0!=ret) {
        return -1;
    }

    printf("Waiting receive message\n");
    netlink_response(&nl);

    close(nl.fd);
    /* Get interface info end */
    return 0;
}
