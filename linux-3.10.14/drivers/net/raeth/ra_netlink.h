#ifndef RA_NETLINK
#define RA_NETLINK

#include "csr_netlink.h"
int rt2880_csr_msgsend(CSR_MSG* csrmsg, int pid);
void rt2880_csr_receiver(struct sk_buff  *skb);
int csr_netlink_init(void);
void csr_netlink_end(void);

#endif
