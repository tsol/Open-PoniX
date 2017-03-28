#include <linux/init.h>
#include <linux/module.h>

static int __init dummy_init(void)
{
	return -EINVAL;
}

module_init(dummy_init)
