/*
 * Copyright 2012    Luis R. Rodriguez <mcgrof@do-not-panic.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Compatibility file for Linux wireless for kernels 2.6.34.
 */

#include <linux/mmc/sdio_func.h>

#include "compat-2.6.34.h"

static mmc_pm_flag_t compat_mmc_pm_flags;

void init_compat_mmc_pm_flags(void)
{
	compat_mmc_pm_flags = 0;
}

mmc_pm_flag_t sdio_get_host_pm_caps(struct sdio_func *func)
{
	return compat_mmc_pm_flags;
}

int sdio_set_host_pm_flags(struct sdio_func *func, mmc_pm_flag_t flags)
{
	return -EINVAL;
}
