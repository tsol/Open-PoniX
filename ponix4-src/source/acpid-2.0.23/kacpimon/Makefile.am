AM_CPPFLAGS = -include $(top_srcdir)/config.h
AM_CFLAGS =  -Wall -Wmissing-declarations -Wmissing-prototypes -Wredundant-decls -Wshadow -Wstrict-prototypes -Wformat=2
sbin_PROGRAMS = kacpimon
kacpimon_SOURCES = acpi_ids.c kacpimon.c connection_list.c input_layer.c libnetlink.c netlink.c \
				   acpi_genetlink.h acpi_ids.h connection_list.h genetlink.h input_layer.h kacpimon.h libnetlink.h netlink.h
dist_man8_MANS = kacpimon.8

EXTRA_DIST = sample.out

