/*
 * Today's hack: quantum tunneling in structs
 *
 * 'entries' and 'term' are never anywhere referenced by word in code. In fact,
 * they serve as the hanging-off data accessed through repl.data[].
 */

// define struct for ipt_standard ipt_replace ipt_error struct
#define xt_alloc_initial_table(type, typ2) ({ \
    //get hook mask for get hook entry
	unsigned int hook_mask = info->valid_hooks; \

    //计算有效的标示位的个数
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
