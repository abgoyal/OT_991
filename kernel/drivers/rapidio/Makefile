obj-y += rio.o rio-access.o rio-driver.o rio-scan.o rio-sysfs.o

obj-$(CONFIG_RAPIDIO)		+= switches/

ifeq ($(CONFIG_RAPIDIO_DEBUG),y)
EXTRA_CFLAGS += -DDEBUG
endif
