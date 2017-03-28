/*
 * include autoconf.h/config.h from proper location
 */
#if defined(CONFIG_HAVE_GENERATED_AUTOCONF_H)
#include <generated/autoconf.h>
#elif defined(CONFIG_HAVE_DEPRECATED_CONFIG_H)
#include <linux/autoconf.h>
#else
#include <linux/config.h>
#endif
#include <linux/version.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 33)
#include <generated/utsrelease.h>
#else
#include <linux/utsrelease.h>
#endif
#include "config-top.h"
