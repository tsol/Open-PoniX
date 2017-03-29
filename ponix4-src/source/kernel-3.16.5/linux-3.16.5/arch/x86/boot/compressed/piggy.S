.section ".rodata..compressed","a",@progbits
.globl z_input_len
z_input_len = 3570424
.globl z_output_len
z_output_len = 10231060
.globl z_extract_offset
z_extract_offset = 0x66b000
.globl z_extract_offset_negative
z_extract_offset_negative = -0x66b000
.globl input_data, input_data_end
input_data:
.incbin "arch/x86/boot/compressed/vmlinux.bin.xz"
input_data_end:
