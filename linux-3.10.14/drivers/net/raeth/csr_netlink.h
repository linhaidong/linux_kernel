#ifndef	CSR_NETLINK_H
#define CSR_NETLINK_H

#define	CSR_NETLINK	30
#define	CSR_READ	0
#define	CSR_WRITE	1
#define	CSR_TEST	2
#define CSR_PORT0   3
#define CSR_PORT1   4
#define CSR_PORT2   5
#define CSR_PORT3   6
#define CSR_PORT4   7
#define CSR_PORT5   8
#define RALINK_CSR_GROUP	 2882	
#define RALINK_NETLINK       29
typedef struct rt2880_csr_msg {
  	int	enable;
  	unsigned long address;
  	unsigned long default_value;
  	unsigned long rx_bytes;
  	unsigned long tx_bytes;
  	int	status;
} CSR_MSG;

int csr_msg_send(CSR_MSG* msg);
int csr_msg_recv(void);

// static CSR_MSG	input_csr_msg;

#endif
