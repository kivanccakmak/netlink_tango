#ifndef _UTILS_H
#define _UTILS_H

int get_iface_ip(const char *ifname, char *ip_out);
int get_iface_mac(const char *ifname, char *mac_out);
int get_iface_flags(const char *ifname, char *flags_out, int *num_flags);

#define MAC_STR "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx"
#define MAC_STR_ARGS(m) m[0], m[1], m[2], m[3], m[4], m[5]

#define sfree(_ptr)         \
    do {                    \
        if (_ptr) {         \
            free(_ptr);     \
            _ptr = NULL;    \
        }                   \
    } while (0)
#endif
