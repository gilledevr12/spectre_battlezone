#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <stdio.h>
#include <string.h>

void mac_eth0(unsigned char MAC_str[13])
{
    #define HWADDR_len 6
    int s,i;
    struct ifreq ifr;
    s = socket(AF_INET, SOCK_DGRAM, 0);
    strcpy(ifr.ifr_name, "wlp1s0");
    ioctl(s, SIOCGIFHWADDR, &ifr);
    for (i=0; i<HWADDR_len; i++)
        sprintf(&MAC_str[i*2],"%02X",((unsigned char*)ifr.ifr_hwaddr.sa_data)[i]);
    MAC_str[12]='\0';
}

int main(int argc, char *argv[])
{
    unsigned char mac[13];

    mac_eth0(mac);
    puts(mac);

    FILE* fout;
    fout = fopen("DEVICE_MAC.h", "w");
    fprintf(fout, "#ifndef DEVICE_MAC_H\n");
    fprintf(fout, "#define DEVICE_MAC_H\n");

    fprintf(fout, "const int DEVICE_MAC = 0x%s;\n", mac);

    fprintf(fout, "#endif //DEVICE_MAC_H\n");

    return 0;
}
