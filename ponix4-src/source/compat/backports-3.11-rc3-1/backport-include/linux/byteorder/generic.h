#ifndef __BACKPORT_BYTEORDER_GENERIC_H
#define __BACKPORT_BYTEORDER_GENERIC_H
#include_next <linux/byteorder/generic.h>
#include <linux/version.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,25)
/* The patch:
 * commit 8b5f6883683c91ad7e1af32b7ceeb604d68e2865
 * Author: Marcin Slusarz <marcin.slusarz@gmail.com>
 * Date:   Fri Feb 8 04:20:12 2008 -0800
 *
 *     byteorder: move le32_add_cpu & friends from OCFS2 to core
 *
 * moves le*_add_cpu and be*_add_cpu functions from OCFS2 to core
 * header (1st) and converted some existing code to it. We port
 * it here as later kernels will most likely use it.
 */
static inline void le16_add_cpu(__le16 *var, u16 val)
{
	*var = cpu_to_le16(le16_to_cpu(*var) + val);
}

static inline void le32_add_cpu(__le32 *var, u32 val)
{
	*var = cpu_to_le32(le32_to_cpu(*var) + val);
}

static inline void le64_add_cpu(__le64 *var, u64 val)
{
	*var = cpu_to_le64(le64_to_cpu(*var) + val);
}

static inline void be16_add_cpu(__be16 *var, u16 val)
{
	u16 v = be16_to_cpu(*var);
	*var = cpu_to_be16(v + val);
}

static inline void be32_add_cpu(__be32 *var, u32 val)
{
	u32 v = be32_to_cpu(*var);
	*var = cpu_to_be32(v + val);
}

static inline void be64_add_cpu(__be64 *var, u64 val)
{
	u64 v = be64_to_cpu(*var);
	*var = cpu_to_be64(v + val);
}
#endif

#endif /* __BACKPORT_BYTEORDER_GENERIC_H */
