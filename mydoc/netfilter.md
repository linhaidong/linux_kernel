
hook点:

```c
enum nf_inet_hooks {
	NF_INET_PRE_ROUTING,
	NF_INET_LOCAL_IN,
	NF_INET_FORWARD,
	NF_INET_LOCAL_OUT,
	NF_INET_POST_ROUTING,
	NF_INET_NUMHOOKS
};
```

```c
include/netfilter/x_tables.h
/* Furniture shopping... */
struct xt_table {
	struct list_head list;
	/* What hooks you will enter on */
	unsigned int valid_hooks;

	/* Man behind the curtain... */
	struct xt_table_info *private;

	/* Set this to THIS_MODULE if you are a module, otherwise NULL */
	struct module *me;

	u_int8_t af;		/* address/protocol family */
	int priority;		/* hook order */

	/* A unique name... */
	const char name[XT_TABLE_MAXNAMELEN];
};


/* The table itself */
struct xt_table_info {
	/* Size per table */
	unsigned int size;
	/* Number of entries: FIXME. --RR */
    //number 为hook点的数目
	unsigned int number;
	/* Initial number of entries. Needed for module usage count */
	unsigned int initial_entries;

	/* Entry points and underflows */
	unsigned int hook_entry[NF_INET_NUMHOOKS];
	unsigned int underflow[NF_INET_NUMHOOKS];

	/*
	 * Number of user chains. Since tables cannot have loops, at most
	 * @stacksize jumps (number of user chains) can possibly be made.
	 */
	unsigned int stacksize;
	unsigned int __percpu *stackptr;
	void ***jumpstack;
	/* ipt_entry tables: one per CPU */
	/* Note : this field MUST be the last one, see XT_TABLE_INFO_SZ */
	void *entries[1];
};
```
注册表在xt_register_table函数(net/netfilter/x_tables.c)中。



每一个table可用的hook标示，用xt_table的vaild_hooks成员标示,每一位标示一个hook点。
在此函数中会分配一个tbl结构体，每一个hook标志，都有一个ipt_standard entry
表示。entries的数量是，mask的有效标示位，会根据mask的有效标示位，从小到大初始化ipt__standard.

例如，在fliter表中使用了下面的标示，
```c
#define FILTER_VALID_HOOKS ((1 << NF_INET_LOCAL_IN) | \
			    (1 << NF_INET_FORWARD) | \
			    (1 << NF_INET_LOCAL_OUT))
```
则会初始化三个ipt_standard entry,下标分别为012。
```c
// define struct for ipt_standard ipt_replace ipt_error struct
#define xt_alloc_initial_table(type, typ2) ({ \
    //get hook mask for get hook entry
	unsigned int hook_mask = info->valid_hooks; \
	unsigned int nhooks = hweight32(hook_mask); \
	unsigned int bytes = 0, hooknum = 0, i = 0; \
	struct { \
		struct type##_replace repl; \
        //ipt_standard entries
		struct type##_standard entries[nhooks]; \
		struct type##_error term; \
	} *tbl = kzalloc(sizeof(*tbl), GFP_KERNEL); \
	if (tbl == NULL) \
		return NULL; \
    //info is xt_table, xt_table has xt_table_info member
	strncpy(tbl->repl.name, info->name, sizeof(tbl->repl.name)); \
	tbl->term = (struct type##_error)typ2##_ERROR_INIT;  \
	tbl->repl.valid_hooks = hook_mask; \
	tbl->repl.num_entries = nhooks + 1; \
	tbl->repl.size = nhooks * sizeof(struct type##_standard) + \
	                 sizeof(struct type##_error); \
    //every mask bytes has ipt_standard entery
    //init entries by every mask
    //按照mask的标示位，初始化entry数组
	for (; hook_mask != 0; hook_mask >>= 1, ++hooknum) { \
		if (!(hook_mask & 1)) \
			continue; \
		tbl->repl.hook_entry[hooknum] = bytes; \
		tbl->repl.underflow[hooknum]  = bytes; \
		tbl->entries[i++] = (struct type##_standard) \
			typ2##_STANDARD_INIT(NF_ACCEPT); \
		bytes += sizeof(struct type##_standard); \
	} \
	tbl; \
})
```



标准处理表
```
/* Standard entry. */
struct ipt_standard {
	struct ipt_entry entry;
	struct xt_standard_target target;
};

/* This structure defines each of the firewall rules.  Consists of 3
   parts which are 1) general IP header stuff 2) match specific
   stuff 3) the target to perform if the rule matches */
struct ipt_entry {
	struct ipt_ip ip;

	/* Mark with fields that we care about. */
	unsigned int nfcache;

	/* Size of ipt_entry + matches */
	__u16 target_offset;
	/* Size of ipt_entry + matches + target */
	__u16 next_offset;

	/* Back pointer */
	unsigned int comefrom;

	/* Packet and byte counters. */
	struct xt_counters counters;

	/* The matches (if any), then the target. */
	unsigned char elems[0];
};
struct xt_standard_target {
	struct xt_entry_target target;
	int verdict;
};

struct xt_entry_target {
	union {
		struct {
			__u16 target_size;

			/* Used by userspace */
			char name[XT_EXTENSION_MAXNAMELEN];
			__u8 revision;
		} user;
		struct {
			__u16 target_size;

			/* Used inside the kernel */
			struct xt_target *target;
		} kernel;

		/* Total length */
		__u16 target_size;
	} u;

	unsigned char data[0];
};
struct ipt_error {
	struct ipt_entry entry;
	struct xt_error_target target;
};
```
ipt_entry 用于标示每一个防火墙的配置规则。
/* This structure defines each of the firewall rules.  Consists of 3
   parts which are 1) general IP header stuff 2) match specific
   stuff 3) the target to perform if the rule matches */
struct ipt_entry {
	struct ipt_ip ip;

	/* Mark with fields that we care about. */
	unsigned int nfcache;

	/* Size of ipt_entry + matches */
	__u16 target_offset;
	/* Size of ipt_entry + matches + target */
	__u16 next_offset;

	/* Back pointer */
	unsigned int comefrom;

	/* Packet and byte counters. */
	struct xt_counters counters;

	/* The matches (if any), then the target. */
	unsigned char elems[0];
};


namespace define
include/net/net_namespace.h定义了net结构体表示一个network namespace。
其中和netfiler相关的结构成员有
```
struct netns_ipv4	ipv4;
struct netns_nf		nf;
struct netns_xt		xt;
```
netns_ipv4中定义了相关的xt_table:
	struct xt_table		*iptable_filter;
	struct xt_table		*iptable_mangle;
	struct xt_table		*iptable_raw;
	struct xt_table		*arptable_filter;

```
struct netns_xt {
	struct list_head tables[NFPROTO_NUMPROTO];
	bool notrack_deprecated_warning;
#if defined(CONFIG_BRIDGE_NF_EBTABLES) || \
    defined(CONFIG_BRIDGE_NF_EBTABLES_MODULE)
	struct ebt_table *broute_table;
	struct ebt_table *frame_filter;
	struct ebt_table *frame_nat;
#endif
};
#endif
```




## question
1. audit
#ifdef CONFIG_AUDIT
	if (audit_enabled) {
		struct audit_buffer *ab;

		ab = audit_log_start(current->audit_context, GFP_KERNEL,
				     AUDIT_NETFILTER_CFG);
		if (ab) {
			audit_log_format(ab, "table=%s family=%u entries=%u",
					 table->name, table->af,
					 private->number);
			audit_log_end(ab);
		}
	}

2. cpu
多个cpu，内核程序如何运行在多个cpu上
