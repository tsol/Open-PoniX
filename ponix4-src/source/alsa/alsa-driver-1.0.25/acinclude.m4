dnl ALSA soundcard configuration
dnl Find out which cards to compile driver for
dnl Copyright (c) by Jaroslav Kysela <perex@perex.cz>,
dnl                  Anders Semb Hermansen <ahermans@vf.telia.no>

AC_DEFUN([ALSA_TOPLEVEL_INIT], [
	CONFIG_SOUND=""
	CONFIG_HAS_IOMEM=""
	CONFIG_SOUND_OSS_CORE=""
	CONFIG_SOUND_OSS_CORE_PRECLAIM=""
	CONFIG_M68K=""
	CONFIG_UML=""
	CONFIG_SND=""
	CONFIG_SND_TIMER=""
	CONFIG_SND_PCM=""
	CONFIG_SND_HWDEP=""
	CONFIG_SND_RAWMIDI=""
	CONFIG_SND_COMPRESS_OFFLOAD=""
	CONFIG_SND_JACK=""
	CONFIG_SND_SEQUENCER=""
	CONFIG_SND_SEQ_DUMMY=""
	CONFIG_SND_OSSEMUL=""
	CONFIG_SND_MIXER_OSS=""
	CONFIG_SND_PCM_OSS=""
	CONFIG_SND_PCM_OSS_PLUGINS=""
	CONFIG_SND_SEQUENCER_OSS=""
	CONFIG_SND_HRTIMER=""
	CONFIG_HIGH_RES_TIMERS=""
	CONFIG_SND_SEQ_HRTIMER_DEFAULT=""
	CONFIG_SND_RTCTIMER=""
	CONFIG_RTC=""
	CONFIG_SND_SEQ_RTCTIMER_DEFAULT=""
	CONFIG_SND_DYNAMIC_MINORS=""
	CONFIG_SND_SUPPORT_OLD_API=""
	CONFIG_SND_VERBOSE_PROCFS=""
	CONFIG_PROC_FS=""
	CONFIG_SND_VERBOSE_PRINTK=""
	CONFIG_SND_DEBUG=""
	CONFIG_SND_DEBUG_VERBOSE=""
	CONFIG_SND_PCM_XRUN_DEBUG=""
	CONFIG_SND_VMASTER=""
	CONFIG_SND_KCTL_JACK=""
	CONFIG_SND_DMA_SGBUF=""
	CONFIG_X86=""
	CONFIG_SND_RAWMIDI_SEQ=""
	CONFIG_SND_OPL3_LIB_SEQ=""
	CONFIG_SND_OPL3_LIB=""
	CONFIG_SND_OPL4_LIB_SEQ=""
	CONFIG_SND_OPL4_LIB=""
	CONFIG_SND_SBAWE_SEQ=""
	CONFIG_SND_SBAWE=""
	CONFIG_SND_EMU10K1_SEQ=""
	CONFIG_SND_EMU10K1=""
	CONFIG_SND_BIT32_EMUL=""
	CONFIG_SND_DEBUG_MEMORY=""
	CONFIG_SND_HPET=""
	CONFIG_HPET=""
	CONFIG_BROKEN=""
	CONFIG_SND_MPU401_UART=""
	CONFIG_SND_VX_LIB=""
	CONFIG_SND_AC97_CODEC=""
	CONFIG_SND_DRIVERS=""
	CONFIG_SND_PCSP=""
	CONFIG_PCSPKR_PLATFORM=""
	CONFIG_INPUT=""
	CONFIG_EXPERIMENTAL=""
	CONFIG_SND_DUMMY=""
	CONFIG_SND_ALOOP=""
	CONFIG_SND_VIRMIDI=""
	CONFIG_SND_MTPAV=""
	CONFIG_SND_MTS64=""
	CONFIG_PARPORT=""
	CONFIG_SND_SERIAL_U16550=""
	CONFIG_SND_MPU401=""
	CONFIG_SND_PORTMAN2X4=""
	CONFIG_SND_ML403_AC97CR=""
	CONFIG_XILINX_VIRTEX=""
	CONFIG_SND_AC97_POWER_SAVE=""
	CONFIG_SND_AC97_POWER_SAVE_DEFAULT=""
	CONFIG_SND_SERIALMIDI=""
	CONFIG_SND_LOOPBACK=""
	CONFIG_SND_WSS_LIB=""
	CONFIG_SND_SB_COMMON=""
	CONFIG_SND_SB8_DSP=""
	CONFIG_SND_SB16_DSP=""
	CONFIG_SND_ISA=""
	CONFIG_ISA=""
	CONFIG_ISA_DMA_API=""
	CONFIG_SND_ADLIB=""
	CONFIG_SND_AD1816A=""
	CONFIG_PNP=""
	CONFIG_ISAPNP=""
	CONFIG_SND_AD1848=""
	CONFIG_SND_ALS100=""
	CONFIG_SND_AZT1605=""
	CONFIG_SND_AZT2316=""
	CONFIG_SND_AZT2320=""
	CONFIG_SND_CMI8330=""
	CONFIG_SND_CS4231=""
	CONFIG_SND_CS4236=""
	CONFIG_SND_ES1688=""
	CONFIG_SND_ES18XX=""
	CONFIG_SND_SC6000=""
	CONFIG_HAS_IOPORT=""
	CONFIG_SND_GUSCLASSIC=""
	CONFIG_SND_GUSEXTREME=""
	CONFIG_SND_GUSMAX=""
	CONFIG_SND_INTERWAVE=""
	CONFIG_SND_INTERWAVE_STB=""
	CONFIG_SND_JAZZ16=""
	CONFIG_SND_OPL3SA2=""
	CONFIG_SND_OPTI92X_AD1848=""
	CONFIG_SND_OPTI92X_CS4231=""
	CONFIG_SND_OPTI93X=""
	CONFIG_SND_MIRO=""
	CONFIG_SND_SB8=""
	CONFIG_SND_SB16=""
	CONFIG_SND_SB16_CSP=""
	CONFIG_PPC=""
	CONFIG_FW_LOADER=""
	CONFIG_SND_SSCAPE=""
	CONFIG_SND_WAVEFRONT=""
	CONFIG_SND_MSND_PINNACLE=""
	CONFIG_SND_MSND_CLASSIC=""
	CONFIG_SND_PC98_CS4232=""
	CONFIG_X86_PC9800=""
	CONFIG_SND_TEA575X=""
	CONFIG_SND_FM801_TEA575X_BOOL=""
	CONFIG_SND_ES1968_RADIO=""
	CONFIG_RADIO_SF16FMR2=""
	CONFIG_SND_FM801=""
	CONFIG_SND_ES1968=""
	CONFIG_SND_PCI=""
	CONFIG_PCI=""
	CONFIG_SND_AD1889=""
	CONFIG_SND_ALS300=""
	CONFIG_SND_ALS4000=""
	CONFIG_SND_ALI5451=""
	CONFIG_SND_ASIHPI=""
	CONFIG_SND_ATIIXP=""
	CONFIG_SND_ATIIXP_MODEM=""
	CONFIG_SND_AU8810=""
	CONFIG_SND_AU8820=""
	CONFIG_SND_AU8830=""
	CONFIG_SND_AW2=""
	CONFIG_SND_AZT3328=""
	CONFIG_SND_BT87X=""
	CONFIG_SND_BT87X_OVERCLOCK=""
	CONFIG_SND_CA0106=""
	CONFIG_SND_CMIPCI=""
	CONFIG_SND_OXYGEN_LIB=""
	CONFIG_SND_OXYGEN=""
	CONFIG_SND_CS4281=""
	CONFIG_SND_CS46XX=""
	CONFIG_SND_CS46XX_NEW_DSP=""
	CONFIG_SND_CS5530=""
	CONFIG_SND_CS5535AUDIO=""
	CONFIG_SND_CTXFI=""
	CONFIG_SND_DARLA20=""
	CONFIG_SND_GINA20=""
	CONFIG_SND_LAYLA20=""
	CONFIG_SND_DARLA24=""
	CONFIG_SND_GINA24=""
	CONFIG_SND_LAYLA24=""
	CONFIG_SND_MONA=""
	CONFIG_SND_MIA=""
	CONFIG_SND_ECHO3G=""
	CONFIG_SND_INDIGO=""
	CONFIG_SND_INDIGOIO=""
	CONFIG_SND_INDIGODJ=""
	CONFIG_SND_INDIGOIOX=""
	CONFIG_SND_INDIGODJX=""
	CONFIG_SND_EMU10K1X=""
	CONFIG_SND_ENS1370=""
	CONFIG_SND_ENS1371=""
	CONFIG_SND_ES1938=""
	CONFIG_SND_ES1968_INPUT=""
	CONFIG_VIDEO_V4L2=""
	CONFIG_SND_HDA_INTEL=""
	CONFIG_SND_HDA_PREALLOC_SIZE=""
	CONFIG_SND_HDA_HWDEP=""
	CONFIG_SND_HDA_RECONFIG=""
	CONFIG_SND_HDA_INPUT_BEEP=""
	CONFIG_SND_HDA_INPUT_BEEP_MODE=""
	CONFIG_SND_HDA_INPUT_JACK=""
	CONFIG_SND_HDA_PATCH_LOADER=""
	CONFIG_SND_HDA_CODEC_REALTEK=""
	CONFIG_SND_HDA_ENABLE_REALTEK_QUIRKS=""
	CONFIG_SND_HDA_CODEC_ANALOG=""
	CONFIG_SND_HDA_CODEC_SIGMATEL=""
	CONFIG_SND_HDA_CODEC_VIA=""
	CONFIG_SND_HDA_CODEC_HDMI=""
	CONFIG_SND_HDA_CODEC_CIRRUS=""
	CONFIG_SND_HDA_CODEC_CONEXANT=""
	CONFIG_SND_HDA_CODEC_CA0110=""
	CONFIG_SND_HDA_CODEC_CA0132=""
	CONFIG_SND_HDA_CODEC_CMEDIA=""
	CONFIG_SND_HDA_CODEC_SI3054=""
	CONFIG_SND_HDA_GENERIC=""
	CONFIG_SND_HDA_POWER_SAVE=""
	CONFIG_PM=""
	CONFIG_SND_HDA_POWER_SAVE_DEFAULT=""
	CONFIG_SND_HDSP=""
	CONFIG_SND_HDSPM=""
	CONFIG_SND_ICE1712=""
	CONFIG_BITREVERSE=""
	CONFIG_SND_ICE1724=""
	CONFIG_SND_INTEL8X0=""
	CONFIG_SND_INTEL8X0M=""
	CONFIG_SND_KORG1212=""
	CONFIG_SND_LOLA=""
	CONFIG_SND_LX6464ES=""
	CONFIG_SND_MAESTRO3=""
	CONFIG_SND_MAESTRO3_INPUT=""
	CONFIG_SND_MIXART=""
	CONFIG_SND_NM256=""
	CONFIG_SND_PCXHR=""
	CONFIG_SND_RIPTIDE=""
	CONFIG_SND_RME32=""
	CONFIG_SND_RME96=""
	CONFIG_SND_RME9652=""
	CONFIG_SND_SIS7019=""
	CONFIG_X86_64=""
	CONFIG_SND_SONICVIBES=""
	CONFIG_SND_TRIDENT=""
	CONFIG_SND_VIA82XX=""
	CONFIG_SND_VIA82XX_MODEM=""
	CONFIG_SND_VIRTUOSO=""
	CONFIG_SND_VX222=""
	CONFIG_SND_YMFPCI=""
	CONFIG_SND_PDPLUS=""
	CONFIG_SND_PPC=""
	CONFIG_SND_POWERMAC=""
	CONFIG_I2C=""
	CONFIG_PPC_PMAC=""
	CONFIG_SND_POWERMAC_AUTO_DRC=""
	CONFIG_SND_PS3=""
	CONFIG_PS3_PS3AV=""
	CONFIG_SND_PS3_DEFAULT_START_DELAY=""
	CONFIG_SND_AOA=""
	CONFIG_SND_AOA_FABRIC_LAYOUT=""
	CONFIG_SND_AOA_ONYX=""
	CONFIG_I2C_POWERMAC=""
	CONFIG_SND_AOA_TAS=""
	CONFIG_SND_AOA_TOONIE=""
	CONFIG_SND_AOA_SOUNDBUS=""
	CONFIG_SND_AOA_SOUNDBUS_I2S=""
	CONFIG_SND_ARM=""
	CONFIG_ARM=""
	CONFIG_SND_ARMAACI=""
	CONFIG_ARM_AMBA=""
	CONFIG_SND_PXA2XX_PCM=""
	CONFIG_SND_PXA2XX_LIB=""
	CONFIG_SND_PXA2XX_LIB_AC97=""
	CONFIG_SND_PXA2XX_AC97=""
	CONFIG_ARCH_PXA=""
	CONFIG_SND_SA11XX_UDA1341=""
	CONFIG_ARCH_SA1100=""
	CONFIG_L3=""
	CONFIG_SND_S3C2410=""
	CONFIG_ARCH_S3C2410=""
	CONFIG_I2C_SENSOR=""
	CONFIG_SND_PXA2XX_I2SOUND=""
	CONFIG_AVR32=""
	CONFIG_ARCH_AT91=""
	CONFIG_SND_ATMEL_ABDAC=""
	CONFIG_DW_DMAC=""
	CONFIG_SND_ATMEL_AC97C=""
	CONFIG_SND_SPI=""
	CONFIG_SPI=""
	CONFIG_SND_AT73C213=""
	CONFIG_ATMEL_SSC=""
	CONFIG_SND_AT73C213_TARGET_BITRATE=""
	CONFIG_SND_MIPS=""
	CONFIG_MIPS=""
	CONFIG_SND_SGI_O2=""
	CONFIG_SGI_IP32=""
	CONFIG_SND_SGI_HAL2=""
	CONFIG_SGI_HAS_HAL2=""
	CONFIG_SND_AU1X00=""
	CONFIG_MIPS_ALCHEMY=""
	CONFIG_SND_SUPERH=""
	CONFIG_SUPERH=""
	CONFIG_SND_AICA=""
	CONFIG_SH_DREAMCAST=""
	CONFIG_G2_DMA=""
	CONFIG_SND_SH_DAC_AUDIO=""
	CONFIG_CPU_SH3=""
	CONFIG_SND_USB=""
	CONFIG_USB=""
	CONFIG_SND_USB_AUDIO=""
	CONFIG_SND_USB_UA101=""
	CONFIG_SND_USB_USX2Y=""
	CONFIG_ALPHA=""
	CONFIG_SND_USB_CAIAQ=""
	CONFIG_SND_USB_CAIAQ_INPUT=""
	CONFIG_SND_USB_US122L=""
	CONFIG_SND_USB_6FIRE=""
	CONFIG_SND_FIREWIRE=""
	CONFIG_FIREWIRE=""
	CONFIG_SND_FIREWIRE_LIB=""
	CONFIG_SND_FIREWIRE_SPEAKERS=""
	CONFIG_SND_ISIGHT=""
	CONFIG_SND_PCMCIA=""
	CONFIG_PCMCIA=""
	CONFIG_SND_VXPOCKET=""
	CONFIG_SND_PDAUDIOCF=""
	CONFIG_SND_SPARC=""
	CONFIG_SPARC=""
	CONFIG_SND_SUN_AMD7930=""
	CONFIG_SBUS=""
	CONFIG_SND_SUN_CS4231=""
	CONFIG_SND_SUN_DBRI=""
	CONFIG_SND_GSC=""
	CONFIG_GSC=""
	CONFIG_SND_HARMONY=""
	CONFIG_SND_SOC=""
	CONFIG_REGMAP_I2C=""
	CONFIG_REGMAP_SPI=""
	CONFIG_SND_SOC_AC97_BUS=""
	CONFIG_SND_ATMEL_SOC=""
	CONFIG_SND_ATMEL_SOC_SSC=""
	CONFIG_SND_AT91_SOC_SAM9G20_WM8731=""
	CONFIG_ARCH_AT91SAM9G20=""
	CONFIG_AT91_PROGRAMMABLE_CLOCKS=""
	CONFIG_SND_AT91_SOC_AFEB9260=""
	CONFIG_MACH_AFEB9260=""
	CONFIG_SND_SOC_AU1XPSC=""
	CONFIG_SND_SOC_AU1XPSC_I2S=""
	CONFIG_SND_SOC_AU1XPSC_AC97=""
	CONFIG_SND_SOC_AU1XAUDIO=""
	CONFIG_SND_SOC_AU1XAC97C=""
	CONFIG_SND_SOC_AU1XI2SC=""
	CONFIG_SND_SOC_DB1000=""
	CONFIG_SND_SOC_DB1200=""
	CONFIG_SND_BF5XX_I2S=""
	CONFIG_BLACKFIN=""
	CONFIG_SND_BF5XX_SOC_SSM2602=""
	CONFIG_SPI_MASTER=""
	CONFIG_SND_SOC_BFIN_EVAL_ADAU1701=""
	CONFIG_SND_SOC_BFIN_EVAL_ADAU1373=""
	CONFIG_SND_SOC_BFIN_EVAL_ADAV80X=""
	CONFIG_SND_BF5XX_SOC_AD73311=""
	CONFIG_SND_BFIN_AD73311_SE=""
	CONFIG_SND_BF5XX_TDM=""
	CONFIG_SND_BF5XX_SOC_AD1836=""
	CONFIG_SND_BF5XX_SOC_AD193X=""
	CONFIG_SND_BF5XX_AC97=""
	CONFIG_SND_BF5XX_MMAP_SUPPORT=""
	CONFIG_SND_BF5XX_MULTICHAN_SUPPORT=""
	CONFIG_SND_BF5XX_HAVE_COLD_RESET=""
	CONFIG_SND_BF5XX_RESET_GPIO_NUM=""
	CONFIG_SND_BF5XX_SOC_AD1980=""
	CONFIG_SND_BF5XX_SOC_SPORT=""
	CONFIG_SND_BF5XX_SOC_I2S=""
	CONFIG_SND_BF5XX_SOC_TDM=""
	CONFIG_SND_BF5XX_SOC_AC97=""
	CONFIG_SND_BF5XX_SPORT_NUM=""
	CONFIG_SND_DAVINCI_SOC=""
	CONFIG_ARCH_DAVINCI=""
	CONFIG_SND_DAVINCI_SOC_I2S=""
	CONFIG_SND_DAVINCI_SOC_MCASP=""
	CONFIG_SND_DAVINCI_SOC_VCIF=""
	CONFIG_SND_DAVINCI_SOC_EVM=""
	CONFIG_MACH_DAVINCI_EVM=""
	CONFIG_MACH_DAVINCI_DM355_EVM=""
	CONFIG_MACH_DAVINCI_DM365_EVM=""
	CONFIG_SND_DM365_AIC3X_CODEC=""
	CONFIG_SND_DM365_VOICE_CODEC=""
	CONFIG_MFD_DAVINCI_VOICECODEC=""
	CONFIG_SND_DM6467_SOC_EVM=""
	CONFIG_MACH_DAVINCI_DM6467_EVM=""
	CONFIG_SND_DAVINCI_SOC_SFFSDR=""
	CONFIG_MACH_SFFSDR=""
	CONFIG_SFFSDR_FPGA=""
	CONFIG_SND_DA830_SOC_EVM=""
	CONFIG_MACH_DAVINCI_DA830_EVM=""
	CONFIG_SND_DA850_SOC_EVM=""
	CONFIG_MACH_DAVINCI_DA850_EVM=""
	CONFIG_SND_EP93XX_SOC=""
	CONFIG_ARCH_EP93XX=""
	CONFIG_SND_EP93XX_SOC_I2S=""
	CONFIG_SND_EP93XX_SOC_AC97=""
	CONFIG_SND_EP93XX_SOC_SNAPPERCL15=""
	CONFIG_MACH_SNAPPER_CL15=""
	CONFIG_SND_EP93XX_SOC_SIMONE=""
	CONFIG_MACH_SIM_ONE=""
	CONFIG_SND_EP93XX_SOC_EDB93XX=""
	CONFIG_MACH_EDB9301=""
	CONFIG_MACH_EDB9302=""
	CONFIG_MACH_EDB9302A=""
	CONFIG_MACH_EDB9307A=""
	CONFIG_MACH_EDB9315A=""
	CONFIG_SND_MPC52XX_DMA=""
	CONFIG_SND_SOC_POWERPC_SSI=""
	CONFIG_FSL_SOC=""
	CONFIG_SND_SOC_MPC8610_HPCD=""
	CONFIG_MPC8610_HPCD=""
	CONFIG_SND_SOC_P1022_DS=""
	CONFIG_P1022_DS=""
	CONFIG_SND_SOC_MPC5200_I2S=""
	CONFIG_PPC_MPC52XX=""
	CONFIG_PPC_BESTCOMM=""
	CONFIG_PPC_BESTCOMM_GEN_BD=""
	CONFIG_SND_SOC_MPC5200_AC97=""
	CONFIG_SND_MPC52XX_SOC_PCM030=""
	CONFIG_PPC_MPC5200_SIMPLE=""
	CONFIG_SND_MPC52XX_SOC_EFIKA=""
	CONFIG_PPC_EFIKA=""
	CONFIG_SND_IMX_SOC=""
	CONFIG_ARCH_MXC=""
	CONFIG_FIQ=""
	CONFIG_SND_MXC_SOC_FIQ=""
	CONFIG_SND_MXC_SOC_MX2=""
	CONFIG_SND_MXC_SOC_WM1133_EV1=""
	CONFIG_MACH_MX31ADS_WM1133_EV1=""
	CONFIG_SND_SOC_MX27VIS_AIC32X4=""
	CONFIG_MACH_IMX27_VISSTRIM_M10=""
	CONFIG_SND_SOC_PHYCORE_AC97=""
	CONFIG_MACH_PCM043=""
	CONFIG_MACH_PCA100=""
	CONFIG_SND_SOC_EUKREA_TLV320=""
	CONFIG_MACH_EUKREA_MBIMX27_BASEBOARD=""
	CONFIG_MACH_EUKREA_MBIMXSD25_BASEBOARD=""
	CONFIG_MACH_EUKREA_MBIMXSD35_BASEBOARD=""
	CONFIG_MACH_EUKREA_MBIMXSD51_BASEBOARD=""
	CONFIG_SND_JZ4740_SOC=""
	CONFIG_MACH_JZ4740=""
	CONFIG_SND_JZ4740_SOC_I2S=""
	CONFIG_SND_JZ4740_SOC_QI_LB60=""
	CONFIG_JZ4740_QI_LB60=""
	CONFIG_SND_SOC_NUC900=""
	CONFIG_ARCH_W90X900=""
	CONFIG_SND_SOC_NUC900_AC97=""
	CONFIG_SND_SOC_NUC900EVB=""
	CONFIG_SND_OMAP_SOC=""
	CONFIG_ARCH_OMAP=""
	CONFIG_SND_OMAP_SOC_DMIC=""
	CONFIG_SND_OMAP_SOC_MCBSP=""
	CONFIG_OMAP_MCBSP=""
	CONFIG_SND_OMAP_SOC_MCPDM=""
	CONFIG_SND_OMAP_SOC_HDMI=""
	CONFIG_SND_OMAP_SOC_N810=""
	CONFIG_MACH_NOKIA_N810=""
	CONFIG_OMAP_MUX=""
	CONFIG_SND_OMAP_SOC_RX51=""
	CONFIG_MACH_NOKIA_RX51=""
	CONFIG_SND_OMAP_SOC_AMS_DELTA=""
	CONFIG_MACH_AMS_DELTA=""
	CONFIG_SND_OMAP_SOC_OSK5912=""
	CONFIG_MACH_OMAP_OSK=""
	CONFIG_SND_OMAP_SOC_OVERO=""
	CONFIG_TWL4030_CORE=""
	CONFIG_MACH_OVERO=""
	CONFIG_MACH_CM_T35=""
	CONFIG_SND_OMAP_SOC_OMAP3EVM=""
	CONFIG_MACH_OMAP3EVM=""
	CONFIG_SND_OMAP_SOC_AM3517EVM=""
	CONFIG_MACH_OMAP3517EVM=""
	CONFIG_SND_OMAP_SOC_SDP3430=""
	CONFIG_MACH_OMAP_3430SDP=""
	CONFIG_SND_OMAP_SOC_SDP4430=""
	CONFIG_MACH_OMAP_4430SDP=""
	CONFIG_SND_OMAP_SOC_OMAP4_HDMI=""
	CONFIG_OMAP4_DSS_HDMI=""
	CONFIG_OMAP2_DSS=""
	CONFIG_ARCH_OMAP4=""
	CONFIG_SND_OMAP_SOC_OMAP3_PANDORA=""
	CONFIG_MACH_OMAP3_PANDORA=""
	CONFIG_SND_OMAP_SOC_OMAP3_BEAGLE=""
	CONFIG_MACH_OMAP3_BEAGLE=""
	CONFIG_MACH_DEVKIT8000=""
	CONFIG_SND_OMAP_SOC_ZOOM2=""
	CONFIG_MACH_OMAP_ZOOM2=""
	CONFIG_SND_OMAP_SOC_IGEP0020=""
	CONFIG_MACH_IGEP0020=""
	CONFIG_SND_KIRKWOOD_SOC=""
	CONFIG_ARCH_KIRKWOOD=""
	CONFIG_SND_KIRKWOOD_SOC_I2S=""
	CONFIG_SND_KIRKWOOD_SOC_OPENRD=""
	CONFIG_MACH_OPENRD_CLIENT=""
	CONFIG_MACH_OPENRD_ULTIMATE=""
	CONFIG_SND_KIRKWOOD_SOC_T5325=""
	CONFIG_MACH_T5325=""
	CONFIG_SND_MFLD_MACHINE=""
	CONFIG_INTEL_SCU_IPC=""
	CONFIG_SND_SST_PLATFORM=""
	CONFIG_SND_MXS_SOC=""
	CONFIG_ARCH_MXS=""
	CONFIG_SND_SOC_MXS_SGTL5000=""
	CONFIG_SND_PXA2XX_SOC=""
	CONFIG_SND_PXA2XX_SOC_AC97=""
	CONFIG_SND_PXA2XX_SOC_I2S=""
	CONFIG_SND_PXA_SOC_SSP=""
	CONFIG_PXA_SSP=""
	CONFIG_SND_PXA2XX_SOC_CORGI=""
	CONFIG_PXA_SHARP_C7XX=""
	CONFIG_SND_PXA2XX_SOC_SPITZ=""
	CONFIG_PXA_SHARP_CXX00=""
	CONFIG_SND_PXA2XX_SOC_Z2=""
	CONFIG_MACH_ZIPIT2=""
	CONFIG_SND_PXA2XX_SOC_POODLE=""
	CONFIG_MACH_POODLE=""
	CONFIG_SND_PXA2XX_SOC_TOSA=""
	CONFIG_MACH_TOSA=""
	CONFIG_MFD_TC6393XB=""
	CONFIG_SND_PXA2XX_SOC_E740=""
	CONFIG_MACH_E740=""
	CONFIG_SND_PXA2XX_SOC_E750=""
	CONFIG_MACH_E750=""
	CONFIG_SND_PXA2XX_SOC_E800=""
	CONFIG_MACH_E800=""
	CONFIG_SND_PXA2XX_SOC_EM_X270=""
	CONFIG_MACH_EM_X270=""
	CONFIG_MACH_EXEDA=""
	CONFIG_MACH_CM_X300=""
	CONFIG_SND_PXA2XX_SOC_PALM27X=""
	CONFIG_MACH_PALMLD=""
	CONFIG_MACH_PALMTX=""
	CONFIG_MACH_PALMT5=""
	CONFIG_MACH_PALMTE2=""
	CONFIG_SND_SOC_SAARB=""
	CONFIG_MACH_SAARB=""
	CONFIG_MFD_88PM860X=""
	CONFIG_SND_SOC_TAVOREVB3=""
	CONFIG_MACH_TAVOREVB3=""
	CONFIG_SND_SOC_ZYLONITE=""
	CONFIG_MACH_ZYLONITE=""
	CONFIG_SND_SOC_RAUMFELD=""
	CONFIG_MACH_RAUMFELD_SPEAKER=""
	CONFIG_MACH_RAUMFELD_CONNECTOR=""
	CONFIG_SND_PXA2XX_SOC_HX4700=""
	CONFIG_MACH_H4700=""
	CONFIG_SND_PXA2XX_SOC_MAGICIAN=""
	CONFIG_MACH_MAGICIAN=""
	CONFIG_SND_PXA2XX_SOC_MIOA701=""
	CONFIG_MACH_MIOA701=""
	CONFIG_SND_PXA2XX_SOC_IMOTE2=""
	CONFIG_SND_SOC_SAMSUNG=""
	CONFIG_ARCH_S3C64XX=""
	CONFIG_ARCH_S5PC100=""
	CONFIG_ARCH_S5PV210=""
	CONFIG_ARCH_S5P64X0=""
	CONFIG_ARCH_EXYNOS4=""
	CONFIG_S3C64XX_DMA=""
	CONFIG_S3C2410_DMA=""
	CONFIG_SND_S3C24XX_I2S=""
	CONFIG_SND_S3C_I2SV2_SOC=""
	CONFIG_SND_S3C2412_SOC_I2S=""
	CONFIG_SND_SAMSUNG_PCM=""
	CONFIG_SND_SAMSUNG_AC97=""
	CONFIG_SND_SAMSUNG_SPDIF=""
	CONFIG_SND_SAMSUNG_I2S=""
	CONFIG_SND_SOC_SAMSUNG_NEO1973_WM8753=""
	CONFIG_MACH_NEO1973_GTA01=""
	CONFIG_MACH_NEO1973_GTA02=""
	CONFIG_SND_SOC_SAMSUNG_JIVE_WM8750=""
	CONFIG_MACH_JIVE=""
	CONFIG_SND_SOC_SAMSUNG_SMDK_WM8580=""
	CONFIG_MACH_SMDK6410=""
	CONFIG_MACH_SMDKC100=""
	CONFIG_MACH_SMDK6440=""
	CONFIG_MACH_SMDK6450=""
	CONFIG_MACH_SMDKV210=""
	CONFIG_MACH_SMDKC110=""
	CONFIG_SND_SOC_SAMSUNG_SMDK_WM8994=""
	CONFIG_MACH_SMDKV310=""
	CONFIG_MACH_SMDKC210=""
	CONFIG_MACH_SMDK4212=""
	CONFIG_GENERIC_HARDIRQS=""
	CONFIG_MFD_WM8994=""
	CONFIG_SND_SOC_SAMSUNG_SMDK2443_WM9710=""
	CONFIG_MACH_SMDK2443=""
	CONFIG_SND_SOC_SAMSUNG_LN2440SBC_ALC650=""
	CONFIG_SND_SOC_SAMSUNG_S3C24XX_UDA134X=""
	CONFIG_SND_SOC_SAMSUNG_SIMTEC=""
	CONFIG_SND_SOC_SAMSUNG_SIMTEC_TLV320AIC23=""
	CONFIG_SND_SOC_SAMSUNG_SIMTEC_HERMES=""
	CONFIG_SND_SOC_SAMSUNG_H1940_UDA1380=""
	CONFIG_ARCH_H1940=""
	CONFIG_SND_SOC_SAMSUNG_RX1950_UDA1380=""
	CONFIG_MACH_RX1950=""
	CONFIG_SND_SOC_SAMSUNG_SMDK_WM9713=""
	CONFIG_SND_SOC_SMARTQ=""
	CONFIG_MACH_SMARTQ=""
	CONFIG_SND_SOC_GONI_AQUILA_WM8994=""
	CONFIG_MACH_GONI=""
	CONFIG_MACH_AQUILA=""
	CONFIG_SND_SOC_SAMSUNG_SMDK_SPDIF=""
	CONFIG_SND_SOC_SMDK_WM8580_PCM=""
	CONFIG_SND_SOC_SMDK_WM8994_PCM=""
	CONFIG_SND_SOC_SPEYSIDE=""
	CONFIG_MACH_WLF_CRAGG_6410=""
	CONFIG_SND_SOC_TOBERMORY=""
	CONFIG_SND_SOC_LOWLAND=""
	CONFIG_SND_SOC_LITTLEMILL=""
	CONFIG_SND_S6000_SOC=""
	CONFIG_XTENSA_VARIANT_S6000=""
	CONFIG_SND_S6000_SOC_I2S=""
	CONFIG_SND_S6000_SOC_S6IPCAM=""
	CONFIG_XTENSA_PLATFORM_S6105=""
	CONFIG_ARCH_SHMOBILE=""
	CONFIG_SND_SOC_PCM_SH7760=""
	CONFIG_CPU_SUBTYPE_SH7760=""
	CONFIG_SH_DMABRG=""
	CONFIG_SND_SOC_SH4_HAC=""
	CONFIG_SND_SOC_SH4_SSI=""
	CONFIG_SND_SOC_SH4_FSI=""
	CONFIG_SND_SOC_SH4_SIU=""
	CONFIG_HAVE_CLK=""
	CONFIG_DMA_ENGINE=""
	CONFIG_DMADEVICES=""
	CONFIG_SH_DMAE=""
	CONFIG_SND_SH7760_AC97=""
	CONFIG_SND_FSI_AK4642=""
	CONFIG_SND_FSI_DA7210=""
	CONFIG_SND_FSI_HDMI=""
	CONFIG_FB_SH_MOBILE_HDMI=""
	CONFIG_SND_SIU_MIGOR=""
	CONFIG_SH_MIGOR=""
	CONFIG_SND_SOC_TEGRA=""
	CONFIG_ARCH_TEGRA=""
	CONFIG_TEGRA_SYSTEM_DMA=""
	CONFIG_SND_SOC_TEGRA_I2S=""
	CONFIG_SND_SOC_TEGRA_SPDIF=""
	CONFIG_MACH_HAS_SND_SOC_TEGRA_WM8903=""
	CONFIG_SND_SOC_TEGRA_WM8903=""
	CONFIG_SND_SOC_TEGRA_TRIMSLICE=""
	CONFIG_MACH_TRIMSLICE=""
	CONFIG_SND_SOC_TEGRA_ALC5632=""
	CONFIG_SND_SOC_TXX9ACLC=""
	CONFIG_HAS_TXX9_ACLC=""
	CONFIG_TXX9_DMAC=""
	CONFIG_SND_SOC_TXX9ACLC_AC97=""
	CONFIG_SND_SOC_TXX9ACLC_GENERIC=""
	CONFIG_SND_SOC_I2C_AND_SPI=""
	CONFIG_SND_SOC_ALL_CODECS=""
	CONFIG_SND_SOC_88PM860X=""
	CONFIG_SND_SOC_WM_HUBS=""
	CONFIG_SND_SOC_AC97_CODEC=""
	CONFIG_SND_SOC_AD1836=""
	CONFIG_SND_SOC_AD193X=""
	CONFIG_SND_SOC_AD1980=""
	CONFIG_SND_SOC_AD73311=""
	CONFIG_SND_SOC_ADAU1701=""
	CONFIG_SND_SOC_ADAU1373=""
	CONFIG_SND_SOC_ADAV80X=""
	CONFIG_SND_SOC_ADS117X=""
	CONFIG_SND_SOC_AK4104=""
	CONFIG_SND_SOC_AK4535=""
	CONFIG_SND_SOC_AK4641=""
	CONFIG_SND_SOC_AK4642=""
	CONFIG_SND_SOC_AK4671=""
	CONFIG_SND_SOC_ALC5623=""
	CONFIG_SND_SOC_ALC5632=""
	CONFIG_SND_SOC_CQ0093VC=""
	CONFIG_SND_SOC_CS42L51=""
	CONFIG_SND_SOC_CS42L73=""
	CONFIG_SND_SOC_CS4270=""
	CONFIG_SND_SOC_CS4270_VD33_ERRATA=""
	CONFIG_SND_SOC_CS4271=""
	CONFIG_SND_SOC_CX20442=""
	CONFIG_SND_SOC_JZ4740_CODEC=""
	CONFIG_SND_SOC_L3=""
	CONFIG_SND_SOC_DA7210=""
	CONFIG_SND_SOC_DFBMCS320=""
	CONFIG_SND_SOC_DMIC=""
	CONFIG_SND_SOC_MAX98088=""
	CONFIG_SND_SOC_MAX98095=""
	CONFIG_SND_SOC_MAX9850=""
	CONFIG_SND_SOC_PCM3008=""
	CONFIG_SND_SOC_RT5631=""
	CONFIG_SND_SOC_SGTL5000=""
	CONFIG_SND_SOC_SIGMADSP=""
	CONFIG_CRC32=""
	CONFIG_SND_SOC_SN95031=""
	CONFIG_SND_SOC_SPDIF=""
	CONFIG_SND_SOC_SSM2602=""
	CONFIG_SND_SOC_STA32X=""
	CONFIG_SND_SOC_STAC9766=""
	CONFIG_SND_SOC_TLV320AIC23=""
	CONFIG_SND_SOC_TLV320AIC26=""
	CONFIG_SND_SOC_TLV320AIC32X4=""
	CONFIG_SND_SOC_TLV320AIC3X=""
	CONFIG_SND_SOC_TLV320DAC33=""
	CONFIG_SND_SOC_TWL4030=""
	CONFIG_MFD_TWL4030_AUDIO=""
	CONFIG_SND_SOC_TWL6040=""
	CONFIG_TWL6040_CORE=""
	CONFIG_SND_SOC_UDA134X=""
	CONFIG_SND_SOC_UDA1380=""
	CONFIG_SND_SOC_WL1273=""
	CONFIG_SND_SOC_WM1250_EV1=""
	CONFIG_SND_SOC_WM2000=""
	CONFIG_SND_SOC_WM5100=""
	CONFIG_SND_SOC_WM8350=""
	CONFIG_SND_SOC_WM8400=""
	CONFIG_SND_SOC_WM8510=""
	CONFIG_SND_SOC_WM8523=""
	CONFIG_SND_SOC_WM8580=""
	CONFIG_SND_SOC_WM8711=""
	CONFIG_SND_SOC_WM8727=""
	CONFIG_SND_SOC_WM8728=""
	CONFIG_SND_SOC_WM8731=""
	CONFIG_SND_SOC_WM8737=""
	CONFIG_SND_SOC_WM8741=""
	CONFIG_SND_SOC_WM8750=""
	CONFIG_SND_SOC_WM8753=""
	CONFIG_SND_SOC_WM8770=""
	CONFIG_SND_SOC_WM8776=""
	CONFIG_SND_SOC_WM8782=""
	CONFIG_SND_SOC_WM8804=""
	CONFIG_SND_SOC_WM8900=""
	CONFIG_SND_SOC_WM8903=""
	CONFIG_SND_SOC_WM8904=""
	CONFIG_SND_SOC_WM8940=""
	CONFIG_SND_SOC_WM8955=""
	CONFIG_SND_SOC_WM8960=""
	CONFIG_SND_SOC_WM8961=""
	CONFIG_SND_SOC_WM8962=""
	CONFIG_SND_SOC_WM8971=""
	CONFIG_SND_SOC_WM8974=""
	CONFIG_SND_SOC_WM8978=""
	CONFIG_SND_SOC_WM8983=""
	CONFIG_SND_SOC_WM8985=""
	CONFIG_SND_SOC_WM8988=""
	CONFIG_SND_SOC_WM8990=""
	CONFIG_SND_SOC_WM8991=""
	CONFIG_SND_SOC_WM8993=""
	CONFIG_SND_SOC_WM8994=""
	CONFIG_SND_SOC_WM8995=""
	CONFIG_SND_SOC_WM8996=""
	CONFIG_SND_SOC_WM9081=""
	CONFIG_SND_SOC_WM9090=""
	CONFIG_SND_SOC_WM9705=""
	CONFIG_SND_SOC_WM9712=""
	CONFIG_SND_SOC_WM9713=""
	CONFIG_SND_SOC_LM4857=""
	CONFIG_SND_SOC_MAX9877=""
	CONFIG_SND_SOC_TPA6130A2=""
	CONFIG_SOUND_PRIME=""
	CONFIG_AC97_BUS=""
])

AC_DEFUN([ALSA_TOPLEVEL_SELECT], [
dnl Check for which cards to compile driver for...
AC_MSG_CHECKING(for cards to compile driver for)
AC_ARG_WITH(cards,
  [  --with-cards=<list> compile driver for cards and options in <list>; ]
  [                        cards may be separated with commas; ]
  [                        'all' compiles all drivers; ]
  [                        Possible cards are: ]
  [                          seq-dummy, hrtimer, rtctimer, sbawe, emu10k1, ]
  [                          hpet, pcsp, dummy, aloop, virmidi, mtpav, mts64, ]
  [                          serial-u16550, mpu401, portman2x4, ml403-ac97cr, ]
  [                          serialmidi, loopback, adlib, ad1816a, ad1848, ]
  [                          als100, azt1605, azt2316, azt2320, cmi8330, ]
  [                          cs4231, cs4236, es1688, es18xx, sc6000, ]
  [                          gusclassic, gusextreme, gusmax, interwave, ]
  [                          interwave-stb, jazz16, opl3sa2, opti92x-ad1848, ]
  [                          opti92x-cs4231, opti93x, miro, sb8, sb16, sscape, ]
  [                          wavefront, msnd-pinnacle, msnd-classic, ]
  [                          pc98-cs4232, fm801, es1968, ad1889, als300, ]
  [                          als4000, ali5451, asihpi, atiixp, atiixp-modem, ]
  [                          au8810, au8820, au8830, aw2, azt3328, bt87x, ]
  [                          ca0106, cmipci, oxygen, cs4281, cs46xx, cs5530, ]
  [                          cs5535audio, ctxfi, darla20, gina20, layla20, ]
  [                          darla24, gina24, layla24, mona, mia, echo3g, ]
  [                          indigo, indigoio, indigodj, indigoiox, indigodjx, ]
  [                          emu10k1x, ens1370, ens1371, es1938, hda-intel, ]
  [                          hdsp, hdspm, ice1712, ice1724, intel8x0, ]
  [                          intel8x0m, korg1212, lola, lx6464es, maestro3, ]
  [                          mixart, nm256, pcxhr, riptide, rme32, rme96, ]
  [                          rme9652, sis7019, sonicvibes, trident, via82xx, ]
  [                          via82xx-modem, virtuoso, vx222, ymfpci, pdplus, ]
  [                          powermac, ps3, aoa, aoa-fabric-layout, aoa-onyx, ]
  [                          aoa-tas, aoa-toonie, aoa-soundbus, ]
  [                          aoa-soundbus-i2s, armaaci, pxa2xx-ac97, ]
  [                          sa11xx-uda1341, s3c2410, atmel-abdac, ]
  [                          atmel-ac97c, at73c213, sgi-o2, sgi-hal2, au1x00, ]
  [                          aica, sh-dac-audio, usb-audio, usb-ua101, ]
  [                          usb-usx2y, usb-caiaq, usb-us122l, usb-6fire, ]
  [                          firewire-speakers, isight, vxpocket, pdaudiocf, ]
  [                          sun-amd7930, sun-cs4231, sun-dbri, harmony, soc, ]
  [                          atmel-soc, at91-soc-sam9g20-wm8731, ]
  [                          at91-soc-afeb9260, soc-au1xpsc, soc-au1xaudio, ]
  [                          soc-db1000, soc-db1200, bf5xx-i2s, ]
  [                          bf5xx-soc-ssm2602, soc-bfin-eval-adau1701, ]
  [                          soc-bfin-eval-adau1373, soc-bfin-eval-adav80x, ]
  [                          bf5xx-soc-ad73311, bf5xx-tdm, bf5xx-soc-ad1836, ]
  [                          bf5xx-soc-ad193x, bf5xx-ac97, bf5xx-soc-ad1980, ]
  [                          davinci-soc, davinci-soc-evm, dm6467-soc-evm, ]
  [                          davinci-soc-sffsdr, da830-soc-evm, da850-soc-evm, ]
  [                          ep93xx-soc, ep93xx-soc-snappercl15, ]
  [                          ep93xx-soc-simone, ep93xx-soc-edb93xx, ]
  [                          soc-mpc8610-hpcd, soc-p1022-ds, soc-mpc5200-i2s, ]
  [                          soc-mpc5200-ac97, mpc52xx-soc-pcm030, ]
  [                          mpc52xx-soc-efika, imx-soc, mxc-soc-wm1133-ev1, ]
  [                          soc-mx27vis-aic32x4, soc-phycore-ac97, ]
  [                          soc-eukrea-tlv320, jz4740-soc, jz4740-soc-i2s, ]
  [                          jz4740-soc-qi-lb60, soc-nuc900, soc-nuc900evb, ]
  [                          omap-soc, omap-soc-n810, omap-soc-rx51, ]
  [                          omap-soc-ams-delta, omap-soc-osk5912, ]
  [                          omap-soc-overo, omap-soc-omap3evm, ]
  [                          omap-soc-am3517evm, omap-soc-sdp3430, ]
  [                          omap-soc-sdp4430, omap-soc-omap4-hdmi, ]
  [                          omap-soc-omap3-pandora, omap-soc-omap3-beagle, ]
  [                          omap-soc-zoom2, omap-soc-igep0020, kirkwood-soc, ]
  [                          kirkwood-soc-openrd, kirkwood-soc-t5325, ]
  [                          mfld-machine, mxs-soc, soc-mxs-sgtl5000, ]
  [                          pxa2xx-soc, pxa2xx-soc-corgi, pxa2xx-soc-spitz, ]
  [                          pxa2xx-soc-z2, pxa2xx-soc-poodle, ]
  [                          pxa2xx-soc-tosa, pxa2xx-soc-e740, ]
  [                          pxa2xx-soc-e750, pxa2xx-soc-e800, ]
  [                          pxa2xx-soc-em-x270, soc-saarb, soc-tavorevb3, ]
  [                          soc-zylonite, soc-raumfeld, pxa2xx-soc-hx4700, ]
  [                          pxa2xx-soc-magician, pxa2xx-soc-mioa701, ]
  [                          soc-samsung, soc-samsung-neo1973-wm8753, ]
  [                          soc-samsung-jive-wm8750, soc-samsung-smdk-wm8580, ]
  [                          soc-samsung-smdk-wm8994, ]
  [                          soc-samsung-smdk2443-wm9710, ]
  [                          soc-samsung-ln2440sbc-alc650, ]
  [                          soc-samsung-s3c24xx-uda134x, ]
  [                          soc-samsung-simtec-tlv320aic23, ]
  [                          soc-samsung-simtec-hermes, ]
  [                          soc-samsung-h1940-uda1380, ]
  [                          soc-samsung-rx1950-uda1380, ]
  [                          soc-samsung-smdk-wm9713, soc-smartq, ]
  [                          soc-goni-aquila-wm8994, soc-samsung-smdk-spdif, ]
  [                          soc-smdk-wm8580-pcm, soc-smdk-wm8994-pcm, ]
  [                          soc-speyside, soc-tobermory, soc-lowland, ]
  [                          soc-littlemill, s6000-soc, s6000-soc-s6ipcam, ]
  [                          soc-pcm-sh7760, soc-sh4-fsi, sh7760-ac97, ]
  [                          fsi-ak4642, fsi-da7210, fsi-hdmi, siu-migor, ]
  [                          soc-tegra, soc-tegra-wm8903, soc-tegra-trimslice, ]
  [                          soc-txx9aclc, soc-txx9aclc-generic, ]
  [                          soc-tlv320aic26 ],
  cards="$withval", cards="all")
SELECTED_CARDS=`echo $cards | sed 's/,/ /g'`
AC_MSG_RESULT($SELECTED_CARDS)
AC_MSG_CHECKING(for additonal options to compile driver for)
AC_ARG_WITH(card_options,
  [  --with-card-options=<list> enable driver options in <list>; ]
  [                        options may be separated with commas; ]
  [                        'all' enables all options; ]
  [                        Possible options are: ]
  [                          seq-hrtimer-default, seq-rtctimer-default, ]
  [                          support-old-api, pcm-xrun-debug, ac97-power-save, ]
  [                          sb16-csp, fm801-tea575x-bool, es1968-radio, ]
  [                          bt87x-overclock, cs46xx-new-dsp, es1968-input, ]
  [                          hda-hwdep, hda-reconfig, hda-input-beep, ]
  [                          hda-input-jack, hda-patch-loader, ]
  [                          hda-codec-realtek, hda-enable-realtek-quirks, ]
  [                          hda-codec-analog, hda-codec-sigmatel, ]
  [                          hda-codec-via, hda-codec-hdmi, hda-codec-cirrus, ]
  [                          hda-codec-conexant, hda-codec-ca0110, ]
  [                          hda-codec-ca0132, hda-codec-cmedia, ]
  [                          hda-codec-si3054, hda-generic, hda-power-save, ]
  [                          maestro3-input, powermac-auto-drc, ]
  [                          usb-caiaq-input, bf5xx-mmap-support, ]
  [                          bf5xx-multichan-support, bf5xx-have-cold-reset, ]
  [                          dm365-aic3x-codec, dm365-voice-codec, ]
  [                          pxa2xx-soc-palm27x ]
  [                        Possible integer options are: ]
  [                          ac97-power-save-default, hda-prealloc-size, ]
  [                          hda-input-beep-mode, hda-power-save-default, ]
  [                          ps3-default-start-delay, at73c213-target-bitrate, ]
  [                          bfin-ad73311-se, bf5xx-reset-gpio-num, ]
  [                          bf5xx-sport-num ],
  cards="$withval", cards="all")
SELECTED_OPTIONS=`echo $cards | sed 's/,/ /g'`
AC_MSG_RESULT($SELECTED_OPTIONS)
])
AC_DEFUN([ALSA_PARSE_KCONFIG], [
alsa_check_kconfig_card () {
  local pat=${1}
  for i in $SELECTED_CARDS; do
    case "$i" in
    $pat=n)
      return 1;;
    all|$pat|$pat=*)
      return 0;;
    esac
  done
  return 1
}
alsa_check_kconfig_option () {
  local pat=${1}
  for i in $SELECTED_OPTIONS; do
    case "$i" in
    $pat=n)
      return 1;;
    all|$pat|$pat=*)
      return 0;;
    esac
  done
  return 1
}
alsa_check_kconfig_int () {
  local pat=${1}
  for i in $SELECTED_OPTIONS; do
    case "$i" in
    $pat=*)
      int_val="${i#*=}"
      return 0;;
    esac
  done
  return 0
}
  CONFIG_SND="m"
  if alsa_check_kconfig_card "seq-dummy"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SEQUENCER" = "y" -o "$CONFIG_SND_SEQUENCER" = "m" ); then
      CONFIG_SND_SEQ_DUMMY="m"
    fi
  fi
  if alsa_check_kconfig_card "hrtimer"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_HIGH_RES_TIMERS" = "y" -o "$CONFIG_HIGH_RES_TIMERS" = "m" ) &&
      ( test $kvernum -ge 206027 ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HRTIMER="m"
    fi
  fi
  if alsa_check_kconfig_option "seq-hrtimer-default"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( ( test "$CONFIG_SND_HRTIMER" = "y" -o "$CONFIG_SND_HRTIMER" = "m" ) &&
      ( test "$CONFIG_SND_SEQUENCER" = "y" -o "$CONFIG_SND_SEQUENCER" = "m" ) ); then
      CONFIG_SND_SEQ_HRTIMER_DEFAULT="y"
    fi
  fi
  if alsa_check_kconfig_card "rtctimer"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_RTC" = "y" -o "$CONFIG_RTC" = "m" ) &&
      ( test $kvernum -ge 206015 ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_RTCTIMER="m"
    fi
  fi
  if alsa_check_kconfig_option "seq-rtctimer-default"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( ( test "$CONFIG_SND_RTCTIMER" = "y" -o "$CONFIG_SND_RTCTIMER" = "m" ) &&
      ( test "$CONFIG_SND_SEQUENCER" = "y" -o "$CONFIG_SND_SEQUENCER" = "m" ) ) &&
       ! ( test "$CONFIG_SND_SEQ_HRTIMER_DEFAULT" = "y" -o "$CONFIG_SND_SEQ_HRTIMER_DEFAULT" = "m" ); then
      CONFIG_SND_SEQ_RTCTIMER_DEFAULT="y"
    fi
  fi
  if alsa_check_kconfig_option "support-old-api"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ); then
      CONFIG_SND_SUPPORT_OLD_API="y"
    fi
  fi
  if alsa_check_kconfig_option "pcm-xrun-debug"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( ( test "$CONFIG_SND_DEBUG" = "y" -o "$CONFIG_SND_DEBUG" = "m" ) &&
      ( test "$CONFIG_SND_VERBOSE_PROCFS" = "y" -o "$CONFIG_SND_VERBOSE_PROCFS" = "m" ) ); then
      CONFIG_SND_PCM_XRUN_DEBUG="y"
    fi
  fi
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_X86" = "y" -o "$CONFIG_X86" = "m" ); then
      CONFIG_SND_DMA_SGBUF="y"
    fi
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( ( test "$CONFIG_SND_SEQUENCER" = "y" -o "$CONFIG_SND_SEQUENCER" = "m" ) &&
      ( test "$CONFIG_SND_RAWMIDI" = "y" -o "$CONFIG_SND_RAWMIDI" = "m" ) ); then
      CONFIG_SND_RAWMIDI_SEQ="m"
    fi
  if test "$CONFIG_CONFIG_SND_OPL3_LIB" = "m" -o "$CONFIG_CONFIG_SND_OPL3_LIB" = "y"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
    fi
  fi
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( ( test "$CONFIG_SND_SEQUENCER" = "y" -o "$CONFIG_SND_SEQUENCER" = "m" ) &&
      ( test "$CONFIG_SND_OPL3_LIB" = "y" -o "$CONFIG_SND_OPL3_LIB" = "m" ) ); then
      CONFIG_SND_OPL3_LIB_SEQ="m"
    fi
  if test "$CONFIG_CONFIG_SND_OPL4_LIB" = "m" -o "$CONFIG_CONFIG_SND_OPL4_LIB" = "y"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
    fi
  fi
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( ( test "$CONFIG_SND_SEQUENCER" = "y" -o "$CONFIG_SND_SEQUENCER" = "m" ) &&
      ( test "$CONFIG_SND_OPL4_LIB" = "y" -o "$CONFIG_SND_OPL4_LIB" = "m" ) ); then
      CONFIG_SND_OPL4_LIB_SEQ="m"
    fi
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( ( test "$CONFIG_ISA" = "y" -o "$CONFIG_ISA" = "m" ) &&
      ( test "$CONFIG_ISA_DMA_API" = "y" -o "$CONFIG_ISA_DMA_API" = "m" ) ); then
      CONFIG_SND_ISA="y"
    fi
  if alsa_check_kconfig_card "sbawe"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_SB_COMMON="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_SB16_DSP="m"
      CONFIG_SND_SBAWE="m"
    fi
  fi
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( ( test "$CONFIG_SND_SEQUENCER" = "y" -o "$CONFIG_SND_SEQUENCER" = "m" ) &&
      ( test "$CONFIG_SND_SBAWE" = "y" -o "$CONFIG_SND_SBAWE" = "m" ) ); then
      CONFIG_SND_SBAWE_SEQ="m"
    fi
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ); then
      CONFIG_SND_PCI="y"
    fi
  if alsa_check_kconfig_card "emu10k1"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_FW_LOADER" = "y" -o "$CONFIG_FW_LOADER" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kvernum -ge 206014 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_EMU10K1="m"
    fi
  fi
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( ( test "$CONFIG_SND_SEQUENCER" = "y" -o "$CONFIG_SND_SEQUENCER" = "m" ) &&
      ( test "$CONFIG_SND_EMU10K1" = "y" -o "$CONFIG_SND_EMU10K1" = "m" ) ); then
      CONFIG_SND_EMU10K1_SEQ="m"
    fi
  if alsa_check_kconfig_card "hpet"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( ( test "$CONFIG_HPET" = "y" -o "$CONFIG_HPET" = "m" ) &&
      ( test "$CONFIG_BROKEN" = "y" -o "$CONFIG_BROKEN" = "m" ) ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HPET="m"
    fi
  fi
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ); then
      CONFIG_SND_DRIVERS="y"
    fi
  if alsa_check_kconfig_card "pcsp"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_DRIVERS" = "y" -o "$CONFIG_SND_DRIVERS" = "m" ) &&
      ( ( test "$CONFIG_PCSPKR_PLATFORM" = "y" -o "$CONFIG_PCSPKR_PLATFORM" = "m" ) &&
      ( test "$CONFIG_X86" = "y" -o "$CONFIG_X86" = "m" ) &&
      ( test "$CONFIG_HIGH_RES_TIMERS" = "y" -o "$CONFIG_HIGH_RES_TIMERS" = "m" ) ) &&
      ( test "$CONFIG_INPUT" = "y" -o "$CONFIG_INPUT" = "m" ) &&
      ( test "$CONFIG_EXPERIMENTAL" = "y" -o "$CONFIG_EXPERIMENTAL" = "m" ) &&
      ( test $kvernum -ge 206027 ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_PCSP="m"
    fi
  fi
  if alsa_check_kconfig_card "dummy"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_DRIVERS" = "y" -o "$CONFIG_SND_DRIVERS" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_DUMMY="m"
    fi
  fi
  if alsa_check_kconfig_card "aloop"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_DRIVERS" = "y" -o "$CONFIG_SND_DRIVERS" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_ALOOP="m"
    fi
  fi
  if alsa_check_kconfig_card "virmidi"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_DRIVERS" = "y" -o "$CONFIG_SND_DRIVERS" = "m" ) &&
      ( test "$CONFIG_SND_SEQUENCER" = "y" -o "$CONFIG_SND_SEQUENCER" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_VIRMIDI="m"
    fi
  fi
  if alsa_check_kconfig_card "mtpav"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_DRIVERS" = "y" -o "$CONFIG_SND_DRIVERS" = "m" ); then
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_MTPAV="m"
    fi
  fi
  if alsa_check_kconfig_card "mts64"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_DRIVERS" = "y" -o "$CONFIG_SND_DRIVERS" = "m" ) &&
      ( test "$CONFIG_PARPORT" = "y" -o "$CONFIG_PARPORT" = "m" ) &&
      ( test $kvernum -ge 206010 ); then
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_MTS64="m"
    fi
  fi
  if alsa_check_kconfig_card "serial-u16550"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_DRIVERS" = "y" -o "$CONFIG_SND_DRIVERS" = "m" ); then
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_SERIAL_U16550="m"
    fi
  fi
  if alsa_check_kconfig_card "mpu401"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_DRIVERS" = "y" -o "$CONFIG_SND_DRIVERS" = "m" ); then
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_MPU401="m"
    fi
  fi
  if alsa_check_kconfig_card "portman2x4"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_DRIVERS" = "y" -o "$CONFIG_SND_DRIVERS" = "m" ) &&
      ( test "$CONFIG_PARPORT" = "y" -o "$CONFIG_PARPORT" = "m" ) &&
      ( test $kvernum -ge 206010 ); then
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PORTMAN2X4="m"
    fi
  fi
  if alsa_check_kconfig_card "ml403-ac97cr"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_DRIVERS" = "y" -o "$CONFIG_SND_DRIVERS" = "m" ) &&
      ( test "$CONFIG_XILINX_VIRTEX" = "y" -o "$CONFIG_XILINX_VIRTEX" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kvernum -ge 206014 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_ML403_AC97CR="m"
    fi
  fi
  if test "$CONFIG_CONFIG_SND_AC97_CODEC" = "m" -o "$CONFIG_CONFIG_SND_AC97_CODEC" = "y"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kvernum -ge 206014 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
    fi
  fi
  if alsa_check_kconfig_option "ac97-power-save"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_DRIVERS" = "y" -o "$CONFIG_SND_DRIVERS" = "m" ) &&
      ( test "$CONFIG_SND_AC97_CODEC" = "y" -o "$CONFIG_SND_AC97_CODEC" = "m" ); then
      CONFIG_SND_AC97_POWER_SAVE="y"
    fi
  fi
  int_val=0
  if alsa_check_kconfig_int "ac97-power-save-default"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_DRIVERS" = "y" -o "$CONFIG_SND_DRIVERS" = "m" ) &&
      ( test "$CONFIG_SND_AC97_POWER_SAVE" = "y" -o "$CONFIG_SND_AC97_POWER_SAVE" = "m" ); then
      CONFIG_SND_AC97_POWER_SAVE_DEFAULT=$int_val
    fi
  fi
  if alsa_check_kconfig_card "serialmidi"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_DRIVERS" = "y" -o "$CONFIG_SND_DRIVERS" = "m" ) &&
      ( test "$CONFIG_BROKEN" = "y" -o "$CONFIG_BROKEN" = "m" ); then
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_SERIALMIDI="m"
    fi
  fi
  if alsa_check_kconfig_card "loopback"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_DRIVERS" = "y" -o "$CONFIG_SND_DRIVERS" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_LOOPBACK="m"
    fi
  fi
  if alsa_check_kconfig_card "adlib"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_ADLIB="m"
    fi
  fi
  if alsa_check_kconfig_card "ad1816a"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ) &&
      ( test "$CONFIG_PNP" = "y" -o "$CONFIG_PNP" = "m" ) &&
      ( test "$CONFIG_ISAPNP" = "y" -o "$CONFIG_ISAPNP" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_AD1816A="m"
    fi
  fi
  if alsa_check_kconfig_card "ad1848"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_WSS_LIB="m"
      CONFIG_SND_AD1848="m"
    fi
  fi
  if alsa_check_kconfig_card "als100"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ) &&
      ( test "$CONFIG_PNP" = "y" -o "$CONFIG_PNP" = "m" ) &&
      ( test "$CONFIG_ISAPNP" = "y" -o "$CONFIG_ISAPNP" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_SB_COMMON="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_SB16_DSP="m"
      CONFIG_SND_ALS100="m"
    fi
  fi
  if alsa_check_kconfig_card "azt1605"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_WSS_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_AZT1605="m"
    fi
  fi
  if alsa_check_kconfig_card "azt2316"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_WSS_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_AZT2316="m"
    fi
  fi
  if alsa_check_kconfig_card "azt2320"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ) &&
      ( test "$CONFIG_PNP" = "y" -o "$CONFIG_PNP" = "m" ) &&
      ( test "$CONFIG_ISAPNP" = "y" -o "$CONFIG_ISAPNP" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_WSS_LIB="m"
      CONFIG_SND_AZT2320="m"
    fi
  fi
  if alsa_check_kconfig_card "cmi8330"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_SB_COMMON="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_WSS_LIB="m"
      CONFIG_SND_SB16_DSP="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_CMI8330="m"
    fi
  fi
  if alsa_check_kconfig_card "cs4231"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ); then
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_WSS_LIB="m"
      CONFIG_SND_CS4231="m"
    fi
  fi
  if alsa_check_kconfig_card "cs4236"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_WSS_LIB="m"
      CONFIG_SND_CS4236="m"
    fi
  fi
  if alsa_check_kconfig_card "es1688"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_ES1688="m"
    fi
  fi
  if alsa_check_kconfig_card "es18xx"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_ES18XX="m"
    fi
  fi
  if alsa_check_kconfig_card "sc6000"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ) &&
      ( test "$CONFIG_HAS_IOPORT" = "y" -o "$CONFIG_HAS_IOPORT" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_WSS_LIB="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_SC6000="m"
    fi
  fi
  if alsa_check_kconfig_card "gusclassic"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_GUSCLASSIC="m"
    fi
  fi
  if alsa_check_kconfig_card "gusextreme"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_GUSEXTREME="m"
    fi
  fi
  if alsa_check_kconfig_card "gusmax"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_WSS_LIB="m"
      CONFIG_SND_GUSMAX="m"
    fi
  fi
  if alsa_check_kconfig_card "interwave"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ) &&
      ( test "$CONFIG_PNP" = "y" -o "$CONFIG_PNP" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_WSS_LIB="m"
      CONFIG_SND_INTERWAVE="m"
    fi
  fi
  if alsa_check_kconfig_card "interwave-stb"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ) &&
      ( test "$CONFIG_PNP" = "y" -o "$CONFIG_PNP" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_WSS_LIB="m"
      CONFIG_SND_INTERWAVE_STB="m"
    fi
  fi
  if alsa_check_kconfig_card "jazz16"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_SB_COMMON="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_SB8_DSP="m"
      CONFIG_SND_JAZZ16="m"
    fi
  fi
  if alsa_check_kconfig_card "opl3sa2"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_WSS_LIB="m"
      CONFIG_SND_OPL3SA2="m"
    fi
  fi
  if alsa_check_kconfig_card "opti92x-ad1848"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_OPL4_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_WSS_LIB="m"
      CONFIG_SND_OPTI92X_AD1848="m"
    fi
  fi
  if alsa_check_kconfig_card "opti92x-cs4231"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_OPL4_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_WSS_LIB="m"
      CONFIG_SND_OPTI92X_CS4231="m"
    fi
  fi
  if alsa_check_kconfig_card "opti93x"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_WSS_LIB="m"
      CONFIG_SND_OPTI93X="m"
    fi
  fi
  if alsa_check_kconfig_card "miro"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_OPL4_LIB="m"
      CONFIG_SND_WSS_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_MIRO="m"
    fi
  fi
  if alsa_check_kconfig_card "sb8"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_SB_COMMON="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_SB8_DSP="m"
      CONFIG_SND_SB8="m"
    fi
  fi
  if alsa_check_kconfig_card "sb16"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_SB_COMMON="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_SB16_DSP="m"
      CONFIG_SND_SB16="m"
    fi
  fi
  if alsa_check_kconfig_option "sb16-csp"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ) &&
      ( ( ( test "$CONFIG_SND_SB16" = "y" -o "$CONFIG_SND_SB16" = "m" ) ||
      ( test "$CONFIG_SND_SBAWE" = "y" -o "$CONFIG_SND_SBAWE" = "m" ) ) &&
      ( ( test "$CONFIG_BROKEN" = "y" -o "$CONFIG_BROKEN" = "m" ) ||
       ! ( test "$CONFIG_PPC" = "y" -o "$CONFIG_PPC" = "m" ) ) ) &&
      ( test "$CONFIG_FW_LOADER" = "y" -o "$CONFIG_FW_LOADER" = "m" ); then
      CONFIG_SND_SB16_CSP="y"
    fi
  fi
  if alsa_check_kconfig_card "sscape"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ) &&
      ( test "$CONFIG_FW_LOADER" = "y" -o "$CONFIG_FW_LOADER" = "m" ); then
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_WSS_LIB="m"
      CONFIG_SND_SSCAPE="m"
    fi
  fi
  if alsa_check_kconfig_card "wavefront"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ) &&
      ( test "$CONFIG_FW_LOADER" = "y" -o "$CONFIG_FW_LOADER" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_WSS_LIB="m"
      CONFIG_SND_WAVEFRONT="m"
    fi
  fi
  if alsa_check_kconfig_card "msnd-pinnacle"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ) &&
      ( ( test "$CONFIG_X86" = "y" -o "$CONFIG_X86" = "m" ) &&
      ( test "$CONFIG_EXPERIMENTAL" = "y" -o "$CONFIG_EXPERIMENTAL" = "m" ) ) &&
      ( test "$CONFIG_FW_LOADER" = "y" -o "$CONFIG_FW_LOADER" = "m" ) &&
      ( test $kvernum -ge 206000 ); then
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_TIMER="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_MSND_PINNACLE="m"
    fi
  fi
  if alsa_check_kconfig_card "msnd-classic"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ) &&
      ( ( test "$CONFIG_X86" = "y" -o "$CONFIG_X86" = "m" ) &&
      ( test "$CONFIG_EXPERIMENTAL" = "y" -o "$CONFIG_EXPERIMENTAL" = "m" ) ) &&
      ( test "$CONFIG_FW_LOADER" = "y" -o "$CONFIG_FW_LOADER" = "m" ) &&
      ( test $kvernum -ge 206000 ); then
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_TIMER="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_MSND_CLASSIC="m"
    fi
  fi
  if alsa_check_kconfig_card "pc98-cs4232"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ) &&
      ( test "$CONFIG_X86_PC9800" = "y" -o "$CONFIG_X86_PC9800" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_PC98_CS4232="m"
    fi
  fi
  if alsa_check_kconfig_card "fm801"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      test $kvernum -ge 206014 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_FM801="m"
    fi
  fi
  if alsa_check_kconfig_option "fm801-tea575x-bool"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_SND_FM801" = "y" -o "$CONFIG_SND_FM801" = "m" ) &&
      ( test "$CONFIG_VIDEO_V4L2" = "y" -o "$CONFIG_VIDEO_V4L2" = "m" ) &&
      ( test $kvernum -ge 206027 ); then
      CONFIG_SND_FM801_TEA575X_BOOL="y"
    fi
  fi
  if alsa_check_kconfig_card "es1968"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kvernum -ge 206014 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_ES1968="m"
    fi
  fi
  if alsa_check_kconfig_option "es1968-radio"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_SND_ES1968" = "y" -o "$CONFIG_SND_ES1968" = "m" ) &&
      ( test "$CONFIG_VIDEO_V4L2" = "y" -o "$CONFIG_VIDEO_V4L2" = "m" ) &&
      ( test $kvernum -ge 206027 ); then
      CONFIG_SND_ES1968_RADIO="y"
    fi
  fi
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( ( test "$CONFIG_SND_FM801_TEA575X_BOOL" = "y" -o "$CONFIG_SND_FM801_TEA575X_BOOL" = "m" ) ||
      ( test "$CONFIG_SND_ES1968_RADIO" = "y" -o "$CONFIG_SND_ES1968_RADIO" = "m" ) ||
      ( test "$CONFIG_RADIO_SF16FMR2" = "y" -o "$CONFIG_RADIO_SF16FMR2" = "m" ) ) &&
      ( ( test "$CONFIG_SND_FM801" = "y" -o "$CONFIG_SND_FM801" = "m" ) ||
      ( test "$CONFIG_SND_ES1968" = "y" -o "$CONFIG_SND_ES1968" = "m" ) ||
      ( test "$CONFIG_RADIO_SF16FMR2" = "y" -o "$CONFIG_RADIO_SF16FMR2" = "m" ) ); then
      CONFIG_SND_TEA575X="m"
    fi
  if alsa_check_kconfig_card "ad1889"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kvernum -ge 206014 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_AD1889="m"
    fi
  fi
  if alsa_check_kconfig_card "als300"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kvernum -ge 206014 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_ALS300="m"
    fi
  fi
  if alsa_check_kconfig_card "als4000"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_ISA_DMA_API" = "y" -o "$CONFIG_ISA_DMA_API" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_SB_COMMON="m"
      CONFIG_SND_ALS4000="m"
    fi
  fi
  if alsa_check_kconfig_card "ali5451"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kvernum -ge 206014 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_ALI5451="m"
    fi
  fi
  if alsa_check_kconfig_card "asihpi"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_X86" = "y" -o "$CONFIG_X86" = "m" ) &&
      ( test "$CONFIG_FW_LOADER" = "y" -o "$CONFIG_FW_LOADER" = "m" ) &&
      ( test $kvernum -ge 206010 ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_ASIHPI="m"
    fi
  fi
  if alsa_check_kconfig_card "atiixp"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kvernum -ge 206014 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_ATIIXP="m"
    fi
  fi
  if alsa_check_kconfig_card "atiixp-modem"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kvernum -ge 206014 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_ATIIXP_MODEM="m"
    fi
  fi
  if alsa_check_kconfig_card "au8810"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kvernum -ge 206014 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_AU8810="m"
    fi
  fi
  if alsa_check_kconfig_card "au8820"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kvernum -ge 206014 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_AU8820="m"
    fi
  fi
  if alsa_check_kconfig_card "au8830"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kvernum -ge 206014 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_AU8830="m"
    fi
  fi
  if alsa_check_kconfig_card "aw2"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_AW2="m"
    fi
  fi
  if alsa_check_kconfig_card "azt3328"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      test $kvernum -ge 206014 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_AZT3328="m"
    fi
  fi
  if alsa_check_kconfig_card "bt87x"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_BT87X="m"
    fi
  fi
  if alsa_check_kconfig_option "bt87x-overclock"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_SND_BT87X" = "y" -o "$CONFIG_SND_BT87X" = "m" ); then
      CONFIG_SND_BT87X_OVERCLOCK="y"
    fi
  fi
  if alsa_check_kconfig_card "ca0106"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kvernum -ge 206014 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_CA0106="m"
    fi
  fi
  if alsa_check_kconfig_card "cmipci"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_CMIPCI="m"
    fi
  fi
  if alsa_check_kconfig_card "oxygen"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_OXYGEN_LIB="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_OXYGEN="m"
    fi
  fi
  if alsa_check_kconfig_card "cs4281"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_PCM="m"
      test $kvernum -ge 206014 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_CS4281="m"
    fi
  fi
  if alsa_check_kconfig_card "cs46xx"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kvernum -ge 206014 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_CS46XX="m"
    fi
  fi
  if alsa_check_kconfig_option "cs46xx-new-dsp"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_SND_CS46XX" = "y" -o "$CONFIG_SND_CS46XX" = "m" ); then
      CONFIG_SND_CS46XX_NEW_DSP="y"
    fi
  fi
  if alsa_check_kconfig_card "cs5530"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_ISA_DMA_API" = "y" -o "$CONFIG_ISA_DMA_API" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_SB_COMMON="m"
      CONFIG_SND_SB16_DSP="m"
      CONFIG_SND_CS5530="m"
    fi
  fi
  if alsa_check_kconfig_card "cs5535audio"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test $kvernum -ge 206010 ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kvernum -ge 206014 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_PCM="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_CS5535AUDIO="m"
    fi
  fi
  if alsa_check_kconfig_card "ctxfi"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_CTXFI="m"
    fi
  fi
  if alsa_check_kconfig_card "darla20"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_FW_LOADER" = "y" -o "$CONFIG_FW_LOADER" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_DARLA20="m"
    fi
  fi
  if alsa_check_kconfig_card "gina20"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_FW_LOADER" = "y" -o "$CONFIG_FW_LOADER" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_GINA20="m"
    fi
  fi
  if alsa_check_kconfig_card "layla20"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_FW_LOADER" = "y" -o "$CONFIG_FW_LOADER" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_LAYLA20="m"
    fi
  fi
  if alsa_check_kconfig_card "darla24"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_FW_LOADER" = "y" -o "$CONFIG_FW_LOADER" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_DARLA24="m"
    fi
  fi
  if alsa_check_kconfig_card "gina24"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_FW_LOADER" = "y" -o "$CONFIG_FW_LOADER" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_GINA24="m"
    fi
  fi
  if alsa_check_kconfig_card "layla24"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_FW_LOADER" = "y" -o "$CONFIG_FW_LOADER" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_LAYLA24="m"
    fi
  fi
  if alsa_check_kconfig_card "mona"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_FW_LOADER" = "y" -o "$CONFIG_FW_LOADER" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_MONA="m"
    fi
  fi
  if alsa_check_kconfig_card "mia"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_FW_LOADER" = "y" -o "$CONFIG_FW_LOADER" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_MIA="m"
    fi
  fi
  if alsa_check_kconfig_card "echo3g"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_FW_LOADER" = "y" -o "$CONFIG_FW_LOADER" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_ECHO3G="m"
    fi
  fi
  if alsa_check_kconfig_card "indigo"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_FW_LOADER" = "y" -o "$CONFIG_FW_LOADER" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_INDIGO="m"
    fi
  fi
  if alsa_check_kconfig_card "indigoio"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_FW_LOADER" = "y" -o "$CONFIG_FW_LOADER" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_INDIGOIO="m"
    fi
  fi
  if alsa_check_kconfig_card "indigodj"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_FW_LOADER" = "y" -o "$CONFIG_FW_LOADER" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_INDIGODJ="m"
    fi
  fi
  if alsa_check_kconfig_card "indigoiox"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_FW_LOADER" = "y" -o "$CONFIG_FW_LOADER" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_INDIGOIOX="m"
    fi
  fi
  if alsa_check_kconfig_card "indigodjx"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_FW_LOADER" = "y" -o "$CONFIG_FW_LOADER" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_INDIGODJX="m"
    fi
  fi
  if alsa_check_kconfig_card "emu10k1x"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kvernum -ge 206014 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_EMU10K1X="m"
    fi
  fi
  if alsa_check_kconfig_card "ens1370"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_ENS1370="m"
    fi
  fi
  if alsa_check_kconfig_card "ens1371"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kvernum -ge 206014 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_ENS1371="m"
    fi
  fi
  if alsa_check_kconfig_card "es1938"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      test $kvernum -ge 206014 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_ES1938="m"
    fi
  fi
  if alsa_check_kconfig_option "es1968-input"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_SND_ES1968" = "y" -o "$CONFIG_SND_ES1968" = "m" ) &&
      ( test "$CONFIG_INPUT" = "y" -o "$CONFIG_INPUT" = "m" ) &&
      ( test $kvernum -ge 206023 ); then
      CONFIG_SND_ES1968_INPUT="y"
    fi
  fi
  if alsa_check_kconfig_card "hda-intel"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test $kvernum -ge 206000 ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_KCTL_JACK="y"
      CONFIG_SND_HDA_INTEL="m"
    fi
  fi
  int_val=64
  if alsa_check_kconfig_int "hda-prealloc-size"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_SND_HDA_INTEL" = "y" -o "$CONFIG_SND_HDA_INTEL" = "m" ); then
      CONFIG_SND_HDA_PREALLOC_SIZE=$int_val
    fi
  fi
  if alsa_check_kconfig_option "hda-hwdep"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_SND_HDA_INTEL" = "y" -o "$CONFIG_SND_HDA_INTEL" = "m" ); then
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_HDA_HWDEP="y"
    fi
  fi
  if alsa_check_kconfig_option "hda-reconfig"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_SND_HDA_INTEL" = "y" -o "$CONFIG_SND_HDA_INTEL" = "m" ) &&
      ( ( test "$CONFIG_SND_HDA_HWDEP" = "y" -o "$CONFIG_SND_HDA_HWDEP" = "m" ) &&
      ( test "$CONFIG_EXPERIMENTAL" = "y" -o "$CONFIG_EXPERIMENTAL" = "m" ) ); then
      CONFIG_SND_HDA_RECONFIG="y"
    fi
  fi
  if alsa_check_kconfig_option "hda-input-beep"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_SND_HDA_INTEL" = "y" -o "$CONFIG_SND_HDA_INTEL" = "m" ) &&
      ( test "$CONFIG_INPUT" = "y" -o "$CONFIG_INPUT" = "m" ) &&
      ( test $kvernum -ge 206000 ); then
      CONFIG_SND_HDA_INPUT_BEEP="y"
    fi
  fi
  int_val=1
  if alsa_check_kconfig_int "hda-input-beep-mode"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_SND_HDA_INTEL" = "y" -o "$CONFIG_SND_HDA_INTEL" = "m" ) &&
      ( test "$CONFIG_SND_HDA_INPUT_BEEP" = "y" -o "$CONFIG_SND_HDA_INPUT_BEEP" = "m" ); then
      CONFIG_SND_HDA_INPUT_BEEP_MODE=$int_val
    fi
  fi
  if alsa_check_kconfig_option "hda-input-jack"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_SND_HDA_INTEL" = "y" -o "$CONFIG_SND_HDA_INTEL" = "m" ) &&
      ( test "$CONFIG_INPUT" = "y" -o "$CONFIG_INPUT" = "m" ); then
      test $kvernum -ge 206027 && CONFIG_SND_JACK="y"
      CONFIG_SND_HDA_INPUT_JACK="y"
    fi
  fi
  if alsa_check_kconfig_option "hda-patch-loader"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_SND_HDA_INTEL" = "y" -o "$CONFIG_SND_HDA_INTEL" = "m" ) &&
      ( test "$CONFIG_EXPERIMENTAL" = "y" -o "$CONFIG_EXPERIMENTAL" = "m" ) &&
      ( test "$CONFIG_FW_LOADER" = "y" -o "$CONFIG_FW_LOADER" = "m" ); then
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_HDA_HWDEP="y"
      CONFIG_SND_HDA_RECONFIG="y"
      CONFIG_SND_HDA_PATCH_LOADER="y"
    fi
  fi
  if alsa_check_kconfig_option "hda-codec-realtek"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_SND_HDA_INTEL" = "y" -o "$CONFIG_SND_HDA_INTEL" = "m" ); then
      CONFIG_SND_HDA_CODEC_REALTEK="y"
    fi
  fi
  if alsa_check_kconfig_option "hda-enable-realtek-quirks"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_SND_HDA_INTEL" = "y" -o "$CONFIG_SND_HDA_INTEL" = "m" ) &&
      ( test "$CONFIG_SND_HDA_CODEC_REALTEK" = "y" -o "$CONFIG_SND_HDA_CODEC_REALTEK" = "m" ); then
      CONFIG_SND_HDA_ENABLE_REALTEK_QUIRKS="y"
    fi
  fi
  if alsa_check_kconfig_option "hda-codec-analog"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_SND_HDA_INTEL" = "y" -o "$CONFIG_SND_HDA_INTEL" = "m" ); then
      CONFIG_SND_HDA_CODEC_ANALOG="y"
    fi
  fi
  if alsa_check_kconfig_option "hda-codec-sigmatel"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_SND_HDA_INTEL" = "y" -o "$CONFIG_SND_HDA_INTEL" = "m" ); then
      CONFIG_SND_HDA_CODEC_SIGMATEL="y"
    fi
  fi
  if alsa_check_kconfig_option "hda-codec-via"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_SND_HDA_INTEL" = "y" -o "$CONFIG_SND_HDA_INTEL" = "m" ); then
      CONFIG_SND_HDA_CODEC_VIA="y"
    fi
  fi
  if alsa_check_kconfig_option "hda-codec-hdmi"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_SND_HDA_INTEL" = "y" -o "$CONFIG_SND_HDA_INTEL" = "m" ); then
      test $kvernum -ge 206000 && CONFIG_SND_DYNAMIC_MINORS="y"
      CONFIG_SND_HDA_CODEC_HDMI="y"
    fi
  fi
  if alsa_check_kconfig_option "hda-codec-cirrus"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_SND_HDA_INTEL" = "y" -o "$CONFIG_SND_HDA_INTEL" = "m" ) &&
      ( test "$CONFIG_SND_HDA_INTEL" = "y" -o "$CONFIG_SND_HDA_INTEL" = "m" ); then
      CONFIG_SND_HDA_CODEC_CIRRUS="y"
    fi
  fi
  if alsa_check_kconfig_option "hda-codec-conexant"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_SND_HDA_INTEL" = "y" -o "$CONFIG_SND_HDA_INTEL" = "m" ); then
      CONFIG_SND_HDA_CODEC_CONEXANT="y"
    fi
  fi
  if alsa_check_kconfig_option "hda-codec-ca0110"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_SND_HDA_INTEL" = "y" -o "$CONFIG_SND_HDA_INTEL" = "m" ) &&
      ( test "$CONFIG_SND_HDA_INTEL" = "y" -o "$CONFIG_SND_HDA_INTEL" = "m" ); then
      CONFIG_SND_HDA_CODEC_CA0110="y"
    fi
  fi
  if alsa_check_kconfig_option "hda-codec-ca0132"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_SND_HDA_INTEL" = "y" -o "$CONFIG_SND_HDA_INTEL" = "m" ) &&
      ( test "$CONFIG_SND_HDA_INTEL" = "y" -o "$CONFIG_SND_HDA_INTEL" = "m" ); then
      CONFIG_SND_HDA_CODEC_CA0132="y"
    fi
  fi
  if alsa_check_kconfig_option "hda-codec-cmedia"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_SND_HDA_INTEL" = "y" -o "$CONFIG_SND_HDA_INTEL" = "m" ); then
      CONFIG_SND_HDA_CODEC_CMEDIA="y"
    fi
  fi
  if alsa_check_kconfig_option "hda-codec-si3054"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_SND_HDA_INTEL" = "y" -o "$CONFIG_SND_HDA_INTEL" = "m" ); then
      CONFIG_SND_HDA_CODEC_SI3054="y"
    fi
  fi
  if alsa_check_kconfig_option "hda-generic"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_SND_HDA_INTEL" = "y" -o "$CONFIG_SND_HDA_INTEL" = "m" ); then
      CONFIG_SND_HDA_GENERIC="y"
    fi
  fi
  if alsa_check_kconfig_option "hda-power-save"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_SND_HDA_INTEL" = "y" -o "$CONFIG_SND_HDA_INTEL" = "m" ) &&
      ( test "$CONFIG_PM" = "y" -o "$CONFIG_PM" = "m" ); then
      CONFIG_SND_HDA_POWER_SAVE="y"
    fi
  fi
  int_val=0
  if alsa_check_kconfig_int "hda-power-save-default"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_SND_HDA_INTEL" = "y" -o "$CONFIG_SND_HDA_INTEL" = "m" ) &&
      ( test "$CONFIG_SND_HDA_POWER_SAVE" = "y" -o "$CONFIG_SND_HDA_POWER_SAVE" = "m" ); then
      CONFIG_SND_HDA_POWER_SAVE_DEFAULT=$int_val
    fi
  fi
  if alsa_check_kconfig_card "hdsp"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_HDSP="m"
    fi
  fi
  if alsa_check_kconfig_card "hdspm"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_HDSPM="m"
    fi
  fi
  if alsa_check_kconfig_card "ice1712"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kvernum -ge 206014 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_ICE1712="m"
    fi
  fi
  if alsa_check_kconfig_card "ice1724"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kvernum -ge 206014 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_ICE1724="m"
    fi
  fi
  if alsa_check_kconfig_card "intel8x0"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kvernum -ge 206014 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_INTEL8X0="m"
    fi
  fi
  if alsa_check_kconfig_card "intel8x0m"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kvernum -ge 206014 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_INTEL8X0M="m"
    fi
  fi
  if alsa_check_kconfig_card "korg1212"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_KORG1212="m"
    fi
  fi
  if alsa_check_kconfig_card "lola"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_LOLA="m"
    fi
  fi
  if alsa_check_kconfig_card "lx6464es"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test $kvernum -ge 206000 ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_LX6464ES="m"
    fi
  fi
  if alsa_check_kconfig_card "maestro3"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kvernum -ge 206014 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_MAESTRO3="m"
    fi
  fi
  if alsa_check_kconfig_option "maestro3-input"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_SND_MAESTRO3" = "y" -o "$CONFIG_SND_MAESTRO3" = "m" ) &&
      ( test "$CONFIG_INPUT" = "y" -o "$CONFIG_INPUT" = "m" ) &&
      ( test $kvernum -ge 206023 ); then
      CONFIG_SND_MAESTRO3_INPUT="y"
    fi
  fi
  if alsa_check_kconfig_card "mixart"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_MIXART="m"
    fi
  fi
  if alsa_check_kconfig_card "nm256"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kvernum -ge 206014 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_NM256="m"
    fi
  fi
  if alsa_check_kconfig_card "pcxhr"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_PCXHR="m"
    fi
  fi
  if alsa_check_kconfig_card "riptide"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_FW_LOADER" = "y" -o "$CONFIG_FW_LOADER" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      test $kvernum -ge 206014 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_RIPTIDE="m"
    fi
  fi
  if alsa_check_kconfig_card "rme32"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_RME32="m"
    fi
  fi
  if alsa_check_kconfig_card "rme96"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_RME96="m"
    fi
  fi
  if alsa_check_kconfig_card "rme9652"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_RME9652="m"
    fi
  fi
  if alsa_check_kconfig_card "sis7019"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( ( test "$CONFIG_X86" = "y" -o "$CONFIG_X86" = "m" ) &&
       ! ( test "$CONFIG_X86_64" = "y" -o "$CONFIG_X86_64" = "m" ) ) &&
      ( test $kvernum -ge 206000 ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kvernum -ge 206014 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_SIS7019="m"
    fi
  fi
  if alsa_check_kconfig_card "sonicvibes"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      test $kvernum -ge 206014 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_SONICVIBES="m"
    fi
  fi
  if alsa_check_kconfig_card "trident"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kvernum -ge 206014 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_TRIDENT="m"
    fi
  fi
  if alsa_check_kconfig_card "via82xx"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kvernum -ge 206014 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_VIA82XX="m"
    fi
  fi
  if alsa_check_kconfig_card "via82xx-modem"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kvernum -ge 206014 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_VIA82XX_MODEM="m"
    fi
  fi
  if alsa_check_kconfig_card "virtuoso"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test $kvernum -ge 206000 ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_OXYGEN_LIB="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_MPU401_UART="m"
      test $kvernum -ge 206027 && CONFIG_SND_JACK="y"
      CONFIG_SND_VIRTUOSO="m"
    fi
  fi
  if alsa_check_kconfig_card "vx222"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_VX_LIB="m"
      CONFIG_SND_VX222="m"
    fi
  fi
  if alsa_check_kconfig_card "ymfpci"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      test $kvernum -ge 206014 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_YMFPCI="m"
    fi
  fi
  if alsa_check_kconfig_card "pdplus"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_PDPLUS="m"
    fi
  fi
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_PPC" = "y" -o "$CONFIG_PPC" = "m" ); then
      CONFIG_SND_PPC="y"
    fi
  if alsa_check_kconfig_card "powermac"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PPC" = "y" -o "$CONFIG_SND_PPC" = "m" ) &&
      ( ( test "$CONFIG_I2C" = "y" -o "$CONFIG_I2C" = "m" ) &&
      ( test "$CONFIG_INPUT" = "y" -o "$CONFIG_INPUT" = "m" ) &&
      ( test "$CONFIG_PPC_PMAC" = "y" -o "$CONFIG_PPC_PMAC" = "m" ) ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_POWERMAC="m"
    fi
  fi
  if alsa_check_kconfig_option "powermac-auto-drc"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PPC" = "y" -o "$CONFIG_SND_PPC" = "m" ) &&
      ( test "$CONFIG_SND_POWERMAC" = "y" -o "$CONFIG_SND_POWERMAC" = "m" ); then
      CONFIG_SND_POWERMAC_AUTO_DRC="y"
    fi
  fi
  if alsa_check_kconfig_card "ps3"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PPC" = "y" -o "$CONFIG_SND_PPC" = "m" ) &&
      ( test "$CONFIG_PS3_PS3AV" = "y" -o "$CONFIG_PS3_PS3AV" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_PS3="m"
    fi
  fi
  int_val=2000
  if alsa_check_kconfig_int "ps3-default-start-delay"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_PPC" = "y" -o "$CONFIG_SND_PPC" = "m" ) &&
      ( test "$CONFIG_SND_PS3" = "y" -o "$CONFIG_SND_PS3" = "m" ); then
      CONFIG_SND_PS3_DEFAULT_START_DELAY=$int_val
    fi
  fi
  if alsa_check_kconfig_card "aoa"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_PPC_PMAC" = "y" -o "$CONFIG_PPC_PMAC" = "m" ) &&
      ( test $kvernum -ge 206016 ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_AOA="m"
    fi
  fi
  if alsa_check_kconfig_card "aoa-fabric-layout"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_AOA" = "y" -o "$CONFIG_SND_AOA" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_AOA_SOUNDBUS="m"
      CONFIG_SND_AOA_SOUNDBUS_I2S="m"
      CONFIG_SND_AOA_FABRIC_LAYOUT="m"
    fi
  fi
  if alsa_check_kconfig_card "aoa-onyx"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_AOA" = "y" -o "$CONFIG_SND_AOA" = "m" ) &&
      ( test "$CONFIG_I2C" = "y" -o "$CONFIG_I2C" = "m" ) &&
      ( test "$CONFIG_I2C_POWERMAC" = "y" -o "$CONFIG_I2C_POWERMAC" = "m" ); then
      CONFIG_SND_AOA_ONYX="m"
    fi
  fi
  if alsa_check_kconfig_card "aoa-tas"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_AOA" = "y" -o "$CONFIG_SND_AOA" = "m" ) &&
      ( test "$CONFIG_I2C" = "y" -o "$CONFIG_I2C" = "m" ) &&
      ( test "$CONFIG_I2C_POWERMAC" = "y" -o "$CONFIG_I2C_POWERMAC" = "m" ); then
      CONFIG_SND_AOA_TAS="m"
    fi
  fi
  if alsa_check_kconfig_card "aoa-toonie"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_AOA" = "y" -o "$CONFIG_SND_AOA" = "m" ); then
      CONFIG_SND_AOA_TOONIE="m"
    fi
  fi
  if alsa_check_kconfig_card "aoa-soundbus"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_AOA" = "y" -o "$CONFIG_SND_AOA" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_AOA_SOUNDBUS="m"
    fi
  fi
  if alsa_check_kconfig_card "aoa-soundbus-i2s"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_AOA" = "y" -o "$CONFIG_SND_AOA" = "m" ) &&
      ( ( test "$CONFIG_SND_AOA_SOUNDBUS" = "y" -o "$CONFIG_SND_AOA_SOUNDBUS" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ) ); then
      CONFIG_SND_AOA_SOUNDBUS_I2S="m"
    fi
  fi
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_ARM" = "y" -o "$CONFIG_ARM" = "m" ); then
      CONFIG_SND_ARM="y"
    fi
  if alsa_check_kconfig_card "armaaci"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_ARM" = "y" -o "$CONFIG_SND_ARM" = "m" ) &&
      ( test "$CONFIG_ARM_AMBA" = "y" -o "$CONFIG_ARM_AMBA" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kvernum -ge 206014 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_PCM="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_ARMAACI="m"
    fi
  fi
  if alsa_check_kconfig_card "soc"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_REGMAP_I2C" = "y" -o "$CONFIG_REGMAP_I2C" = "m" ) &&
      ( test "$CONFIG_REGMAP_SPI" = "y" -o "$CONFIG_REGMAP_SPI" = "m" ) &&
      ( test $kvernum -ge 206010 ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kvernum -ge 206014 && CONFIG_AC97_BUS="m"
      test $kvernum -ge 206027 && CONFIG_SND_JACK="y"
      CONFIG_SND_SOC="m"
    fi
  fi
  if alsa_check_kconfig_card "pxa2xx-ac97"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_ARM" = "y" -o "$CONFIG_SND_ARM" = "m" ) &&
      ( test "$CONFIG_ARCH_PXA" = "y" -o "$CONFIG_ARCH_PXA" = "m" ) &&
       ! ( ( ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
       ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kvernum -ge 206014 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_AC97_CODEC="m"
      test $kvernum -ge 206000 && CONFIG_SND_PXA2XX_PCM="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_PXA2XX_LIB="m"
      CONFIG_SND_PXA2XX_LIB_AC97="y"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_PXA2XX_AC97="m"
    fi
  fi
  if alsa_check_kconfig_card "sa11xx-uda1341"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_ARM" = "y" -o "$CONFIG_SND_ARM" = "m" ) &&
      ( ( test "$CONFIG_ARCH_SA1100" = "y" -o "$CONFIG_ARCH_SA1100" = "m" ) &&
      ( test "$CONFIG_L3" = "y" -o "$CONFIG_L3" = "m" ) ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_SA11XX_UDA1341="m"
    fi
  fi
  if alsa_check_kconfig_card "s3c2410"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_ARM" = "y" -o "$CONFIG_SND_ARM" = "m" ) &&
      ( test "$CONFIG_ARCH_S3C2410" = "y" -o "$CONFIG_ARCH_S3C2410" = "m" ) &&
      ( test "$CONFIG_I2C_SENSOR" = "y" -o "$CONFIG_I2C_SENSOR" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_S3C2410="m"
    fi
  fi
  if alsa_check_kconfig_card "atmel-abdac"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( ( test "$CONFIG_AVR32" = "y" -o "$CONFIG_AVR32" = "m" ) ||
      ( test "$CONFIG_ARCH_AT91" = "y" -o "$CONFIG_ARCH_AT91" = "m" ) ) &&
      ( ( test "$CONFIG_DW_DMAC" = "y" -o "$CONFIG_DW_DMAC" = "m" ) &&
      ( test "$CONFIG_AVR32" = "y" -o "$CONFIG_AVR32" = "m" ) ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_ATMEL_ABDAC="m"
    fi
  fi
  if alsa_check_kconfig_card "atmel-ac97c"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( ( test "$CONFIG_AVR32" = "y" -o "$CONFIG_AVR32" = "m" ) ||
      ( test "$CONFIG_ARCH_AT91" = "y" -o "$CONFIG_ARCH_AT91" = "m" ) ) &&
      ( ( ( test "$CONFIG_DW_DMAC" = "y" -o "$CONFIG_DW_DMAC" = "m" ) &&
      ( test "$CONFIG_AVR32" = "y" -o "$CONFIG_AVR32" = "m" ) ) ||
      ( test "$CONFIG_ARCH_AT91" = "y" -o "$CONFIG_ARCH_AT91" = "m" ) ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kvernum -ge 206014 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_PCM="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_ATMEL_AC97C="m"
    fi
  fi
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SPI" = "y" -o "$CONFIG_SPI" = "m" ); then
      CONFIG_SND_SPI="y"
    fi
  if alsa_check_kconfig_card "at73c213"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SPI" = "y" -o "$CONFIG_SND_SPI" = "m" ) &&
      ( test "$CONFIG_ATMEL_SSC" = "y" -o "$CONFIG_ATMEL_SSC" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_AT73C213="m"
    fi
  fi
  int_val=48000
  if alsa_check_kconfig_int "at73c213-target-bitrate"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SPI" = "y" -o "$CONFIG_SND_SPI" = "m" ) &&
      ( test "$CONFIG_SND_AT73C213" = "y" -o "$CONFIG_SND_AT73C213" = "m" ); then
      CONFIG_SND_AT73C213_TARGET_BITRATE=$int_val
    fi
  fi
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_MIPS" = "y" -o "$CONFIG_MIPS" = "m" ); then
      CONFIG_SND_MIPS="y"
    fi
  if alsa_check_kconfig_card "sgi-o2"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_MIPS" = "y" -o "$CONFIG_SND_MIPS" = "m" ) &&
      ( test "$CONFIG_SGI_IP32" = "y" -o "$CONFIG_SGI_IP32" = "m" ); then
      CONFIG_SND_SGI_O2="m"
    fi
  fi
  if alsa_check_kconfig_card "sgi-hal2"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_MIPS" = "y" -o "$CONFIG_SND_MIPS" = "m" ) &&
      ( test "$CONFIG_SGI_HAS_HAL2" = "y" -o "$CONFIG_SGI_HAS_HAL2" = "m" ); then
      CONFIG_SND_SGI_HAL2="m"
    fi
  fi
  if alsa_check_kconfig_card "au1x00"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_MIPS" = "y" -o "$CONFIG_SND_MIPS" = "m" ) &&
      ( test "$CONFIG_MIPS_ALCHEMY" = "y" -o "$CONFIG_MIPS_ALCHEMY" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kvernum -ge 206014 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_PCM="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_AU1X00="m"
    fi
  fi
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SUPERH" = "y" -o "$CONFIG_SUPERH" = "m" ); then
      CONFIG_SND_SUPERH="y"
    fi
  if alsa_check_kconfig_card "aica"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SUPERH" = "y" -o "$CONFIG_SND_SUPERH" = "m" ) &&
      ( test "$CONFIG_SH_DREAMCAST" = "y" -o "$CONFIG_SH_DREAMCAST" = "m" ) &&
      ( test "$CONFIG_G2_DMA" = "y" -o "$CONFIG_G2_DMA" = "m" ) &&
      ( test $kvernum -ge 206000 ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_AICA="m"
    fi
  fi
  if alsa_check_kconfig_card "sh-dac-audio"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SUPERH" = "y" -o "$CONFIG_SND_SUPERH" = "m" ) &&
      ( ( test "$CONFIG_CPU_SH3" = "y" -o "$CONFIG_CPU_SH3" = "m" ) &&
      ( test "$CONFIG_HIGH_RES_TIMERS" = "y" -o "$CONFIG_HIGH_RES_TIMERS" = "m" ) ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_SH_DAC_AUDIO="m"
    fi
  fi
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_USB" = "y" -o "$CONFIG_USB" = "m" ); then
      CONFIG_SND_USB="y"
    fi
  if alsa_check_kconfig_card "usb-audio"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( ( test "$CONFIG_SND_USB" = "y" -o "$CONFIG_SND_USB" = "m" ) &&
      ( test "$CONFIG_USB" = "y" -o "$CONFIG_USB" = "m" ) ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_USB_AUDIO="m"
    fi
  fi
  if alsa_check_kconfig_card "usb-ua101"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( ( test "$CONFIG_SND_USB" = "y" -o "$CONFIG_SND_USB" = "m" ) &&
      ( test "$CONFIG_USB" = "y" -o "$CONFIG_USB" = "m" ) ) &&
      ( test $kvernum -ge 206032 ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_USB_UA101="m"
    fi
  fi
  if alsa_check_kconfig_card "usb-usx2y"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( ( test "$CONFIG_SND_USB" = "y" -o "$CONFIG_SND_USB" = "m" ) &&
      ( test "$CONFIG_USB" = "y" -o "$CONFIG_USB" = "m" ) ) &&
      ( ( test "$CONFIG_X86" = "y" -o "$CONFIG_X86" = "m" ) ||
      ( test "$CONFIG_PPC" = "y" -o "$CONFIG_PPC" = "m" ) ||
      ( test "$CONFIG_ALPHA" = "y" -o "$CONFIG_ALPHA" = "m" ) ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_USB_USX2Y="m"
    fi
  fi
  if alsa_check_kconfig_card "usb-caiaq"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( ( test "$CONFIG_SND_USB" = "y" -o "$CONFIG_SND_USB" = "m" ) &&
      ( test "$CONFIG_USB" = "y" -o "$CONFIG_USB" = "m" ) ) &&
      ( test $kvernum -ge 206015 ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_USB_CAIAQ="m"
    fi
  fi
  if alsa_check_kconfig_option "usb-caiaq-input"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( ( test "$CONFIG_SND_USB" = "y" -o "$CONFIG_SND_USB" = "m" ) &&
      ( test "$CONFIG_USB" = "y" -o "$CONFIG_USB" = "m" ) ) &&
      ( test "$CONFIG_SND_USB_CAIAQ" = "y" -o "$CONFIG_SND_USB_CAIAQ" = "m" ) &&
      ( test "$CONFIG_INPUT" = "y" -o "$CONFIG_INPUT" = "m" ); then
      CONFIG_SND_USB_CAIAQ_INPUT="y"
    fi
  fi
  if alsa_check_kconfig_card "usb-us122l"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( ( test "$CONFIG_SND_USB" = "y" -o "$CONFIG_SND_USB" = "m" ) &&
      ( test "$CONFIG_USB" = "y" -o "$CONFIG_USB" = "m" ) ) &&
      ( ( test "$CONFIG_X86" = "y" -o "$CONFIG_X86" = "m" ) &&
      ( test "$CONFIG_EXPERIMENTAL" = "y" -o "$CONFIG_EXPERIMENTAL" = "m" ) ) &&
      ( test $kvernum -ge 206026 ); then
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_USB_US122L="m"
    fi
  fi
  if alsa_check_kconfig_card "usb-6fire"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( ( test "$CONFIG_SND_USB" = "y" -o "$CONFIG_SND_USB" = "m" ) &&
      ( test "$CONFIG_USB" = "y" -o "$CONFIG_USB" = "m" ) ) &&
      ( test "$CONFIG_FW_LOADER" = "y" -o "$CONFIG_FW_LOADER" = "m" ) &&
      ( test $kvernum -ge 206028 ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_USB_6FIRE="m"
    fi
  fi
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_FIREWIRE" = "y" -o "$CONFIG_FIREWIRE" = "m" ); then
      CONFIG_SND_FIREWIRE="y"
    fi
  if alsa_check_kconfig_card "firewire-speakers"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( ( test "$CONFIG_SND_FIREWIRE" = "y" -o "$CONFIG_SND_FIREWIRE" = "m" ) &&
      ( test "$CONFIG_FIREWIRE" = "y" -o "$CONFIG_FIREWIRE" = "m" ) ) &&
      ( test $kvernum -ge 206039 ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_FIREWIRE_LIB="m"
      CONFIG_SND_FIREWIRE_SPEAKERS="m"
    fi
  fi
  if alsa_check_kconfig_card "isight"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( ( test "$CONFIG_SND_FIREWIRE" = "y" -o "$CONFIG_SND_FIREWIRE" = "m" ) &&
      ( test "$CONFIG_FIREWIRE" = "y" -o "$CONFIG_FIREWIRE" = "m" ) ) &&
      ( test $kvernum -ge 206039 ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_FIREWIRE_LIB="m"
      CONFIG_SND_ISIGHT="m"
    fi
  fi
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_PCMCIA" = "y" -o "$CONFIG_PCMCIA" = "m" ); then
      CONFIG_SND_PCMCIA="y"
    fi
  if alsa_check_kconfig_card "vxpocket"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( ( test "$CONFIG_SND_PCMCIA" = "y" -o "$CONFIG_SND_PCMCIA" = "m" ) &&
      ( test "$CONFIG_PCMCIA" = "y" -o "$CONFIG_PCMCIA" = "m" ) ); then
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_VX_LIB="m"
      CONFIG_SND_VXPOCKET="m"
    fi
  fi
  if alsa_check_kconfig_card "pdaudiocf"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( ( test "$CONFIG_SND_PCMCIA" = "y" -o "$CONFIG_SND_PCMCIA" = "m" ) &&
      ( test "$CONFIG_PCMCIA" = "y" -o "$CONFIG_PCMCIA" = "m" ) ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_PDAUDIOCF="m"
    fi
  fi
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SPARC" = "y" -o "$CONFIG_SPARC" = "m" ); then
      CONFIG_SND_SPARC="y"
    fi
  if alsa_check_kconfig_card "sun-amd7930"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SPARC" = "y" -o "$CONFIG_SND_SPARC" = "m" ) &&
      ( test "$CONFIG_SBUS" = "y" -o "$CONFIG_SBUS" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_SUN_AMD7930="m"
    fi
  fi
  if alsa_check_kconfig_card "sun-cs4231"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SPARC" = "y" -o "$CONFIG_SND_SPARC" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_SUN_CS4231="m"
    fi
  fi
  if alsa_check_kconfig_card "sun-dbri"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SPARC" = "y" -o "$CONFIG_SND_SPARC" = "m" ) &&
      ( test "$CONFIG_SBUS" = "y" -o "$CONFIG_SBUS" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_SUN_DBRI="m"
    fi
  fi
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_GSC" = "y" -o "$CONFIG_GSC" = "m" ); then
      CONFIG_SND_GSC="y"
    fi
  if alsa_check_kconfig_card "harmony"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_GSC" = "y" -o "$CONFIG_SND_GSC" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_HARMONY="m"
    fi
  fi
  if alsa_check_kconfig_card "atmel-soc"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_ARCH_AT91" = "y" -o "$CONFIG_ARCH_AT91" = "m" ); then
      CONFIG_SND_ATMEL_SOC="m"
    fi
  fi
  if alsa_check_kconfig_card "at91-soc-sam9g20-wm8731"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_ATMEL_SSC" = "y" -o "$CONFIG_ATMEL_SSC" = "m" ) &&
      ( test "$CONFIG_ARCH_AT91SAM9G20" = "y" -o "$CONFIG_ARCH_AT91SAM9G20" = "m" ) &&
      ( test "$CONFIG_SND_ATMEL_SOC" = "y" -o "$CONFIG_SND_ATMEL_SOC" = "m" ) &&
      ( test "$CONFIG_AT91_PROGRAMMABLE_CLOCKS" = "y" -o "$CONFIG_AT91_PROGRAMMABLE_CLOCKS" = "m" ) ); then
      CONFIG_SND_ATMEL_SOC_SSC="m"
      CONFIG_SND_SOC_WM8731="m"
      CONFIG_SND_AT91_SOC_SAM9G20_WM8731="m"
    fi
  fi
  if alsa_check_kconfig_card "at91-soc-afeb9260"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_ATMEL_SSC" = "y" -o "$CONFIG_ATMEL_SSC" = "m" ) &&
      ( test "$CONFIG_ARCH_AT91" = "y" -o "$CONFIG_ARCH_AT91" = "m" ) &&
      ( test "$CONFIG_MACH_AFEB9260" = "y" -o "$CONFIG_MACH_AFEB9260" = "m" ) &&
      ( test "$CONFIG_SND_ATMEL_SOC" = "y" -o "$CONFIG_SND_ATMEL_SOC" = "m" ) ); then
      CONFIG_SND_ATMEL_SOC_SSC="m"
      CONFIG_SND_SOC_TLV320AIC23="m"
      CONFIG_SND_AT91_SOC_AFEB9260="m"
    fi
  fi
  if alsa_check_kconfig_card "soc-au1xpsc"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_MIPS_ALCHEMY" = "y" -o "$CONFIG_MIPS_ALCHEMY" = "m" ); then
      CONFIG_SND_SOC_AU1XPSC="m"
    fi
  fi
  if alsa_check_kconfig_card "soc-au1xaudio"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_MIPS_ALCHEMY" = "y" -o "$CONFIG_MIPS_ALCHEMY" = "m" ); then
      CONFIG_SND_SOC_AU1XAUDIO="m"
    fi
  fi
  if alsa_check_kconfig_card "soc-db1000"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_SND_SOC_AU1XAUDIO" = "y" -o "$CONFIG_SND_SOC_AU1XAUDIO" = "m" ); then
      test $kvernum -ge 206014 && CONFIG_AC97_BUS="m"
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_SOC_AC97_BUS="y"
      CONFIG_SND_SOC_AU1XAC97C="m"
      CONFIG_SND_SOC_AC97_CODEC="m"
      CONFIG_SND_SOC_DB1000="m"
    fi
  fi
  if alsa_check_kconfig_card "soc-db1200"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_SND_SOC_AU1XPSC" = "y" -o "$CONFIG_SND_SOC_AU1XPSC" = "m" ); then
      test $kvernum -ge 206014 && CONFIG_AC97_BUS="m"
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_SOC_AC97_BUS="y"
      CONFIG_SND_SOC_AU1XPSC_AC97="m"
      CONFIG_SND_SOC_AC97_CODEC="m"
      CONFIG_SND_SOC_WM9712="m"
      CONFIG_SND_SOC_AU1XPSC_I2S="m"
      CONFIG_SND_SOC_WM8731="m"
      CONFIG_SND_SOC_DB1200="m"
    fi
  fi
  if alsa_check_kconfig_card "bf5xx-i2s"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_BLACKFIN" = "y" -o "$CONFIG_BLACKFIN" = "m" ); then
      CONFIG_SND_BF5XX_SOC_SPORT="m"
      CONFIG_SND_BF5XX_I2S="m"
    fi
  fi
  if alsa_check_kconfig_card "bf5xx-soc-ssm2602"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_BF5XX_I2S" = "y" -o "$CONFIG_SND_BF5XX_I2S" = "m" ) &&
      ( ( test "$CONFIG_SPI_MASTER" = "y" -o "$CONFIG_SPI_MASTER" = "m" ) ||
      ( test "$CONFIG_I2C" = "y" -o "$CONFIG_I2C" = "m" ) ) ); then
      CONFIG_SND_BF5XX_SOC_I2S="m"
      CONFIG_SND_SOC_SSM2602="m"
      CONFIG_SND_BF5XX_SOC_SSM2602="m"
    fi
  fi
  if alsa_check_kconfig_card "soc-bfin-eval-adau1701"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_SND_BF5XX_I2S" = "y" -o "$CONFIG_SND_BF5XX_I2S" = "m" ) &&
      ( test "$CONFIG_I2C" = "y" -o "$CONFIG_I2C" = "m" ); then
      CONFIG_SND_SOC_SIGMADSP="m"
      CONFIG_SND_BF5XX_SOC_I2S="m"
      CONFIG_SND_SOC_ADAU1701="m"
      CONFIG_SND_SOC_BFIN_EVAL_ADAU1701="m"
    fi
  fi
  if alsa_check_kconfig_card "soc-bfin-eval-adau1373"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_BF5XX_I2S" = "y" -o "$CONFIG_SND_BF5XX_I2S" = "m" ) &&
      ( test "$CONFIG_I2C" = "y" -o "$CONFIG_I2C" = "m" ) ); then
      CONFIG_SND_BF5XX_SOC_I2S="m"
      CONFIG_SND_SOC_ADAU1373="m"
      CONFIG_SND_SOC_BFIN_EVAL_ADAU1373="m"
    fi
  fi
  if alsa_check_kconfig_card "soc-bfin-eval-adav80x"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_BF5XX_I2S" = "y" -o "$CONFIG_SND_BF5XX_I2S" = "m" ) &&
      ( ( test "$CONFIG_SPI_MASTER" = "y" -o "$CONFIG_SPI_MASTER" = "m" ) ||
      ( test "$CONFIG_I2C" = "y" -o "$CONFIG_I2C" = "m" ) ) ); then
      CONFIG_SND_BF5XX_SOC_I2S="m"
      CONFIG_SND_SOC_ADAV80X="m"
      CONFIG_SND_SOC_BFIN_EVAL_ADAV80X="m"
    fi
  fi
  if alsa_check_kconfig_card "bf5xx-soc-ad73311"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_SND_BF5XX_I2S" = "y" -o "$CONFIG_SND_BF5XX_I2S" = "m" ); then
      CONFIG_SND_BF5XX_SOC_I2S="m"
      CONFIG_SND_SOC_AD73311="m"
      CONFIG_SND_BF5XX_SOC_AD73311="m"
    fi
  fi
  int_val=4
  if alsa_check_kconfig_int "bfin-ad73311-se"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_SND_BF5XX_SOC_AD73311" = "y" -o "$CONFIG_SND_BF5XX_SOC_AD73311" = "m" ); then
      CONFIG_SND_BFIN_AD73311_SE=$int_val
    fi
  fi
  if alsa_check_kconfig_card "bf5xx-tdm"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( ( test "$CONFIG_BLACKFIN" = "y" -o "$CONFIG_BLACKFIN" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) ) ); then
      CONFIG_SND_BF5XX_SOC_SPORT="m"
      CONFIG_SND_BF5XX_TDM="m"
    fi
  fi
  if alsa_check_kconfig_card "bf5xx-soc-ad1836"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_SND_BF5XX_TDM" = "y" -o "$CONFIG_SND_BF5XX_TDM" = "m" ); then
      CONFIG_SND_BF5XX_SOC_TDM="m"
      CONFIG_SND_SOC_AD1836="m"
      CONFIG_SND_BF5XX_SOC_AD1836="m"
    fi
  fi
  if alsa_check_kconfig_card "bf5xx-soc-ad193x"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_SND_BF5XX_TDM" = "y" -o "$CONFIG_SND_BF5XX_TDM" = "m" ); then
      CONFIG_SND_BF5XX_SOC_TDM="m"
      CONFIG_SND_SOC_AD193X="m"
      CONFIG_SND_BF5XX_SOC_AD193X="m"
    fi
  fi
  if alsa_check_kconfig_card "bf5xx-ac97"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_BLACKFIN" = "y" -o "$CONFIG_BLACKFIN" = "m" ); then
      test $kvernum -ge 206014 && CONFIG_AC97_BUS="m"
      CONFIG_SND_SOC_AC97_BUS="y"
      CONFIG_SND_BF5XX_SOC_SPORT="m"
      CONFIG_SND_BF5XX_SOC_AC97="m"
      CONFIG_SND_BF5XX_AC97="m"
    fi
  fi
  if alsa_check_kconfig_option "bf5xx-mmap-support"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_SND_BF5XX_AC97" = "y" -o "$CONFIG_SND_BF5XX_AC97" = "m" ); then
      CONFIG_SND_BF5XX_MMAP_SUPPORT="y"
    fi
  fi
  if alsa_check_kconfig_option "bf5xx-multichan-support"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_SND_BF5XX_AC97" = "y" -o "$CONFIG_SND_BF5XX_AC97" = "m" ); then
      CONFIG_SND_BF5XX_MULTICHAN_SUPPORT="y"
    fi
  fi
  if alsa_check_kconfig_option "bf5xx-have-cold-reset"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_SND_BF5XX_AC97" = "y" -o "$CONFIG_SND_BF5XX_AC97" = "m" ); then
      CONFIG_SND_BF5XX_HAVE_COLD_RESET="y"
    fi
  fi
  int_val=0
  if alsa_check_kconfig_int "bf5xx-reset-gpio-num"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_SND_BF5XX_HAVE_COLD_RESET" = "y" -o "$CONFIG_SND_BF5XX_HAVE_COLD_RESET" = "m" ); then
      CONFIG_SND_BF5XX_RESET_GPIO_NUM=$int_val
    fi
  fi
  if alsa_check_kconfig_card "bf5xx-soc-ad1980"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_SND_BF5XX_AC97" = "y" -o "$CONFIG_SND_BF5XX_AC97" = "m" ); then
      CONFIG_SND_BF5XX_SOC_AC97="m"
      CONFIG_SND_SOC_AD1980="m"
      CONFIG_SND_BF5XX_SOC_AD1980="m"
    fi
  fi
  int_val=0
  if alsa_check_kconfig_int "bf5xx-sport-num"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( ( test "$CONFIG_SND_BF5XX_I2S" = "y" -o "$CONFIG_SND_BF5XX_I2S" = "m" ) ||
      ( test "$CONFIG_SND_BF5XX_AC97" = "y" -o "$CONFIG_SND_BF5XX_AC97" = "m" ) ||
      ( test "$CONFIG_SND_BF5XX_TDM" = "y" -o "$CONFIG_SND_BF5XX_TDM" = "m" ) ) ); then
      CONFIG_SND_BF5XX_SPORT_NUM=$int_val
    fi
  fi
  if alsa_check_kconfig_card "davinci-soc"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_ARCH_DAVINCI" = "y" -o "$CONFIG_ARCH_DAVINCI" = "m" ); then
      CONFIG_SND_DAVINCI_SOC="m"
    fi
  fi
  if alsa_check_kconfig_card "davinci-soc-evm"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_SND_DAVINCI_SOC" = "y" -o "$CONFIG_SND_DAVINCI_SOC" = "m" ) &&
      ( ( test "$CONFIG_MACH_DAVINCI_EVM" = "y" -o "$CONFIG_MACH_DAVINCI_EVM" = "m" ) ||
      ( test "$CONFIG_MACH_DAVINCI_DM355_EVM" = "y" -o "$CONFIG_MACH_DAVINCI_DM355_EVM" = "m" ) ||
      ( test "$CONFIG_MACH_DAVINCI_DM365_EVM" = "y" -o "$CONFIG_MACH_DAVINCI_DM365_EVM" = "m" ) ); then
      CONFIG_SND_DAVINCI_SOC_I2S="m"
      CONFIG_SND_SOC_TLV320AIC3X="m"
      CONFIG_SND_DAVINCI_SOC_EVM="m"
    fi
  fi
  if alsa_check_kconfig_option "dm365-aic3x-codec"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ); then
      CONFIG_SND_DM365_AIC3X_CODEC="y"
    fi
  fi
  if alsa_check_kconfig_option "dm365-voice-codec"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_MFD_DAVINCI_VOICECODEC" = "y" -o "$CONFIG_MFD_DAVINCI_VOICECODEC" = "m" ); then
      CONFIG_SND_DAVINCI_SOC_VCIF="m"
      CONFIG_SND_SOC_CQ0093VC="m"
      CONFIG_SND_DM365_VOICE_CODEC="y"
    fi
  fi
  if alsa_check_kconfig_card "dm6467-soc-evm"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_DAVINCI_SOC" = "y" -o "$CONFIG_SND_DAVINCI_SOC" = "m" ) &&
      ( test "$CONFIG_MACH_DAVINCI_DM6467_EVM" = "y" -o "$CONFIG_MACH_DAVINCI_DM6467_EVM" = "m" ) ); then
      CONFIG_SND_DAVINCI_SOC_MCASP="m"
      CONFIG_SND_SOC_TLV320AIC3X="m"
      CONFIG_SND_SOC_SPDIF="m"
      CONFIG_SND_DM6467_SOC_EVM="m"
    fi
  fi
  if alsa_check_kconfig_card "davinci-soc-sffsdr"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_DAVINCI_SOC" = "y" -o "$CONFIG_SND_DAVINCI_SOC" = "m" ) &&
      ( test "$CONFIG_MACH_SFFSDR" = "y" -o "$CONFIG_MACH_SFFSDR" = "m" ) ) &&
      ( test "$CONFIG_SFFSDR_FPGA" = "y" -o "$CONFIG_SFFSDR_FPGA" = "m" ); then
      CONFIG_SND_DAVINCI_SOC_I2S="m"
      CONFIG_SND_SOC_PCM3008="m"
      CONFIG_SND_DAVINCI_SOC_SFFSDR="m"
    fi
  fi
  if alsa_check_kconfig_card "da830-soc-evm"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_DAVINCI_SOC" = "y" -o "$CONFIG_SND_DAVINCI_SOC" = "m" ) &&
      ( test "$CONFIG_MACH_DAVINCI_DA830_EVM" = "y" -o "$CONFIG_MACH_DAVINCI_DA830_EVM" = "m" ) ); then
      CONFIG_SND_DAVINCI_SOC_MCASP="m"
      CONFIG_SND_SOC_TLV320AIC3X="m"
      CONFIG_SND_DA830_SOC_EVM="m"
    fi
  fi
  if alsa_check_kconfig_card "da850-soc-evm"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_DAVINCI_SOC" = "y" -o "$CONFIG_SND_DAVINCI_SOC" = "m" ) &&
      ( test "$CONFIG_MACH_DAVINCI_DA850_EVM" = "y" -o "$CONFIG_MACH_DAVINCI_DA850_EVM" = "m" ) ); then
      CONFIG_SND_DAVINCI_SOC_MCASP="m"
      CONFIG_SND_SOC_TLV320AIC3X="m"
      CONFIG_SND_DA850_SOC_EVM="m"
    fi
  fi
  if alsa_check_kconfig_card "ep93xx-soc"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_ARCH_EP93XX" = "y" -o "$CONFIG_ARCH_EP93XX" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) ); then
      CONFIG_SND_EP93XX_SOC="m"
    fi
  fi
  if alsa_check_kconfig_card "ep93xx-soc-snappercl15"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_EP93XX_SOC" = "y" -o "$CONFIG_SND_EP93XX_SOC" = "m" ) &&
      ( test "$CONFIG_MACH_SNAPPER_CL15" = "y" -o "$CONFIG_MACH_SNAPPER_CL15" = "m" ) ); then
      CONFIG_SND_EP93XX_SOC_I2S="m"
      CONFIG_SND_SOC_TLV320AIC23="m"
      CONFIG_SND_EP93XX_SOC_SNAPPERCL15="m"
    fi
  fi
  if alsa_check_kconfig_card "ep93xx-soc-simone"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_EP93XX_SOC" = "y" -o "$CONFIG_SND_EP93XX_SOC" = "m" ) &&
      ( test "$CONFIG_MACH_SIM_ONE" = "y" -o "$CONFIG_MACH_SIM_ONE" = "m" ) ); then
      test $kvernum -ge 206014 && CONFIG_AC97_BUS="m"
      CONFIG_SND_SOC_AC97_BUS="y"
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_EP93XX_SOC_AC97="m"
      CONFIG_SND_SOC_AC97_CODEC="m"
      CONFIG_SND_EP93XX_SOC_SIMONE="m"
    fi
  fi
  if alsa_check_kconfig_card "ep93xx-soc-edb93xx"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_EP93XX_SOC" = "y" -o "$CONFIG_SND_EP93XX_SOC" = "m" ) &&
      ( ( test "$CONFIG_MACH_EDB9301" = "y" -o "$CONFIG_MACH_EDB9301" = "m" ) ||
      ( test "$CONFIG_MACH_EDB9302" = "y" -o "$CONFIG_MACH_EDB9302" = "m" ) ||
      ( test "$CONFIG_MACH_EDB9302A" = "y" -o "$CONFIG_MACH_EDB9302A" = "m" ) ||
      ( test "$CONFIG_MACH_EDB9307A" = "y" -o "$CONFIG_MACH_EDB9307A" = "m" ) ||
      ( test "$CONFIG_MACH_EDB9315A" = "y" -o "$CONFIG_MACH_EDB9315A" = "m" ) ) ); then
      CONFIG_SND_EP93XX_SOC_I2S="m"
      CONFIG_SND_SOC_CS4271="m"
      CONFIG_SND_EP93XX_SOC_EDB93XX="m"
    fi
  fi
  if alsa_check_kconfig_card "soc-mpc8610-hpcd"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_MPC8610_HPCD" = "y" -o "$CONFIG_MPC8610_HPCD" = "m" ) &&
      ( test "$CONFIG_I2C" = "y" -o "$CONFIG_I2C" = "m" ) ); then
      CONFIG_SND_SOC_POWERPC_SSI="m"
      CONFIG_SND_SOC_CS4270="m"
      CONFIG_SND_SOC_CS4270_VD33_ERRATA="y"
      CONFIG_SND_SOC_MPC8610_HPCD="m"
    fi
  fi
  if alsa_check_kconfig_card "soc-p1022-ds"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_P1022_DS" = "y" -o "$CONFIG_P1022_DS" = "m" ) &&
      ( test "$CONFIG_I2C" = "y" -o "$CONFIG_I2C" = "m" ) ); then
      CONFIG_SND_SOC_POWERPC_SSI="m"
      CONFIG_SND_SOC_WM8776="m"
      CONFIG_SND_SOC_P1022_DS="m"
    fi
  fi
  if alsa_check_kconfig_card "soc-mpc5200-i2s"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_PPC_MPC52xx" = "y" -o "$CONFIG_PPC_MPC52xx" = "m" ) &&
      ( test "$CONFIG_PPC_BESTCOMM" = "y" -o "$CONFIG_PPC_BESTCOMM" = "m" ) ) &&
      ( test "$CONFIG_PPC_BESTCOMM_GEN_BD" = "y" -o "$CONFIG_PPC_BESTCOMM_GEN_BD" = "m" ); then
      CONFIG_SND_MPC52xx_DMA="m"
      CONFIG_SND_SOC_MPC5200_I2S="m"
    fi
  fi
  if alsa_check_kconfig_card "soc-mpc5200-ac97"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_PPC_MPC52xx" = "y" -o "$CONFIG_PPC_MPC52xx" = "m" ) &&
      ( test "$CONFIG_PPC_BESTCOMM" = "y" -o "$CONFIG_PPC_BESTCOMM" = "m" ) ) &&
      ( test "$CONFIG_PPC_BESTCOMM_GEN_BD" = "y" -o "$CONFIG_PPC_BESTCOMM_GEN_BD" = "m" ); then
      CONFIG_SND_SOC_AC97_BUS="y"
      CONFIG_SND_MPC52xx_DMA="m"
      CONFIG_SND_SOC_MPC5200_AC97="m"
    fi
  fi
  if alsa_check_kconfig_card "mpc52xx-soc-pcm030"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_PPC_MPC5200_SIMPLE" = "y" -o "$CONFIG_PPC_MPC5200_SIMPLE" = "m" ); then
      CONFIG_SND_SOC_AC97_BUS="y"
      CONFIG_SND_MPC52xx_DMA="m"
      CONFIG_SND_SOC_MPC5200_AC97="m"
      CONFIG_SND_SOC_WM9712="m"
      CONFIG_SND_MPC52XX_SOC_PCM030="m"
    fi
  fi
  if alsa_check_kconfig_card "mpc52xx-soc-efika"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_PPC_EFIKA" = "y" -o "$CONFIG_PPC_EFIKA" = "m" ); then
      CONFIG_SND_SOC_AC97_BUS="y"
      CONFIG_SND_MPC52xx_DMA="m"
      CONFIG_SND_SOC_MPC5200_AC97="m"
      CONFIG_SND_SOC_STAC9766="m"
      CONFIG_SND_MPC52XX_SOC_EFIKA="m"
    fi
  fi
  if alsa_check_kconfig_card "imx-soc"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_ARCH_MXC" = "y" -o "$CONFIG_ARCH_MXC" = "m" ) &&
      ( test "$CONFIG_FIQ" = "y" -o "$CONFIG_FIQ" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_SOC_AC97_BUS="y"
      CONFIG_SND_IMX_SOC="m"
    fi
  fi
  if alsa_check_kconfig_card "mxc-soc-wm1133-ev1"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_SND_IMX_SOC" = "y" -o "$CONFIG_SND_IMX_SOC" = "m" ) &&
      ( ( test "$CONFIG_MACH_MX31ADS_WM1133_EV1" = "y" -o "$CONFIG_MACH_MX31ADS_WM1133_EV1" = "m" ) &&
      ( test "$CONFIG_EXPERIMENTAL" = "y" -o "$CONFIG_EXPERIMENTAL" = "m" ) ); then
      CONFIG_SND_SOC_WM8350="m"
      CONFIG_SND_MXC_SOC_FIQ="m"
      CONFIG_SND_MXC_SOC_WM1133_EV1="m"
    fi
  fi
  if alsa_check_kconfig_card "soc-mx27vis-aic32x4"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_SND_IMX_SOC" = "y" -o "$CONFIG_SND_IMX_SOC" = "m" ) &&
      ( ( test "$CONFIG_MACH_IMX27_VISSTRIM_M10" = "y" -o "$CONFIG_MACH_IMX27_VISSTRIM_M10" = "m" ) &&
      ( test "$CONFIG_I2C" = "y" -o "$CONFIG_I2C" = "m" ) ); then
      CONFIG_SND_SOC_TLV320AIC32X4="m"
      CONFIG_SND_MXC_SOC_MX2="m"
      CONFIG_SND_SOC_MX27VIS_AIC32X4="m"
    fi
  fi
  if alsa_check_kconfig_card "soc-phycore-ac97"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_SND_IMX_SOC" = "y" -o "$CONFIG_SND_IMX_SOC" = "m" ) &&
      ( ( test "$CONFIG_MACH_PCM043" = "y" -o "$CONFIG_MACH_PCM043" = "m" ) ||
      ( test "$CONFIG_MACH_PCA100" = "y" -o "$CONFIG_MACH_PCA100" = "m" ) ); then
      CONFIG_SND_SOC_WM9712="m"
      CONFIG_SND_MXC_SOC_FIQ="m"
      CONFIG_SND_SOC_PHYCORE_AC97="m"
    fi
  fi
  if alsa_check_kconfig_card "soc-eukrea-tlv320"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_SND_IMX_SOC" = "y" -o "$CONFIG_SND_IMX_SOC" = "m" ) &&
      ( ( test "$CONFIG_MACH_EUKREA_MBIMX27_BASEBOARD" = "y" -o "$CONFIG_MACH_EUKREA_MBIMX27_BASEBOARD" = "m" ) ||
      ( test "$CONFIG_MACH_EUKREA_MBIMXSD25_BASEBOARD" = "y" -o "$CONFIG_MACH_EUKREA_MBIMXSD25_BASEBOARD" = "m" ) ||
      ( test "$CONFIG_MACH_EUKREA_MBIMXSD35_BASEBOARD" = "y" -o "$CONFIG_MACH_EUKREA_MBIMXSD35_BASEBOARD" = "m" ) ||
      ( test "$CONFIG_MACH_EUKREA_MBIMXSD51_BASEBOARD" = "y" -o "$CONFIG_MACH_EUKREA_MBIMXSD51_BASEBOARD" = "m" ) ) &&
      ( test "$CONFIG_I2C" = "y" -o "$CONFIG_I2C" = "m" ); then
      CONFIG_SND_SOC_TLV320AIC23="m"
      CONFIG_SND_MXC_SOC_FIQ="m"
      CONFIG_SND_SOC_EUKREA_TLV320="m"
    fi
  fi
  if alsa_check_kconfig_card "jz4740-soc"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_MACH_JZ4740" = "y" -o "$CONFIG_MACH_JZ4740" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) ); then
      CONFIG_SND_JZ4740_SOC="m"
    fi
  fi
  if alsa_check_kconfig_card "jz4740-soc-i2s"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_SND_JZ4740_SOC" = "y" -o "$CONFIG_SND_JZ4740_SOC" = "m" ); then
      CONFIG_SND_JZ4740_SOC_I2S="m"
    fi
  fi
  if alsa_check_kconfig_card "jz4740-soc-qi-lb60"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_JZ4740_SOC" = "y" -o "$CONFIG_SND_JZ4740_SOC" = "m" ) &&
      ( test "$CONFIG_JZ4740_QI_LB60" = "y" -o "$CONFIG_JZ4740_QI_LB60" = "m" ) ); then
      CONFIG_SND_JZ4740_SOC_I2S="m"
      CONFIG_SND_JZ4740_SOC_QI_LB60="m"
    fi
  fi
  if alsa_check_kconfig_card "soc-nuc900"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_ARCH_W90X900" = "y" -o "$CONFIG_ARCH_W90X900" = "m" ); then
      CONFIG_SND_SOC_NUC900="m"
    fi
  fi
  if alsa_check_kconfig_card "soc-nuc900evb"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_SND_SOC_NUC900" = "y" -o "$CONFIG_SND_SOC_NUC900" = "m" ); then
      test $kvernum -ge 206014 && CONFIG_AC97_BUS="m"
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_SOC_AC97_BUS="y"
      CONFIG_SND_SOC_NUC900_AC97="m"
      CONFIG_SND_SOC_AC97_CODEC="m"
      CONFIG_SND_SOC_NUC900EVB="m"
    fi
  fi
  if alsa_check_kconfig_card "omap-soc"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_ARCH_OMAP" = "y" -o "$CONFIG_ARCH_OMAP" = "m" ); then
      CONFIG_SND_OMAP_SOC="m"
    fi
  fi
  if alsa_check_kconfig_card "omap-soc-n810"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_OMAP_SOC" = "y" -o "$CONFIG_SND_OMAP_SOC" = "m" ) &&
      ( test "$CONFIG_MACH_NOKIA_N810" = "y" -o "$CONFIG_MACH_NOKIA_N810" = "m" ) &&
      ( test "$CONFIG_I2C" = "y" -o "$CONFIG_I2C" = "m" ) ) &&
      ( test "$CONFIG_OMAP_MUX" = "y" -o "$CONFIG_OMAP_MUX" = "m" ); then
      CONFIG_SND_OMAP_SOC_MCBSP="m"
      CONFIG_SND_SOC_TLV320AIC3X="m"
      CONFIG_SND_OMAP_SOC_N810="m"
    fi
  fi
  if alsa_check_kconfig_card "omap-soc-rx51"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_OMAP_SOC" = "y" -o "$CONFIG_SND_OMAP_SOC" = "m" ) &&
      ( test "$CONFIG_MACH_NOKIA_RX51" = "y" -o "$CONFIG_MACH_NOKIA_RX51" = "m" ) ) &&
      ( test "$CONFIG_OMAP_MCBSP" = "y" -o "$CONFIG_OMAP_MCBSP" = "m" ); then
      CONFIG_SND_OMAP_SOC_MCBSP="m"
      CONFIG_SND_SOC_TLV320AIC3X="m"
      CONFIG_SND_SOC_TPA6130A2="m"
      CONFIG_SND_OMAP_SOC_RX51="m"
    fi
  fi
  if alsa_check_kconfig_card "omap-soc-ams-delta"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_OMAP_SOC" = "y" -o "$CONFIG_SND_OMAP_SOC" = "m" ) &&
      ( test "$CONFIG_MACH_AMS_DELTA" = "y" -o "$CONFIG_MACH_AMS_DELTA" = "m" ) ); then
      CONFIG_SND_OMAP_SOC_MCBSP="m"
      CONFIG_SND_SOC_CX20442="m"
      CONFIG_SND_OMAP_SOC_AMS_DELTA="m"
    fi
  fi
  if alsa_check_kconfig_card "omap-soc-osk5912"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_OMAP_SOC" = "y" -o "$CONFIG_SND_OMAP_SOC" = "m" ) &&
      ( test "$CONFIG_MACH_OMAP_OSK" = "y" -o "$CONFIG_MACH_OMAP_OSK" = "m" ) &&
      ( test "$CONFIG_I2C" = "y" -o "$CONFIG_I2C" = "m" ) ); then
      CONFIG_SND_OMAP_SOC_MCBSP="m"
      CONFIG_SND_SOC_TLV320AIC23="m"
      CONFIG_SND_OMAP_SOC_OSK5912="m"
    fi
  fi
  if alsa_check_kconfig_card "omap-soc-overo"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_TWL4030_CORE" = "y" -o "$CONFIG_TWL4030_CORE" = "m" ) &&
      ( test "$CONFIG_SND_OMAP_SOC" = "y" -o "$CONFIG_SND_OMAP_SOC" = "m" ) &&
      ( ( test "$CONFIG_MACH_OVERO" = "y" -o "$CONFIG_MACH_OVERO" = "m" ) ||
      ( test "$CONFIG_MACH_CM_T35" = "y" -o "$CONFIG_MACH_CM_T35" = "m" ) ) ); then
      CONFIG_SND_OMAP_SOC_MCBSP="m"
      CONFIG_SND_SOC_TWL4030="m"
      CONFIG_SND_OMAP_SOC_OVERO="m"
    fi
  fi
  if alsa_check_kconfig_card "omap-soc-omap3evm"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_TWL4030_CORE" = "y" -o "$CONFIG_TWL4030_CORE" = "m" ) &&
      ( test "$CONFIG_SND_OMAP_SOC" = "y" -o "$CONFIG_SND_OMAP_SOC" = "m" ) &&
      ( test "$CONFIG_MACH_OMAP3EVM" = "y" -o "$CONFIG_MACH_OMAP3EVM" = "m" ) ); then
      CONFIG_SND_OMAP_SOC_MCBSP="m"
      CONFIG_SND_SOC_TWL4030="m"
      CONFIG_SND_OMAP_SOC_OMAP3EVM="m"
    fi
  fi
  if alsa_check_kconfig_card "omap-soc-am3517evm"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_OMAP_SOC" = "y" -o "$CONFIG_SND_OMAP_SOC" = "m" ) &&
      ( test "$CONFIG_MACH_OMAP3517EVM" = "y" -o "$CONFIG_MACH_OMAP3517EVM" = "m" ) &&
      ( test "$CONFIG_I2C" = "y" -o "$CONFIG_I2C" = "m" ) ); then
      CONFIG_SND_OMAP_SOC_MCBSP="m"
      CONFIG_SND_SOC_TLV320AIC23="m"
      CONFIG_SND_OMAP_SOC_AM3517EVM="m"
    fi
  fi
  if alsa_check_kconfig_card "omap-soc-sdp3430"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_TWL4030_CORE" = "y" -o "$CONFIG_TWL4030_CORE" = "m" ) &&
      ( test "$CONFIG_SND_OMAP_SOC" = "y" -o "$CONFIG_SND_OMAP_SOC" = "m" ) &&
      ( test "$CONFIG_MACH_OMAP_3430SDP" = "y" -o "$CONFIG_MACH_OMAP_3430SDP" = "m" ) ); then
      CONFIG_SND_OMAP_SOC_MCBSP="m"
      CONFIG_SND_SOC_TWL4030="m"
      CONFIG_SND_OMAP_SOC_SDP3430="m"
    fi
  fi
  if alsa_check_kconfig_card "omap-soc-sdp4430"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_TWL4030_CORE" = "y" -o "$CONFIG_TWL4030_CORE" = "m" ) &&
      ( test "$CONFIG_SND_OMAP_SOC" = "y" -o "$CONFIG_SND_OMAP_SOC" = "m" ) &&
      ( test "$CONFIG_MACH_OMAP_4430SDP" = "y" -o "$CONFIG_MACH_OMAP_4430SDP" = "m" ) ); then
      CONFIG_SND_OMAP_SOC_DMIC="m"
      CONFIG_SND_OMAP_SOC_MCPDM="m"
      CONFIG_SND_SOC_TWL6040="m"
      CONFIG_SND_SOC_DMIC="m"
      CONFIG_SND_OMAP_SOC_SDP4430="m"
    fi
  fi
  if alsa_check_kconfig_card "omap-soc-omap4-hdmi"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_OMAP_SOC" = "y" -o "$CONFIG_SND_OMAP_SOC" = "m" ) &&
      ( test "$CONFIG_OMAP4_DSS_HDMI" = "y" -o "$CONFIG_OMAP4_DSS_HDMI" = "m" ) &&
      ( test "$CONFIG_OMAP2_DSS" = "y" -o "$CONFIG_OMAP2_DSS" = "m" ) &&
      ( test "$CONFIG_ARCH_OMAP4" = "y" -o "$CONFIG_ARCH_OMAP4" = "m" ) ); then
      CONFIG_SND_OMAP_SOC_HDMI="m"
      CONFIG_SND_OMAP_SOC_OMAP4_HDMI="m"
    fi
  fi
  if alsa_check_kconfig_card "omap-soc-omap3-pandora"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_TWL4030_CORE" = "y" -o "$CONFIG_TWL4030_CORE" = "m" ) &&
      ( test "$CONFIG_SND_OMAP_SOC" = "y" -o "$CONFIG_SND_OMAP_SOC" = "m" ) &&
      ( test "$CONFIG_MACH_OMAP3_PANDORA" = "y" -o "$CONFIG_MACH_OMAP3_PANDORA" = "m" ) ); then
      CONFIG_SND_OMAP_SOC_MCBSP="m"
      CONFIG_SND_SOC_TWL4030="m"
      CONFIG_SND_OMAP_SOC_OMAP3_PANDORA="m"
    fi
  fi
  if alsa_check_kconfig_card "omap-soc-omap3-beagle"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_TWL4030_CORE" = "y" -o "$CONFIG_TWL4030_CORE" = "m" ) &&
      ( test "$CONFIG_SND_OMAP_SOC" = "y" -o "$CONFIG_SND_OMAP_SOC" = "m" ) ) &&
      ( ( ( test "$CONFIG_MACH_OMAP3_BEAGLE" = "y" -o "$CONFIG_MACH_OMAP3_BEAGLE" = "m" ) ||
      ( test "$CONFIG_MACH_DEVKIT8000" = "y" -o "$CONFIG_MACH_DEVKIT8000" = "m" ) ) ); then
      CONFIG_SND_OMAP_SOC_MCBSP="m"
      CONFIG_SND_SOC_TWL4030="m"
      CONFIG_SND_OMAP_SOC_OMAP3_BEAGLE="m"
    fi
  fi
  if alsa_check_kconfig_card "omap-soc-zoom2"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_TWL4030_CORE" = "y" -o "$CONFIG_TWL4030_CORE" = "m" ) &&
      ( test "$CONFIG_SND_OMAP_SOC" = "y" -o "$CONFIG_SND_OMAP_SOC" = "m" ) &&
      ( test "$CONFIG_MACH_OMAP_ZOOM2" = "y" -o "$CONFIG_MACH_OMAP_ZOOM2" = "m" ) ); then
      CONFIG_SND_OMAP_SOC_MCBSP="m"
      CONFIG_SND_SOC_TWL4030="m"
      CONFIG_SND_OMAP_SOC_ZOOM2="m"
    fi
  fi
  if alsa_check_kconfig_card "omap-soc-igep0020"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_TWL4030_CORE" = "y" -o "$CONFIG_TWL4030_CORE" = "m" ) &&
      ( test "$CONFIG_SND_OMAP_SOC" = "y" -o "$CONFIG_SND_OMAP_SOC" = "m" ) &&
      ( test "$CONFIG_MACH_IGEP0020" = "y" -o "$CONFIG_MACH_IGEP0020" = "m" ) ); then
      CONFIG_SND_OMAP_SOC_MCBSP="m"
      CONFIG_SND_SOC_TWL4030="m"
      CONFIG_SND_OMAP_SOC_IGEP0020="m"
    fi
  fi
  if alsa_check_kconfig_card "kirkwood-soc"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_ARCH_KIRKWOOD" = "y" -o "$CONFIG_ARCH_KIRKWOOD" = "m" ); then
      CONFIG_SND_KIRKWOOD_SOC="m"
    fi
  fi
  if alsa_check_kconfig_card "kirkwood-soc-openrd"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_KIRKWOOD_SOC" = "y" -o "$CONFIG_SND_KIRKWOOD_SOC" = "m" ) &&
      ( ( test "$CONFIG_MACH_OPENRD_CLIENT" = "y" -o "$CONFIG_MACH_OPENRD_CLIENT" = "m" ) ||
      ( test "$CONFIG_MACH_OPENRD_ULTIMATE" = "y" -o "$CONFIG_MACH_OPENRD_ULTIMATE" = "m" ) ) ) &&
      ( test "$CONFIG_I2C" = "y" -o "$CONFIG_I2C" = "m" ); then
      CONFIG_SND_KIRKWOOD_SOC_I2S="m"
      CONFIG_SND_SOC_CS42L51="m"
      CONFIG_SND_KIRKWOOD_SOC_OPENRD="m"
    fi
  fi
  if alsa_check_kconfig_card "kirkwood-soc-t5325"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_KIRKWOOD_SOC" = "y" -o "$CONFIG_SND_KIRKWOOD_SOC" = "m" ) &&
      ( test "$CONFIG_MACH_T5325" = "y" -o "$CONFIG_MACH_T5325" = "m" ) &&
      ( test "$CONFIG_I2C" = "y" -o "$CONFIG_I2C" = "m" ) ); then
      CONFIG_SND_KIRKWOOD_SOC_I2S="m"
      CONFIG_SND_SOC_ALC5623="m"
      CONFIG_SND_KIRKWOOD_SOC_T5325="m"
    fi
  fi
  if alsa_check_kconfig_card "mfld-machine"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_INTEL_SCU_IPC" = "y" -o "$CONFIG_INTEL_SCU_IPC" = "m" ); then
      CONFIG_SND_SOC_SN95031="m"
      CONFIG_SND_SST_PLATFORM="m"
      CONFIG_SND_MFLD_MACHINE="m"
    fi
  fi
  if alsa_check_kconfig_card "mxs-soc"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_ARCH_MXS" = "y" -o "$CONFIG_ARCH_MXS" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_MXS_SOC="m"
    fi
  fi
  if alsa_check_kconfig_card "soc-mxs-sgtl5000"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_SND_MXS_SOC" = "y" -o "$CONFIG_SND_MXS_SOC" = "m" ) &&
      ( test "$CONFIG_I2C" = "y" -o "$CONFIG_I2C" = "m" ); then
      CONFIG_SND_SOC_SGTL5000="m"
      CONFIG_SND_SOC_MXS_SGTL5000="m"
    fi
  fi
  if alsa_check_kconfig_card "pxa2xx-soc"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_ARCH_PXA" = "y" -o "$CONFIG_ARCH_PXA" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kvernum -ge 206014 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_ARM="y"
      CONFIG_SND_PXA2XX_LIB="m"
      CONFIG_SND_PXA2XX_SOC="m"
    fi
  fi
  if alsa_check_kconfig_card "pxa2xx-soc-corgi"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_PXA2XX_SOC" = "y" -o "$CONFIG_SND_PXA2XX_SOC" = "m" ) &&
      ( test "$CONFIG_PXA_SHARP_C7xx" = "y" -o "$CONFIG_PXA_SHARP_C7xx" = "m" ) ); then
      CONFIG_SND_PXA2XX_SOC_I2S="m"
      CONFIG_SND_SOC_WM8731="m"
      CONFIG_SND_PXA2XX_SOC_CORGI="m"
    fi
  fi
  if alsa_check_kconfig_card "pxa2xx-soc-spitz"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_PXA2XX_SOC" = "y" -o "$CONFIG_SND_PXA2XX_SOC" = "m" ) &&
      ( test "$CONFIG_PXA_SHARP_Cxx00" = "y" -o "$CONFIG_PXA_SHARP_Cxx00" = "m" ) ); then
      CONFIG_SND_PXA2XX_SOC_I2S="m"
      CONFIG_SND_SOC_WM8750="m"
      CONFIG_SND_PXA2XX_SOC_SPITZ="m"
    fi
  fi
  if alsa_check_kconfig_card "pxa2xx-soc-z2"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_PXA2XX_SOC" = "y" -o "$CONFIG_SND_PXA2XX_SOC" = "m" ) &&
      ( test "$CONFIG_MACH_ZIPIT2" = "y" -o "$CONFIG_MACH_ZIPIT2" = "m" ) ); then
      CONFIG_SND_PXA2XX_SOC_I2S="m"
      CONFIG_SND_SOC_WM8750="m"
      CONFIG_SND_PXA2XX_SOC_Z2="m"
    fi
  fi
  if alsa_check_kconfig_card "pxa2xx-soc-poodle"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_PXA2XX_SOC" = "y" -o "$CONFIG_SND_PXA2XX_SOC" = "m" ) &&
      ( test "$CONFIG_MACH_POODLE" = "y" -o "$CONFIG_MACH_POODLE" = "m" ) ); then
      CONFIG_SND_PXA2XX_SOC_I2S="m"
      CONFIG_SND_SOC_WM8731="m"
      CONFIG_SND_PXA2XX_SOC_POODLE="m"
    fi
  fi
  if alsa_check_kconfig_card "pxa2xx-soc-tosa"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_PXA2XX_SOC" = "y" -o "$CONFIG_SND_PXA2XX_SOC" = "m" ) &&
      ( test "$CONFIG_MACH_TOSA" = "y" -o "$CONFIG_MACH_TOSA" = "m" ) ) &&
      ( test "$CONFIG_MFD_TC6393XB" = "y" -o "$CONFIG_MFD_TC6393XB" = "m" ); then
      test $kvernum -ge 206014 && CONFIG_AC97_BUS="m"
      CONFIG_SND_ARM="y"
      CONFIG_SND_PXA2XX_LIB_AC97="y"
      CONFIG_SND_SOC_AC97_BUS="y"
      CONFIG_SND_PXA2XX_SOC_AC97="m"
      CONFIG_SND_SOC_WM9712="m"
      CONFIG_SND_PXA2XX_SOC_TOSA="m"
    fi
  fi
  if alsa_check_kconfig_card "pxa2xx-soc-e740"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_PXA2XX_SOC" = "y" -o "$CONFIG_SND_PXA2XX_SOC" = "m" ) &&
      ( test "$CONFIG_MACH_E740" = "y" -o "$CONFIG_MACH_E740" = "m" ) ); then
      test $kvernum -ge 206014 && CONFIG_AC97_BUS="m"
      CONFIG_SND_ARM="y"
      CONFIG_SND_PXA2XX_LIB_AC97="y"
      CONFIG_SND_SOC_AC97_BUS="y"
      CONFIG_SND_SOC_WM9705="m"
      CONFIG_SND_PXA2XX_SOC_AC97="m"
      CONFIG_SND_PXA2XX_SOC_E740="m"
    fi
  fi
  if alsa_check_kconfig_card "pxa2xx-soc-e750"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_PXA2XX_SOC" = "y" -o "$CONFIG_SND_PXA2XX_SOC" = "m" ) &&
      ( test "$CONFIG_MACH_E750" = "y" -o "$CONFIG_MACH_E750" = "m" ) ); then
      test $kvernum -ge 206014 && CONFIG_AC97_BUS="m"
      CONFIG_SND_ARM="y"
      CONFIG_SND_PXA2XX_LIB_AC97="y"
      CONFIG_SND_SOC_AC97_BUS="y"
      CONFIG_SND_SOC_WM9705="m"
      CONFIG_SND_PXA2XX_SOC_AC97="m"
      CONFIG_SND_PXA2XX_SOC_E750="m"
    fi
  fi
  if alsa_check_kconfig_card "pxa2xx-soc-e800"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_PXA2XX_SOC" = "y" -o "$CONFIG_SND_PXA2XX_SOC" = "m" ) &&
      ( test "$CONFIG_MACH_E800" = "y" -o "$CONFIG_MACH_E800" = "m" ) ); then
      test $kvernum -ge 206014 && CONFIG_AC97_BUS="m"
      CONFIG_SND_ARM="y"
      CONFIG_SND_PXA2XX_LIB_AC97="y"
      CONFIG_SND_SOC_AC97_BUS="y"
      CONFIG_SND_SOC_WM9712="m"
      CONFIG_SND_PXA2XX_SOC_AC97="m"
      CONFIG_SND_PXA2XX_SOC_E800="m"
    fi
  fi
  if alsa_check_kconfig_card "pxa2xx-soc-em-x270"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_PXA2XX_SOC" = "y" -o "$CONFIG_SND_PXA2XX_SOC" = "m" ) &&
      ( ( test "$CONFIG_MACH_EM_X270" = "y" -o "$CONFIG_MACH_EM_X270" = "m" ) ||
      ( test "$CONFIG_MACH_EXEDA" = "y" -o "$CONFIG_MACH_EXEDA" = "m" ) ||
      ( test "$CONFIG_MACH_CM_X300" = "y" -o "$CONFIG_MACH_CM_X300" = "m" ) ) ); then
      test $kvernum -ge 206014 && CONFIG_AC97_BUS="m"
      CONFIG_SND_ARM="y"
      CONFIG_SND_PXA2XX_LIB_AC97="y"
      CONFIG_SND_SOC_AC97_BUS="y"
      CONFIG_SND_PXA2XX_SOC_AC97="m"
      CONFIG_SND_SOC_WM9712="m"
      CONFIG_SND_PXA2XX_SOC_EM_X270="m"
    fi
  fi
  if alsa_check_kconfig_option "pxa2xx-soc-palm27x"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_PXA2XX_SOC" = "y" -o "$CONFIG_SND_PXA2XX_SOC" = "m" ) &&
      ( ( test "$CONFIG_MACH_PALMLD" = "y" -o "$CONFIG_MACH_PALMLD" = "m" ) ||
      ( test "$CONFIG_MACH_PALMTX" = "y" -o "$CONFIG_MACH_PALMTX" = "m" ) ||
      ( test "$CONFIG_MACH_PALMT5" = "y" -o "$CONFIG_MACH_PALMT5" = "m" ) ||
      ( test "$CONFIG_MACH_PALMTE2" = "y" -o "$CONFIG_MACH_PALMTE2" = "m" ) ) ); then
      test $kvernum -ge 206014 && CONFIG_AC97_BUS="m"
      CONFIG_SND_ARM="y"
      CONFIG_SND_PXA2XX_LIB_AC97="y"
      CONFIG_SND_SOC_AC97_BUS="y"
      CONFIG_SND_PXA2XX_SOC_AC97="m"
      CONFIG_SND_SOC_WM9712="m"
      CONFIG_SND_PXA2XX_SOC_PALM27X="y"
    fi
  fi
  if alsa_check_kconfig_card "soc-saarb"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_PXA2XX_SOC" = "y" -o "$CONFIG_SND_PXA2XX_SOC" = "m" ) &&
      ( test "$CONFIG_MACH_SAARB" = "y" -o "$CONFIG_MACH_SAARB" = "m" ) ) &&
      ( test "$CONFIG_MFD_88PM860X" = "y" -o "$CONFIG_MFD_88PM860X" = "m" ); then
      CONFIG_SND_PXA_SOC_SSP="m"
      CONFIG_SND_SOC_88PM860X="m"
      CONFIG_SND_SOC_SAARB="m"
    fi
  fi
  if alsa_check_kconfig_card "soc-tavorevb3"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_PXA2XX_SOC" = "y" -o "$CONFIG_SND_PXA2XX_SOC" = "m" ) &&
      ( test "$CONFIG_MACH_TAVOREVB3" = "y" -o "$CONFIG_MACH_TAVOREVB3" = "m" ) ) &&
      ( test "$CONFIG_MFD_88PM860X" = "y" -o "$CONFIG_MFD_88PM860X" = "m" ); then
      CONFIG_SND_PXA_SOC_SSP="m"
      CONFIG_SND_SOC_88PM860X="m"
      CONFIG_SND_SOC_TAVOREVB3="m"
    fi
  fi
  if alsa_check_kconfig_card "soc-zylonite"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_PXA2XX_SOC" = "y" -o "$CONFIG_SND_PXA2XX_SOC" = "m" ) &&
      ( test "$CONFIG_MACH_ZYLONITE" = "y" -o "$CONFIG_MACH_ZYLONITE" = "m" ) ); then
      test $kvernum -ge 206014 && CONFIG_AC97_BUS="m"
      CONFIG_SND_ARM="y"
      CONFIG_SND_PXA2XX_LIB_AC97="y"
      CONFIG_SND_SOC_AC97_BUS="y"
      CONFIG_SND_PXA2XX_SOC_AC97="m"
      CONFIG_SND_PXA_SOC_SSP="m"
      CONFIG_SND_SOC_WM9713="m"
      CONFIG_SND_SOC_ZYLONITE="m"
    fi
  fi
  if alsa_check_kconfig_card "soc-raumfeld"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_PXA2XX_SOC" = "y" -o "$CONFIG_SND_PXA2XX_SOC" = "m" ) &&
      ( ( test "$CONFIG_MACH_RAUMFELD_SPEAKER" = "y" -o "$CONFIG_MACH_RAUMFELD_SPEAKER" = "m" ) ||
      ( test "$CONFIG_MACH_RAUMFELD_CONNECTOR" = "y" -o "$CONFIG_MACH_RAUMFELD_CONNECTOR" = "m" ) ) ) &&
      ( ( test "$CONFIG_I2C" = "y" -o "$CONFIG_I2C" = "m" ) &&
      ( test "$CONFIG_SPI_MASTER" = "y" -o "$CONFIG_SPI_MASTER" = "m" ) ); then
      CONFIG_SND_PXA_SOC_SSP="m"
      CONFIG_SND_SOC_CS4270="m"
      CONFIG_SND_SOC_AK4104="m"
      CONFIG_SND_SOC_RAUMFELD="m"
    fi
  fi
  if alsa_check_kconfig_card "pxa2xx-soc-hx4700"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_PXA2XX_SOC" = "y" -o "$CONFIG_SND_PXA2XX_SOC" = "m" ) &&
      ( test "$CONFIG_MACH_H4700" = "y" -o "$CONFIG_MACH_H4700" = "m" ) &&
      ( test "$CONFIG_I2C" = "y" -o "$CONFIG_I2C" = "m" ) ); then
      CONFIG_SND_PXA2XX_SOC_I2S="m"
      CONFIG_SND_SOC_AK4641="m"
      CONFIG_SND_PXA2XX_SOC_HX4700="m"
    fi
  fi
  if alsa_check_kconfig_card "pxa2xx-soc-magician"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_PXA2XX_SOC" = "y" -o "$CONFIG_SND_PXA2XX_SOC" = "m" ) &&
      ( test "$CONFIG_MACH_MAGICIAN" = "y" -o "$CONFIG_MACH_MAGICIAN" = "m" ) ); then
      CONFIG_SND_PXA2XX_SOC_I2S="m"
      CONFIG_SND_PXA_SOC_SSP="m"
      CONFIG_SND_SOC_UDA1380="m"
      CONFIG_SND_PXA2XX_SOC_MAGICIAN="m"
    fi
  fi
  if alsa_check_kconfig_card "pxa2xx-soc-mioa701"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_PXA2XX_SOC" = "y" -o "$CONFIG_SND_PXA2XX_SOC" = "m" ) &&
      ( test "$CONFIG_MACH_MIOA701" = "y" -o "$CONFIG_MACH_MIOA701" = "m" ) ); then
      test $kvernum -ge 206014 && CONFIG_AC97_BUS="m"
      CONFIG_SND_ARM="y"
      CONFIG_SND_PXA2XX_LIB_AC97="y"
      CONFIG_SND_SOC_AC97_BUS="y"
      CONFIG_SND_PXA2XX_SOC_AC97="m"
      CONFIG_SND_SOC_WM9713="m"
      CONFIG_SND_PXA2XX_SOC_MIOA701="m"
    fi
  fi
  if alsa_check_kconfig_card "soc-samsung"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_ARCH_S3C2410" = "y" -o "$CONFIG_ARCH_S3C2410" = "m" ) ||
      ( test "$CONFIG_ARCH_S3C64XX" = "y" -o "$CONFIG_ARCH_S3C64XX" = "m" ) ||
      ( test "$CONFIG_ARCH_S5PC100" = "y" -o "$CONFIG_ARCH_S5PC100" = "m" ) ||
      ( test "$CONFIG_ARCH_S5PV210" = "y" -o "$CONFIG_ARCH_S5PV210" = "m" ) ||
      ( test "$CONFIG_ARCH_S5P64X0" = "y" -o "$CONFIG_ARCH_S5P64X0" = "m" ) ||
      ( test "$CONFIG_ARCH_EXYNOS4" = "y" -o "$CONFIG_ARCH_EXYNOS4" = "m" ) ) &&
      ( test "$CONFIG_S3C64XX_DMA" = "y" -o "$CONFIG_S3C64XX_DMA" = "m" ) &&
      ( test "$CONFIG_S3C2410_DMA" = "y" -o "$CONFIG_S3C2410_DMA" = "m" ); then
      CONFIG_SND_SOC_SAMSUNG="m"
    fi
  fi
  if alsa_check_kconfig_card "soc-samsung-neo1973-wm8753"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_SOC_SAMSUNG" = "y" -o "$CONFIG_SND_SOC_SAMSUNG" = "m" ) &&
      ( ( test "$CONFIG_MACH_NEO1973_GTA01" = "y" -o "$CONFIG_MACH_NEO1973_GTA01" = "m" ) ||
      ( test "$CONFIG_MACH_NEO1973_GTA02" = "y" -o "$CONFIG_MACH_NEO1973_GTA02" = "m" ) ) ); then
      CONFIG_SND_S3C24XX_I2S="m"
      CONFIG_SND_SOC_WM8753="m"
      CONFIG_SND_SOC_LM4857="m"
      CONFIG_SND_SOC_DFBMCS320="m"
      CONFIG_SND_SOC_SAMSUNG_NEO1973_WM8753="m"
    fi
  fi
  if alsa_check_kconfig_card "soc-samsung-jive-wm8750"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_SOC_SAMSUNG" = "y" -o "$CONFIG_SND_SOC_SAMSUNG" = "m" ) &&
      ( test "$CONFIG_MACH_JIVE" = "y" -o "$CONFIG_MACH_JIVE" = "m" ) ); then
      CONFIG_SND_S3C_I2SV2_SOC="m"
      CONFIG_SND_SOC_WM8750="m"
      CONFIG_SND_S3C2412_SOC_I2S="m"
      CONFIG_SND_SOC_SAMSUNG_JIVE_WM8750="m"
    fi
  fi
  if alsa_check_kconfig_card "soc-samsung-smdk-wm8580"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_SOC_SAMSUNG" = "y" -o "$CONFIG_SND_SOC_SAMSUNG" = "m" ) &&
      ( ( test "$CONFIG_MACH_SMDK6410" = "y" -o "$CONFIG_MACH_SMDK6410" = "m" ) ||
      ( test "$CONFIG_MACH_SMDKC100" = "y" -o "$CONFIG_MACH_SMDKC100" = "m" ) ||
      ( test "$CONFIG_MACH_SMDK6440" = "y" -o "$CONFIG_MACH_SMDK6440" = "m" ) ||
      ( test "$CONFIG_MACH_SMDK6450" = "y" -o "$CONFIG_MACH_SMDK6450" = "m" ) ||
      ( test "$CONFIG_MACH_SMDKV210" = "y" -o "$CONFIG_MACH_SMDKV210" = "m" ) ||
      ( test "$CONFIG_MACH_SMDKC110" = "y" -o "$CONFIG_MACH_SMDKC110" = "m" ) ) ); then
      CONFIG_SND_SOC_WM8580="m"
      CONFIG_SND_SAMSUNG_I2S="m"
      CONFIG_SND_SOC_SAMSUNG_SMDK_WM8580="m"
    fi
  fi
  if alsa_check_kconfig_card "soc-samsung-smdk-wm8994"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_SOC_SAMSUNG" = "y" -o "$CONFIG_SND_SOC_SAMSUNG" = "m" ) &&
      ( ( test "$CONFIG_MACH_SMDKV310" = "y" -o "$CONFIG_MACH_SMDKV310" = "m" ) ||
      ( test "$CONFIG_MACH_SMDKC210" = "y" -o "$CONFIG_MACH_SMDKC210" = "m" ) ||
      ( test "$CONFIG_MACH_SMDK4212" = "y" -o "$CONFIG_MACH_SMDK4212" = "m" ) ) ) &&
      ( ( test "$CONFIG_I2C" = "y" -o "$CONFIG_I2C" = "m" ) &&
      ( test "$CONFIG_GENERIC_HARDIRQS" = "y" -o "$CONFIG_GENERIC_HARDIRQS" = "m" ) ) &&
      ( test "$CONFIG_MFD_WM8994" = "y" -o "$CONFIG_MFD_WM8994" = "m" ); then
      CONFIG_SND_SOC_WM8994="m"
      CONFIG_SND_SAMSUNG_I2S="m"
      CONFIG_SND_SOC_SAMSUNG_SMDK_WM8994="m"
    fi
  fi
  if alsa_check_kconfig_card "soc-samsung-smdk2443-wm9710"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_SOC_SAMSUNG" = "y" -o "$CONFIG_SND_SOC_SAMSUNG" = "m" ) &&
      ( test "$CONFIG_MACH_SMDK2443" = "y" -o "$CONFIG_MACH_SMDK2443" = "m" ) ) &&
      ( test "$CONFIG_S3C2410_DMA" = "y" -o "$CONFIG_S3C2410_DMA" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kvernum -ge 206014 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_SOC_AC97_BUS="y"
      test $kvernum -ge 206014 && CONFIG_AC97_BUS="m"
      CONFIG_SND_SOC_AC97_CODEC="m"
      CONFIG_SND_SAMSUNG_AC97="m"
      CONFIG_SND_SOC_SAMSUNG_SMDK2443_WM9710="m"
    fi
  fi
  if alsa_check_kconfig_card "soc-samsung-ln2440sbc-alc650"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_SOC_SAMSUNG" = "y" -o "$CONFIG_SND_SOC_SAMSUNG" = "m" ) &&
      ( test "$CONFIG_ARCH_S3C2410" = "y" -o "$CONFIG_ARCH_S3C2410" = "m" ) ) &&
      ( test "$CONFIG_S3C2410_DMA" = "y" -o "$CONFIG_S3C2410_DMA" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test $kvernum -ge 206014 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_SOC_AC97_BUS="y"
      test $kvernum -ge 206014 && CONFIG_AC97_BUS="m"
      CONFIG_SND_SOC_AC97_CODEC="m"
      CONFIG_SND_SAMSUNG_AC97="m"
      CONFIG_SND_SOC_SAMSUNG_LN2440SBC_ALC650="m"
    fi
  fi
  if alsa_check_kconfig_card "soc-samsung-s3c24xx-uda134x"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_SOC_SAMSUNG" = "y" -o "$CONFIG_SND_SOC_SAMSUNG" = "m" ) &&
      ( test "$CONFIG_ARCH_S3C2410" = "y" -o "$CONFIG_ARCH_S3C2410" = "m" ) ); then
      CONFIG_SND_S3C24XX_I2S="m"
      CONFIG_SND_SOC_L3="m"
      CONFIG_SND_SOC_UDA134X="m"
      CONFIG_SND_SOC_SAMSUNG_S3C24XX_UDA134X="m"
    fi
  fi
  if alsa_check_kconfig_card "soc-samsung-simtec-tlv320aic23"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_SOC_SAMSUNG" = "y" -o "$CONFIG_SND_SOC_SAMSUNG" = "m" ) &&
      ( test "$CONFIG_ARCH_S3C2410" = "y" -o "$CONFIG_ARCH_S3C2410" = "m" ) ); then
      CONFIG_SND_S3C24XX_I2S="m"
      CONFIG_SND_SOC_TLV320AIC23="m"
      CONFIG_SND_SOC_SAMSUNG_SIMTEC="m"
      CONFIG_SND_SOC_SAMSUNG_SIMTEC_TLV320AIC23="m"
    fi
  fi
  if alsa_check_kconfig_card "soc-samsung-simtec-hermes"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_SOC_SAMSUNG" = "y" -o "$CONFIG_SND_SOC_SAMSUNG" = "m" ) &&
      ( test "$CONFIG_ARCH_S3C2410" = "y" -o "$CONFIG_ARCH_S3C2410" = "m" ) ); then
      CONFIG_SND_S3C24XX_I2S="m"
      CONFIG_SND_SOC_TLV320AIC3X="m"
      CONFIG_SND_SOC_SAMSUNG_SIMTEC="m"
      CONFIG_SND_SOC_SAMSUNG_SIMTEC_HERMES="m"
    fi
  fi
  if alsa_check_kconfig_card "soc-samsung-h1940-uda1380"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_SOC_SAMSUNG" = "y" -o "$CONFIG_SND_SOC_SAMSUNG" = "m" ) &&
      ( test "$CONFIG_ARCH_H1940" = "y" -o "$CONFIG_ARCH_H1940" = "m" ) ); then
      CONFIG_SND_S3C24XX_I2S="m"
      CONFIG_SND_SOC_UDA1380="m"
      CONFIG_SND_SOC_SAMSUNG_H1940_UDA1380="m"
    fi
  fi
  if alsa_check_kconfig_card "soc-samsung-rx1950-uda1380"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_SOC_SAMSUNG" = "y" -o "$CONFIG_SND_SOC_SAMSUNG" = "m" ) &&
      ( test "$CONFIG_MACH_RX1950" = "y" -o "$CONFIG_MACH_RX1950" = "m" ) ); then
      CONFIG_SND_S3C24XX_I2S="m"
      CONFIG_SND_SOC_UDA1380="m"
      CONFIG_SND_SOC_SAMSUNG_RX1950_UDA1380="m"
    fi
  fi
  if alsa_check_kconfig_card "soc-samsung-smdk-wm9713"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_SOC_SAMSUNG" = "y" -o "$CONFIG_SND_SOC_SAMSUNG" = "m" ) &&
      ( ( test "$CONFIG_MACH_SMDK6410" = "y" -o "$CONFIG_MACH_SMDK6410" = "m" ) ||
      ( test "$CONFIG_MACH_SMDKC100" = "y" -o "$CONFIG_MACH_SMDKC100" = "m" ) ||
      ( test "$CONFIG_MACH_SMDKV210" = "y" -o "$CONFIG_MACH_SMDKV210" = "m" ) ||
      ( test "$CONFIG_MACH_SMDKC110" = "y" -o "$CONFIG_MACH_SMDKC110" = "m" ) ||
      ( test "$CONFIG_MACH_SMDKV310" = "y" -o "$CONFIG_MACH_SMDKV310" = "m" ) ||
      ( test "$CONFIG_MACH_SMDKC210" = "y" -o "$CONFIG_MACH_SMDKC210" = "m" ) ) ); then
      CONFIG_SND_SOC_AC97_BUS="y"
      CONFIG_SND_SOC_WM9713="m"
      CONFIG_SND_SAMSUNG_AC97="m"
      CONFIG_SND_SOC_SAMSUNG_SMDK_WM9713="m"
    fi
  fi
  if alsa_check_kconfig_card "soc-smartq"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_SOC_SAMSUNG" = "y" -o "$CONFIG_SND_SOC_SAMSUNG" = "m" ) &&
      ( test "$CONFIG_MACH_SMARTQ" = "y" -o "$CONFIG_MACH_SMARTQ" = "m" ) ); then
      CONFIG_SND_SAMSUNG_I2S="m"
      CONFIG_SND_SOC_WM8750="m"
      CONFIG_SND_SOC_SMARTQ="m"
    fi
  fi
  if alsa_check_kconfig_card "soc-goni-aquila-wm8994"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_SOC_SAMSUNG" = "y" -o "$CONFIG_SND_SOC_SAMSUNG" = "m" ) &&
      ( ( test "$CONFIG_MACH_GONI" = "y" -o "$CONFIG_MACH_GONI" = "m" ) ||
      ( test "$CONFIG_MACH_AQUILA" = "y" -o "$CONFIG_MACH_AQUILA" = "m" ) ) ) &&
      ( ( test "$CONFIG_I2C" = "y" -o "$CONFIG_I2C" = "m" ) &&
      ( test "$CONFIG_GENERIC_HARDIRQS" = "y" -o "$CONFIG_GENERIC_HARDIRQS" = "m" ) ) &&
      ( test "$CONFIG_MFD_WM8994" = "y" -o "$CONFIG_MFD_WM8994" = "m" ); then
      CONFIG_SND_SAMSUNG_I2S="m"
      CONFIG_SND_SOC_WM8994="m"
      CONFIG_SND_SOC_GONI_AQUILA_WM8994="m"
    fi
  fi
  if alsa_check_kconfig_card "soc-samsung-smdk-spdif"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_SOC_SAMSUNG" = "y" -o "$CONFIG_SND_SOC_SAMSUNG" = "m" ) &&
      ( ( test "$CONFIG_MACH_SMDKC100" = "y" -o "$CONFIG_MACH_SMDKC100" = "m" ) ||
      ( test "$CONFIG_MACH_SMDKC110" = "y" -o "$CONFIG_MACH_SMDKC110" = "m" ) ||
      ( test "$CONFIG_MACH_SMDKV210" = "y" -o "$CONFIG_MACH_SMDKV210" = "m" ) ||
      ( test "$CONFIG_MACH_SMDKV310" = "y" -o "$CONFIG_MACH_SMDKV310" = "m" ) ||
      ( test "$CONFIG_MACH_SMDK4212" = "y" -o "$CONFIG_MACH_SMDK4212" = "m" ) ) ); then
      CONFIG_SND_SOC_SPDIF="m"
      CONFIG_SND_SAMSUNG_SPDIF="m"
      CONFIG_SND_SOC_SAMSUNG_SMDK_SPDIF="m"
    fi
  fi
  if alsa_check_kconfig_card "soc-smdk-wm8580-pcm"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_SOC_SAMSUNG" = "y" -o "$CONFIG_SND_SOC_SAMSUNG" = "m" ) &&
      ( ( test "$CONFIG_MACH_SMDK6450" = "y" -o "$CONFIG_MACH_SMDK6450" = "m" ) ||
      ( test "$CONFIG_MACH_SMDKV210" = "y" -o "$CONFIG_MACH_SMDKV210" = "m" ) ||
      ( test "$CONFIG_MACH_SMDKC110" = "y" -o "$CONFIG_MACH_SMDKC110" = "m" ) ) ); then
      CONFIG_SND_SOC_WM8580="m"
      CONFIG_SND_SAMSUNG_PCM="m"
      CONFIG_SND_SOC_SMDK_WM8580_PCM="m"
    fi
  fi
  if alsa_check_kconfig_card "soc-smdk-wm8994-pcm"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_SOC_SAMSUNG" = "y" -o "$CONFIG_SND_SOC_SAMSUNG" = "m" ) &&
      ( ( test "$CONFIG_MACH_SMDKC210" = "y" -o "$CONFIG_MACH_SMDKC210" = "m" ) ||
      ( test "$CONFIG_MACH_SMDKV310" = "y" -o "$CONFIG_MACH_SMDKV310" = "m" ) ||
      ( test "$CONFIG_MACH_SMDK4212" = "y" -o "$CONFIG_MACH_SMDK4212" = "m" ) ) ) &&
      ( ( test "$CONFIG_I2C" = "y" -o "$CONFIG_I2C" = "m" ) &&
      ( test "$CONFIG_GENERIC_HARDIRQS" = "y" -o "$CONFIG_GENERIC_HARDIRQS" = "m" ) ) &&
      ( test "$CONFIG_MFD_WM8994" = "y" -o "$CONFIG_MFD_WM8994" = "m" ); then
      CONFIG_SND_SOC_WM8994="m"
      CONFIG_SND_SAMSUNG_PCM="m"
      CONFIG_SND_SOC_SMDK_WM8994_PCM="m"
    fi
  fi
  if alsa_check_kconfig_card "soc-speyside"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_SOC_SAMSUNG" = "y" -o "$CONFIG_SND_SOC_SAMSUNG" = "m" ) &&
      ( test "$CONFIG_MACH_WLF_CRAGG_6410" = "y" -o "$CONFIG_MACH_WLF_CRAGG_6410" = "m" ) ); then
      CONFIG_SND_SAMSUNG_I2S="m"
      CONFIG_SND_SOC_WM8996="m"
      CONFIG_SND_SOC_WM9081="m"
      CONFIG_SND_SOC_WM1250_EV1="m"
      CONFIG_SND_SOC_SPEYSIDE="m"
    fi
  fi
  if alsa_check_kconfig_card "soc-tobermory"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_SOC_SAMSUNG" = "y" -o "$CONFIG_SND_SOC_SAMSUNG" = "m" ) &&
      ( test "$CONFIG_MACH_WLF_CRAGG_6410" = "y" -o "$CONFIG_MACH_WLF_CRAGG_6410" = "m" ) ); then
      CONFIG_SND_SAMSUNG_I2S="m"
      CONFIG_SND_SOC_WM8962="m"
      CONFIG_SND_SOC_TOBERMORY="m"
    fi
  fi
  if alsa_check_kconfig_card "soc-lowland"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_SOC_SAMSUNG" = "y" -o "$CONFIG_SND_SOC_SAMSUNG" = "m" ) &&
      ( test "$CONFIG_MACH_WLF_CRAGG_6410" = "y" -o "$CONFIG_MACH_WLF_CRAGG_6410" = "m" ) ); then
      CONFIG_SND_SAMSUNG_I2S="m"
      CONFIG_SND_SOC_WM5100="m"
      CONFIG_SND_SOC_WM9081="m"
      CONFIG_SND_SOC_LOWLAND="m"
    fi
  fi
  if alsa_check_kconfig_card "soc-littlemill"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_SOC_SAMSUNG" = "y" -o "$CONFIG_SND_SOC_SAMSUNG" = "m" ) &&
      ( test "$CONFIG_MACH_WLF_CRAGG_6410" = "y" -o "$CONFIG_MACH_WLF_CRAGG_6410" = "m" ) ) &&
      ( test "$CONFIG_MFD_WM8994" = "y" -o "$CONFIG_MFD_WM8994" = "m" ); then
      CONFIG_SND_SAMSUNG_I2S="m"
      CONFIG_SND_SOC_WM8994="m"
      CONFIG_SND_SOC_LITTLEMILL="m"
    fi
  fi
  if alsa_check_kconfig_card "s6000-soc"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_XTENSA_VARIANT_S6000" = "y" -o "$CONFIG_XTENSA_VARIANT_S6000" = "m" ); then
      CONFIG_SND_S6000_SOC="m"
    fi
  fi
  if alsa_check_kconfig_card "s6000-soc-s6ipcam"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_S6000_SOC" = "y" -o "$CONFIG_SND_S6000_SOC" = "m" ) &&
      ( test "$CONFIG_XTENSA_PLATFORM_S6105" = "y" -o "$CONFIG_XTENSA_PLATFORM_S6105" = "m" ) ); then
      CONFIG_SND_S6000_SOC_I2S="m"
      CONFIG_SND_SOC_TLV320AIC3X="m"
      CONFIG_SND_S6000_SOC_S6IPCAM="m"
    fi
  fi
  if alsa_check_kconfig_card "soc-pcm-sh7760"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SUPERH" = "y" -o "$CONFIG_SUPERH" = "m" ) ||
      ( test "$CONFIG_ARCH_SHMOBILE" = "y" -o "$CONFIG_ARCH_SHMOBILE" = "m" ) ) &&
      ( ( test "$CONFIG_CPU_SUBTYPE_SH7760" = "y" -o "$CONFIG_CPU_SUBTYPE_SH7760" = "m" ) &&
      ( test "$CONFIG_SH_DMABRG" = "y" -o "$CONFIG_SH_DMABRG" = "m" ) ); then
      CONFIG_SND_SOC_PCM_SH7760="m"
    fi
  fi
  if alsa_check_kconfig_card "soc-sh4-fsi"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SUPERH" = "y" -o "$CONFIG_SUPERH" = "m" ) ||
      ( test "$CONFIG_ARCH_SHMOBILE" = "y" -o "$CONFIG_ARCH_SHMOBILE" = "m" ) ); then
      CONFIG_SND_SOC_SH4_FSI="m"
    fi
  fi
  if alsa_check_kconfig_card "sh7760-ac97"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SUPERH" = "y" -o "$CONFIG_SUPERH" = "m" ) ||
      ( test "$CONFIG_ARCH_SHMOBILE" = "y" -o "$CONFIG_ARCH_SHMOBILE" = "m" ) ) &&
      ( ( test "$CONFIG_CPU_SUBTYPE_SH7760" = "y" -o "$CONFIG_CPU_SUBTYPE_SH7760" = "m" ) &&
      ( test "$CONFIG_SND_SOC_PCM_SH7760" = "y" -o "$CONFIG_SND_SOC_PCM_SH7760" = "m" ) ); then
      test $kvernum -ge 206014 && CONFIG_AC97_BUS="m"
      CONFIG_SND_SOC_AC97_BUS="y"
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_SOC_SH4_HAC="m"
      CONFIG_SND_SOC_AC97_CODEC="m"
      CONFIG_SND_SH7760_AC97="m"
    fi
  fi
  if alsa_check_kconfig_card "fsi-ak4642"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SUPERH" = "y" -o "$CONFIG_SUPERH" = "m" ) ||
      ( test "$CONFIG_ARCH_SHMOBILE" = "y" -o "$CONFIG_ARCH_SHMOBILE" = "m" ) ) &&
      ( ( test "$CONFIG_SND_SOC_SH4_FSI" = "y" -o "$CONFIG_SND_SOC_SH4_FSI" = "m" ) &&
      ( test "$CONFIG_I2C" = "y" -o "$CONFIG_I2C" = "m" ) ); then
      CONFIG_SND_SOC_AK4642="m"
      CONFIG_SND_FSI_AK4642="m"
    fi
  fi
  if alsa_check_kconfig_card "fsi-da7210"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SUPERH" = "y" -o "$CONFIG_SUPERH" = "m" ) ||
      ( test "$CONFIG_ARCH_SHMOBILE" = "y" -o "$CONFIG_ARCH_SHMOBILE" = "m" ) ) &&
      ( ( test "$CONFIG_SND_SOC_SH4_FSI" = "y" -o "$CONFIG_SND_SOC_SH4_FSI" = "m" ) &&
      ( test "$CONFIG_I2C" = "y" -o "$CONFIG_I2C" = "m" ) ); then
      CONFIG_SND_SOC_DA7210="m"
      CONFIG_SND_FSI_DA7210="m"
    fi
  fi
  if alsa_check_kconfig_card "fsi-hdmi"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SUPERH" = "y" -o "$CONFIG_SUPERH" = "m" ) ||
      ( test "$CONFIG_ARCH_SHMOBILE" = "y" -o "$CONFIG_ARCH_SHMOBILE" = "m" ) ) &&
      ( ( test "$CONFIG_SND_SOC_SH4_FSI" = "y" -o "$CONFIG_SND_SOC_SH4_FSI" = "m" ) &&
      ( test "$CONFIG_FB_SH_MOBILE_HDMI" = "y" -o "$CONFIG_FB_SH_MOBILE_HDMI" = "m" ) ); then
      CONFIG_SND_FSI_HDMI="m"
    fi
  fi
  if alsa_check_kconfig_card "siu-migor"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SUPERH" = "y" -o "$CONFIG_SUPERH" = "m" ) ||
      ( test "$CONFIG_ARCH_SHMOBILE" = "y" -o "$CONFIG_ARCH_SHMOBILE" = "m" ) ) &&
      ( test "$CONFIG_SH_MIGOR" = "y" -o "$CONFIG_SH_MIGOR" = "m" ); then
      CONFIG_SND_SOC_SH4_SIU="m"
      CONFIG_SND_SOC_WM8978="m"
      CONFIG_SND_SIU_MIGOR="m"
    fi
  fi
  if alsa_check_kconfig_card "soc-tegra"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_ARCH_TEGRA" = "y" -o "$CONFIG_ARCH_TEGRA" = "m" ) &&
      ( test "$CONFIG_TEGRA_SYSTEM_DMA" = "y" -o "$CONFIG_TEGRA_SYSTEM_DMA" = "m" ) ); then
      CONFIG_SND_SOC_TEGRA="m"
    fi
  fi
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_SND_SOC_TEGRA" = "y" -o "$CONFIG_SND_SOC_TEGRA" = "m" ); then
      CONFIG_SND_SOC_TEGRA_SPDIF="m"
    fi
  if alsa_check_kconfig_card "soc-tegra-wm8903"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_SOC_TEGRA" = "y" -o "$CONFIG_SND_SOC_TEGRA" = "m" ) &&
      ( test "$CONFIG_I2C" = "y" -o "$CONFIG_I2C" = "m" ) ) &&
      ( test "$CONFIG_MACH_HAS_SND_SOC_TEGRA_WM8903" = "y" -o "$CONFIG_MACH_HAS_SND_SOC_TEGRA_WM8903" = "m" ); then
      CONFIG_SND_SOC_TEGRA_I2S="m"
      CONFIG_SND_SOC_WM8903="m"
      CONFIG_SND_SOC_TEGRA_WM8903="m"
    fi
  fi
  if alsa_check_kconfig_card "soc-tegra-trimslice"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_SOC_TEGRA" = "y" -o "$CONFIG_SND_SOC_TEGRA" = "m" ) &&
      ( test "$CONFIG_MACH_TRIMSLICE" = "y" -o "$CONFIG_MACH_TRIMSLICE" = "m" ) &&
      ( test "$CONFIG_I2C" = "y" -o "$CONFIG_I2C" = "m" ) ); then
      CONFIG_SND_SOC_TEGRA_I2S="m"
      CONFIG_SND_SOC_TLV320AIC23="m"
      CONFIG_SND_SOC_TEGRA_TRIMSLICE="m"
    fi
  fi
  if alsa_check_kconfig_card "soc-txx9aclc"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_HAS_TXX9_ACLC" = "y" -o "$CONFIG_HAS_TXX9_ACLC" = "m" ) &&
      ( test "$CONFIG_TXX9_DMAC" = "y" -o "$CONFIG_TXX9_DMAC" = "m" ) ); then
      CONFIG_SND_SOC_TXX9ACLC="m"
    fi
  fi
  if alsa_check_kconfig_card "soc-txx9aclc-generic"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_SND_SOC_TXX9ACLC" = "y" -o "$CONFIG_SND_SOC_TXX9ACLC" = "m" ); then
      test $kvernum -ge 206014 && CONFIG_AC97_BUS="m"
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_SOC_AC97_BUS="y"
      CONFIG_SND_SOC_TXX9ACLC_AC97="m"
      CONFIG_SND_SOC_AC97_CODEC="m"
      CONFIG_SND_SOC_TXX9ACLC_GENERIC="m"
    fi
  fi
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ); then
      CONFIG_SND_SOC_I2C_AND_SPI="m"
    fi
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ); then
      CONFIG_SND_SOC_WM_HUBS="m"
    fi
  if alsa_check_kconfig_card "soc-tlv320aic26"; then
    if  ! ( test "$CONFIG_UML" = "y" -o "$CONFIG_UML" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_SPI" = "y" -o "$CONFIG_SPI" = "m" ); then
      CONFIG_SND_SOC_TLV320AIC26="m"
    fi
  fi
])

AC_DEFUN([ALSA_TOPLEVEL_DEFINES], [
if test -n "$CONFIG_SOUND"; then
  AC_DEFINE(CONFIG_SOUND_MODULE)
fi
if test -n "$CONFIG_SOUND_OSS_CORE"; then
  AC_DEFINE(CONFIG_SOUND_OSS_CORE)
fi
if test -n "$CONFIG_SOUND_OSS_CORE_PRECLAIM"; then
  AC_DEFINE(CONFIG_SOUND_OSS_CORE_PRECLAIM)
fi
if test -n "$CONFIG_SND"; then
  AC_DEFINE(CONFIG_SND_MODULE)
fi
if test -n "$CONFIG_SND_TIMER"; then
  AC_DEFINE(CONFIG_SND_TIMER_MODULE)
fi
if test -n "$CONFIG_SND_PCM"; then
  AC_DEFINE(CONFIG_SND_PCM_MODULE)
fi
if test -n "$CONFIG_SND_HWDEP"; then
  AC_DEFINE(CONFIG_SND_HWDEP_MODULE)
fi
if test -n "$CONFIG_SND_RAWMIDI"; then
  AC_DEFINE(CONFIG_SND_RAWMIDI_MODULE)
fi
if test -n "$CONFIG_SND_COMPRESS_OFFLOAD"; then
  AC_DEFINE(CONFIG_SND_COMPRESS_OFFLOAD_MODULE)
fi
if test -n "$CONFIG_SND_JACK"; then
  AC_DEFINE(CONFIG_SND_JACK)
fi
if test -n "$CONFIG_SND_SEQUENCER"; then
  AC_DEFINE(CONFIG_SND_SEQUENCER_MODULE)
fi
if test -n "$CONFIG_SND_SEQ_DUMMY"; then
  AC_DEFINE(CONFIG_SND_SEQ_DUMMY_MODULE)
fi
if test -n "$CONFIG_SND_OSSEMUL"; then
  AC_DEFINE(CONFIG_SND_OSSEMUL)
fi
if test -n "$CONFIG_SND_MIXER_OSS"; then
  AC_DEFINE(CONFIG_SND_MIXER_OSS_MODULE)
fi
if test -n "$CONFIG_SND_PCM_OSS"; then
  AC_DEFINE(CONFIG_SND_PCM_OSS_MODULE)
fi
if test -n "$CONFIG_SND_PCM_OSS_PLUGINS"; then
  AC_DEFINE(CONFIG_SND_PCM_OSS_PLUGINS)
fi
if test -n "$CONFIG_SND_SEQUENCER_OSS"; then
  AC_DEFINE(CONFIG_SND_SEQUENCER_OSS)
fi
if test -n "$CONFIG_SND_HRTIMER"; then
  AC_DEFINE(CONFIG_SND_HRTIMER_MODULE)
fi
if test -n "$CONFIG_SND_SEQ_HRTIMER_DEFAULT"; then
  AC_DEFINE(CONFIG_SND_SEQ_HRTIMER_DEFAULT)
fi
if test -n "$CONFIG_SND_RTCTIMER"; then
  AC_DEFINE(CONFIG_SND_RTCTIMER_MODULE)
fi
if test -n "$CONFIG_SND_SEQ_RTCTIMER_DEFAULT"; then
  AC_DEFINE(CONFIG_SND_SEQ_RTCTIMER_DEFAULT)
fi
if test -n "$CONFIG_SND_DYNAMIC_MINORS"; then
  AC_DEFINE(CONFIG_SND_DYNAMIC_MINORS)
fi
if test -n "$CONFIG_SND_SUPPORT_OLD_API"; then
  AC_DEFINE(CONFIG_SND_SUPPORT_OLD_API)
fi
if test -n "$CONFIG_SND_VERBOSE_PROCFS"; then
  AC_DEFINE(CONFIG_SND_VERBOSE_PROCFS)
fi
if test -n "$CONFIG_SND_VERBOSE_PRINTK"; then
  AC_DEFINE(CONFIG_SND_VERBOSE_PRINTK)
fi
if test -n "$CONFIG_SND_DEBUG"; then
  AC_DEFINE(CONFIG_SND_DEBUG)
fi
if test -n "$CONFIG_SND_DEBUG_VERBOSE"; then
  AC_DEFINE(CONFIG_SND_DEBUG_VERBOSE)
fi
if test -n "$CONFIG_SND_PCM_XRUN_DEBUG"; then
  AC_DEFINE(CONFIG_SND_PCM_XRUN_DEBUG)
fi
if test -n "$CONFIG_SND_VMASTER"; then
  AC_DEFINE(CONFIG_SND_VMASTER)
fi
if test -n "$CONFIG_SND_KCTL_JACK"; then
  AC_DEFINE(CONFIG_SND_KCTL_JACK)
fi
if test -n "$CONFIG_SND_DMA_SGBUF"; then
  AC_DEFINE(CONFIG_SND_DMA_SGBUF)
fi
if test -n "$CONFIG_SND_RAWMIDI_SEQ"; then
  AC_DEFINE(CONFIG_SND_RAWMIDI_SEQ_MODULE)
fi
if test -n "$CONFIG_SND_OPL3_LIB_SEQ"; then
  AC_DEFINE(CONFIG_SND_OPL3_LIB_SEQ_MODULE)
fi
if test -n "$CONFIG_SND_OPL3_LIB"; then
  AC_DEFINE(CONFIG_SND_OPL3_LIB_MODULE)
fi
if test -n "$CONFIG_SND_OPL4_LIB_SEQ"; then
  AC_DEFINE(CONFIG_SND_OPL4_LIB_SEQ_MODULE)
fi
if test -n "$CONFIG_SND_OPL4_LIB"; then
  AC_DEFINE(CONFIG_SND_OPL4_LIB_MODULE)
fi
if test -n "$CONFIG_SND_SBAWE_SEQ"; then
  AC_DEFINE(CONFIG_SND_SBAWE_SEQ_MODULE)
fi
if test -n "$CONFIG_SND_SBAWE"; then
  AC_DEFINE(CONFIG_SND_SBAWE_MODULE)
fi
if test -n "$CONFIG_SND_EMU10K1_SEQ"; then
  AC_DEFINE(CONFIG_SND_EMU10K1_SEQ_MODULE)
fi
if test -n "$CONFIG_SND_EMU10K1"; then
  AC_DEFINE(CONFIG_SND_EMU10K1_MODULE)
fi
if test -n "$CONFIG_SND_BIT32_EMUL"; then
  AC_DEFINE(CONFIG_SND_BIT32_EMUL_MODULE)
fi
if test -n "$CONFIG_SND_DEBUG_MEMORY"; then
  AC_DEFINE(CONFIG_SND_DEBUG_MEMORY)
fi
if test -n "$CONFIG_SND_HPET"; then
  AC_DEFINE(CONFIG_SND_HPET_MODULE)
fi
if test -n "$CONFIG_SND_MPU401_UART"; then
  AC_DEFINE(CONFIG_SND_MPU401_UART_MODULE)
fi
if test -n "$CONFIG_SND_VX_LIB"; then
  AC_DEFINE(CONFIG_SND_VX_LIB_MODULE)
fi
if test -n "$CONFIG_SND_AC97_CODEC"; then
  AC_DEFINE(CONFIG_SND_AC97_CODEC_MODULE)
fi
if test -n "$CONFIG_SND_DRIVERS"; then
  AC_DEFINE(CONFIG_SND_DRIVERS)
fi
if test -n "$CONFIG_SND_PCSP"; then
  AC_DEFINE(CONFIG_SND_PCSP_MODULE)
fi
if test -n "$CONFIG_SND_DUMMY"; then
  AC_DEFINE(CONFIG_SND_DUMMY_MODULE)
fi
if test -n "$CONFIG_SND_ALOOP"; then
  AC_DEFINE(CONFIG_SND_ALOOP_MODULE)
fi
if test -n "$CONFIG_SND_VIRMIDI"; then
  AC_DEFINE(CONFIG_SND_VIRMIDI_MODULE)
fi
if test -n "$CONFIG_SND_MTPAV"; then
  AC_DEFINE(CONFIG_SND_MTPAV_MODULE)
fi
if test -n "$CONFIG_SND_MTS64"; then
  AC_DEFINE(CONFIG_SND_MTS64_MODULE)
fi
if test -n "$CONFIG_SND_SERIAL_U16550"; then
  AC_DEFINE(CONFIG_SND_SERIAL_U16550_MODULE)
fi
if test -n "$CONFIG_SND_MPU401"; then
  AC_DEFINE(CONFIG_SND_MPU401_MODULE)
fi
if test -n "$CONFIG_SND_PORTMAN2X4"; then
  AC_DEFINE(CONFIG_SND_PORTMAN2X4_MODULE)
fi
if test -n "$CONFIG_SND_ML403_AC97CR"; then
  AC_DEFINE(CONFIG_SND_ML403_AC97CR_MODULE)
fi
if test -n "$CONFIG_SND_AC97_POWER_SAVE"; then
  AC_DEFINE(CONFIG_SND_AC97_POWER_SAVE)
fi
if test -n "$CONFIG_SND_AC97_POWER_SAVE_DEFAULT"; then
  AC_DEFINE_UNQUOTED([CONFIG_SND_AC97_POWER_SAVE_DEFAULT], [$CONFIG_SND_AC97_POWER_SAVE_DEFAULT])
fi
if test -n "$CONFIG_SND_SERIALMIDI"; then
  AC_DEFINE(CONFIG_SND_SERIALMIDI_MODULE)
fi
if test -n "$CONFIG_SND_LOOPBACK"; then
  AC_DEFINE(CONFIG_SND_LOOPBACK_MODULE)
fi
if test -n "$CONFIG_SND_WSS_LIB"; then
  AC_DEFINE(CONFIG_SND_WSS_LIB_MODULE)
fi
if test -n "$CONFIG_SND_SB_COMMON"; then
  AC_DEFINE(CONFIG_SND_SB_COMMON_MODULE)
fi
if test -n "$CONFIG_SND_SB8_DSP"; then
  AC_DEFINE(CONFIG_SND_SB8_DSP_MODULE)
fi
if test -n "$CONFIG_SND_SB16_DSP"; then
  AC_DEFINE(CONFIG_SND_SB16_DSP_MODULE)
fi
if test -n "$CONFIG_SND_ISA"; then
  AC_DEFINE(CONFIG_SND_ISA)
fi
if test -n "$CONFIG_SND_ADLIB"; then
  AC_DEFINE(CONFIG_SND_ADLIB_MODULE)
fi
if test -n "$CONFIG_SND_AD1816A"; then
  AC_DEFINE(CONFIG_SND_AD1816A_MODULE)
fi
if test -n "$CONFIG_SND_AD1848"; then
  AC_DEFINE(CONFIG_SND_AD1848_MODULE)
fi
if test -n "$CONFIG_SND_ALS100"; then
  AC_DEFINE(CONFIG_SND_ALS100_MODULE)
fi
if test -n "$CONFIG_SND_AZT1605"; then
  AC_DEFINE(CONFIG_SND_AZT1605_MODULE)
fi
if test -n "$CONFIG_SND_AZT2316"; then
  AC_DEFINE(CONFIG_SND_AZT2316_MODULE)
fi
if test -n "$CONFIG_SND_AZT2320"; then
  AC_DEFINE(CONFIG_SND_AZT2320_MODULE)
fi
if test -n "$CONFIG_SND_CMI8330"; then
  AC_DEFINE(CONFIG_SND_CMI8330_MODULE)
fi
if test -n "$CONFIG_SND_CS4231"; then
  AC_DEFINE(CONFIG_SND_CS4231_MODULE)
fi
if test -n "$CONFIG_SND_CS4236"; then
  AC_DEFINE(CONFIG_SND_CS4236_MODULE)
fi
if test -n "$CONFIG_SND_ES1688"; then
  AC_DEFINE(CONFIG_SND_ES1688_MODULE)
fi
if test -n "$CONFIG_SND_ES18XX"; then
  AC_DEFINE(CONFIG_SND_ES18XX_MODULE)
fi
if test -n "$CONFIG_SND_SC6000"; then
  AC_DEFINE(CONFIG_SND_SC6000_MODULE)
fi
if test -n "$CONFIG_SND_GUSCLASSIC"; then
  AC_DEFINE(CONFIG_SND_GUSCLASSIC_MODULE)
fi
if test -n "$CONFIG_SND_GUSEXTREME"; then
  AC_DEFINE(CONFIG_SND_GUSEXTREME_MODULE)
fi
if test -n "$CONFIG_SND_GUSMAX"; then
  AC_DEFINE(CONFIG_SND_GUSMAX_MODULE)
fi
if test -n "$CONFIG_SND_INTERWAVE"; then
  AC_DEFINE(CONFIG_SND_INTERWAVE_MODULE)
fi
if test -n "$CONFIG_SND_INTERWAVE_STB"; then
  AC_DEFINE(CONFIG_SND_INTERWAVE_STB_MODULE)
fi
if test -n "$CONFIG_SND_JAZZ16"; then
  AC_DEFINE(CONFIG_SND_JAZZ16_MODULE)
fi
if test -n "$CONFIG_SND_OPL3SA2"; then
  AC_DEFINE(CONFIG_SND_OPL3SA2_MODULE)
fi
if test -n "$CONFIG_SND_OPTI92X_AD1848"; then
  AC_DEFINE(CONFIG_SND_OPTI92X_AD1848_MODULE)
fi
if test -n "$CONFIG_SND_OPTI92X_CS4231"; then
  AC_DEFINE(CONFIG_SND_OPTI92X_CS4231_MODULE)
fi
if test -n "$CONFIG_SND_OPTI93X"; then
  AC_DEFINE(CONFIG_SND_OPTI93X_MODULE)
fi
if test -n "$CONFIG_SND_MIRO"; then
  AC_DEFINE(CONFIG_SND_MIRO_MODULE)
fi
if test -n "$CONFIG_SND_SB8"; then
  AC_DEFINE(CONFIG_SND_SB8_MODULE)
fi
if test -n "$CONFIG_SND_SB16"; then
  AC_DEFINE(CONFIG_SND_SB16_MODULE)
fi
if test -n "$CONFIG_SND_SB16_CSP"; then
  AC_DEFINE(CONFIG_SND_SB16_CSP)
fi
if test -n "$CONFIG_SND_SSCAPE"; then
  AC_DEFINE(CONFIG_SND_SSCAPE_MODULE)
fi
if test -n "$CONFIG_SND_WAVEFRONT"; then
  AC_DEFINE(CONFIG_SND_WAVEFRONT_MODULE)
fi
if test -n "$CONFIG_SND_MSND_PINNACLE"; then
  AC_DEFINE(CONFIG_SND_MSND_PINNACLE_MODULE)
fi
if test -n "$CONFIG_SND_MSND_CLASSIC"; then
  AC_DEFINE(CONFIG_SND_MSND_CLASSIC_MODULE)
fi
if test -n "$CONFIG_SND_PC98_CS4232"; then
  AC_DEFINE(CONFIG_SND_PC98_CS4232_MODULE)
fi
if test -n "$CONFIG_SND_TEA575X"; then
  AC_DEFINE(CONFIG_SND_TEA575X_MODULE)
fi
if test -n "$CONFIG_SND_FM801_TEA575X_BOOL"; then
  AC_DEFINE(CONFIG_SND_FM801_TEA575X_BOOL)
fi
if test -n "$CONFIG_SND_ES1968_RADIO"; then
  AC_DEFINE(CONFIG_SND_ES1968_RADIO)
fi
if test -n "$CONFIG_SND_FM801"; then
  AC_DEFINE(CONFIG_SND_FM801_MODULE)
fi
if test -n "$CONFIG_SND_ES1968"; then
  AC_DEFINE(CONFIG_SND_ES1968_MODULE)
fi
if test -n "$CONFIG_SND_PCI"; then
  AC_DEFINE(CONFIG_SND_PCI)
fi
if test -n "$CONFIG_SND_AD1889"; then
  AC_DEFINE(CONFIG_SND_AD1889_MODULE)
fi
if test -n "$CONFIG_SND_ALS300"; then
  AC_DEFINE(CONFIG_SND_ALS300_MODULE)
fi
if test -n "$CONFIG_SND_ALS4000"; then
  AC_DEFINE(CONFIG_SND_ALS4000_MODULE)
fi
if test -n "$CONFIG_SND_ALI5451"; then
  AC_DEFINE(CONFIG_SND_ALI5451_MODULE)
fi
if test -n "$CONFIG_SND_ASIHPI"; then
  AC_DEFINE(CONFIG_SND_ASIHPI_MODULE)
fi
if test -n "$CONFIG_SND_ATIIXP"; then
  AC_DEFINE(CONFIG_SND_ATIIXP_MODULE)
fi
if test -n "$CONFIG_SND_ATIIXP_MODEM"; then
  AC_DEFINE(CONFIG_SND_ATIIXP_MODEM_MODULE)
fi
if test -n "$CONFIG_SND_AU8810"; then
  AC_DEFINE(CONFIG_SND_AU8810_MODULE)
fi
if test -n "$CONFIG_SND_AU8820"; then
  AC_DEFINE(CONFIG_SND_AU8820_MODULE)
fi
if test -n "$CONFIG_SND_AU8830"; then
  AC_DEFINE(CONFIG_SND_AU8830_MODULE)
fi
if test -n "$CONFIG_SND_AW2"; then
  AC_DEFINE(CONFIG_SND_AW2_MODULE)
fi
if test -n "$CONFIG_SND_AZT3328"; then
  AC_DEFINE(CONFIG_SND_AZT3328_MODULE)
fi
if test -n "$CONFIG_SND_BT87X"; then
  AC_DEFINE(CONFIG_SND_BT87X_MODULE)
fi
if test -n "$CONFIG_SND_BT87X_OVERCLOCK"; then
  AC_DEFINE(CONFIG_SND_BT87X_OVERCLOCK)
fi
if test -n "$CONFIG_SND_CA0106"; then
  AC_DEFINE(CONFIG_SND_CA0106_MODULE)
fi
if test -n "$CONFIG_SND_CMIPCI"; then
  AC_DEFINE(CONFIG_SND_CMIPCI_MODULE)
fi
if test -n "$CONFIG_SND_OXYGEN_LIB"; then
  AC_DEFINE(CONFIG_SND_OXYGEN_LIB_MODULE)
fi
if test -n "$CONFIG_SND_OXYGEN"; then
  AC_DEFINE(CONFIG_SND_OXYGEN_MODULE)
fi
if test -n "$CONFIG_SND_CS4281"; then
  AC_DEFINE(CONFIG_SND_CS4281_MODULE)
fi
if test -n "$CONFIG_SND_CS46XX"; then
  AC_DEFINE(CONFIG_SND_CS46XX_MODULE)
fi
if test -n "$CONFIG_SND_CS46XX_NEW_DSP"; then
  AC_DEFINE(CONFIG_SND_CS46XX_NEW_DSP)
fi
if test -n "$CONFIG_SND_CS5530"; then
  AC_DEFINE(CONFIG_SND_CS5530_MODULE)
fi
if test -n "$CONFIG_SND_CS5535AUDIO"; then
  AC_DEFINE(CONFIG_SND_CS5535AUDIO_MODULE)
fi
if test -n "$CONFIG_SND_CTXFI"; then
  AC_DEFINE(CONFIG_SND_CTXFI_MODULE)
fi
if test -n "$CONFIG_SND_DARLA20"; then
  AC_DEFINE(CONFIG_SND_DARLA20_MODULE)
fi
if test -n "$CONFIG_SND_GINA20"; then
  AC_DEFINE(CONFIG_SND_GINA20_MODULE)
fi
if test -n "$CONFIG_SND_LAYLA20"; then
  AC_DEFINE(CONFIG_SND_LAYLA20_MODULE)
fi
if test -n "$CONFIG_SND_DARLA24"; then
  AC_DEFINE(CONFIG_SND_DARLA24_MODULE)
fi
if test -n "$CONFIG_SND_GINA24"; then
  AC_DEFINE(CONFIG_SND_GINA24_MODULE)
fi
if test -n "$CONFIG_SND_LAYLA24"; then
  AC_DEFINE(CONFIG_SND_LAYLA24_MODULE)
fi
if test -n "$CONFIG_SND_MONA"; then
  AC_DEFINE(CONFIG_SND_MONA_MODULE)
fi
if test -n "$CONFIG_SND_MIA"; then
  AC_DEFINE(CONFIG_SND_MIA_MODULE)
fi
if test -n "$CONFIG_SND_ECHO3G"; then
  AC_DEFINE(CONFIG_SND_ECHO3G_MODULE)
fi
if test -n "$CONFIG_SND_INDIGO"; then
  AC_DEFINE(CONFIG_SND_INDIGO_MODULE)
fi
if test -n "$CONFIG_SND_INDIGOIO"; then
  AC_DEFINE(CONFIG_SND_INDIGOIO_MODULE)
fi
if test -n "$CONFIG_SND_INDIGODJ"; then
  AC_DEFINE(CONFIG_SND_INDIGODJ_MODULE)
fi
if test -n "$CONFIG_SND_INDIGOIOX"; then
  AC_DEFINE(CONFIG_SND_INDIGOIOX_MODULE)
fi
if test -n "$CONFIG_SND_INDIGODJX"; then
  AC_DEFINE(CONFIG_SND_INDIGODJX_MODULE)
fi
if test -n "$CONFIG_SND_EMU10K1X"; then
  AC_DEFINE(CONFIG_SND_EMU10K1X_MODULE)
fi
if test -n "$CONFIG_SND_ENS1370"; then
  AC_DEFINE(CONFIG_SND_ENS1370_MODULE)
fi
if test -n "$CONFIG_SND_ENS1371"; then
  AC_DEFINE(CONFIG_SND_ENS1371_MODULE)
fi
if test -n "$CONFIG_SND_ES1938"; then
  AC_DEFINE(CONFIG_SND_ES1938_MODULE)
fi
if test -n "$CONFIG_SND_ES1968_INPUT"; then
  AC_DEFINE(CONFIG_SND_ES1968_INPUT)
fi
if test -n "$CONFIG_SND_HDA_INTEL"; then
  AC_DEFINE(CONFIG_SND_HDA_INTEL_MODULE)
fi
if test -n "$CONFIG_SND_HDA_PREALLOC_SIZE"; then
  AC_DEFINE_UNQUOTED([CONFIG_SND_HDA_PREALLOC_SIZE], [$CONFIG_SND_HDA_PREALLOC_SIZE])
fi
if test -n "$CONFIG_SND_HDA_HWDEP"; then
  AC_DEFINE(CONFIG_SND_HDA_HWDEP)
fi
if test -n "$CONFIG_SND_HDA_RECONFIG"; then
  AC_DEFINE(CONFIG_SND_HDA_RECONFIG)
fi
if test -n "$CONFIG_SND_HDA_INPUT_BEEP"; then
  AC_DEFINE(CONFIG_SND_HDA_INPUT_BEEP)
fi
if test -n "$CONFIG_SND_HDA_INPUT_BEEP_MODE"; then
  AC_DEFINE_UNQUOTED([CONFIG_SND_HDA_INPUT_BEEP_MODE], [$CONFIG_SND_HDA_INPUT_BEEP_MODE])
fi
if test -n "$CONFIG_SND_HDA_INPUT_JACK"; then
  AC_DEFINE(CONFIG_SND_HDA_INPUT_JACK)
fi
if test -n "$CONFIG_SND_HDA_PATCH_LOADER"; then
  AC_DEFINE(CONFIG_SND_HDA_PATCH_LOADER)
fi
if test -n "$CONFIG_SND_HDA_CODEC_REALTEK"; then
  AC_DEFINE(CONFIG_SND_HDA_CODEC_REALTEK)
fi
if test -n "$CONFIG_SND_HDA_ENABLE_REALTEK_QUIRKS"; then
  AC_DEFINE(CONFIG_SND_HDA_ENABLE_REALTEK_QUIRKS)
fi
if test -n "$CONFIG_SND_HDA_CODEC_ANALOG"; then
  AC_DEFINE(CONFIG_SND_HDA_CODEC_ANALOG)
fi
if test -n "$CONFIG_SND_HDA_CODEC_SIGMATEL"; then
  AC_DEFINE(CONFIG_SND_HDA_CODEC_SIGMATEL)
fi
if test -n "$CONFIG_SND_HDA_CODEC_VIA"; then
  AC_DEFINE(CONFIG_SND_HDA_CODEC_VIA)
fi
if test -n "$CONFIG_SND_HDA_CODEC_HDMI"; then
  AC_DEFINE(CONFIG_SND_HDA_CODEC_HDMI)
fi
if test -n "$CONFIG_SND_HDA_CODEC_CIRRUS"; then
  AC_DEFINE(CONFIG_SND_HDA_CODEC_CIRRUS)
fi
if test -n "$CONFIG_SND_HDA_CODEC_CONEXANT"; then
  AC_DEFINE(CONFIG_SND_HDA_CODEC_CONEXANT)
fi
if test -n "$CONFIG_SND_HDA_CODEC_CA0110"; then
  AC_DEFINE(CONFIG_SND_HDA_CODEC_CA0110)
fi
if test -n "$CONFIG_SND_HDA_CODEC_CA0132"; then
  AC_DEFINE(CONFIG_SND_HDA_CODEC_CA0132)
fi
if test -n "$CONFIG_SND_HDA_CODEC_CMEDIA"; then
  AC_DEFINE(CONFIG_SND_HDA_CODEC_CMEDIA)
fi
if test -n "$CONFIG_SND_HDA_CODEC_SI3054"; then
  AC_DEFINE(CONFIG_SND_HDA_CODEC_SI3054)
fi
if test -n "$CONFIG_SND_HDA_GENERIC"; then
  AC_DEFINE(CONFIG_SND_HDA_GENERIC)
fi
if test -n "$CONFIG_SND_HDA_POWER_SAVE"; then
  AC_DEFINE(CONFIG_SND_HDA_POWER_SAVE)
fi
if test -n "$CONFIG_SND_HDA_POWER_SAVE_DEFAULT"; then
  AC_DEFINE_UNQUOTED([CONFIG_SND_HDA_POWER_SAVE_DEFAULT], [$CONFIG_SND_HDA_POWER_SAVE_DEFAULT])
fi
if test -n "$CONFIG_SND_HDSP"; then
  AC_DEFINE(CONFIG_SND_HDSP_MODULE)
fi
if test -n "$CONFIG_SND_HDSPM"; then
  AC_DEFINE(CONFIG_SND_HDSPM_MODULE)
fi
if test -n "$CONFIG_SND_ICE1712"; then
  AC_DEFINE(CONFIG_SND_ICE1712_MODULE)
fi
if test -n "$CONFIG_SND_ICE1724"; then
  AC_DEFINE(CONFIG_SND_ICE1724_MODULE)
fi
if test -n "$CONFIG_SND_INTEL8X0"; then
  AC_DEFINE(CONFIG_SND_INTEL8X0_MODULE)
fi
if test -n "$CONFIG_SND_INTEL8X0M"; then
  AC_DEFINE(CONFIG_SND_INTEL8X0M_MODULE)
fi
if test -n "$CONFIG_SND_KORG1212"; then
  AC_DEFINE(CONFIG_SND_KORG1212_MODULE)
fi
if test -n "$CONFIG_SND_LOLA"; then
  AC_DEFINE(CONFIG_SND_LOLA_MODULE)
fi
if test -n "$CONFIG_SND_LX6464ES"; then
  AC_DEFINE(CONFIG_SND_LX6464ES_MODULE)
fi
if test -n "$CONFIG_SND_MAESTRO3"; then
  AC_DEFINE(CONFIG_SND_MAESTRO3_MODULE)
fi
if test -n "$CONFIG_SND_MAESTRO3_INPUT"; then
  AC_DEFINE(CONFIG_SND_MAESTRO3_INPUT)
fi
if test -n "$CONFIG_SND_MIXART"; then
  AC_DEFINE(CONFIG_SND_MIXART_MODULE)
fi
if test -n "$CONFIG_SND_NM256"; then
  AC_DEFINE(CONFIG_SND_NM256_MODULE)
fi
if test -n "$CONFIG_SND_PCXHR"; then
  AC_DEFINE(CONFIG_SND_PCXHR_MODULE)
fi
if test -n "$CONFIG_SND_RIPTIDE"; then
  AC_DEFINE(CONFIG_SND_RIPTIDE_MODULE)
fi
if test -n "$CONFIG_SND_RME32"; then
  AC_DEFINE(CONFIG_SND_RME32_MODULE)
fi
if test -n "$CONFIG_SND_RME96"; then
  AC_DEFINE(CONFIG_SND_RME96_MODULE)
fi
if test -n "$CONFIG_SND_RME9652"; then
  AC_DEFINE(CONFIG_SND_RME9652_MODULE)
fi
if test -n "$CONFIG_SND_SIS7019"; then
  AC_DEFINE(CONFIG_SND_SIS7019_MODULE)
fi
if test -n "$CONFIG_SND_SONICVIBES"; then
  AC_DEFINE(CONFIG_SND_SONICVIBES_MODULE)
fi
if test -n "$CONFIG_SND_TRIDENT"; then
  AC_DEFINE(CONFIG_SND_TRIDENT_MODULE)
fi
if test -n "$CONFIG_SND_VIA82XX"; then
  AC_DEFINE(CONFIG_SND_VIA82XX_MODULE)
fi
if test -n "$CONFIG_SND_VIA82XX_MODEM"; then
  AC_DEFINE(CONFIG_SND_VIA82XX_MODEM_MODULE)
fi
if test -n "$CONFIG_SND_VIRTUOSO"; then
  AC_DEFINE(CONFIG_SND_VIRTUOSO_MODULE)
fi
if test -n "$CONFIG_SND_VX222"; then
  AC_DEFINE(CONFIG_SND_VX222_MODULE)
fi
if test -n "$CONFIG_SND_YMFPCI"; then
  AC_DEFINE(CONFIG_SND_YMFPCI_MODULE)
fi
if test -n "$CONFIG_SND_PDPLUS"; then
  AC_DEFINE(CONFIG_SND_PDPLUS_MODULE)
fi
if test -n "$CONFIG_SND_PPC"; then
  AC_DEFINE(CONFIG_SND_PPC)
fi
if test -n "$CONFIG_SND_POWERMAC"; then
  AC_DEFINE(CONFIG_SND_POWERMAC_MODULE)
fi
if test -n "$CONFIG_SND_POWERMAC_AUTO_DRC"; then
  AC_DEFINE(CONFIG_SND_POWERMAC_AUTO_DRC)
fi
if test -n "$CONFIG_SND_PS3"; then
  AC_DEFINE(CONFIG_SND_PS3_MODULE)
fi
if test -n "$CONFIG_SND_PS3_DEFAULT_START_DELAY"; then
  AC_DEFINE_UNQUOTED([CONFIG_SND_PS3_DEFAULT_START_DELAY], [$CONFIG_SND_PS3_DEFAULT_START_DELAY])
fi
if test -n "$CONFIG_SND_AOA"; then
  AC_DEFINE(CONFIG_SND_AOA_MODULE)
fi
if test -n "$CONFIG_SND_AOA_FABRIC_LAYOUT"; then
  AC_DEFINE(CONFIG_SND_AOA_FABRIC_LAYOUT_MODULE)
fi
if test -n "$CONFIG_SND_AOA_ONYX"; then
  AC_DEFINE(CONFIG_SND_AOA_ONYX_MODULE)
fi
if test -n "$CONFIG_SND_AOA_TAS"; then
  AC_DEFINE(CONFIG_SND_AOA_TAS_MODULE)
fi
if test -n "$CONFIG_SND_AOA_TOONIE"; then
  AC_DEFINE(CONFIG_SND_AOA_TOONIE_MODULE)
fi
if test -n "$CONFIG_SND_AOA_SOUNDBUS"; then
  AC_DEFINE(CONFIG_SND_AOA_SOUNDBUS_MODULE)
fi
if test -n "$CONFIG_SND_AOA_SOUNDBUS_I2S"; then
  AC_DEFINE(CONFIG_SND_AOA_SOUNDBUS_I2S_MODULE)
fi
if test -n "$CONFIG_SND_ARM"; then
  AC_DEFINE(CONFIG_SND_ARM)
fi
if test -n "$CONFIG_SND_ARMAACI"; then
  AC_DEFINE(CONFIG_SND_ARMAACI_MODULE)
fi
if test -n "$CONFIG_SND_PXA2XX_PCM"; then
  AC_DEFINE(CONFIG_SND_PXA2XX_PCM_MODULE)
fi
if test -n "$CONFIG_SND_PXA2XX_LIB"; then
  AC_DEFINE(CONFIG_SND_PXA2XX_LIB_MODULE)
fi
if test -n "$CONFIG_SND_PXA2XX_LIB_AC97"; then
  AC_DEFINE(CONFIG_SND_PXA2XX_LIB_AC97)
fi
if test -n "$CONFIG_SND_PXA2XX_AC97"; then
  AC_DEFINE(CONFIG_SND_PXA2XX_AC97_MODULE)
fi
if test -n "$CONFIG_SND_SA11XX_UDA1341"; then
  AC_DEFINE(CONFIG_SND_SA11XX_UDA1341_MODULE)
fi
if test -n "$CONFIG_SND_S3C2410"; then
  AC_DEFINE(CONFIG_SND_S3C2410_MODULE)
fi
if test -n "$CONFIG_SND_PXA2XX_I2SOUND"; then
  AC_DEFINE(CONFIG_SND_PXA2XX_I2SOUND_MODULE)
fi
if test -n "$CONFIG_SND_ATMEL_ABDAC"; then
  AC_DEFINE(CONFIG_SND_ATMEL_ABDAC_MODULE)
fi
if test -n "$CONFIG_SND_ATMEL_AC97C"; then
  AC_DEFINE(CONFIG_SND_ATMEL_AC97C_MODULE)
fi
if test -n "$CONFIG_SND_SPI"; then
  AC_DEFINE(CONFIG_SND_SPI)
fi
if test -n "$CONFIG_SND_AT73C213"; then
  AC_DEFINE(CONFIG_SND_AT73C213_MODULE)
fi
if test -n "$CONFIG_SND_AT73C213_TARGET_BITRATE"; then
  AC_DEFINE_UNQUOTED([CONFIG_SND_AT73C213_TARGET_BITRATE], [$CONFIG_SND_AT73C213_TARGET_BITRATE])
fi
if test -n "$CONFIG_SND_MIPS"; then
  AC_DEFINE(CONFIG_SND_MIPS)
fi
if test -n "$CONFIG_SND_SGI_O2"; then
  AC_DEFINE(CONFIG_SND_SGI_O2_MODULE)
fi
if test -n "$CONFIG_SND_SGI_HAL2"; then
  AC_DEFINE(CONFIG_SND_SGI_HAL2_MODULE)
fi
if test -n "$CONFIG_SND_AU1X00"; then
  AC_DEFINE(CONFIG_SND_AU1X00_MODULE)
fi
if test -n "$CONFIG_SND_SUPERH"; then
  AC_DEFINE(CONFIG_SND_SUPERH)
fi
if test -n "$CONFIG_SND_AICA"; then
  AC_DEFINE(CONFIG_SND_AICA_MODULE)
fi
if test -n "$CONFIG_SND_SH_DAC_AUDIO"; then
  AC_DEFINE(CONFIG_SND_SH_DAC_AUDIO_MODULE)
fi
if test -n "$CONFIG_SND_USB"; then
  AC_DEFINE(CONFIG_SND_USB)
fi
if test -n "$CONFIG_SND_USB_AUDIO"; then
  AC_DEFINE(CONFIG_SND_USB_AUDIO_MODULE)
fi
if test -n "$CONFIG_SND_USB_UA101"; then
  AC_DEFINE(CONFIG_SND_USB_UA101_MODULE)
fi
if test -n "$CONFIG_SND_USB_USX2Y"; then
  AC_DEFINE(CONFIG_SND_USB_USX2Y_MODULE)
fi
if test -n "$CONFIG_SND_USB_CAIAQ"; then
  AC_DEFINE(CONFIG_SND_USB_CAIAQ_MODULE)
fi
if test -n "$CONFIG_SND_USB_CAIAQ_INPUT"; then
  AC_DEFINE(CONFIG_SND_USB_CAIAQ_INPUT)
fi
if test -n "$CONFIG_SND_USB_US122L"; then
  AC_DEFINE(CONFIG_SND_USB_US122L_MODULE)
fi
if test -n "$CONFIG_SND_USB_6FIRE"; then
  AC_DEFINE(CONFIG_SND_USB_6FIRE_MODULE)
fi
if test -n "$CONFIG_SND_FIREWIRE"; then
  AC_DEFINE(CONFIG_SND_FIREWIRE)
fi
if test -n "$CONFIG_SND_FIREWIRE_LIB"; then
  AC_DEFINE(CONFIG_SND_FIREWIRE_LIB_MODULE)
fi
if test -n "$CONFIG_SND_FIREWIRE_SPEAKERS"; then
  AC_DEFINE(CONFIG_SND_FIREWIRE_SPEAKERS_MODULE)
fi
if test -n "$CONFIG_SND_ISIGHT"; then
  AC_DEFINE(CONFIG_SND_ISIGHT_MODULE)
fi
if test -n "$CONFIG_SND_PCMCIA"; then
  AC_DEFINE(CONFIG_SND_PCMCIA)
fi
if test -n "$CONFIG_SND_VXPOCKET"; then
  AC_DEFINE(CONFIG_SND_VXPOCKET_MODULE)
fi
if test -n "$CONFIG_SND_PDAUDIOCF"; then
  AC_DEFINE(CONFIG_SND_PDAUDIOCF_MODULE)
fi
if test -n "$CONFIG_SND_SPARC"; then
  AC_DEFINE(CONFIG_SND_SPARC)
fi
if test -n "$CONFIG_SND_SUN_AMD7930"; then
  AC_DEFINE(CONFIG_SND_SUN_AMD7930_MODULE)
fi
if test -n "$CONFIG_SND_SUN_CS4231"; then
  AC_DEFINE(CONFIG_SND_SUN_CS4231_MODULE)
fi
if test -n "$CONFIG_SND_SUN_DBRI"; then
  AC_DEFINE(CONFIG_SND_SUN_DBRI_MODULE)
fi
if test -n "$CONFIG_SND_GSC"; then
  AC_DEFINE(CONFIG_SND_GSC)
fi
if test -n "$CONFIG_SND_HARMONY"; then
  AC_DEFINE(CONFIG_SND_HARMONY_MODULE)
fi
if test -n "$CONFIG_SND_SOC"; then
  AC_DEFINE(CONFIG_SND_SOC_MODULE)
fi
if test -n "$CONFIG_SND_SOC_AC97_BUS"; then
  AC_DEFINE(CONFIG_SND_SOC_AC97_BUS)
fi
if test -n "$CONFIG_SND_ATMEL_SOC"; then
  AC_DEFINE(CONFIG_SND_ATMEL_SOC_MODULE)
fi
if test -n "$CONFIG_SND_ATMEL_SOC_SSC"; then
  AC_DEFINE(CONFIG_SND_ATMEL_SOC_SSC_MODULE)
fi
if test -n "$CONFIG_SND_AT91_SOC_SAM9G20_WM8731"; then
  AC_DEFINE(CONFIG_SND_AT91_SOC_SAM9G20_WM8731_MODULE)
fi
if test -n "$CONFIG_SND_AT91_SOC_AFEB9260"; then
  AC_DEFINE(CONFIG_SND_AT91_SOC_AFEB9260_MODULE)
fi
if test -n "$CONFIG_SND_SOC_AU1XPSC"; then
  AC_DEFINE(CONFIG_SND_SOC_AU1XPSC_MODULE)
fi
if test -n "$CONFIG_SND_SOC_AU1XPSC_I2S"; then
  AC_DEFINE(CONFIG_SND_SOC_AU1XPSC_I2S_MODULE)
fi
if test -n "$CONFIG_SND_SOC_AU1XPSC_AC97"; then
  AC_DEFINE(CONFIG_SND_SOC_AU1XPSC_AC97_MODULE)
fi
if test -n "$CONFIG_SND_SOC_AU1XAUDIO"; then
  AC_DEFINE(CONFIG_SND_SOC_AU1XAUDIO_MODULE)
fi
if test -n "$CONFIG_SND_SOC_AU1XAC97C"; then
  AC_DEFINE(CONFIG_SND_SOC_AU1XAC97C_MODULE)
fi
if test -n "$CONFIG_SND_SOC_AU1XI2SC"; then
  AC_DEFINE(CONFIG_SND_SOC_AU1XI2SC_MODULE)
fi
if test -n "$CONFIG_SND_SOC_DB1000"; then
  AC_DEFINE(CONFIG_SND_SOC_DB1000_MODULE)
fi
if test -n "$CONFIG_SND_SOC_DB1200"; then
  AC_DEFINE(CONFIG_SND_SOC_DB1200_MODULE)
fi
if test -n "$CONFIG_SND_BF5XX_I2S"; then
  AC_DEFINE(CONFIG_SND_BF5XX_I2S_MODULE)
fi
if test -n "$CONFIG_SND_BF5XX_SOC_SSM2602"; then
  AC_DEFINE(CONFIG_SND_BF5XX_SOC_SSM2602_MODULE)
fi
if test -n "$CONFIG_SND_SOC_BFIN_EVAL_ADAU1701"; then
  AC_DEFINE(CONFIG_SND_SOC_BFIN_EVAL_ADAU1701_MODULE)
fi
if test -n "$CONFIG_SND_SOC_BFIN_EVAL_ADAU1373"; then
  AC_DEFINE(CONFIG_SND_SOC_BFIN_EVAL_ADAU1373_MODULE)
fi
if test -n "$CONFIG_SND_SOC_BFIN_EVAL_ADAV80X"; then
  AC_DEFINE(CONFIG_SND_SOC_BFIN_EVAL_ADAV80X_MODULE)
fi
if test -n "$CONFIG_SND_BF5XX_SOC_AD73311"; then
  AC_DEFINE(CONFIG_SND_BF5XX_SOC_AD73311_MODULE)
fi
if test -n "$CONFIG_SND_BFIN_AD73311_SE"; then
  AC_DEFINE_UNQUOTED([CONFIG_SND_BFIN_AD73311_SE], [$CONFIG_SND_BFIN_AD73311_SE])
fi
if test -n "$CONFIG_SND_BF5XX_TDM"; then
  AC_DEFINE(CONFIG_SND_BF5XX_TDM_MODULE)
fi
if test -n "$CONFIG_SND_BF5XX_SOC_AD1836"; then
  AC_DEFINE(CONFIG_SND_BF5XX_SOC_AD1836_MODULE)
fi
if test -n "$CONFIG_SND_BF5XX_SOC_AD193X"; then
  AC_DEFINE(CONFIG_SND_BF5XX_SOC_AD193X_MODULE)
fi
if test -n "$CONFIG_SND_BF5XX_AC97"; then
  AC_DEFINE(CONFIG_SND_BF5XX_AC97_MODULE)
fi
if test -n "$CONFIG_SND_BF5XX_MMAP_SUPPORT"; then
  AC_DEFINE(CONFIG_SND_BF5XX_MMAP_SUPPORT)
fi
if test -n "$CONFIG_SND_BF5XX_MULTICHAN_SUPPORT"; then
  AC_DEFINE(CONFIG_SND_BF5XX_MULTICHAN_SUPPORT)
fi
if test -n "$CONFIG_SND_BF5XX_HAVE_COLD_RESET"; then
  AC_DEFINE(CONFIG_SND_BF5XX_HAVE_COLD_RESET)
fi
if test -n "$CONFIG_SND_BF5XX_RESET_GPIO_NUM"; then
  AC_DEFINE_UNQUOTED([CONFIG_SND_BF5XX_RESET_GPIO_NUM], [$CONFIG_SND_BF5XX_RESET_GPIO_NUM])
fi
if test -n "$CONFIG_SND_BF5XX_SOC_AD1980"; then
  AC_DEFINE(CONFIG_SND_BF5XX_SOC_AD1980_MODULE)
fi
if test -n "$CONFIG_SND_BF5XX_SOC_SPORT"; then
  AC_DEFINE(CONFIG_SND_BF5XX_SOC_SPORT_MODULE)
fi
if test -n "$CONFIG_SND_BF5XX_SOC_I2S"; then
  AC_DEFINE(CONFIG_SND_BF5XX_SOC_I2S_MODULE)
fi
if test -n "$CONFIG_SND_BF5XX_SOC_TDM"; then
  AC_DEFINE(CONFIG_SND_BF5XX_SOC_TDM_MODULE)
fi
if test -n "$CONFIG_SND_BF5XX_SOC_AC97"; then
  AC_DEFINE(CONFIG_SND_BF5XX_SOC_AC97_MODULE)
fi
if test -n "$CONFIG_SND_BF5XX_SPORT_NUM"; then
  AC_DEFINE_UNQUOTED([CONFIG_SND_BF5XX_SPORT_NUM], [$CONFIG_SND_BF5XX_SPORT_NUM])
fi
if test -n "$CONFIG_SND_DAVINCI_SOC"; then
  AC_DEFINE(CONFIG_SND_DAVINCI_SOC_MODULE)
fi
if test -n "$CONFIG_SND_DAVINCI_SOC_I2S"; then
  AC_DEFINE(CONFIG_SND_DAVINCI_SOC_I2S_MODULE)
fi
if test -n "$CONFIG_SND_DAVINCI_SOC_MCASP"; then
  AC_DEFINE(CONFIG_SND_DAVINCI_SOC_MCASP_MODULE)
fi
if test -n "$CONFIG_SND_DAVINCI_SOC_VCIF"; then
  AC_DEFINE(CONFIG_SND_DAVINCI_SOC_VCIF_MODULE)
fi
if test -n "$CONFIG_SND_DAVINCI_SOC_EVM"; then
  AC_DEFINE(CONFIG_SND_DAVINCI_SOC_EVM_MODULE)
fi
if test -n "$CONFIG_SND_DM365_AIC3X_CODEC"; then
  AC_DEFINE(CONFIG_SND_DM365_AIC3X_CODEC)
fi
if test -n "$CONFIG_SND_DM365_VOICE_CODEC"; then
  AC_DEFINE(CONFIG_SND_DM365_VOICE_CODEC)
fi
if test -n "$CONFIG_SND_DM6467_SOC_EVM"; then
  AC_DEFINE(CONFIG_SND_DM6467_SOC_EVM_MODULE)
fi
if test -n "$CONFIG_SND_DAVINCI_SOC_SFFSDR"; then
  AC_DEFINE(CONFIG_SND_DAVINCI_SOC_SFFSDR_MODULE)
fi
if test -n "$CONFIG_SND_DA830_SOC_EVM"; then
  AC_DEFINE(CONFIG_SND_DA830_SOC_EVM_MODULE)
fi
if test -n "$CONFIG_SND_DA850_SOC_EVM"; then
  AC_DEFINE(CONFIG_SND_DA850_SOC_EVM_MODULE)
fi
if test -n "$CONFIG_SND_EP93XX_SOC"; then
  AC_DEFINE(CONFIG_SND_EP93XX_SOC_MODULE)
fi
if test -n "$CONFIG_SND_EP93XX_SOC_I2S"; then
  AC_DEFINE(CONFIG_SND_EP93XX_SOC_I2S_MODULE)
fi
if test -n "$CONFIG_SND_EP93XX_SOC_AC97"; then
  AC_DEFINE(CONFIG_SND_EP93XX_SOC_AC97_MODULE)
fi
if test -n "$CONFIG_SND_EP93XX_SOC_SNAPPERCL15"; then
  AC_DEFINE(CONFIG_SND_EP93XX_SOC_SNAPPERCL15_MODULE)
fi
if test -n "$CONFIG_SND_EP93XX_SOC_SIMONE"; then
  AC_DEFINE(CONFIG_SND_EP93XX_SOC_SIMONE_MODULE)
fi
if test -n "$CONFIG_SND_EP93XX_SOC_EDB93XX"; then
  AC_DEFINE(CONFIG_SND_EP93XX_SOC_EDB93XX_MODULE)
fi
if test -n "$CONFIG_SND_MPC52XX_DMA"; then
  AC_DEFINE(CONFIG_SND_MPC52XX_DMA_MODULE)
fi
if test -n "$CONFIG_SND_SOC_POWERPC_SSI"; then
  AC_DEFINE(CONFIG_SND_SOC_POWERPC_SSI_MODULE)
fi
if test -n "$CONFIG_SND_SOC_MPC8610_HPCD"; then
  AC_DEFINE(CONFIG_SND_SOC_MPC8610_HPCD_MODULE)
fi
if test -n "$CONFIG_SND_SOC_P1022_DS"; then
  AC_DEFINE(CONFIG_SND_SOC_P1022_DS_MODULE)
fi
if test -n "$CONFIG_SND_SOC_MPC5200_I2S"; then
  AC_DEFINE(CONFIG_SND_SOC_MPC5200_I2S_MODULE)
fi
if test -n "$CONFIG_SND_SOC_MPC5200_AC97"; then
  AC_DEFINE(CONFIG_SND_SOC_MPC5200_AC97_MODULE)
fi
if test -n "$CONFIG_SND_MPC52XX_SOC_PCM030"; then
  AC_DEFINE(CONFIG_SND_MPC52XX_SOC_PCM030_MODULE)
fi
if test -n "$CONFIG_SND_MPC52XX_SOC_EFIKA"; then
  AC_DEFINE(CONFIG_SND_MPC52XX_SOC_EFIKA_MODULE)
fi
if test -n "$CONFIG_SND_IMX_SOC"; then
  AC_DEFINE(CONFIG_SND_IMX_SOC_MODULE)
fi
if test -n "$CONFIG_SND_MXC_SOC_FIQ"; then
  AC_DEFINE(CONFIG_SND_MXC_SOC_FIQ_MODULE)
fi
if test -n "$CONFIG_SND_MXC_SOC_MX2"; then
  AC_DEFINE(CONFIG_SND_MXC_SOC_MX2_MODULE)
fi
if test -n "$CONFIG_SND_MXC_SOC_WM1133_EV1"; then
  AC_DEFINE(CONFIG_SND_MXC_SOC_WM1133_EV1_MODULE)
fi
if test -n "$CONFIG_SND_SOC_MX27VIS_AIC32X4"; then
  AC_DEFINE(CONFIG_SND_SOC_MX27VIS_AIC32X4_MODULE)
fi
if test -n "$CONFIG_SND_SOC_PHYCORE_AC97"; then
  AC_DEFINE(CONFIG_SND_SOC_PHYCORE_AC97_MODULE)
fi
if test -n "$CONFIG_SND_SOC_EUKREA_TLV320"; then
  AC_DEFINE(CONFIG_SND_SOC_EUKREA_TLV320_MODULE)
fi
if test -n "$CONFIG_SND_JZ4740_SOC"; then
  AC_DEFINE(CONFIG_SND_JZ4740_SOC_MODULE)
fi
if test -n "$CONFIG_SND_JZ4740_SOC_I2S"; then
  AC_DEFINE(CONFIG_SND_JZ4740_SOC_I2S_MODULE)
fi
if test -n "$CONFIG_SND_JZ4740_SOC_QI_LB60"; then
  AC_DEFINE(CONFIG_SND_JZ4740_SOC_QI_LB60_MODULE)
fi
if test -n "$CONFIG_SND_SOC_NUC900"; then
  AC_DEFINE(CONFIG_SND_SOC_NUC900_MODULE)
fi
if test -n "$CONFIG_SND_SOC_NUC900_AC97"; then
  AC_DEFINE(CONFIG_SND_SOC_NUC900_AC97_MODULE)
fi
if test -n "$CONFIG_SND_SOC_NUC900EVB"; then
  AC_DEFINE(CONFIG_SND_SOC_NUC900EVB_MODULE)
fi
if test -n "$CONFIG_SND_OMAP_SOC"; then
  AC_DEFINE(CONFIG_SND_OMAP_SOC_MODULE)
fi
if test -n "$CONFIG_SND_OMAP_SOC_DMIC"; then
  AC_DEFINE(CONFIG_SND_OMAP_SOC_DMIC_MODULE)
fi
if test -n "$CONFIG_SND_OMAP_SOC_MCBSP"; then
  AC_DEFINE(CONFIG_SND_OMAP_SOC_MCBSP_MODULE)
fi
if test -n "$CONFIG_SND_OMAP_SOC_MCPDM"; then
  AC_DEFINE(CONFIG_SND_OMAP_SOC_MCPDM_MODULE)
fi
if test -n "$CONFIG_SND_OMAP_SOC_HDMI"; then
  AC_DEFINE(CONFIG_SND_OMAP_SOC_HDMI_MODULE)
fi
if test -n "$CONFIG_SND_OMAP_SOC_N810"; then
  AC_DEFINE(CONFIG_SND_OMAP_SOC_N810_MODULE)
fi
if test -n "$CONFIG_SND_OMAP_SOC_RX51"; then
  AC_DEFINE(CONFIG_SND_OMAP_SOC_RX51_MODULE)
fi
if test -n "$CONFIG_SND_OMAP_SOC_AMS_DELTA"; then
  AC_DEFINE(CONFIG_SND_OMAP_SOC_AMS_DELTA_MODULE)
fi
if test -n "$CONFIG_SND_OMAP_SOC_OSK5912"; then
  AC_DEFINE(CONFIG_SND_OMAP_SOC_OSK5912_MODULE)
fi
if test -n "$CONFIG_SND_OMAP_SOC_OVERO"; then
  AC_DEFINE(CONFIG_SND_OMAP_SOC_OVERO_MODULE)
fi
if test -n "$CONFIG_SND_OMAP_SOC_OMAP3EVM"; then
  AC_DEFINE(CONFIG_SND_OMAP_SOC_OMAP3EVM_MODULE)
fi
if test -n "$CONFIG_SND_OMAP_SOC_AM3517EVM"; then
  AC_DEFINE(CONFIG_SND_OMAP_SOC_AM3517EVM_MODULE)
fi
if test -n "$CONFIG_SND_OMAP_SOC_SDP3430"; then
  AC_DEFINE(CONFIG_SND_OMAP_SOC_SDP3430_MODULE)
fi
if test -n "$CONFIG_SND_OMAP_SOC_SDP4430"; then
  AC_DEFINE(CONFIG_SND_OMAP_SOC_SDP4430_MODULE)
fi
if test -n "$CONFIG_SND_OMAP_SOC_OMAP4_HDMI"; then
  AC_DEFINE(CONFIG_SND_OMAP_SOC_OMAP4_HDMI_MODULE)
fi
if test -n "$CONFIG_SND_OMAP_SOC_OMAP3_PANDORA"; then
  AC_DEFINE(CONFIG_SND_OMAP_SOC_OMAP3_PANDORA_MODULE)
fi
if test -n "$CONFIG_SND_OMAP_SOC_OMAP3_BEAGLE"; then
  AC_DEFINE(CONFIG_SND_OMAP_SOC_OMAP3_BEAGLE_MODULE)
fi
if test -n "$CONFIG_SND_OMAP_SOC_ZOOM2"; then
  AC_DEFINE(CONFIG_SND_OMAP_SOC_ZOOM2_MODULE)
fi
if test -n "$CONFIG_SND_OMAP_SOC_IGEP0020"; then
  AC_DEFINE(CONFIG_SND_OMAP_SOC_IGEP0020_MODULE)
fi
if test -n "$CONFIG_SND_KIRKWOOD_SOC"; then
  AC_DEFINE(CONFIG_SND_KIRKWOOD_SOC_MODULE)
fi
if test -n "$CONFIG_SND_KIRKWOOD_SOC_I2S"; then
  AC_DEFINE(CONFIG_SND_KIRKWOOD_SOC_I2S_MODULE)
fi
if test -n "$CONFIG_SND_KIRKWOOD_SOC_OPENRD"; then
  AC_DEFINE(CONFIG_SND_KIRKWOOD_SOC_OPENRD_MODULE)
fi
if test -n "$CONFIG_SND_KIRKWOOD_SOC_T5325"; then
  AC_DEFINE(CONFIG_SND_KIRKWOOD_SOC_T5325_MODULE)
fi
if test -n "$CONFIG_SND_MFLD_MACHINE"; then
  AC_DEFINE(CONFIG_SND_MFLD_MACHINE_MODULE)
fi
if test -n "$CONFIG_SND_SST_PLATFORM"; then
  AC_DEFINE(CONFIG_SND_SST_PLATFORM_MODULE)
fi
if test -n "$CONFIG_SND_MXS_SOC"; then
  AC_DEFINE(CONFIG_SND_MXS_SOC_MODULE)
fi
if test -n "$CONFIG_SND_SOC_MXS_SGTL5000"; then
  AC_DEFINE(CONFIG_SND_SOC_MXS_SGTL5000_MODULE)
fi
if test -n "$CONFIG_SND_PXA2XX_SOC"; then
  AC_DEFINE(CONFIG_SND_PXA2XX_SOC_MODULE)
fi
if test -n "$CONFIG_SND_PXA2XX_SOC_AC97"; then
  AC_DEFINE(CONFIG_SND_PXA2XX_SOC_AC97_MODULE)
fi
if test -n "$CONFIG_SND_PXA2XX_SOC_I2S"; then
  AC_DEFINE(CONFIG_SND_PXA2XX_SOC_I2S_MODULE)
fi
if test -n "$CONFIG_SND_PXA_SOC_SSP"; then
  AC_DEFINE(CONFIG_SND_PXA_SOC_SSP_MODULE)
fi
if test -n "$CONFIG_SND_PXA2XX_SOC_CORGI"; then
  AC_DEFINE(CONFIG_SND_PXA2XX_SOC_CORGI_MODULE)
fi
if test -n "$CONFIG_SND_PXA2XX_SOC_SPITZ"; then
  AC_DEFINE(CONFIG_SND_PXA2XX_SOC_SPITZ_MODULE)
fi
if test -n "$CONFIG_SND_PXA2XX_SOC_Z2"; then
  AC_DEFINE(CONFIG_SND_PXA2XX_SOC_Z2_MODULE)
fi
if test -n "$CONFIG_SND_PXA2XX_SOC_POODLE"; then
  AC_DEFINE(CONFIG_SND_PXA2XX_SOC_POODLE_MODULE)
fi
if test -n "$CONFIG_SND_PXA2XX_SOC_TOSA"; then
  AC_DEFINE(CONFIG_SND_PXA2XX_SOC_TOSA_MODULE)
fi
if test -n "$CONFIG_SND_PXA2XX_SOC_E740"; then
  AC_DEFINE(CONFIG_SND_PXA2XX_SOC_E740_MODULE)
fi
if test -n "$CONFIG_SND_PXA2XX_SOC_E750"; then
  AC_DEFINE(CONFIG_SND_PXA2XX_SOC_E750_MODULE)
fi
if test -n "$CONFIG_SND_PXA2XX_SOC_E800"; then
  AC_DEFINE(CONFIG_SND_PXA2XX_SOC_E800_MODULE)
fi
if test -n "$CONFIG_SND_PXA2XX_SOC_EM_X270"; then
  AC_DEFINE(CONFIG_SND_PXA2XX_SOC_EM_X270_MODULE)
fi
if test -n "$CONFIG_SND_PXA2XX_SOC_PALM27X"; then
  AC_DEFINE(CONFIG_SND_PXA2XX_SOC_PALM27X)
fi
if test -n "$CONFIG_SND_SOC_SAARB"; then
  AC_DEFINE(CONFIG_SND_SOC_SAARB_MODULE)
fi
if test -n "$CONFIG_SND_SOC_TAVOREVB3"; then
  AC_DEFINE(CONFIG_SND_SOC_TAVOREVB3_MODULE)
fi
if test -n "$CONFIG_SND_SOC_ZYLONITE"; then
  AC_DEFINE(CONFIG_SND_SOC_ZYLONITE_MODULE)
fi
if test -n "$CONFIG_SND_SOC_RAUMFELD"; then
  AC_DEFINE(CONFIG_SND_SOC_RAUMFELD_MODULE)
fi
if test -n "$CONFIG_SND_PXA2XX_SOC_HX4700"; then
  AC_DEFINE(CONFIG_SND_PXA2XX_SOC_HX4700_MODULE)
fi
if test -n "$CONFIG_SND_PXA2XX_SOC_MAGICIAN"; then
  AC_DEFINE(CONFIG_SND_PXA2XX_SOC_MAGICIAN_MODULE)
fi
if test -n "$CONFIG_SND_PXA2XX_SOC_MIOA701"; then
  AC_DEFINE(CONFIG_SND_PXA2XX_SOC_MIOA701_MODULE)
fi
if test -n "$CONFIG_SND_PXA2XX_SOC_IMOTE2"; then
  AC_DEFINE(CONFIG_SND_PXA2XX_SOC_IMOTE2_MODULE)
fi
if test -n "$CONFIG_SND_SOC_SAMSUNG"; then
  AC_DEFINE(CONFIG_SND_SOC_SAMSUNG_MODULE)
fi
if test -n "$CONFIG_SND_S3C24XX_I2S"; then
  AC_DEFINE(CONFIG_SND_S3C24XX_I2S_MODULE)
fi
if test -n "$CONFIG_SND_S3C_I2SV2_SOC"; then
  AC_DEFINE(CONFIG_SND_S3C_I2SV2_SOC_MODULE)
fi
if test -n "$CONFIG_SND_S3C2412_SOC_I2S"; then
  AC_DEFINE(CONFIG_SND_S3C2412_SOC_I2S_MODULE)
fi
if test -n "$CONFIG_SND_SAMSUNG_PCM"; then
  AC_DEFINE(CONFIG_SND_SAMSUNG_PCM_MODULE)
fi
if test -n "$CONFIG_SND_SAMSUNG_AC97"; then
  AC_DEFINE(CONFIG_SND_SAMSUNG_AC97_MODULE)
fi
if test -n "$CONFIG_SND_SAMSUNG_SPDIF"; then
  AC_DEFINE(CONFIG_SND_SAMSUNG_SPDIF_MODULE)
fi
if test -n "$CONFIG_SND_SAMSUNG_I2S"; then
  AC_DEFINE(CONFIG_SND_SAMSUNG_I2S_MODULE)
fi
if test -n "$CONFIG_SND_SOC_SAMSUNG_NEO1973_WM8753"; then
  AC_DEFINE(CONFIG_SND_SOC_SAMSUNG_NEO1973_WM8753_MODULE)
fi
if test -n "$CONFIG_SND_SOC_SAMSUNG_JIVE_WM8750"; then
  AC_DEFINE(CONFIG_SND_SOC_SAMSUNG_JIVE_WM8750_MODULE)
fi
if test -n "$CONFIG_SND_SOC_SAMSUNG_SMDK_WM8580"; then
  AC_DEFINE(CONFIG_SND_SOC_SAMSUNG_SMDK_WM8580_MODULE)
fi
if test -n "$CONFIG_SND_SOC_SAMSUNG_SMDK_WM8994"; then
  AC_DEFINE(CONFIG_SND_SOC_SAMSUNG_SMDK_WM8994_MODULE)
fi
if test -n "$CONFIG_SND_SOC_SAMSUNG_SMDK2443_WM9710"; then
  AC_DEFINE(CONFIG_SND_SOC_SAMSUNG_SMDK2443_WM9710_MODULE)
fi
if test -n "$CONFIG_SND_SOC_SAMSUNG_LN2440SBC_ALC650"; then
  AC_DEFINE(CONFIG_SND_SOC_SAMSUNG_LN2440SBC_ALC650_MODULE)
fi
if test -n "$CONFIG_SND_SOC_SAMSUNG_S3C24XX_UDA134X"; then
  AC_DEFINE(CONFIG_SND_SOC_SAMSUNG_S3C24XX_UDA134X_MODULE)
fi
if test -n "$CONFIG_SND_SOC_SAMSUNG_SIMTEC"; then
  AC_DEFINE(CONFIG_SND_SOC_SAMSUNG_SIMTEC_MODULE)
fi
if test -n "$CONFIG_SND_SOC_SAMSUNG_SIMTEC_TLV320AIC23"; then
  AC_DEFINE(CONFIG_SND_SOC_SAMSUNG_SIMTEC_TLV320AIC23_MODULE)
fi
if test -n "$CONFIG_SND_SOC_SAMSUNG_SIMTEC_HERMES"; then
  AC_DEFINE(CONFIG_SND_SOC_SAMSUNG_SIMTEC_HERMES_MODULE)
fi
if test -n "$CONFIG_SND_SOC_SAMSUNG_H1940_UDA1380"; then
  AC_DEFINE(CONFIG_SND_SOC_SAMSUNG_H1940_UDA1380_MODULE)
fi
if test -n "$CONFIG_SND_SOC_SAMSUNG_RX1950_UDA1380"; then
  AC_DEFINE(CONFIG_SND_SOC_SAMSUNG_RX1950_UDA1380_MODULE)
fi
if test -n "$CONFIG_SND_SOC_SAMSUNG_SMDK_WM9713"; then
  AC_DEFINE(CONFIG_SND_SOC_SAMSUNG_SMDK_WM9713_MODULE)
fi
if test -n "$CONFIG_SND_SOC_SMARTQ"; then
  AC_DEFINE(CONFIG_SND_SOC_SMARTQ_MODULE)
fi
if test -n "$CONFIG_SND_SOC_GONI_AQUILA_WM8994"; then
  AC_DEFINE(CONFIG_SND_SOC_GONI_AQUILA_WM8994_MODULE)
fi
if test -n "$CONFIG_SND_SOC_SAMSUNG_SMDK_SPDIF"; then
  AC_DEFINE(CONFIG_SND_SOC_SAMSUNG_SMDK_SPDIF_MODULE)
fi
if test -n "$CONFIG_SND_SOC_SMDK_WM8580_PCM"; then
  AC_DEFINE(CONFIG_SND_SOC_SMDK_WM8580_PCM_MODULE)
fi
if test -n "$CONFIG_SND_SOC_SMDK_WM8994_PCM"; then
  AC_DEFINE(CONFIG_SND_SOC_SMDK_WM8994_PCM_MODULE)
fi
if test -n "$CONFIG_SND_SOC_SPEYSIDE"; then
  AC_DEFINE(CONFIG_SND_SOC_SPEYSIDE_MODULE)
fi
if test -n "$CONFIG_SND_SOC_TOBERMORY"; then
  AC_DEFINE(CONFIG_SND_SOC_TOBERMORY_MODULE)
fi
if test -n "$CONFIG_SND_SOC_LOWLAND"; then
  AC_DEFINE(CONFIG_SND_SOC_LOWLAND_MODULE)
fi
if test -n "$CONFIG_SND_SOC_LITTLEMILL"; then
  AC_DEFINE(CONFIG_SND_SOC_LITTLEMILL_MODULE)
fi
if test -n "$CONFIG_SND_S6000_SOC"; then
  AC_DEFINE(CONFIG_SND_S6000_SOC_MODULE)
fi
if test -n "$CONFIG_SND_S6000_SOC_I2S"; then
  AC_DEFINE(CONFIG_SND_S6000_SOC_I2S_MODULE)
fi
if test -n "$CONFIG_SND_S6000_SOC_S6IPCAM"; then
  AC_DEFINE(CONFIG_SND_S6000_SOC_S6IPCAM_MODULE)
fi
if test -n "$CONFIG_SND_SOC_PCM_SH7760"; then
  AC_DEFINE(CONFIG_SND_SOC_PCM_SH7760_MODULE)
fi
if test -n "$CONFIG_SND_SOC_SH4_HAC"; then
  AC_DEFINE(CONFIG_SND_SOC_SH4_HAC_MODULE)
fi
if test -n "$CONFIG_SND_SOC_SH4_SSI"; then
  AC_DEFINE(CONFIG_SND_SOC_SH4_SSI_MODULE)
fi
if test -n "$CONFIG_SND_SOC_SH4_FSI"; then
  AC_DEFINE(CONFIG_SND_SOC_SH4_FSI_MODULE)
fi
if test -n "$CONFIG_SND_SOC_SH4_SIU"; then
  AC_DEFINE(CONFIG_SND_SOC_SH4_SIU_MODULE)
fi
if test -n "$CONFIG_SND_SH7760_AC97"; then
  AC_DEFINE(CONFIG_SND_SH7760_AC97_MODULE)
fi
if test -n "$CONFIG_SND_FSI_AK4642"; then
  AC_DEFINE(CONFIG_SND_FSI_AK4642_MODULE)
fi
if test -n "$CONFIG_SND_FSI_DA7210"; then
  AC_DEFINE(CONFIG_SND_FSI_DA7210_MODULE)
fi
if test -n "$CONFIG_SND_FSI_HDMI"; then
  AC_DEFINE(CONFIG_SND_FSI_HDMI_MODULE)
fi
if test -n "$CONFIG_SND_SIU_MIGOR"; then
  AC_DEFINE(CONFIG_SND_SIU_MIGOR_MODULE)
fi
if test -n "$CONFIG_SND_SOC_TEGRA"; then
  AC_DEFINE(CONFIG_SND_SOC_TEGRA_MODULE)
fi
if test -n "$CONFIG_SND_SOC_TEGRA_I2S"; then
  AC_DEFINE(CONFIG_SND_SOC_TEGRA_I2S_MODULE)
fi
if test -n "$CONFIG_SND_SOC_TEGRA_SPDIF"; then
  AC_DEFINE(CONFIG_SND_SOC_TEGRA_SPDIF_MODULE)
fi
if test -n "$CONFIG_MACH_HAS_SND_SOC_TEGRA_WM8903"; then
  AC_DEFINE(CONFIG_MACH_HAS_SND_SOC_TEGRA_WM8903)
fi
if test -n "$CONFIG_SND_SOC_TEGRA_WM8903"; then
  AC_DEFINE(CONFIG_SND_SOC_TEGRA_WM8903_MODULE)
fi
if test -n "$CONFIG_SND_SOC_TEGRA_TRIMSLICE"; then
  AC_DEFINE(CONFIG_SND_SOC_TEGRA_TRIMSLICE_MODULE)
fi
if test -n "$CONFIG_SND_SOC_TEGRA_ALC5632"; then
  AC_DEFINE(CONFIG_SND_SOC_TEGRA_ALC5632_MODULE)
fi
if test -n "$CONFIG_SND_SOC_TXX9ACLC"; then
  AC_DEFINE(CONFIG_SND_SOC_TXX9ACLC_MODULE)
fi
if test -n "$CONFIG_HAS_TXX9_ACLC"; then
  AC_DEFINE(CONFIG_HAS_TXX9_ACLC)
fi
if test -n "$CONFIG_SND_SOC_TXX9ACLC_AC97"; then
  AC_DEFINE(CONFIG_SND_SOC_TXX9ACLC_AC97_MODULE)
fi
if test -n "$CONFIG_SND_SOC_TXX9ACLC_GENERIC"; then
  AC_DEFINE(CONFIG_SND_SOC_TXX9ACLC_GENERIC_MODULE)
fi
if test -n "$CONFIG_SND_SOC_I2C_AND_SPI"; then
  AC_DEFINE(CONFIG_SND_SOC_I2C_AND_SPI_MODULE)
fi
if test -n "$CONFIG_SND_SOC_ALL_CODECS"; then
  AC_DEFINE(CONFIG_SND_SOC_ALL_CODECS_MODULE)
fi
if test -n "$CONFIG_SND_SOC_88PM860X"; then
  AC_DEFINE(CONFIG_SND_SOC_88PM860X_MODULE)
fi
if test -n "$CONFIG_SND_SOC_WM_HUBS"; then
  AC_DEFINE(CONFIG_SND_SOC_WM_HUBS_MODULE)
fi
if test -n "$CONFIG_SND_SOC_AC97_CODEC"; then
  AC_DEFINE(CONFIG_SND_SOC_AC97_CODEC_MODULE)
fi
if test -n "$CONFIG_SND_SOC_AD1836"; then
  AC_DEFINE(CONFIG_SND_SOC_AD1836_MODULE)
fi
if test -n "$CONFIG_SND_SOC_AD193X"; then
  AC_DEFINE(CONFIG_SND_SOC_AD193X_MODULE)
fi
if test -n "$CONFIG_SND_SOC_AD1980"; then
  AC_DEFINE(CONFIG_SND_SOC_AD1980_MODULE)
fi
if test -n "$CONFIG_SND_SOC_AD73311"; then
  AC_DEFINE(CONFIG_SND_SOC_AD73311_MODULE)
fi
if test -n "$CONFIG_SND_SOC_ADAU1701"; then
  AC_DEFINE(CONFIG_SND_SOC_ADAU1701_MODULE)
fi
if test -n "$CONFIG_SND_SOC_ADAU1373"; then
  AC_DEFINE(CONFIG_SND_SOC_ADAU1373_MODULE)
fi
if test -n "$CONFIG_SND_SOC_ADAV80X"; then
  AC_DEFINE(CONFIG_SND_SOC_ADAV80X_MODULE)
fi
if test -n "$CONFIG_SND_SOC_ADS117X"; then
  AC_DEFINE(CONFIG_SND_SOC_ADS117X_MODULE)
fi
if test -n "$CONFIG_SND_SOC_AK4104"; then
  AC_DEFINE(CONFIG_SND_SOC_AK4104_MODULE)
fi
if test -n "$CONFIG_SND_SOC_AK4535"; then
  AC_DEFINE(CONFIG_SND_SOC_AK4535_MODULE)
fi
if test -n "$CONFIG_SND_SOC_AK4641"; then
  AC_DEFINE(CONFIG_SND_SOC_AK4641_MODULE)
fi
if test -n "$CONFIG_SND_SOC_AK4642"; then
  AC_DEFINE(CONFIG_SND_SOC_AK4642_MODULE)
fi
if test -n "$CONFIG_SND_SOC_AK4671"; then
  AC_DEFINE(CONFIG_SND_SOC_AK4671_MODULE)
fi
if test -n "$CONFIG_SND_SOC_ALC5623"; then
  AC_DEFINE(CONFIG_SND_SOC_ALC5623_MODULE)
fi
if test -n "$CONFIG_SND_SOC_ALC5632"; then
  AC_DEFINE(CONFIG_SND_SOC_ALC5632_MODULE)
fi
if test -n "$CONFIG_SND_SOC_CQ0093VC"; then
  AC_DEFINE(CONFIG_SND_SOC_CQ0093VC_MODULE)
fi
if test -n "$CONFIG_SND_SOC_CS42L51"; then
  AC_DEFINE(CONFIG_SND_SOC_CS42L51_MODULE)
fi
if test -n "$CONFIG_SND_SOC_CS42L73"; then
  AC_DEFINE(CONFIG_SND_SOC_CS42L73_MODULE)
fi
if test -n "$CONFIG_SND_SOC_CS4270"; then
  AC_DEFINE(CONFIG_SND_SOC_CS4270_MODULE)
fi
if test -n "$CONFIG_SND_SOC_CS4270_VD33_ERRATA"; then
  AC_DEFINE(CONFIG_SND_SOC_CS4270_VD33_ERRATA)
fi
if test -n "$CONFIG_SND_SOC_CS4271"; then
  AC_DEFINE(CONFIG_SND_SOC_CS4271_MODULE)
fi
if test -n "$CONFIG_SND_SOC_CX20442"; then
  AC_DEFINE(CONFIG_SND_SOC_CX20442_MODULE)
fi
if test -n "$CONFIG_SND_SOC_JZ4740_CODEC"; then
  AC_DEFINE(CONFIG_SND_SOC_JZ4740_CODEC_MODULE)
fi
if test -n "$CONFIG_SND_SOC_L3"; then
  AC_DEFINE(CONFIG_SND_SOC_L3_MODULE)
fi
if test -n "$CONFIG_SND_SOC_DA7210"; then
  AC_DEFINE(CONFIG_SND_SOC_DA7210_MODULE)
fi
if test -n "$CONFIG_SND_SOC_DFBMCS320"; then
  AC_DEFINE(CONFIG_SND_SOC_DFBMCS320_MODULE)
fi
if test -n "$CONFIG_SND_SOC_DMIC"; then
  AC_DEFINE(CONFIG_SND_SOC_DMIC_MODULE)
fi
if test -n "$CONFIG_SND_SOC_MAX98088"; then
  AC_DEFINE(CONFIG_SND_SOC_MAX98088_MODULE)
fi
if test -n "$CONFIG_SND_SOC_MAX98095"; then
  AC_DEFINE(CONFIG_SND_SOC_MAX98095_MODULE)
fi
if test -n "$CONFIG_SND_SOC_MAX9850"; then
  AC_DEFINE(CONFIG_SND_SOC_MAX9850_MODULE)
fi
if test -n "$CONFIG_SND_SOC_PCM3008"; then
  AC_DEFINE(CONFIG_SND_SOC_PCM3008_MODULE)
fi
if test -n "$CONFIG_SND_SOC_RT5631"; then
  AC_DEFINE(CONFIG_SND_SOC_RT5631_MODULE)
fi
if test -n "$CONFIG_SND_SOC_SGTL5000"; then
  AC_DEFINE(CONFIG_SND_SOC_SGTL5000_MODULE)
fi
if test -n "$CONFIG_SND_SOC_SIGMADSP"; then
  AC_DEFINE(CONFIG_SND_SOC_SIGMADSP_MODULE)
fi
if test -n "$CONFIG_SND_SOC_SN95031"; then
  AC_DEFINE(CONFIG_SND_SOC_SN95031_MODULE)
fi
if test -n "$CONFIG_SND_SOC_SPDIF"; then
  AC_DEFINE(CONFIG_SND_SOC_SPDIF_MODULE)
fi
if test -n "$CONFIG_SND_SOC_SSM2602"; then
  AC_DEFINE(CONFIG_SND_SOC_SSM2602_MODULE)
fi
if test -n "$CONFIG_SND_SOC_STA32X"; then
  AC_DEFINE(CONFIG_SND_SOC_STA32X_MODULE)
fi
if test -n "$CONFIG_SND_SOC_STAC9766"; then
  AC_DEFINE(CONFIG_SND_SOC_STAC9766_MODULE)
fi
if test -n "$CONFIG_SND_SOC_TLV320AIC23"; then
  AC_DEFINE(CONFIG_SND_SOC_TLV320AIC23_MODULE)
fi
if test -n "$CONFIG_SND_SOC_TLV320AIC26"; then
  AC_DEFINE(CONFIG_SND_SOC_TLV320AIC26_MODULE)
fi
if test -n "$CONFIG_SND_SOC_TLV320AIC32X4"; then
  AC_DEFINE(CONFIG_SND_SOC_TLV320AIC32X4_MODULE)
fi
if test -n "$CONFIG_SND_SOC_TLV320AIC3X"; then
  AC_DEFINE(CONFIG_SND_SOC_TLV320AIC3X_MODULE)
fi
if test -n "$CONFIG_SND_SOC_TLV320DAC33"; then
  AC_DEFINE(CONFIG_SND_SOC_TLV320DAC33_MODULE)
fi
if test -n "$CONFIG_SND_SOC_TWL4030"; then
  AC_DEFINE(CONFIG_SND_SOC_TWL4030_MODULE)
fi
if test -n "$CONFIG_SND_SOC_TWL6040"; then
  AC_DEFINE(CONFIG_SND_SOC_TWL6040_MODULE)
fi
if test -n "$CONFIG_SND_SOC_UDA134X"; then
  AC_DEFINE(CONFIG_SND_SOC_UDA134X_MODULE)
fi
if test -n "$CONFIG_SND_SOC_UDA1380"; then
  AC_DEFINE(CONFIG_SND_SOC_UDA1380_MODULE)
fi
if test -n "$CONFIG_SND_SOC_WL1273"; then
  AC_DEFINE(CONFIG_SND_SOC_WL1273_MODULE)
fi
if test -n "$CONFIG_SND_SOC_WM1250_EV1"; then
  AC_DEFINE(CONFIG_SND_SOC_WM1250_EV1_MODULE)
fi
if test -n "$CONFIG_SND_SOC_WM2000"; then
  AC_DEFINE(CONFIG_SND_SOC_WM2000_MODULE)
fi
if test -n "$CONFIG_SND_SOC_WM5100"; then
  AC_DEFINE(CONFIG_SND_SOC_WM5100_MODULE)
fi
if test -n "$CONFIG_SND_SOC_WM8350"; then
  AC_DEFINE(CONFIG_SND_SOC_WM8350_MODULE)
fi
if test -n "$CONFIG_SND_SOC_WM8400"; then
  AC_DEFINE(CONFIG_SND_SOC_WM8400_MODULE)
fi
if test -n "$CONFIG_SND_SOC_WM8510"; then
  AC_DEFINE(CONFIG_SND_SOC_WM8510_MODULE)
fi
if test -n "$CONFIG_SND_SOC_WM8523"; then
  AC_DEFINE(CONFIG_SND_SOC_WM8523_MODULE)
fi
if test -n "$CONFIG_SND_SOC_WM8580"; then
  AC_DEFINE(CONFIG_SND_SOC_WM8580_MODULE)
fi
if test -n "$CONFIG_SND_SOC_WM8711"; then
  AC_DEFINE(CONFIG_SND_SOC_WM8711_MODULE)
fi
if test -n "$CONFIG_SND_SOC_WM8727"; then
  AC_DEFINE(CONFIG_SND_SOC_WM8727_MODULE)
fi
if test -n "$CONFIG_SND_SOC_WM8728"; then
  AC_DEFINE(CONFIG_SND_SOC_WM8728_MODULE)
fi
if test -n "$CONFIG_SND_SOC_WM8731"; then
  AC_DEFINE(CONFIG_SND_SOC_WM8731_MODULE)
fi
if test -n "$CONFIG_SND_SOC_WM8737"; then
  AC_DEFINE(CONFIG_SND_SOC_WM8737_MODULE)
fi
if test -n "$CONFIG_SND_SOC_WM8741"; then
  AC_DEFINE(CONFIG_SND_SOC_WM8741_MODULE)
fi
if test -n "$CONFIG_SND_SOC_WM8750"; then
  AC_DEFINE(CONFIG_SND_SOC_WM8750_MODULE)
fi
if test -n "$CONFIG_SND_SOC_WM8753"; then
  AC_DEFINE(CONFIG_SND_SOC_WM8753_MODULE)
fi
if test -n "$CONFIG_SND_SOC_WM8770"; then
  AC_DEFINE(CONFIG_SND_SOC_WM8770_MODULE)
fi
if test -n "$CONFIG_SND_SOC_WM8776"; then
  AC_DEFINE(CONFIG_SND_SOC_WM8776_MODULE)
fi
if test -n "$CONFIG_SND_SOC_WM8782"; then
  AC_DEFINE(CONFIG_SND_SOC_WM8782_MODULE)
fi
if test -n "$CONFIG_SND_SOC_WM8804"; then
  AC_DEFINE(CONFIG_SND_SOC_WM8804_MODULE)
fi
if test -n "$CONFIG_SND_SOC_WM8900"; then
  AC_DEFINE(CONFIG_SND_SOC_WM8900_MODULE)
fi
if test -n "$CONFIG_SND_SOC_WM8903"; then
  AC_DEFINE(CONFIG_SND_SOC_WM8903_MODULE)
fi
if test -n "$CONFIG_SND_SOC_WM8904"; then
  AC_DEFINE(CONFIG_SND_SOC_WM8904_MODULE)
fi
if test -n "$CONFIG_SND_SOC_WM8940"; then
  AC_DEFINE(CONFIG_SND_SOC_WM8940_MODULE)
fi
if test -n "$CONFIG_SND_SOC_WM8955"; then
  AC_DEFINE(CONFIG_SND_SOC_WM8955_MODULE)
fi
if test -n "$CONFIG_SND_SOC_WM8960"; then
  AC_DEFINE(CONFIG_SND_SOC_WM8960_MODULE)
fi
if test -n "$CONFIG_SND_SOC_WM8961"; then
  AC_DEFINE(CONFIG_SND_SOC_WM8961_MODULE)
fi
if test -n "$CONFIG_SND_SOC_WM8962"; then
  AC_DEFINE(CONFIG_SND_SOC_WM8962_MODULE)
fi
if test -n "$CONFIG_SND_SOC_WM8971"; then
  AC_DEFINE(CONFIG_SND_SOC_WM8971_MODULE)
fi
if test -n "$CONFIG_SND_SOC_WM8974"; then
  AC_DEFINE(CONFIG_SND_SOC_WM8974_MODULE)
fi
if test -n "$CONFIG_SND_SOC_WM8978"; then
  AC_DEFINE(CONFIG_SND_SOC_WM8978_MODULE)
fi
if test -n "$CONFIG_SND_SOC_WM8983"; then
  AC_DEFINE(CONFIG_SND_SOC_WM8983_MODULE)
fi
if test -n "$CONFIG_SND_SOC_WM8985"; then
  AC_DEFINE(CONFIG_SND_SOC_WM8985_MODULE)
fi
if test -n "$CONFIG_SND_SOC_WM8988"; then
  AC_DEFINE(CONFIG_SND_SOC_WM8988_MODULE)
fi
if test -n "$CONFIG_SND_SOC_WM8990"; then
  AC_DEFINE(CONFIG_SND_SOC_WM8990_MODULE)
fi
if test -n "$CONFIG_SND_SOC_WM8991"; then
  AC_DEFINE(CONFIG_SND_SOC_WM8991_MODULE)
fi
if test -n "$CONFIG_SND_SOC_WM8993"; then
  AC_DEFINE(CONFIG_SND_SOC_WM8993_MODULE)
fi
if test -n "$CONFIG_SND_SOC_WM8994"; then
  AC_DEFINE(CONFIG_SND_SOC_WM8994_MODULE)
fi
if test -n "$CONFIG_SND_SOC_WM8995"; then
  AC_DEFINE(CONFIG_SND_SOC_WM8995_MODULE)
fi
if test -n "$CONFIG_SND_SOC_WM8996"; then
  AC_DEFINE(CONFIG_SND_SOC_WM8996_MODULE)
fi
if test -n "$CONFIG_SND_SOC_WM9081"; then
  AC_DEFINE(CONFIG_SND_SOC_WM9081_MODULE)
fi
if test -n "$CONFIG_SND_SOC_WM9090"; then
  AC_DEFINE(CONFIG_SND_SOC_WM9090_MODULE)
fi
if test -n "$CONFIG_SND_SOC_WM9705"; then
  AC_DEFINE(CONFIG_SND_SOC_WM9705_MODULE)
fi
if test -n "$CONFIG_SND_SOC_WM9712"; then
  AC_DEFINE(CONFIG_SND_SOC_WM9712_MODULE)
fi
if test -n "$CONFIG_SND_SOC_WM9713"; then
  AC_DEFINE(CONFIG_SND_SOC_WM9713_MODULE)
fi
if test -n "$CONFIG_SND_SOC_LM4857"; then
  AC_DEFINE(CONFIG_SND_SOC_LM4857_MODULE)
fi
if test -n "$CONFIG_SND_SOC_MAX9877"; then
  AC_DEFINE(CONFIG_SND_SOC_MAX9877_MODULE)
fi
if test -n "$CONFIG_SND_SOC_TPA6130A2"; then
  AC_DEFINE(CONFIG_SND_SOC_TPA6130A2_MODULE)
fi
if test -n "$CONFIG_SOUND_PRIME"; then
  AC_DEFINE(CONFIG_SOUND_PRIME_MODULE)
fi
if test -n "$CONFIG_AC97_BUS"; then
  AC_DEFINE(CONFIG_AC97_BUS_MODULE)
fi
])

AC_DEFUN([ALSA_TOPLEVEL_OUTPUT], [
dnl output all subst
AC_SUBST(CONFIG_SOUND)
AC_SUBST(CONFIG_HAS_IOMEM)
AC_SUBST(CONFIG_SOUND_OSS_CORE)
AC_SUBST(CONFIG_SOUND_OSS_CORE_PRECLAIM)
AC_SUBST(CONFIG_M68K)
AC_SUBST(CONFIG_UML)
AC_SUBST(CONFIG_SND)
AC_SUBST(CONFIG_SND_TIMER)
AC_SUBST(CONFIG_SND_PCM)
AC_SUBST(CONFIG_SND_HWDEP)
AC_SUBST(CONFIG_SND_RAWMIDI)
AC_SUBST(CONFIG_SND_COMPRESS_OFFLOAD)
AC_SUBST(CONFIG_SND_JACK)
AC_SUBST(CONFIG_SND_SEQUENCER)
AC_SUBST(CONFIG_SND_SEQ_DUMMY)
AC_SUBST(CONFIG_SND_OSSEMUL)
AC_SUBST(CONFIG_SND_MIXER_OSS)
AC_SUBST(CONFIG_SND_PCM_OSS)
AC_SUBST(CONFIG_SND_PCM_OSS_PLUGINS)
AC_SUBST(CONFIG_SND_SEQUENCER_OSS)
AC_SUBST(CONFIG_SND_HRTIMER)
AC_SUBST(CONFIG_HIGH_RES_TIMERS)
AC_SUBST(CONFIG_SND_SEQ_HRTIMER_DEFAULT)
AC_SUBST(CONFIG_SND_RTCTIMER)
AC_SUBST(CONFIG_RTC)
AC_SUBST(CONFIG_SND_SEQ_RTCTIMER_DEFAULT)
AC_SUBST(CONFIG_SND_DYNAMIC_MINORS)
AC_SUBST(CONFIG_SND_SUPPORT_OLD_API)
AC_SUBST(CONFIG_SND_VERBOSE_PROCFS)
AC_SUBST(CONFIG_PROC_FS)
AC_SUBST(CONFIG_SND_VERBOSE_PRINTK)
AC_SUBST(CONFIG_SND_DEBUG)
AC_SUBST(CONFIG_SND_DEBUG_VERBOSE)
AC_SUBST(CONFIG_SND_PCM_XRUN_DEBUG)
AC_SUBST(CONFIG_SND_VMASTER)
AC_SUBST(CONFIG_SND_KCTL_JACK)
AC_SUBST(CONFIG_SND_DMA_SGBUF)
AC_SUBST(CONFIG_X86)
AC_SUBST(CONFIG_SND_RAWMIDI_SEQ)
AC_SUBST(CONFIG_SND_OPL3_LIB_SEQ)
AC_SUBST(CONFIG_SND_OPL3_LIB)
AC_SUBST(CONFIG_SND_OPL4_LIB_SEQ)
AC_SUBST(CONFIG_SND_OPL4_LIB)
AC_SUBST(CONFIG_SND_SBAWE_SEQ)
AC_SUBST(CONFIG_SND_SBAWE)
AC_SUBST(CONFIG_SND_EMU10K1_SEQ)
AC_SUBST(CONFIG_SND_EMU10K1)
AC_SUBST(CONFIG_SND_BIT32_EMUL)
AC_SUBST(CONFIG_SND_DEBUG_MEMORY)
AC_SUBST(CONFIG_SND_HPET)
AC_SUBST(CONFIG_HPET)
AC_SUBST(CONFIG_BROKEN)
AC_SUBST(CONFIG_SND_MPU401_UART)
AC_SUBST(CONFIG_SND_VX_LIB)
AC_SUBST(CONFIG_SND_AC97_CODEC)
AC_SUBST(CONFIG_SND_DRIVERS)
AC_SUBST(CONFIG_SND_PCSP)
AC_SUBST(CONFIG_PCSPKR_PLATFORM)
AC_SUBST(CONFIG_INPUT)
AC_SUBST(CONFIG_EXPERIMENTAL)
AC_SUBST(CONFIG_SND_DUMMY)
AC_SUBST(CONFIG_SND_ALOOP)
AC_SUBST(CONFIG_SND_VIRMIDI)
AC_SUBST(CONFIG_SND_MTPAV)
AC_SUBST(CONFIG_SND_MTS64)
AC_SUBST(CONFIG_PARPORT)
AC_SUBST(CONFIG_SND_SERIAL_U16550)
AC_SUBST(CONFIG_SND_MPU401)
AC_SUBST(CONFIG_SND_PORTMAN2X4)
AC_SUBST(CONFIG_SND_ML403_AC97CR)
AC_SUBST(CONFIG_XILINX_VIRTEX)
AC_SUBST(CONFIG_SND_AC97_POWER_SAVE)
AC_SUBST(CONFIG_SND_AC97_POWER_SAVE_DEFAULT)
AC_SUBST(CONFIG_SND_SERIALMIDI)
AC_SUBST(CONFIG_SND_LOOPBACK)
AC_SUBST(CONFIG_SND_WSS_LIB)
AC_SUBST(CONFIG_SND_SB_COMMON)
AC_SUBST(CONFIG_SND_SB8_DSP)
AC_SUBST(CONFIG_SND_SB16_DSP)
AC_SUBST(CONFIG_SND_ISA)
AC_SUBST(CONFIG_ISA)
AC_SUBST(CONFIG_ISA_DMA_API)
AC_SUBST(CONFIG_SND_ADLIB)
AC_SUBST(CONFIG_SND_AD1816A)
AC_SUBST(CONFIG_PNP)
AC_SUBST(CONFIG_ISAPNP)
AC_SUBST(CONFIG_SND_AD1848)
AC_SUBST(CONFIG_SND_ALS100)
AC_SUBST(CONFIG_SND_AZT1605)
AC_SUBST(CONFIG_SND_AZT2316)
AC_SUBST(CONFIG_SND_AZT2320)
AC_SUBST(CONFIG_SND_CMI8330)
AC_SUBST(CONFIG_SND_CS4231)
AC_SUBST(CONFIG_SND_CS4236)
AC_SUBST(CONFIG_SND_ES1688)
AC_SUBST(CONFIG_SND_ES18XX)
AC_SUBST(CONFIG_SND_SC6000)
AC_SUBST(CONFIG_HAS_IOPORT)
AC_SUBST(CONFIG_SND_GUSCLASSIC)
AC_SUBST(CONFIG_SND_GUSEXTREME)
AC_SUBST(CONFIG_SND_GUSMAX)
AC_SUBST(CONFIG_SND_INTERWAVE)
AC_SUBST(CONFIG_SND_INTERWAVE_STB)
AC_SUBST(CONFIG_SND_JAZZ16)
AC_SUBST(CONFIG_SND_OPL3SA2)
AC_SUBST(CONFIG_SND_OPTI92X_AD1848)
AC_SUBST(CONFIG_SND_OPTI92X_CS4231)
AC_SUBST(CONFIG_SND_OPTI93X)
AC_SUBST(CONFIG_SND_MIRO)
AC_SUBST(CONFIG_SND_SB8)
AC_SUBST(CONFIG_SND_SB16)
AC_SUBST(CONFIG_SND_SB16_CSP)
AC_SUBST(CONFIG_PPC)
AC_SUBST(CONFIG_FW_LOADER)
AC_SUBST(CONFIG_SND_SSCAPE)
AC_SUBST(CONFIG_SND_WAVEFRONT)
AC_SUBST(CONFIG_SND_MSND_PINNACLE)
AC_SUBST(CONFIG_SND_MSND_CLASSIC)
AC_SUBST(CONFIG_SND_PC98_CS4232)
AC_SUBST(CONFIG_X86_PC9800)
AC_SUBST(CONFIG_SND_TEA575X)
AC_SUBST(CONFIG_SND_FM801_TEA575X_BOOL)
AC_SUBST(CONFIG_SND_ES1968_RADIO)
AC_SUBST(CONFIG_RADIO_SF16FMR2)
AC_SUBST(CONFIG_SND_FM801)
AC_SUBST(CONFIG_SND_ES1968)
AC_SUBST(CONFIG_SND_PCI)
AC_SUBST(CONFIG_PCI)
AC_SUBST(CONFIG_SND_AD1889)
AC_SUBST(CONFIG_SND_ALS300)
AC_SUBST(CONFIG_SND_ALS4000)
AC_SUBST(CONFIG_SND_ALI5451)
AC_SUBST(CONFIG_SND_ASIHPI)
AC_SUBST(CONFIG_SND_ATIIXP)
AC_SUBST(CONFIG_SND_ATIIXP_MODEM)
AC_SUBST(CONFIG_SND_AU8810)
AC_SUBST(CONFIG_SND_AU8820)
AC_SUBST(CONFIG_SND_AU8830)
AC_SUBST(CONFIG_SND_AW2)
AC_SUBST(CONFIG_SND_AZT3328)
AC_SUBST(CONFIG_SND_BT87X)
AC_SUBST(CONFIG_SND_BT87X_OVERCLOCK)
AC_SUBST(CONFIG_SND_CA0106)
AC_SUBST(CONFIG_SND_CMIPCI)
AC_SUBST(CONFIG_SND_OXYGEN_LIB)
AC_SUBST(CONFIG_SND_OXYGEN)
AC_SUBST(CONFIG_SND_CS4281)
AC_SUBST(CONFIG_SND_CS46XX)
AC_SUBST(CONFIG_SND_CS46XX_NEW_DSP)
AC_SUBST(CONFIG_SND_CS5530)
AC_SUBST(CONFIG_SND_CS5535AUDIO)
AC_SUBST(CONFIG_SND_CTXFI)
AC_SUBST(CONFIG_SND_DARLA20)
AC_SUBST(CONFIG_SND_GINA20)
AC_SUBST(CONFIG_SND_LAYLA20)
AC_SUBST(CONFIG_SND_DARLA24)
AC_SUBST(CONFIG_SND_GINA24)
AC_SUBST(CONFIG_SND_LAYLA24)
AC_SUBST(CONFIG_SND_MONA)
AC_SUBST(CONFIG_SND_MIA)
AC_SUBST(CONFIG_SND_ECHO3G)
AC_SUBST(CONFIG_SND_INDIGO)
AC_SUBST(CONFIG_SND_INDIGOIO)
AC_SUBST(CONFIG_SND_INDIGODJ)
AC_SUBST(CONFIG_SND_INDIGOIOX)
AC_SUBST(CONFIG_SND_INDIGODJX)
AC_SUBST(CONFIG_SND_EMU10K1X)
AC_SUBST(CONFIG_SND_ENS1370)
AC_SUBST(CONFIG_SND_ENS1371)
AC_SUBST(CONFIG_SND_ES1938)
AC_SUBST(CONFIG_SND_ES1968_INPUT)
AC_SUBST(CONFIG_VIDEO_V4L2)
AC_SUBST(CONFIG_SND_HDA_INTEL)
AC_SUBST(CONFIG_SND_HDA_PREALLOC_SIZE)
AC_SUBST(CONFIG_SND_HDA_HWDEP)
AC_SUBST(CONFIG_SND_HDA_RECONFIG)
AC_SUBST(CONFIG_SND_HDA_INPUT_BEEP)
AC_SUBST(CONFIG_SND_HDA_INPUT_BEEP_MODE)
AC_SUBST(CONFIG_SND_HDA_INPUT_JACK)
AC_SUBST(CONFIG_SND_HDA_PATCH_LOADER)
AC_SUBST(CONFIG_SND_HDA_CODEC_REALTEK)
AC_SUBST(CONFIG_SND_HDA_ENABLE_REALTEK_QUIRKS)
AC_SUBST(CONFIG_SND_HDA_CODEC_ANALOG)
AC_SUBST(CONFIG_SND_HDA_CODEC_SIGMATEL)
AC_SUBST(CONFIG_SND_HDA_CODEC_VIA)
AC_SUBST(CONFIG_SND_HDA_CODEC_HDMI)
AC_SUBST(CONFIG_SND_HDA_CODEC_CIRRUS)
AC_SUBST(CONFIG_SND_HDA_CODEC_CONEXANT)
AC_SUBST(CONFIG_SND_HDA_CODEC_CA0110)
AC_SUBST(CONFIG_SND_HDA_CODEC_CA0132)
AC_SUBST(CONFIG_SND_HDA_CODEC_CMEDIA)
AC_SUBST(CONFIG_SND_HDA_CODEC_SI3054)
AC_SUBST(CONFIG_SND_HDA_GENERIC)
AC_SUBST(CONFIG_SND_HDA_POWER_SAVE)
AC_SUBST(CONFIG_PM)
AC_SUBST(CONFIG_SND_HDA_POWER_SAVE_DEFAULT)
AC_SUBST(CONFIG_SND_HDSP)
AC_SUBST(CONFIG_SND_HDSPM)
AC_SUBST(CONFIG_SND_ICE1712)
AC_SUBST(CONFIG_BITREVERSE)
AC_SUBST(CONFIG_SND_ICE1724)
AC_SUBST(CONFIG_SND_INTEL8X0)
AC_SUBST(CONFIG_SND_INTEL8X0M)
AC_SUBST(CONFIG_SND_KORG1212)
AC_SUBST(CONFIG_SND_LOLA)
AC_SUBST(CONFIG_SND_LX6464ES)
AC_SUBST(CONFIG_SND_MAESTRO3)
AC_SUBST(CONFIG_SND_MAESTRO3_INPUT)
AC_SUBST(CONFIG_SND_MIXART)
AC_SUBST(CONFIG_SND_NM256)
AC_SUBST(CONFIG_SND_PCXHR)
AC_SUBST(CONFIG_SND_RIPTIDE)
AC_SUBST(CONFIG_SND_RME32)
AC_SUBST(CONFIG_SND_RME96)
AC_SUBST(CONFIG_SND_RME9652)
AC_SUBST(CONFIG_SND_SIS7019)
AC_SUBST(CONFIG_X86_64)
AC_SUBST(CONFIG_SND_SONICVIBES)
AC_SUBST(CONFIG_SND_TRIDENT)
AC_SUBST(CONFIG_SND_VIA82XX)
AC_SUBST(CONFIG_SND_VIA82XX_MODEM)
AC_SUBST(CONFIG_SND_VIRTUOSO)
AC_SUBST(CONFIG_SND_VX222)
AC_SUBST(CONFIG_SND_YMFPCI)
AC_SUBST(CONFIG_SND_PDPLUS)
AC_SUBST(CONFIG_SND_PPC)
AC_SUBST(CONFIG_SND_POWERMAC)
AC_SUBST(CONFIG_I2C)
AC_SUBST(CONFIG_PPC_PMAC)
AC_SUBST(CONFIG_SND_POWERMAC_AUTO_DRC)
AC_SUBST(CONFIG_SND_PS3)
AC_SUBST(CONFIG_PS3_PS3AV)
AC_SUBST(CONFIG_SND_PS3_DEFAULT_START_DELAY)
AC_SUBST(CONFIG_SND_AOA)
AC_SUBST(CONFIG_SND_AOA_FABRIC_LAYOUT)
AC_SUBST(CONFIG_SND_AOA_ONYX)
AC_SUBST(CONFIG_I2C_POWERMAC)
AC_SUBST(CONFIG_SND_AOA_TAS)
AC_SUBST(CONFIG_SND_AOA_TOONIE)
AC_SUBST(CONFIG_SND_AOA_SOUNDBUS)
AC_SUBST(CONFIG_SND_AOA_SOUNDBUS_I2S)
AC_SUBST(CONFIG_SND_ARM)
AC_SUBST(CONFIG_ARM)
AC_SUBST(CONFIG_SND_ARMAACI)
AC_SUBST(CONFIG_ARM_AMBA)
AC_SUBST(CONFIG_SND_PXA2XX_PCM)
AC_SUBST(CONFIG_SND_PXA2XX_LIB)
AC_SUBST(CONFIG_SND_PXA2XX_LIB_AC97)
AC_SUBST(CONFIG_SND_PXA2XX_AC97)
AC_SUBST(CONFIG_ARCH_PXA)
AC_SUBST(CONFIG_SND_SA11XX_UDA1341)
AC_SUBST(CONFIG_ARCH_SA1100)
AC_SUBST(CONFIG_L3)
AC_SUBST(CONFIG_SND_S3C2410)
AC_SUBST(CONFIG_ARCH_S3C2410)
AC_SUBST(CONFIG_I2C_SENSOR)
AC_SUBST(CONFIG_SND_PXA2XX_I2SOUND)
AC_SUBST(CONFIG_AVR32)
AC_SUBST(CONFIG_ARCH_AT91)
AC_SUBST(CONFIG_SND_ATMEL_ABDAC)
AC_SUBST(CONFIG_DW_DMAC)
AC_SUBST(CONFIG_SND_ATMEL_AC97C)
AC_SUBST(CONFIG_SND_SPI)
AC_SUBST(CONFIG_SPI)
AC_SUBST(CONFIG_SND_AT73C213)
AC_SUBST(CONFIG_ATMEL_SSC)
AC_SUBST(CONFIG_SND_AT73C213_TARGET_BITRATE)
AC_SUBST(CONFIG_SND_MIPS)
AC_SUBST(CONFIG_MIPS)
AC_SUBST(CONFIG_SND_SGI_O2)
AC_SUBST(CONFIG_SGI_IP32)
AC_SUBST(CONFIG_SND_SGI_HAL2)
AC_SUBST(CONFIG_SGI_HAS_HAL2)
AC_SUBST(CONFIG_SND_AU1X00)
AC_SUBST(CONFIG_MIPS_ALCHEMY)
AC_SUBST(CONFIG_SND_SUPERH)
AC_SUBST(CONFIG_SUPERH)
AC_SUBST(CONFIG_SND_AICA)
AC_SUBST(CONFIG_SH_DREAMCAST)
AC_SUBST(CONFIG_G2_DMA)
AC_SUBST(CONFIG_SND_SH_DAC_AUDIO)
AC_SUBST(CONFIG_CPU_SH3)
AC_SUBST(CONFIG_SND_USB)
AC_SUBST(CONFIG_USB)
AC_SUBST(CONFIG_SND_USB_AUDIO)
AC_SUBST(CONFIG_SND_USB_UA101)
AC_SUBST(CONFIG_SND_USB_USX2Y)
AC_SUBST(CONFIG_ALPHA)
AC_SUBST(CONFIG_SND_USB_CAIAQ)
AC_SUBST(CONFIG_SND_USB_CAIAQ_INPUT)
AC_SUBST(CONFIG_SND_USB_US122L)
AC_SUBST(CONFIG_SND_USB_6FIRE)
AC_SUBST(CONFIG_SND_FIREWIRE)
AC_SUBST(CONFIG_FIREWIRE)
AC_SUBST(CONFIG_SND_FIREWIRE_LIB)
AC_SUBST(CONFIG_SND_FIREWIRE_SPEAKERS)
AC_SUBST(CONFIG_SND_ISIGHT)
AC_SUBST(CONFIG_SND_PCMCIA)
AC_SUBST(CONFIG_PCMCIA)
AC_SUBST(CONFIG_SND_VXPOCKET)
AC_SUBST(CONFIG_SND_PDAUDIOCF)
AC_SUBST(CONFIG_SND_SPARC)
AC_SUBST(CONFIG_SPARC)
AC_SUBST(CONFIG_SND_SUN_AMD7930)
AC_SUBST(CONFIG_SBUS)
AC_SUBST(CONFIG_SND_SUN_CS4231)
AC_SUBST(CONFIG_SND_SUN_DBRI)
AC_SUBST(CONFIG_SND_GSC)
AC_SUBST(CONFIG_GSC)
AC_SUBST(CONFIG_SND_HARMONY)
AC_SUBST(CONFIG_SND_SOC)
AC_SUBST(CONFIG_REGMAP_I2C)
AC_SUBST(CONFIG_REGMAP_SPI)
AC_SUBST(CONFIG_SND_SOC_AC97_BUS)
AC_SUBST(CONFIG_SND_ATMEL_SOC)
AC_SUBST(CONFIG_SND_ATMEL_SOC_SSC)
AC_SUBST(CONFIG_SND_AT91_SOC_SAM9G20_WM8731)
AC_SUBST(CONFIG_ARCH_AT91SAM9G20)
AC_SUBST(CONFIG_AT91_PROGRAMMABLE_CLOCKS)
AC_SUBST(CONFIG_SND_AT91_SOC_AFEB9260)
AC_SUBST(CONFIG_MACH_AFEB9260)
AC_SUBST(CONFIG_SND_SOC_AU1XPSC)
AC_SUBST(CONFIG_SND_SOC_AU1XPSC_I2S)
AC_SUBST(CONFIG_SND_SOC_AU1XPSC_AC97)
AC_SUBST(CONFIG_SND_SOC_AU1XAUDIO)
AC_SUBST(CONFIG_SND_SOC_AU1XAC97C)
AC_SUBST(CONFIG_SND_SOC_AU1XI2SC)
AC_SUBST(CONFIG_SND_SOC_DB1000)
AC_SUBST(CONFIG_SND_SOC_DB1200)
AC_SUBST(CONFIG_SND_BF5XX_I2S)
AC_SUBST(CONFIG_BLACKFIN)
AC_SUBST(CONFIG_SND_BF5XX_SOC_SSM2602)
AC_SUBST(CONFIG_SPI_MASTER)
AC_SUBST(CONFIG_SND_SOC_BFIN_EVAL_ADAU1701)
AC_SUBST(CONFIG_SND_SOC_BFIN_EVAL_ADAU1373)
AC_SUBST(CONFIG_SND_SOC_BFIN_EVAL_ADAV80X)
AC_SUBST(CONFIG_SND_BF5XX_SOC_AD73311)
AC_SUBST(CONFIG_SND_BFIN_AD73311_SE)
AC_SUBST(CONFIG_SND_BF5XX_TDM)
AC_SUBST(CONFIG_SND_BF5XX_SOC_AD1836)
AC_SUBST(CONFIG_SND_BF5XX_SOC_AD193X)
AC_SUBST(CONFIG_SND_BF5XX_AC97)
AC_SUBST(CONFIG_SND_BF5XX_MMAP_SUPPORT)
AC_SUBST(CONFIG_SND_BF5XX_MULTICHAN_SUPPORT)
AC_SUBST(CONFIG_SND_BF5XX_HAVE_COLD_RESET)
AC_SUBST(CONFIG_SND_BF5XX_RESET_GPIO_NUM)
AC_SUBST(CONFIG_SND_BF5XX_SOC_AD1980)
AC_SUBST(CONFIG_SND_BF5XX_SOC_SPORT)
AC_SUBST(CONFIG_SND_BF5XX_SOC_I2S)
AC_SUBST(CONFIG_SND_BF5XX_SOC_TDM)
AC_SUBST(CONFIG_SND_BF5XX_SOC_AC97)
AC_SUBST(CONFIG_SND_BF5XX_SPORT_NUM)
AC_SUBST(CONFIG_SND_DAVINCI_SOC)
AC_SUBST(CONFIG_ARCH_DAVINCI)
AC_SUBST(CONFIG_SND_DAVINCI_SOC_I2S)
AC_SUBST(CONFIG_SND_DAVINCI_SOC_MCASP)
AC_SUBST(CONFIG_SND_DAVINCI_SOC_VCIF)
AC_SUBST(CONFIG_SND_DAVINCI_SOC_EVM)
AC_SUBST(CONFIG_MACH_DAVINCI_EVM)
AC_SUBST(CONFIG_MACH_DAVINCI_DM355_EVM)
AC_SUBST(CONFIG_MACH_DAVINCI_DM365_EVM)
AC_SUBST(CONFIG_SND_DM365_AIC3X_CODEC)
AC_SUBST(CONFIG_SND_DM365_VOICE_CODEC)
AC_SUBST(CONFIG_MFD_DAVINCI_VOICECODEC)
AC_SUBST(CONFIG_SND_DM6467_SOC_EVM)
AC_SUBST(CONFIG_MACH_DAVINCI_DM6467_EVM)
AC_SUBST(CONFIG_SND_DAVINCI_SOC_SFFSDR)
AC_SUBST(CONFIG_MACH_SFFSDR)
AC_SUBST(CONFIG_SFFSDR_FPGA)
AC_SUBST(CONFIG_SND_DA830_SOC_EVM)
AC_SUBST(CONFIG_MACH_DAVINCI_DA830_EVM)
AC_SUBST(CONFIG_SND_DA850_SOC_EVM)
AC_SUBST(CONFIG_MACH_DAVINCI_DA850_EVM)
AC_SUBST(CONFIG_SND_EP93XX_SOC)
AC_SUBST(CONFIG_ARCH_EP93XX)
AC_SUBST(CONFIG_SND_EP93XX_SOC_I2S)
AC_SUBST(CONFIG_SND_EP93XX_SOC_AC97)
AC_SUBST(CONFIG_SND_EP93XX_SOC_SNAPPERCL15)
AC_SUBST(CONFIG_MACH_SNAPPER_CL15)
AC_SUBST(CONFIG_SND_EP93XX_SOC_SIMONE)
AC_SUBST(CONFIG_MACH_SIM_ONE)
AC_SUBST(CONFIG_SND_EP93XX_SOC_EDB93XX)
AC_SUBST(CONFIG_MACH_EDB9301)
AC_SUBST(CONFIG_MACH_EDB9302)
AC_SUBST(CONFIG_MACH_EDB9302A)
AC_SUBST(CONFIG_MACH_EDB9307A)
AC_SUBST(CONFIG_MACH_EDB9315A)
AC_SUBST(CONFIG_SND_MPC52XX_DMA)
AC_SUBST(CONFIG_SND_SOC_POWERPC_SSI)
AC_SUBST(CONFIG_FSL_SOC)
AC_SUBST(CONFIG_SND_SOC_MPC8610_HPCD)
AC_SUBST(CONFIG_MPC8610_HPCD)
AC_SUBST(CONFIG_SND_SOC_P1022_DS)
AC_SUBST(CONFIG_P1022_DS)
AC_SUBST(CONFIG_SND_SOC_MPC5200_I2S)
AC_SUBST(CONFIG_PPC_MPC52XX)
AC_SUBST(CONFIG_PPC_BESTCOMM)
AC_SUBST(CONFIG_PPC_BESTCOMM_GEN_BD)
AC_SUBST(CONFIG_SND_SOC_MPC5200_AC97)
AC_SUBST(CONFIG_SND_MPC52XX_SOC_PCM030)
AC_SUBST(CONFIG_PPC_MPC5200_SIMPLE)
AC_SUBST(CONFIG_SND_MPC52XX_SOC_EFIKA)
AC_SUBST(CONFIG_PPC_EFIKA)
AC_SUBST(CONFIG_SND_IMX_SOC)
AC_SUBST(CONFIG_ARCH_MXC)
AC_SUBST(CONFIG_FIQ)
AC_SUBST(CONFIG_SND_MXC_SOC_FIQ)
AC_SUBST(CONFIG_SND_MXC_SOC_MX2)
AC_SUBST(CONFIG_SND_MXC_SOC_WM1133_EV1)
AC_SUBST(CONFIG_MACH_MX31ADS_WM1133_EV1)
AC_SUBST(CONFIG_SND_SOC_MX27VIS_AIC32X4)
AC_SUBST(CONFIG_MACH_IMX27_VISSTRIM_M10)
AC_SUBST(CONFIG_SND_SOC_PHYCORE_AC97)
AC_SUBST(CONFIG_MACH_PCM043)
AC_SUBST(CONFIG_MACH_PCA100)
AC_SUBST(CONFIG_SND_SOC_EUKREA_TLV320)
AC_SUBST(CONFIG_MACH_EUKREA_MBIMX27_BASEBOARD)
AC_SUBST(CONFIG_MACH_EUKREA_MBIMXSD25_BASEBOARD)
AC_SUBST(CONFIG_MACH_EUKREA_MBIMXSD35_BASEBOARD)
AC_SUBST(CONFIG_MACH_EUKREA_MBIMXSD51_BASEBOARD)
AC_SUBST(CONFIG_SND_JZ4740_SOC)
AC_SUBST(CONFIG_MACH_JZ4740)
AC_SUBST(CONFIG_SND_JZ4740_SOC_I2S)
AC_SUBST(CONFIG_SND_JZ4740_SOC_QI_LB60)
AC_SUBST(CONFIG_JZ4740_QI_LB60)
AC_SUBST(CONFIG_SND_SOC_NUC900)
AC_SUBST(CONFIG_ARCH_W90X900)
AC_SUBST(CONFIG_SND_SOC_NUC900_AC97)
AC_SUBST(CONFIG_SND_SOC_NUC900EVB)
AC_SUBST(CONFIG_SND_OMAP_SOC)
AC_SUBST(CONFIG_ARCH_OMAP)
AC_SUBST(CONFIG_SND_OMAP_SOC_DMIC)
AC_SUBST(CONFIG_SND_OMAP_SOC_MCBSP)
AC_SUBST(CONFIG_OMAP_MCBSP)
AC_SUBST(CONFIG_SND_OMAP_SOC_MCPDM)
AC_SUBST(CONFIG_SND_OMAP_SOC_HDMI)
AC_SUBST(CONFIG_SND_OMAP_SOC_N810)
AC_SUBST(CONFIG_MACH_NOKIA_N810)
AC_SUBST(CONFIG_OMAP_MUX)
AC_SUBST(CONFIG_SND_OMAP_SOC_RX51)
AC_SUBST(CONFIG_MACH_NOKIA_RX51)
AC_SUBST(CONFIG_SND_OMAP_SOC_AMS_DELTA)
AC_SUBST(CONFIG_MACH_AMS_DELTA)
AC_SUBST(CONFIG_SND_OMAP_SOC_OSK5912)
AC_SUBST(CONFIG_MACH_OMAP_OSK)
AC_SUBST(CONFIG_SND_OMAP_SOC_OVERO)
AC_SUBST(CONFIG_TWL4030_CORE)
AC_SUBST(CONFIG_MACH_OVERO)
AC_SUBST(CONFIG_MACH_CM_T35)
AC_SUBST(CONFIG_SND_OMAP_SOC_OMAP3EVM)
AC_SUBST(CONFIG_MACH_OMAP3EVM)
AC_SUBST(CONFIG_SND_OMAP_SOC_AM3517EVM)
AC_SUBST(CONFIG_MACH_OMAP3517EVM)
AC_SUBST(CONFIG_SND_OMAP_SOC_SDP3430)
AC_SUBST(CONFIG_MACH_OMAP_3430SDP)
AC_SUBST(CONFIG_SND_OMAP_SOC_SDP4430)
AC_SUBST(CONFIG_MACH_OMAP_4430SDP)
AC_SUBST(CONFIG_SND_OMAP_SOC_OMAP4_HDMI)
AC_SUBST(CONFIG_OMAP4_DSS_HDMI)
AC_SUBST(CONFIG_OMAP2_DSS)
AC_SUBST(CONFIG_ARCH_OMAP4)
AC_SUBST(CONFIG_SND_OMAP_SOC_OMAP3_PANDORA)
AC_SUBST(CONFIG_MACH_OMAP3_PANDORA)
AC_SUBST(CONFIG_SND_OMAP_SOC_OMAP3_BEAGLE)
AC_SUBST(CONFIG_MACH_OMAP3_BEAGLE)
AC_SUBST(CONFIG_MACH_DEVKIT8000)
AC_SUBST(CONFIG_SND_OMAP_SOC_ZOOM2)
AC_SUBST(CONFIG_MACH_OMAP_ZOOM2)
AC_SUBST(CONFIG_SND_OMAP_SOC_IGEP0020)
AC_SUBST(CONFIG_MACH_IGEP0020)
AC_SUBST(CONFIG_SND_KIRKWOOD_SOC)
AC_SUBST(CONFIG_ARCH_KIRKWOOD)
AC_SUBST(CONFIG_SND_KIRKWOOD_SOC_I2S)
AC_SUBST(CONFIG_SND_KIRKWOOD_SOC_OPENRD)
AC_SUBST(CONFIG_MACH_OPENRD_CLIENT)
AC_SUBST(CONFIG_MACH_OPENRD_ULTIMATE)
AC_SUBST(CONFIG_SND_KIRKWOOD_SOC_T5325)
AC_SUBST(CONFIG_MACH_T5325)
AC_SUBST(CONFIG_SND_MFLD_MACHINE)
AC_SUBST(CONFIG_INTEL_SCU_IPC)
AC_SUBST(CONFIG_SND_SST_PLATFORM)
AC_SUBST(CONFIG_SND_MXS_SOC)
AC_SUBST(CONFIG_ARCH_MXS)
AC_SUBST(CONFIG_SND_SOC_MXS_SGTL5000)
AC_SUBST(CONFIG_SND_PXA2XX_SOC)
AC_SUBST(CONFIG_SND_PXA2XX_SOC_AC97)
AC_SUBST(CONFIG_SND_PXA2XX_SOC_I2S)
AC_SUBST(CONFIG_SND_PXA_SOC_SSP)
AC_SUBST(CONFIG_PXA_SSP)
AC_SUBST(CONFIG_SND_PXA2XX_SOC_CORGI)
AC_SUBST(CONFIG_PXA_SHARP_C7XX)
AC_SUBST(CONFIG_SND_PXA2XX_SOC_SPITZ)
AC_SUBST(CONFIG_PXA_SHARP_CXX00)
AC_SUBST(CONFIG_SND_PXA2XX_SOC_Z2)
AC_SUBST(CONFIG_MACH_ZIPIT2)
AC_SUBST(CONFIG_SND_PXA2XX_SOC_POODLE)
AC_SUBST(CONFIG_MACH_POODLE)
AC_SUBST(CONFIG_SND_PXA2XX_SOC_TOSA)
AC_SUBST(CONFIG_MACH_TOSA)
AC_SUBST(CONFIG_MFD_TC6393XB)
AC_SUBST(CONFIG_SND_PXA2XX_SOC_E740)
AC_SUBST(CONFIG_MACH_E740)
AC_SUBST(CONFIG_SND_PXA2XX_SOC_E750)
AC_SUBST(CONFIG_MACH_E750)
AC_SUBST(CONFIG_SND_PXA2XX_SOC_E800)
AC_SUBST(CONFIG_MACH_E800)
AC_SUBST(CONFIG_SND_PXA2XX_SOC_EM_X270)
AC_SUBST(CONFIG_MACH_EM_X270)
AC_SUBST(CONFIG_MACH_EXEDA)
AC_SUBST(CONFIG_MACH_CM_X300)
AC_SUBST(CONFIG_SND_PXA2XX_SOC_PALM27X)
AC_SUBST(CONFIG_MACH_PALMLD)
AC_SUBST(CONFIG_MACH_PALMTX)
AC_SUBST(CONFIG_MACH_PALMT5)
AC_SUBST(CONFIG_MACH_PALMTE2)
AC_SUBST(CONFIG_SND_SOC_SAARB)
AC_SUBST(CONFIG_MACH_SAARB)
AC_SUBST(CONFIG_MFD_88PM860X)
AC_SUBST(CONFIG_SND_SOC_TAVOREVB3)
AC_SUBST(CONFIG_MACH_TAVOREVB3)
AC_SUBST(CONFIG_SND_SOC_ZYLONITE)
AC_SUBST(CONFIG_MACH_ZYLONITE)
AC_SUBST(CONFIG_SND_SOC_RAUMFELD)
AC_SUBST(CONFIG_MACH_RAUMFELD_SPEAKER)
AC_SUBST(CONFIG_MACH_RAUMFELD_CONNECTOR)
AC_SUBST(CONFIG_SND_PXA2XX_SOC_HX4700)
AC_SUBST(CONFIG_MACH_H4700)
AC_SUBST(CONFIG_SND_PXA2XX_SOC_MAGICIAN)
AC_SUBST(CONFIG_MACH_MAGICIAN)
AC_SUBST(CONFIG_SND_PXA2XX_SOC_MIOA701)
AC_SUBST(CONFIG_MACH_MIOA701)
AC_SUBST(CONFIG_SND_PXA2XX_SOC_IMOTE2)
AC_SUBST(CONFIG_SND_SOC_SAMSUNG)
AC_SUBST(CONFIG_ARCH_S3C64XX)
AC_SUBST(CONFIG_ARCH_S5PC100)
AC_SUBST(CONFIG_ARCH_S5PV210)
AC_SUBST(CONFIG_ARCH_S5P64X0)
AC_SUBST(CONFIG_ARCH_EXYNOS4)
AC_SUBST(CONFIG_S3C64XX_DMA)
AC_SUBST(CONFIG_S3C2410_DMA)
AC_SUBST(CONFIG_SND_S3C24XX_I2S)
AC_SUBST(CONFIG_SND_S3C_I2SV2_SOC)
AC_SUBST(CONFIG_SND_S3C2412_SOC_I2S)
AC_SUBST(CONFIG_SND_SAMSUNG_PCM)
AC_SUBST(CONFIG_SND_SAMSUNG_AC97)
AC_SUBST(CONFIG_SND_SAMSUNG_SPDIF)
AC_SUBST(CONFIG_SND_SAMSUNG_I2S)
AC_SUBST(CONFIG_SND_SOC_SAMSUNG_NEO1973_WM8753)
AC_SUBST(CONFIG_MACH_NEO1973_GTA01)
AC_SUBST(CONFIG_MACH_NEO1973_GTA02)
AC_SUBST(CONFIG_SND_SOC_SAMSUNG_JIVE_WM8750)
AC_SUBST(CONFIG_MACH_JIVE)
AC_SUBST(CONFIG_SND_SOC_SAMSUNG_SMDK_WM8580)
AC_SUBST(CONFIG_MACH_SMDK6410)
AC_SUBST(CONFIG_MACH_SMDKC100)
AC_SUBST(CONFIG_MACH_SMDK6440)
AC_SUBST(CONFIG_MACH_SMDK6450)
AC_SUBST(CONFIG_MACH_SMDKV210)
AC_SUBST(CONFIG_MACH_SMDKC110)
AC_SUBST(CONFIG_SND_SOC_SAMSUNG_SMDK_WM8994)
AC_SUBST(CONFIG_MACH_SMDKV310)
AC_SUBST(CONFIG_MACH_SMDKC210)
AC_SUBST(CONFIG_MACH_SMDK4212)
AC_SUBST(CONFIG_GENERIC_HARDIRQS)
AC_SUBST(CONFIG_MFD_WM8994)
AC_SUBST(CONFIG_SND_SOC_SAMSUNG_SMDK2443_WM9710)
AC_SUBST(CONFIG_MACH_SMDK2443)
AC_SUBST(CONFIG_SND_SOC_SAMSUNG_LN2440SBC_ALC650)
AC_SUBST(CONFIG_SND_SOC_SAMSUNG_S3C24XX_UDA134X)
AC_SUBST(CONFIG_SND_SOC_SAMSUNG_SIMTEC)
AC_SUBST(CONFIG_SND_SOC_SAMSUNG_SIMTEC_TLV320AIC23)
AC_SUBST(CONFIG_SND_SOC_SAMSUNG_SIMTEC_HERMES)
AC_SUBST(CONFIG_SND_SOC_SAMSUNG_H1940_UDA1380)
AC_SUBST(CONFIG_ARCH_H1940)
AC_SUBST(CONFIG_SND_SOC_SAMSUNG_RX1950_UDA1380)
AC_SUBST(CONFIG_MACH_RX1950)
AC_SUBST(CONFIG_SND_SOC_SAMSUNG_SMDK_WM9713)
AC_SUBST(CONFIG_SND_SOC_SMARTQ)
AC_SUBST(CONFIG_MACH_SMARTQ)
AC_SUBST(CONFIG_SND_SOC_GONI_AQUILA_WM8994)
AC_SUBST(CONFIG_MACH_GONI)
AC_SUBST(CONFIG_MACH_AQUILA)
AC_SUBST(CONFIG_SND_SOC_SAMSUNG_SMDK_SPDIF)
AC_SUBST(CONFIG_SND_SOC_SMDK_WM8580_PCM)
AC_SUBST(CONFIG_SND_SOC_SMDK_WM8994_PCM)
AC_SUBST(CONFIG_SND_SOC_SPEYSIDE)
AC_SUBST(CONFIG_MACH_WLF_CRAGG_6410)
AC_SUBST(CONFIG_SND_SOC_TOBERMORY)
AC_SUBST(CONFIG_SND_SOC_LOWLAND)
AC_SUBST(CONFIG_SND_SOC_LITTLEMILL)
AC_SUBST(CONFIG_SND_S6000_SOC)
AC_SUBST(CONFIG_XTENSA_VARIANT_S6000)
AC_SUBST(CONFIG_SND_S6000_SOC_I2S)
AC_SUBST(CONFIG_SND_S6000_SOC_S6IPCAM)
AC_SUBST(CONFIG_XTENSA_PLATFORM_S6105)
AC_SUBST(CONFIG_ARCH_SHMOBILE)
AC_SUBST(CONFIG_SND_SOC_PCM_SH7760)
AC_SUBST(CONFIG_CPU_SUBTYPE_SH7760)
AC_SUBST(CONFIG_SH_DMABRG)
AC_SUBST(CONFIG_SND_SOC_SH4_HAC)
AC_SUBST(CONFIG_SND_SOC_SH4_SSI)
AC_SUBST(CONFIG_SND_SOC_SH4_FSI)
AC_SUBST(CONFIG_SND_SOC_SH4_SIU)
AC_SUBST(CONFIG_HAVE_CLK)
AC_SUBST(CONFIG_DMA_ENGINE)
AC_SUBST(CONFIG_DMADEVICES)
AC_SUBST(CONFIG_SH_DMAE)
AC_SUBST(CONFIG_SND_SH7760_AC97)
AC_SUBST(CONFIG_SND_FSI_AK4642)
AC_SUBST(CONFIG_SND_FSI_DA7210)
AC_SUBST(CONFIG_SND_FSI_HDMI)
AC_SUBST(CONFIG_FB_SH_MOBILE_HDMI)
AC_SUBST(CONFIG_SND_SIU_MIGOR)
AC_SUBST(CONFIG_SH_MIGOR)
AC_SUBST(CONFIG_SND_SOC_TEGRA)
AC_SUBST(CONFIG_ARCH_TEGRA)
AC_SUBST(CONFIG_TEGRA_SYSTEM_DMA)
AC_SUBST(CONFIG_SND_SOC_TEGRA_I2S)
AC_SUBST(CONFIG_SND_SOC_TEGRA_SPDIF)
AC_SUBST(CONFIG_MACH_HAS_SND_SOC_TEGRA_WM8903)
AC_SUBST(CONFIG_SND_SOC_TEGRA_WM8903)
AC_SUBST(CONFIG_SND_SOC_TEGRA_TRIMSLICE)
AC_SUBST(CONFIG_MACH_TRIMSLICE)
AC_SUBST(CONFIG_SND_SOC_TEGRA_ALC5632)
AC_SUBST(CONFIG_SND_SOC_TXX9ACLC)
AC_SUBST(CONFIG_HAS_TXX9_ACLC)
AC_SUBST(CONFIG_TXX9_DMAC)
AC_SUBST(CONFIG_SND_SOC_TXX9ACLC_AC97)
AC_SUBST(CONFIG_SND_SOC_TXX9ACLC_GENERIC)
AC_SUBST(CONFIG_SND_SOC_I2C_AND_SPI)
AC_SUBST(CONFIG_SND_SOC_ALL_CODECS)
AC_SUBST(CONFIG_SND_SOC_88PM860X)
AC_SUBST(CONFIG_SND_SOC_WM_HUBS)
AC_SUBST(CONFIG_SND_SOC_AC97_CODEC)
AC_SUBST(CONFIG_SND_SOC_AD1836)
AC_SUBST(CONFIG_SND_SOC_AD193X)
AC_SUBST(CONFIG_SND_SOC_AD1980)
AC_SUBST(CONFIG_SND_SOC_AD73311)
AC_SUBST(CONFIG_SND_SOC_ADAU1701)
AC_SUBST(CONFIG_SND_SOC_ADAU1373)
AC_SUBST(CONFIG_SND_SOC_ADAV80X)
AC_SUBST(CONFIG_SND_SOC_ADS117X)
AC_SUBST(CONFIG_SND_SOC_AK4104)
AC_SUBST(CONFIG_SND_SOC_AK4535)
AC_SUBST(CONFIG_SND_SOC_AK4641)
AC_SUBST(CONFIG_SND_SOC_AK4642)
AC_SUBST(CONFIG_SND_SOC_AK4671)
AC_SUBST(CONFIG_SND_SOC_ALC5623)
AC_SUBST(CONFIG_SND_SOC_ALC5632)
AC_SUBST(CONFIG_SND_SOC_CQ0093VC)
AC_SUBST(CONFIG_SND_SOC_CS42L51)
AC_SUBST(CONFIG_SND_SOC_CS42L73)
AC_SUBST(CONFIG_SND_SOC_CS4270)
AC_SUBST(CONFIG_SND_SOC_CS4270_VD33_ERRATA)
AC_SUBST(CONFIG_SND_SOC_CS4271)
AC_SUBST(CONFIG_SND_SOC_CX20442)
AC_SUBST(CONFIG_SND_SOC_JZ4740_CODEC)
AC_SUBST(CONFIG_SND_SOC_L3)
AC_SUBST(CONFIG_SND_SOC_DA7210)
AC_SUBST(CONFIG_SND_SOC_DFBMCS320)
AC_SUBST(CONFIG_SND_SOC_DMIC)
AC_SUBST(CONFIG_SND_SOC_MAX98088)
AC_SUBST(CONFIG_SND_SOC_MAX98095)
AC_SUBST(CONFIG_SND_SOC_MAX9850)
AC_SUBST(CONFIG_SND_SOC_PCM3008)
AC_SUBST(CONFIG_SND_SOC_RT5631)
AC_SUBST(CONFIG_SND_SOC_SGTL5000)
AC_SUBST(CONFIG_SND_SOC_SIGMADSP)
AC_SUBST(CONFIG_CRC32)
AC_SUBST(CONFIG_SND_SOC_SN95031)
AC_SUBST(CONFIG_SND_SOC_SPDIF)
AC_SUBST(CONFIG_SND_SOC_SSM2602)
AC_SUBST(CONFIG_SND_SOC_STA32X)
AC_SUBST(CONFIG_SND_SOC_STAC9766)
AC_SUBST(CONFIG_SND_SOC_TLV320AIC23)
AC_SUBST(CONFIG_SND_SOC_TLV320AIC26)
AC_SUBST(CONFIG_SND_SOC_TLV320AIC32X4)
AC_SUBST(CONFIG_SND_SOC_TLV320AIC3X)
AC_SUBST(CONFIG_SND_SOC_TLV320DAC33)
AC_SUBST(CONFIG_SND_SOC_TWL4030)
AC_SUBST(CONFIG_MFD_TWL4030_AUDIO)
AC_SUBST(CONFIG_SND_SOC_TWL6040)
AC_SUBST(CONFIG_TWL6040_CORE)
AC_SUBST(CONFIG_SND_SOC_UDA134X)
AC_SUBST(CONFIG_SND_SOC_UDA1380)
AC_SUBST(CONFIG_SND_SOC_WL1273)
AC_SUBST(CONFIG_SND_SOC_WM1250_EV1)
AC_SUBST(CONFIG_SND_SOC_WM2000)
AC_SUBST(CONFIG_SND_SOC_WM5100)
AC_SUBST(CONFIG_SND_SOC_WM8350)
AC_SUBST(CONFIG_SND_SOC_WM8400)
AC_SUBST(CONFIG_SND_SOC_WM8510)
AC_SUBST(CONFIG_SND_SOC_WM8523)
AC_SUBST(CONFIG_SND_SOC_WM8580)
AC_SUBST(CONFIG_SND_SOC_WM8711)
AC_SUBST(CONFIG_SND_SOC_WM8727)
AC_SUBST(CONFIG_SND_SOC_WM8728)
AC_SUBST(CONFIG_SND_SOC_WM8731)
AC_SUBST(CONFIG_SND_SOC_WM8737)
AC_SUBST(CONFIG_SND_SOC_WM8741)
AC_SUBST(CONFIG_SND_SOC_WM8750)
AC_SUBST(CONFIG_SND_SOC_WM8753)
AC_SUBST(CONFIG_SND_SOC_WM8770)
AC_SUBST(CONFIG_SND_SOC_WM8776)
AC_SUBST(CONFIG_SND_SOC_WM8782)
AC_SUBST(CONFIG_SND_SOC_WM8804)
AC_SUBST(CONFIG_SND_SOC_WM8900)
AC_SUBST(CONFIG_SND_SOC_WM8903)
AC_SUBST(CONFIG_SND_SOC_WM8904)
AC_SUBST(CONFIG_SND_SOC_WM8940)
AC_SUBST(CONFIG_SND_SOC_WM8955)
AC_SUBST(CONFIG_SND_SOC_WM8960)
AC_SUBST(CONFIG_SND_SOC_WM8961)
AC_SUBST(CONFIG_SND_SOC_WM8962)
AC_SUBST(CONFIG_SND_SOC_WM8971)
AC_SUBST(CONFIG_SND_SOC_WM8974)
AC_SUBST(CONFIG_SND_SOC_WM8978)
AC_SUBST(CONFIG_SND_SOC_WM8983)
AC_SUBST(CONFIG_SND_SOC_WM8985)
AC_SUBST(CONFIG_SND_SOC_WM8988)
AC_SUBST(CONFIG_SND_SOC_WM8990)
AC_SUBST(CONFIG_SND_SOC_WM8991)
AC_SUBST(CONFIG_SND_SOC_WM8993)
AC_SUBST(CONFIG_SND_SOC_WM8994)
AC_SUBST(CONFIG_SND_SOC_WM8995)
AC_SUBST(CONFIG_SND_SOC_WM8996)
AC_SUBST(CONFIG_SND_SOC_WM9081)
AC_SUBST(CONFIG_SND_SOC_WM9090)
AC_SUBST(CONFIG_SND_SOC_WM9705)
AC_SUBST(CONFIG_SND_SOC_WM9712)
AC_SUBST(CONFIG_SND_SOC_WM9713)
AC_SUBST(CONFIG_SND_SOC_LM4857)
AC_SUBST(CONFIG_SND_SOC_MAX9877)
AC_SUBST(CONFIG_SND_SOC_TPA6130A2)
AC_SUBST(CONFIG_SOUND_PRIME)
AC_SUBST(CONFIG_AC97_BUS)
])

