static const char x86_cap_strs[] =
#if REQUIRED_MASK0 & (1 << 0)
	"\x00\x00""fpu\0"
#endif
#if REQUIRED_MASK0 & (1 << 1)
	"\x00\x01""vme\0"
#endif
#if REQUIRED_MASK0 & (1 << 2)
	"\x00\x02""de\0"
#endif
#if REQUIRED_MASK0 & (1 << 3)
	"\x00\x03""pse\0"
#endif
#if REQUIRED_MASK0 & (1 << 4)
	"\x00\x04""tsc\0"
#endif
#if REQUIRED_MASK0 & (1 << 5)
	"\x00\x05""msr\0"
#endif
#if REQUIRED_MASK0 & (1 << 6)
	"\x00\x06""pae\0"
#endif
#if REQUIRED_MASK0 & (1 << 7)
	"\x00\x07""mce\0"
#endif
#if REQUIRED_MASK0 & (1 << 8)
	"\x00\x08""cx8\0"
#endif
#if REQUIRED_MASK0 & (1 << 9)
	"\x00\x09""apic\0"
#endif
#if REQUIRED_MASK0 & (1 << 11)
	"\x00\x0b""sep\0"
#endif
#if REQUIRED_MASK0 & (1 << 12)
	"\x00\x0c""mtrr\0"
#endif
#if REQUIRED_MASK0 & (1 << 13)
	"\x00\x0d""pge\0"
#endif
#if REQUIRED_MASK0 & (1 << 14)
	"\x00\x0e""mca\0"
#endif
#if REQUIRED_MASK0 & (1 << 15)
	"\x00\x0f""cmov\0"
#endif
#if REQUIRED_MASK0 & (1 << 16)
	"\x00\x10""pat\0"
#endif
#if REQUIRED_MASK0 & (1 << 17)
	"\x00\x11""pse36\0"
#endif
#if REQUIRED_MASK0 & (1 << 18)
	"\x00\x12""pn\0"
#endif
#if REQUIRED_MASK0 & (1 << 19)
	"\x00\x13""clflush\0"
#endif
#if REQUIRED_MASK0 & (1 << 21)
	"\x00\x15""dts\0"
#endif
#if REQUIRED_MASK0 & (1 << 22)
	"\x00\x16""acpi\0"
#endif
#if REQUIRED_MASK0 & (1 << 23)
	"\x00\x17""mmx\0"
#endif
#if REQUIRED_MASK0 & (1 << 24)
	"\x00\x18""fxsr\0"
#endif
#if REQUIRED_MASK0 & (1 << 25)
	"\x00\x19""sse\0"
#endif
#if REQUIRED_MASK0 & (1 << 26)
	"\x00\x1a""sse2\0"
#endif
#if REQUIRED_MASK0 & (1 << 27)
	"\x00\x1b""ss\0"
#endif
#if REQUIRED_MASK0 & (1 << 28)
	"\x00\x1c""ht\0"
#endif
#if REQUIRED_MASK0 & (1 << 29)
	"\x00\x1d""tm\0"
#endif
#if REQUIRED_MASK0 & (1 << 30)
	"\x00\x1e""ia64\0"
#endif
#if REQUIRED_MASK0 & (1 << 31)
	"\x00\x1f""pbe\0"
#endif
#if REQUIRED_MASK1 & (1 << 11)
	"\x01\x0b""syscall\0"
#endif
#if REQUIRED_MASK1 & (1 << 19)
	"\x01\x13""mp\0"
#endif
#if REQUIRED_MASK1 & (1 << 20)
	"\x01\x14""nx\0"
#endif
#if REQUIRED_MASK1 & (1 << 22)
	"\x01\x16""mmxext\0"
#endif
#if REQUIRED_MASK1 & (1 << 25)
	"\x01\x19""fxsr_opt\0"
#endif
#if REQUIRED_MASK1 & (1 << 26)
	"\x01\x1a""pdpe1gb\0"
#endif
#if REQUIRED_MASK1 & (1 << 27)
	"\x01\x1b""rdtscp\0"
#endif
#if REQUIRED_MASK1 & (1 << 29)
	"\x01\x1d""lm\0"
#endif
#if REQUIRED_MASK1 & (1 << 30)
	"\x01\x1e""3dnowext\0"
#endif
#if REQUIRED_MASK1 & (1 << 31)
	"\x01\x1f""3dnow\0"
#endif
#if REQUIRED_MASK2 & (1 << 0)
	"\x02\x00""recovery\0"
#endif
#if REQUIRED_MASK2 & (1 << 1)
	"\x02\x01""longrun\0"
#endif
#if REQUIRED_MASK2 & (1 << 3)
	"\x02\x03""lrti\0"
#endif
#if REQUIRED_MASK3 & (1 << 0)
	"\x03\x00""cxmmx\0"
#endif
#if REQUIRED_MASK3 & (1 << 1)
	"\x03\x01""k6_mtrr\0"
#endif
#if REQUIRED_MASK3 & (1 << 2)
	"\x03\x02""cyrix_arr\0"
#endif
#if REQUIRED_MASK3 & (1 << 3)
	"\x03\x03""centaur_mcr\0"
#endif
#if REQUIRED_MASK3 & (1 << 8)
	"\x03\x08""constant_tsc\0"
#endif
#if REQUIRED_MASK3 & (1 << 9)
	"\x03\x09""up\0"
#endif
#if REQUIRED_MASK3 & (1 << 11)
	"\x03\x0b""arch_perfmon\0"
#endif
#if REQUIRED_MASK3 & (1 << 12)
	"\x03\x0c""pebs\0"
#endif
#if REQUIRED_MASK3 & (1 << 13)
	"\x03\x0d""bts\0"
#endif
#if REQUIRED_MASK3 & (1 << 16)
	"\x03\x10""rep_good\0"
#endif
#if REQUIRED_MASK3 & (1 << 20)
	"\x03\x14""nopl\0"
#endif
#if REQUIRED_MASK3 & (1 << 22)
	"\x03\x16""xtopology\0"
#endif
#if REQUIRED_MASK3 & (1 << 23)
	"\x03\x17""tsc_reliable\0"
#endif
#if REQUIRED_MASK3 & (1 << 24)
	"\x03\x18""nonstop_tsc\0"
#endif
#if REQUIRED_MASK3 & (1 << 26)
	"\x03\x1a""extd_apicid\0"
#endif
#if REQUIRED_MASK3 & (1 << 27)
	"\x03\x1b""amd_dcm\0"
#endif
#if REQUIRED_MASK3 & (1 << 28)
	"\x03\x1c""aperfmperf\0"
#endif
#if REQUIRED_MASK3 & (1 << 29)
	"\x03\x1d""eagerfpu\0"
#endif
#if REQUIRED_MASK3 & (1 << 30)
	"\x03\x1e""nonstop_tsc_s3\0"
#endif
#if REQUIRED_MASK4 & (1 << 0)
	"\x04\x00""pni\0"
#endif
#if REQUIRED_MASK4 & (1 << 1)
	"\x04\x01""pclmulqdq\0"
#endif
#if REQUIRED_MASK4 & (1 << 2)
	"\x04\x02""dtes64\0"
#endif
#if REQUIRED_MASK4 & (1 << 3)
	"\x04\x03""monitor\0"
#endif
#if REQUIRED_MASK4 & (1 << 4)
	"\x04\x04""ds_cpl\0"
#endif
#if REQUIRED_MASK4 & (1 << 5)
	"\x04\x05""vmx\0"
#endif
#if REQUIRED_MASK4 & (1 << 6)
	"\x04\x06""smx\0"
#endif
#if REQUIRED_MASK4 & (1 << 7)
	"\x04\x07""est\0"
#endif
#if REQUIRED_MASK4 & (1 << 8)
	"\x04\x08""tm2\0"
#endif
#if REQUIRED_MASK4 & (1 << 9)
	"\x04\x09""ssse3\0"
#endif
#if REQUIRED_MASK4 & (1 << 10)
	"\x04\x0a""cid\0"
#endif
#if REQUIRED_MASK4 & (1 << 12)
	"\x04\x0c""fma\0"
#endif
#if REQUIRED_MASK4 & (1 << 13)
	"\x04\x0d""cx16\0"
#endif
#if REQUIRED_MASK4 & (1 << 14)
	"\x04\x0e""xtpr\0"
#endif
#if REQUIRED_MASK4 & (1 << 15)
	"\x04\x0f""pdcm\0"
#endif
#if REQUIRED_MASK4 & (1 << 17)
	"\x04\x11""pcid\0"
#endif
#if REQUIRED_MASK4 & (1 << 18)
	"\x04\x12""dca\0"
#endif
#if REQUIRED_MASK4 & (1 << 19)
	"\x04\x13""sse4_1\0"
#endif
#if REQUIRED_MASK4 & (1 << 20)
	"\x04\x14""sse4_2\0"
#endif
#if REQUIRED_MASK4 & (1 << 21)
	"\x04\x15""x2apic\0"
#endif
#if REQUIRED_MASK4 & (1 << 22)
	"\x04\x16""movbe\0"
#endif
#if REQUIRED_MASK4 & (1 << 23)
	"\x04\x17""popcnt\0"
#endif
#if REQUIRED_MASK4 & (1 << 24)
	"\x04\x18""tsc_deadline_timer\0"
#endif
#if REQUIRED_MASK4 & (1 << 25)
	"\x04\x19""aes\0"
#endif
#if REQUIRED_MASK4 & (1 << 26)
	"\x04\x1a""xsave\0"
#endif
#if REQUIRED_MASK4 & (1 << 28)
	"\x04\x1c""avx\0"
#endif
#if REQUIRED_MASK4 & (1 << 29)
	"\x04\x1d""f16c\0"
#endif
#if REQUIRED_MASK4 & (1 << 30)
	"\x04\x1e""rdrand\0"
#endif
#if REQUIRED_MASK4 & (1 << 31)
	"\x04\x1f""hypervisor\0"
#endif
#if REQUIRED_MASK5 & (1 << 2)
	"\x05\x02""rng\0"
#endif
#if REQUIRED_MASK5 & (1 << 3)
	"\x05\x03""rng_en\0"
#endif
#if REQUIRED_MASK5 & (1 << 6)
	"\x05\x06""ace\0"
#endif
#if REQUIRED_MASK5 & (1 << 7)
	"\x05\x07""ace_en\0"
#endif
#if REQUIRED_MASK5 & (1 << 8)
	"\x05\x08""ace2\0"
#endif
#if REQUIRED_MASK5 & (1 << 9)
	"\x05\x09""ace2_en\0"
#endif
#if REQUIRED_MASK5 & (1 << 10)
	"\x05\x0a""phe\0"
#endif
#if REQUIRED_MASK5 & (1 << 11)
	"\x05\x0b""phe_en\0"
#endif
#if REQUIRED_MASK5 & (1 << 12)
	"\x05\x0c""pmm\0"
#endif
#if REQUIRED_MASK5 & (1 << 13)
	"\x05\x0d""pmm_en\0"
#endif
#if REQUIRED_MASK6 & (1 << 0)
	"\x06\x00""lahf_lm\0"
#endif
#if REQUIRED_MASK6 & (1 << 1)
	"\x06\x01""cmp_legacy\0"
#endif
#if REQUIRED_MASK6 & (1 << 2)
	"\x06\x02""svm\0"
#endif
#if REQUIRED_MASK6 & (1 << 3)
	"\x06\x03""extapic\0"
#endif
#if REQUIRED_MASK6 & (1 << 4)
	"\x06\x04""cr8_legacy\0"
#endif
#if REQUIRED_MASK6 & (1 << 5)
	"\x06\x05""abm\0"
#endif
#if REQUIRED_MASK6 & (1 << 6)
	"\x06\x06""sse4a\0"
#endif
#if REQUIRED_MASK6 & (1 << 7)
	"\x06\x07""misalignsse\0"
#endif
#if REQUIRED_MASK6 & (1 << 8)
	"\x06\x08""3dnowprefetch\0"
#endif
#if REQUIRED_MASK6 & (1 << 9)
	"\x06\x09""osvw\0"
#endif
#if REQUIRED_MASK6 & (1 << 10)
	"\x06\x0a""ibs\0"
#endif
#if REQUIRED_MASK6 & (1 << 11)
	"\x06\x0b""xop\0"
#endif
#if REQUIRED_MASK6 & (1 << 12)
	"\x06\x0c""skinit\0"
#endif
#if REQUIRED_MASK6 & (1 << 13)
	"\x06\x0d""wdt\0"
#endif
#if REQUIRED_MASK6 & (1 << 15)
	"\x06\x0f""lwp\0"
#endif
#if REQUIRED_MASK6 & (1 << 16)
	"\x06\x10""fma4\0"
#endif
#if REQUIRED_MASK6 & (1 << 17)
	"\x06\x11""tce\0"
#endif
#if REQUIRED_MASK6 & (1 << 19)
	"\x06\x13""nodeid_msr\0"
#endif
#if REQUIRED_MASK6 & (1 << 21)
	"\x06\x15""tbm\0"
#endif
#if REQUIRED_MASK6 & (1 << 22)
	"\x06\x16""topoext\0"
#endif
#if REQUIRED_MASK6 & (1 << 23)
	"\x06\x17""perfctr_core\0"
#endif
#if REQUIRED_MASK6 & (1 << 24)
	"\x06\x18""perfctr_nb\0"
#endif
#if REQUIRED_MASK6 & (1 << 28)
	"\x06\x1c""perfctr_l2\0"
#endif
#if REQUIRED_MASK7 & (1 << 0)
	"\x07\x00""ida\0"
#endif
#if REQUIRED_MASK7 & (1 << 1)
	"\x07\x01""arat\0"
#endif
#if REQUIRED_MASK7 & (1 << 2)
	"\x07\x02""cpb\0"
#endif
#if REQUIRED_MASK7 & (1 << 3)
	"\x07\x03""epb\0"
#endif
#if REQUIRED_MASK7 & (1 << 4)
	"\x07\x04""xsaveopt\0"
#endif
#if REQUIRED_MASK7 & (1 << 5)
	"\x07\x05""pln\0"
#endif
#if REQUIRED_MASK7 & (1 << 6)
	"\x07\x06""pts\0"
#endif
#if REQUIRED_MASK7 & (1 << 7)
	"\x07\x07""dtherm\0"
#endif
#if REQUIRED_MASK7 & (1 << 8)
	"\x07\x08""hw_pstate\0"
#endif
#if REQUIRED_MASK7 & (1 << 9)
	"\x07\x09""proc_feedback\0"
#endif
#if REQUIRED_MASK8 & (1 << 0)
	"\x08\x00""tpr_shadow\0"
#endif
#if REQUIRED_MASK8 & (1 << 1)
	"\x08\x01""vnmi\0"
#endif
#if REQUIRED_MASK8 & (1 << 2)
	"\x08\x02""flexpriority\0"
#endif
#if REQUIRED_MASK8 & (1 << 3)
	"\x08\x03""ept\0"
#endif
#if REQUIRED_MASK8 & (1 << 4)
	"\x08\x04""vpid\0"
#endif
#if REQUIRED_MASK8 & (1 << 5)
	"\x08\x05""npt\0"
#endif
#if REQUIRED_MASK8 & (1 << 6)
	"\x08\x06""lbrv\0"
#endif
#if REQUIRED_MASK8 & (1 << 7)
	"\x08\x07""svm_lock\0"
#endif
#if REQUIRED_MASK8 & (1 << 8)
	"\x08\x08""nrip_save\0"
#endif
#if REQUIRED_MASK8 & (1 << 9)
	"\x08\x09""tsc_scale\0"
#endif
#if REQUIRED_MASK8 & (1 << 10)
	"\x08\x0a""vmcb_clean\0"
#endif
#if REQUIRED_MASK8 & (1 << 11)
	"\x08\x0b""flushbyasid\0"
#endif
#if REQUIRED_MASK8 & (1 << 12)
	"\x08\x0c""decodeassists\0"
#endif
#if REQUIRED_MASK8 & (1 << 13)
	"\x08\x0d""pausefilter\0"
#endif
#if REQUIRED_MASK8 & (1 << 14)
	"\x08\x0e""pfthreshold\0"
#endif
#if REQUIRED_MASK9 & (1 << 0)
	"\x09\x00""fsgsbase\0"
#endif
#if REQUIRED_MASK9 & (1 << 1)
	"\x09\x01""tsc_adjust\0"
#endif
#if REQUIRED_MASK9 & (1 << 3)
	"\x09\x03""bmi1\0"
#endif
#if REQUIRED_MASK9 & (1 << 4)
	"\x09\x04""hle\0"
#endif
#if REQUIRED_MASK9 & (1 << 5)
	"\x09\x05""avx2\0"
#endif
#if REQUIRED_MASK9 & (1 << 7)
	"\x09\x07""smep\0"
#endif
#if REQUIRED_MASK9 & (1 << 8)
	"\x09\x08""bmi2\0"
#endif
#if REQUIRED_MASK9 & (1 << 9)
	"\x09\x09""erms\0"
#endif
#if REQUIRED_MASK9 & (1 << 10)
	"\x09\x0a""invpcid\0"
#endif
#if REQUIRED_MASK9 & (1 << 11)
	"\x09\x0b""rtm\0"
#endif
#if REQUIRED_MASK9 & (1 << 14)
	"\x09\x0e""mpx\0"
#endif
#if REQUIRED_MASK9 & (1 << 16)
	"\x09\x10""avx512f\0"
#endif
#if REQUIRED_MASK9 & (1 << 18)
	"\x09\x12""rdseed\0"
#endif
#if REQUIRED_MASK9 & (1 << 19)
	"\x09\x13""adx\0"
#endif
#if REQUIRED_MASK9 & (1 << 20)
	"\x09\x14""smap\0"
#endif
#if REQUIRED_MASK9 & (1 << 23)
	"\x09\x17""clflushopt\0"
#endif
#if REQUIRED_MASK9 & (1 << 26)
	"\x09\x1a""avx512pf\0"
#endif
#if REQUIRED_MASK9 & (1 << 27)
	"\x09\x1b""avx512er\0"
#endif
#if REQUIRED_MASK9 & (1 << 28)
	"\x09\x1c""avx512cd\0"
#endif
	"\x09\x1f"""
	;
