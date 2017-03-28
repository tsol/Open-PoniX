#ifndef __DMI_COMPAT_H
#define __DMI_COMPAT_H
/* dummy dmi handlers */

#include <linux/list.h>

enum dmi_field {
	DMI_NONE,
	DMI_BIOS_VENDOR,
	DMI_BIOS_VERSION,
	DMI_BIOS_DATE,
	DMI_SYS_VENDOR,
	DMI_PRODUCT_NAME,
	DMI_PRODUCT_VERSION,
	DMI_PRODUCT_SERIAL,
	DMI_PRODUCT_UUID,
	DMI_BOARD_VENDOR,
	DMI_BOARD_NAME,
	DMI_BOARD_VERSION,
	DMI_BOARD_SERIAL,
	DMI_BOARD_ASSET_TAG,
	DMI_CHASSIS_VENDOR,
	DMI_CHASSIS_TYPE,
	DMI_CHASSIS_VERSION,
	DMI_CHASSIS_SERIAL,
	DMI_CHASSIS_ASSET_TAG,
	DMI_STRING_MAX,
};

enum dmi_device_type {
	DMI_DEV_TYPE_ANY = 0,
	DMI_DEV_TYPE_OTHER,
	DMI_DEV_TYPE_UNKNOWN,
	DMI_DEV_TYPE_VIDEO,
	DMI_DEV_TYPE_SCSI,
	DMI_DEV_TYPE_ETHERNET,
	DMI_DEV_TYPE_TOKENRING,
	DMI_DEV_TYPE_SOUND,
	DMI_DEV_TYPE_PATA,
	DMI_DEV_TYPE_SATA,
	DMI_DEV_TYPE_SAS,
	DMI_DEV_TYPE_IPMI = -1,
	DMI_DEV_TYPE_OEM_STRING = -2,
};

struct dmi_header {
	u8 type;
	u8 length;
	u16 handle;
};

struct dmi_device {
	struct list_head list;
	int type;
	const char *name;
	void *device_data;	/* Type specific data */
};

struct dmi_system_id;

static inline int dmi_check_system(const struct dmi_system_id *list) { return 0; }
static inline const char * dmi_get_system_info(int field) { return NULL; }
static inline const struct dmi_device * dmi_find_device(int type, const char *name,
	const struct dmi_device *from) { return NULL; }
static inline void dmi_scan_machine(void) { return; }
static inline bool dmi_get_date(int field, int *yearp, int *monthp, int *dayp)
{
	if (yearp)
		*yearp = 0;
	if (monthp)
		*monthp = 0;
	if (dayp)
		*dayp = 0;
	return false;
}
static inline int dmi_name_in_vendors(const char *s) { return 0; }
static inline int dmi_name_in_serial(const char *s) { return 0; }
#define dmi_available 0
static inline int dmi_walk(void (*decode)(const struct dmi_header *, void *),
	void *private_data) { return -1; }
static inline bool dmi_match(enum dmi_field f, const char *str)
	{ return false; }
static inline const struct dmi_system_id *
	dmi_first_match(const struct dmi_system_id *list) { return NULL; }

#endif /* __DMI_COMPAT_H */
