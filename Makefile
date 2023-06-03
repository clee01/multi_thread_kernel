all:
	nasm -f bin ./src/boot/boot.asm -o ./bin/boot.bin
	dd if=./message.txt >> ./boot.bin
	dd if=/dev/zero bs=512 count=1 >> ./boot.bin

clean:
	rm -rf ./bin/boot.bin