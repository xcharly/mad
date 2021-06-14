#Compile with
#make ARCH=riscv CROSS_COMPILE=~/noel-buildroot/output/host/bin/riscv64-gaisler-linux-gnu- KERNEL_DIR=~/noel-buildroot/output/build/linux-5.7.19/

MODULE_NAME = mad

SRC := src/mad.c
TST_SRC := src/mad_ex.c

$(MODULE_NAME)-objs = $(SRC:.c=.o)

obj-m += $(MODULE_NAME).o
PWD := $(shell pwd)

ARCH ?= x86_64
KERNEL_DIR ?= /lib/modules/$(shell uname -r)/build

EXTRA_CFLAGS := -I$(PWD)/src

all:
	$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNEL_DIR) M=$(PWD) modules

test:
	$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE)

clean:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) clean
