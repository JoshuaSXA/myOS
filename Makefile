CFLAGS=-nostdlib -nostdinc -fno-builtin -fno-stack-protector -m32
ASFLAGS=--32

OBJS=./src/boot.o ./src/common-s.o ./src/main.o ./src/common.o ./src/gdt-idt-s.o ./src/gdt-idt.o ./src/isr.o ./src/monitor.o ./src/timer.o ./src/kheap.o ./src/paging-s.o ./src/paging.o ./src/ordered-array.o ./src/fs.o ./src/initrd.o ./src/process.o ./src/task.o ./src/syscall.o ./src/syscall-s.o ./src/ds.o ./src/keyboard.o ./src/shell.o
 
all: $(OBJS)
	ld -T ./src/link.ld -m elf_i386 -o ./src/kernel $(OBJS)
./src/boot.o: ./src/boot.s
	as -o ./src/boot.o ./src/boot.s $(ASFLAGS)
./src/main.o: ./src/main.c
	gcc -c ./src/main.c $(CFLAGS) -o ./src/main.o
./src/common-s.o: ./src/common.s
	as -o ./src/common-s.o ./src/common.s $(ASFLAGS)
./src/common.o: ./src/common.c
	gcc -c ./src/common.c $(CFLAGS) -o ./src/common.o
./src/gdt-idt-s.o: ./src/gdt-idt.s
	as -o ./src/gdt-idt-s.o ./src/gdt-idt.s $(ASFLAGS)
./src/gdt-idt.o: ./src/gdt-idt.c
	gcc -c ./src/gdt-idt.c $(CFLAGS) -o ./src/gdt-idt.o
./src/isr.o: ./src/isr.c
	gcc -c ./src/isr.c $(CFLAGS) -o ./src/isr.o
./src/keyboard.o: ./src/keyboard.c
	gcc -c ./src/keyboard.c $(CFLAGS) -o ./src/keyboard.o
./src/monitor.o: ./src/monitor.c
	gcc -c ./src/monitor.c $(CFLAGS) -o ./src/monitor.o
./src/timer.o: ./src/timer.c
	gcc -c ./src/timer.c $(CFLAGS) -o ./src/timer.o
./src/kheap.o: ./src/kheap.c
	gcc -c ./src/kheap.c $(CFLAGS) -o ./src/kheap.o
./src/paging-s.o: ./src/paging.s
	as -o ./src/paging-s.o ./src/paging.s $(ASFLAGS)
./src/paging.o: ./src/paging.c
	gcc -c ./src/paging.c $(CFLAGS) -o ./src/paging.o
./src/ordered-array.o: ./src/ordered-array.c
	gcc -c ./src/ordered-array.c $(CFLAGS) -o ./src/ordered-array.o
./src/ds.o: ./src/ds.c
	gcc -c ./src/ds.c $(CFLAGS) -o ./src/ds.o
./src/fs.o: ./src/fs.c
	gcc -c ./src/fs.c $(CFLAGS) -o ./src/fs.o
./src/initrd.o: ./src/initrd.c
	gcc -c ./src/initrd.c $(CFLAGS) -o ./src/initrd.o
./src/process.o: ./src/process.s
	as -o ./src/process.o ./src/process.s $(ASFLAGS)
./src/task.o: ./src/task.c
	gcc -c ./src/task.c $(CFLAGS) -o ./src/task.o
./src/syscall.o: ./src/syscall.c
	gcc -c ./src/syscall.c $(CFLAGS) -o ./src/syscall.o
./src/syscall-s.o: ./src/syscall.s
	as -o ./src/syscall-s.o ./src/syscall.s $(ASFLAGS)
./src/shell.o: ./src/shell.c
	gcc -c ./src/shell.c $(CFLAGS) -o ./src/shell.o

clean:
	rm ./src/*.o ./src/kernel 

clean-initrd:
	rm ./initrd/mk-initrd ./image/initrd.img

mk-initrd:
	gcc -o ./initrd/mk-initrd ./initrd/mk-initrd.c
	./initrd/mk-initrd ./initrd/file1.txt ./initrd/file1.txt ./initrd/file2.txt ./initrd/file2.txt

prepare:
	sudo losetup -d /dev/loop0

update:
	sudo losetup /dev/loop0 ./image/CocOS.img
	sudo mount /dev/loop0 /mnt
	sudo cp ./src/kernel /mnt/kernel
	sudo cp ./image/initrd.img /mnt/initrd
	sudo umount /dev/loop0
	sudo losetup -d /dev/loop0

run:
	sudo losetup /dev/loop0 ./image/CocOS.img
	sudo bochs -f ./configure/bochsrc
	sudo losetup -d /dev/loop0
