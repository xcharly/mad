#Compile with
#make ARCH=riscv CROSS_COMPILE=~/noel-buildroot/output/host/bin/riscv64-gaisler-linux-gnu- KERNEL_DIR=~/noel-buildroot/output/build/linux-5.7.19/

## Definitions for compiling the kernel module
MODULE_NAME = mad

SRC := src/mad.c
TST_SRC := src/mad_ex.c

$(MODULE_NAME)-objs = $(SRC:.c=.o)

obj-m += $(MODULE_NAME).o
PWD := $(shell pwd)

ARCH ?= x86_64
KERNEL_DIR ?= /lib/modules/$(shell uname -r)/build

EXTRA_CFLAGS := -I$(PWD)/src

## Definition for compiling tests
CC=gcc
TST_SRC = src/mad_ex.c
TST_OBJ = $(patsubst %.c,%.o, $(TST_SRC))
TST_CFLAGS := -I$(PWD)/src

all: drv mad_test

clean: drv_clean mad_clean

drv:
	$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNEL_DIR) M=$(PWD) modules

drv_clean:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) clean

%o: %.c
	$(CC) $(TST_CFLAGS) -c $<

mad_test: $(TST_OBJ)	
	$(CC) $(TST_CFLAGS) $^ -o $@

mad_clean:
	-rm -f $(TST_OBJ)
	-rm -f mad_test


