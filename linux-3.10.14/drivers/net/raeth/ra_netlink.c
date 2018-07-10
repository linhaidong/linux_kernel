// for netlink header
#include <asm/types.h>
#include <net/sock.h>
#include <linux/socket.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>
#include <linux/net.h>
#include <linux/version.h>

#include "csr_netlink.h"
#include "ra2882ethreg.h"
#include "ra_netlink.h"
#include "ra_mac.h"
static struct sock *csr_msg_socket = NULL; // synchronize socket for netlink use
unsigned int flags;

void rt2880_csr_receiver(struct sk_buff *skb)
{
    int err;
    struct nlmsghdr *nlh;
    int pid;
    unsigned int reg_value = 0;
    CSR_MSG *csrmsg;
    RAETH_PRINT("csr netlink receiver!\n");
    if (skb == NULL) 
    {
        printk("rt2880_csr_receiver(): No data received, error!\n");
        return;
    }

    nlh = (struct nlmsghdr*)skb->data;
    pid = nlh->nlmsg_pid;
    csrmsg = NLMSG_DATA(nlh);
    switch(csrmsg->enable)
    {
        case CSR_READ:
            reg_value = sysRegRead(csrmsg->address);
            break;
        case CSR_WRITE:
            sysRegWrite(csrmsg->address, csrmsg->default_value);
            reg_value = sysRegRead(csrmsg->address);
            break;
        case CSR_TEST:
            reg_value = sysRegRead(csrmsg->address);
            break;
#if defined CONFIG_RALINK_MT7620
        case CSR_PORT0:
            csrmsg->rx_bytes = read_port(0,1);
            csrmsg->tx_bytes = read_port(0,0);
            reg_value = 11;
            break;
        case CSR_PORT1:
            csrmsg->rx_bytes = read_port(1,1);
            csrmsg->rx_bytes = read_port(1,0);
            reg_value = 11;
            break;
        case CSR_PORT2:
            csrmsg->rx_bytes =  read_port(2,1);
            csrmsg->tx_bytes =  read_port(2,0);
            reg_value = 11;
            break;
        case CSR_PORT3:
            csrmsg->rx_bytes =  read_port(3,1);
            csrmsg->tx_bytes =  read_port(3,0);
            reg_value = 11;
            break;
        case CSR_PORT4:
            csrmsg->rx_bytes =  read_port(4,1);
            csrmsg->tx_bytes =  read_port(4,0);
            reg_value = 11;
            break;
        case CSR_PORT5:
            csrmsg->rx_bytes =  read_port(5,1);
            csrmsg->tx_bytes =  read_port(5,0);
            reg_value = 11;
            break;
#endif
    }

    csrmsg->default_value = reg_value;
    err = rt2880_csr_msgsend(csrmsg, pid);
    if ( err == -2 )
        printk("drv: msg send error!\n");
}

int rt2880_csr_msgsend(CSR_MSG* csrmsg, int pid)
{
    struct sk_buff *skb;
    struct nlmsghdr *nlh = NULL;
    size_t size = 0;
    struct sock *send_syncnl = csr_msg_socket;

    CSR_MSG* csr_reg;
    if (send_syncnl == NULL)
    {
        printk("drv: netlink_kernel_create() failed!\n");
        return -1;
    }
    size = NLMSG_SPACE(sizeof(CSR_MSG));
    skb = alloc_skb(size, GFP_ATOMIC);
    if(!skb)
    {
        printk("rt2880_csr_msgsend() : error! msg structure not available\n");
        return -1;
    }
    nlh = nlmsg_put(skb, 0, 0, NLMSG_DONE, size - sizeof(struct nlmsghdr), 0);

    if (!nlh)
    {
        printk("rt2880_csr_msgsend() : error! nlh structure not available\n");
        return -1;
    }

    csr_reg = NLMSG_DATA(nlh);
    if (!csr_reg)
    {
        printk("rt2880_csr_msgsend() : error! nlh structure not available\n");
        return -1;
    }
    
    csr_reg->enable  = csrmsg->enable;
    csr_reg->address = csrmsg->address;
    csr_reg->default_value  = csrmsg->default_value;
#if defined CONFIG_RALINK_MT7620
    csr_reg->rx_bytes = csrmsg->rx_bytes;
    csr_reg->tx_bytes = csrmsg->tx_bytes;
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,21)
    NETLINK_CB(skb).dst_group = 0;
#else
    NETLINK_CB(skb).dst_groups = 0;
#endif
    nlmsg_unicast(send_syncnl, skb, pid);
    return 0;
}

int csr_netlink_init()
{
    struct netlink_kernel_cfg cfg = {
        .input = rt2880_csr_receiver,
    };
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,21)
    csr_msg_socket = netlink_kernel_create(&init_net, RALINK_NETLINK, &cfg);
#else
    csr_msg_socket = netlink_kernel_create(&init_net, RALINK_NETLINK, 0, rt2880_csr_receiver, NULL, THIS_MODULE);
#endif
    if ( csr_msg_socket == NULL)
        printk("unable to create netlink socket!\n");
    else
        printk("Netlink init ok!\n");
    return 0;
}

void csr_netlink_end()
{
    if (csr_msg_socket != NULL)
    {
        netlink_kernel_release(csr_msg_socket);
        printk("Netlink end...\n");
    }
}
