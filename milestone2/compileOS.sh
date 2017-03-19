dd if=bootload of=floppya.img bs=512 count=1 conv=notrunc
dd if=map.img of=floppya.img bs=512 count=1 seek=1 conv=notrunc
dd if=dir.img of=floppya.img bs=512 count=1 seek=2 conv=notrunc
#bcc -ansi -c -o kernel.o kernel.c
#as86 kernel.asm -o kernel_asm.o
#ld86 -o kernel -d kernel.o kernel_asm.o

bcc -ansi -c -o p3_kernel.o p3_kernel.c
as86 -o kernel_asm.o kernel.asm
ld86 -o kernel -d p3_kernel.o kernel_asm.o

dd if=kernel of=floppya.img bs=512 conv=notrunc seek=3
dd if=message.txt of=floppya.img bs=512 count=1 seek=30 conv=notrunc

bcc -ansi -c -o shell.o shell.c
as86 lib.asm -o lib_asm.o
ld86 -o shell -d shell.o lib_asm.o

as86 lib.asm -o lib.o
bcc -ansi -c -o uprog1.o uprog1.c
ld86 -o uprog1 -d uprog1.o lib.o

./loadFile uprog1
./loadFile shell
./loadFile message.txt
