cmd_sound/core/seq/snd-seq.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o sound/core/seq/snd-seq.ko sound/core/seq/snd-seq.o sound/core/seq/snd-seq.mod.o
