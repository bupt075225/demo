#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

void is_network_byte_order(void)
{
    uint16_t value = 0x12;
    uint8_t tester = 0;

    if (1 == *((uint8_t *)&value)) {
        printf("This is a big endian system\n");    
    }

    printf("This is a little endian system\n");    
}

void transfer_addr_presentation_to_numeric(int32_t family, 
                   char *str, size_t str_len, void *addr)
{
    int32_t ret = 0;
    const char *dst = NULL;
    int32_t host_addr;

    ret = inet_pton(family, str, addr);
    if (1 != ret) {
        printf("Address presentation transfer to numeric failed, ret\n", ret);
        return;
    } else {
        host_addr = ntohl(((struct in_addr *)addr)->s_addr);
    }

    dst = inet_ntop(family, addr, str, str_len);
    if (NULL == dst) {
        printf("Address numeric transfer to presentation failed\n");
        return;
    }

    printf("Address presentation:%s\n", dst);

    return;
}

void get_addrinfo_from_name(void)
{
    int32_t ret = 0;
    struct addrinfo hints, *res, *res_first;
    char service_name[64];
    char *server_address = NULL;
    char addr_str[INET_ADDRSTRLEN];
    const char *addr = NULL;
    struct sockaddr_in *ipv4_sock;

    memset(service_name, 0, sizeof(service_name));
    snprintf(service_name, sizeof(service_name), "%d", 3260);

    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    ret = getaddrinfo(server_address, service_name, &hints, &res_first);
    if (0!=ret) {
        printf("Get address info failed, %s\n", gai_strerror(ret));
    }

    for (res=res_first; res; res=res->ai_next) {
        ipv4_sock = (struct sockaddr_in *)(res->ai_addr);
        addr = inet_ntop(res->ai_family, &((ipv4_sock->sin_addr).s_addr),
                    addr_str, INET_ADDRSTRLEN);
        if (NULL!=addr) {
            printf("Address family:%d, socket type:%d, protocol:%d, addr:%s, port:%d\n",
                        res->ai_family, res->ai_socktype, 
                        res->ai_protocol, addr, ntohs(ipv4_sock->sin_port));
        } else {
            printf("Get address presentation failed\n");    
        }
    }
    
}

int32_t main(void)
{
    char addr_str[INET_ADDRSTRLEN] = "192.168.30.111";
    struct in_addr inet_addr;

    get_addrinfo_from_name();

    transfer_addr_presentation_to_numeric(AF_INET, addr_str, 
            INET_ADDRSTRLEN, &inet_addr);

    is_network_byte_order();
    return 0;
}
