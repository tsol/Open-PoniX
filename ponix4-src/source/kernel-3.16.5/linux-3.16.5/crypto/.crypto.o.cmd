cmd_crypto/crypto.o := ld -m elf_i386   -r -o crypto/crypto.o crypto/api.o crypto/cipher.o crypto/compress.o crypto/memneq.o 
