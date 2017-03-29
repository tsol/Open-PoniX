cmd_net/wireless/cfg80211.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o net/wireless/cfg80211.ko net/wireless/cfg80211.o net/wireless/cfg80211.mod.o
