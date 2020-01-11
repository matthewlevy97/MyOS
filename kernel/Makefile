CC=i686-elf-gcc
AS=i686-elf-as

KERNEL_NAME=kernel

KERNEL_HDA=hda.img

KERNEL_INCLUDE_DIR=includes/kernel
LIBC_INCLUDE_DIR=includes/libc
LINKER_SCRIPT=linker.ld
SYS_ROOT=sysroot
KERNEL_BIN=$(KERNEL_NAME).bin
KERNEL_ISO=$(KERNEL_NAME).iso
MULTIBOOT_CMDLINE="serial"

CFLAGS:=$(CFLAGS) --sysroot=$(SYS_ROOT) -isystem=/usr/include -I$(KERNEL_INCLUDE_DIR) -I$(LIBC_INCLUDE_DIR)
CFLAGS:=$(CFLAGS) -O2 -g -std=gnu11 -ffreestanding -Wall -Wextra -nostdlib
CFLAGS:=$(CFLAGS) -D__KERNEL_CODE
LDFLAGS:=$(LDFLAGS)
LIBS:=$(LIBS)

include src/make.config

OBJS=\
$(KERNEL_ARCH_CRT_I_OBJS)\
$(KERNEL_OBJS)\
$(LIBC_OBJS)\
$(KERNEL_ARCH_CRT_N_OBJS)

LINK_LIST=\
$(LDFLAGS)\
$(KERNEL_ARCH_CRT_I_OBJS)\
$(KERNEL_OBJS)\
$(LIBC_OBJS)\
$(LIBS)\
$(KERNEL_ARCH_CRT_N_OBJS)

.PHONY: all clean install install_headers install_kernel build run debug
.SUFFIXES: .o .c .S

all: $(KERNEL_BIN)

$(KERNEL_BIN): $(SYS_ROOT) $(OBJS) $(LINKER_SCRIPT)
	$(CC) -T $(LINKER_SCRIPT) -o $@ $(CFLAGS) $(LINK_LIST)

# Probably a better way to resolve these, but this works so...
src/kernel/i686/crtbegin.o src/kernel/i686/crtend.o:
	OBJ=`$(CC) $(CFLAGS) $(LDFLAGS) -print-file-name=$(@F)` && cp "$$OBJ" $@

.c.o:
	$(CC) -MD -c $< -o $@ $(CFLAGS)

.S.o:
	$(CC) -MD -c $< -o $@ $(CFLAGS)

$(SYS_ROOT): install_headers

run: install
	qemu-system-i386 -cdrom $(KERNEL_ISO) \
	-hda $(KERNEL_HDA) \
	-device ich9-ahci \
	-m 128 \
	-serial stdio \
	-no-reboot

debug: install
	qemu-system-i386 -cdrom $(KERNEL_ISO) \
	-hda $(KERNEL_HDA) \
	-device ich9-ahci \
	-m 128 \
	-serial stdio \
	-s -S \
	-d int,cpu_reset \
	-no-reboot -no-shutdown

install: install_headers install_kernel build $(KERNEL_HDA)

install_headers:
	mkdir -p $(SYS_ROOT)/usr/lib
	mkdir -p $(SYS_ROOT)/usr/include/kernel
	mkdir -p $(SYS_ROOT)/usr/include/libc
	cp -R --preserve=timestamps $(KERNEL_INCLUDE_DIR)/. $(SYS_ROOT)/usr/include/kernel
	cp -R --preserve=timestamps $(LIBC_INCLUDE_DIR)/. $(SYS_ROOT)/usr/include/libc

install_kernel: $(KERNEL_BIN)
	mkdir -p $(SYS_ROOT)/boot/grub
	cp --preserve=timestamps $(KERNEL_BIN) $(SYS_ROOT)/boot
	@echo 'set timeout=0'                     > $(SYS_ROOT)/boot/grub/grub.cfg
	@echo 'set default=0'                     >> $(SYS_ROOT)/boot/grub/grub.cfg
	@echo "menuentry '$(KERNEL_NAME)' {"      >> $(SYS_ROOT)/boot/grub/grub.cfg
	@echo "\tmultiboot2 /boot/$(KERNEL_BIN) $(MULTIBOOT_CMDLINE)" >> $(SYS_ROOT)/boot/grub/grub.cfg
	@echo '\tboot'                            >> $(SYS_ROOT)/boot/grub/grub.cfg
	@echo '}'                                 >> $(SYS_ROOT)/boot/grub/grub.cfg

$(KERNEL_HDA):
	dd if=/dev/zero of=$(KERNEL_HDA) bs=1024 count=10240
	echo 'y' | mkfs.ext2 -q -I 128 $(KERNEL_HDA)

build: $(SYS_ROOT)
	@grub-mkrescue -o $(KERNEL_ISO) $(SYS_ROOT)

clean:
	rm -f $(KERNEL_BIN) $(KERNEL_ISO) $(KERNEL_HDA)
	rm -rf $(SYS_ROOT)
	rm -f $(OBJS)
	rm -f $(OBJS:.o=.d)
