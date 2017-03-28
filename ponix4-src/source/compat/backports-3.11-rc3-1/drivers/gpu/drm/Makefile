#
# Makefile for the drm device driver.  This driver provides support for the
# Direct Rendering Infrastructure (DRI) in XFree86 4.1.0 and higher.

ccflags-y := -Iinclude/drm

drm-y       :=	drm_auth.o drm_buffer.o drm_bufs.o drm_cache.o \
		drm_context.o drm_dma.o \
		drm_drv.o drm_fops.o drm_gem.o drm_ioctl.o drm_irq.o \
		drm_lock.o drm_memory.o drm_proc.o drm_stub.o drm_vm.o \
		drm_agpsupport.o drm_scatter.o drm_pci.o \
		drm_platform.o drm_sysfs.o drm_hashtab.o drm_mm.o \
		drm_crtc.o drm_modes.o drm_edid.o \
		drm_info.o drm_debugfs.o drm_encoder_slave.o \
		drm_trace_points.o drm_global.o drm_prime.o \
		drm_rect.o

drm-$(CONFIG_COMPAT) += drm_ioc32.o
drm-$(CPTCFG_DRM_GEM_CMA_HELPER) += drm_gem_cma_helper.o
drm-$(CONFIG_PCI) += ati_pcigart.o

drm-usb-y   := drm_usb.o

drm_kms_helper-y := drm_fb_helper.o drm_crtc_helper.o drm_dp_helper.o
drm_kms_helper-$(CPTCFG_DRM_LOAD_EDID_FIRMWARE) += drm_edid_load.o
drm_kms_helper-$(CPTCFG_DRM_KMS_CMA_HELPER) += drm_fb_cma_helper.o

obj-$(CPTCFG_DRM_KMS_HELPER) += drm_kms_helper.o

CFLAGS_drm_trace_points.o := -I$(src)

obj-$(CPTCFG_DRM)	+= drm.o
obj-$(CPTCFG_DRM_USB)   += drm_usb.o
obj-$(CPTCFG_DRM_TTM)	+= ttm/
obj-$(CPTCFG_DRM_TDFX)	+= tdfx/
obj-$(CPTCFG_DRM_R128)	+= r128/
obj-$(CPTCFG_DRM_RADEON)+= radeon/
obj-$(CPTCFG_DRM_MGA)	+= mga/
obj-$(CPTCFG_DRM_I810)	+= i810/
obj-$(CPTCFG_DRM_I915)  += i915/
obj-$(CPTCFG_DRM_MGAG200) += mgag200/
obj-$(CPTCFG_DRM_CIRRUS_QEMU) += cirrus/
obj-$(CPTCFG_DRM_SIS)   += sis/
obj-$(CPTCFG_DRM_SAVAGE)+= savage/
obj-$(CPTCFG_DRM_VMWGFX)+= vmwgfx/
obj-$(CPTCFG_DRM_VIA)	+=via/
obj-$(CPTCFG_DRM_NOUVEAU) +=nouveau/
obj-$(CPTCFG_DRM_EXYNOS) +=exynos/
obj-$(CPTCFG_DRM_GMA500) += gma500/
obj-$(CPTCFG_DRM_UDL) += udl/
obj-$(CPTCFG_DRM_AST) += ast/
obj-$(CPTCFG_DRM_RCAR_DU) += rcar-du/
obj-$(CPTCFG_DRM_SHMOBILE) +=shmobile/
obj-$(CPTCFG_DRM_OMAP)	+= omapdrm/
obj-$(CPTCFG_DRM_TILCDC)	+= tilcdc/
obj-$(CPTCFG_DRM_QXL) += qxl/
obj-y			+= i2c/
