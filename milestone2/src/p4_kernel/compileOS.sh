nasm bootload.asm -o bootload
dd if=/dev/zero of=floppya.img bs=512 count=2880
dd if=bootload of=floppya.img bs=512 count=1 conv=notrunc
dd if=map.img of=floppya.img bs=512 count=1 seek=1 conv=notrunc
dd if=dir.img of=floppya.img bs=512 count=1 seek=2 conv=notrunc
dd if=message.txt of=floppya.img bs=512 count=1 seek=30 conv=notrunc
as86 -o kernel_asm.o kernel.asm
bcc -ansi -c -o shell.o shell.c
as86 lib.asm -o lib_asm.o
ld86 -o shell -d shell.o lib_asm.o
gcc -o loadFile loadFile.c
./loadFile shell
./loadFile message.txt

# kernel default
#bcc -ansi -c -o kernel.o kernel.c
#ld86 -o kernel -d kernel.o kernel_asm.o
#dd if=kernel of=floppya.img bs=512 conv=notrunc seek=3

# p4
bcc -ansi -c -o p4_kernel.o p4_kernel.c
ld86 -o p4_kernel -d p4_kernel.o kernel_asm.o
dd if=p4_kernel of=floppya.img bs=512 conv=notrunc seek=3