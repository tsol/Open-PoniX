obj-$(CONFIG_WIL6210) += wil6210.o

wil6210-objs := main.o
wil6210-objs += netdev.o
wil6210-objs += cfg80211.o
wil6210-objs += pcie_bus.o
wil6210-objs += debugfs.o
wil6210-objs += wmi.o
wil6210-objs += interrupt.o
wil6210-objs += debug.o
wil6210-objs += txrx.o

#subdir-ccflags-y += -Werror

# Debug Tx/Rx flows
#subdir-ccflags-y += -DWIL_DEBUG_TXRX

# Use Clear-On-Read
subdir-ccflags-y += -DWIL6210_ISR_COR=1

