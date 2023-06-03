all:
	nasm -f bin ./src/boot/boot.asm -o ./bin/boot.bin
	dd if=./src/boot/message.txt >> ./bin/boot.bin
	dd if=/dev/zero bs=512 count=1 >> ./bin/boot.bin

clean:
	rm -rf ./bin/boot.bin