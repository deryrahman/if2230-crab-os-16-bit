nasm bootload.asm -o bootload
dd if=bootload of=floppya.img bs=512 count=1 conv=notrunc
dd if=map.img of=floppya.img bs=512 count=1 seek=1 conv=notrunc
dd if=dir.img of=floppya.img bs=512 count=1 seek=2 conv=notrunc

as86 lib.asm -o lib_asm.o
bcc -ansi -c -o lib.o lib.c
bcc -ansi -c -o pcb.o pcb.c

as86 kernel.asm -o kernel_asm.o
bcc -ansi -c -o kernel.o kernel.c
ld86 -o kernel -d kernel.o kernel_asm.o pcb.o lib_asm.o lib.o

dd if=kernel of=floppya.img bs=512 conv=notrunc seek=3
dd if=message.txt of=floppya.img bs=512 count=1 seek=30 conv=notrunc

bcc -ansi -c -o shell.o shell.c
ld86 -o shell -d shell.o lib_asm.o

bcc -ansi -c -o hello.o hello.c
ld86 -o hello -d hello.o lib.o lib_asm.o

./loadFile hello
./loadFile shell
./loadFile message.txt
