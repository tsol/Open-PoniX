AM_CFLAGS = \
	@XORG_CFLAGS@ \
	@XATRACKER_CFLAGS@ \
	-Wall \
	-Werror \
	-I$(top_srcdir)/system-includes/ \
	-I$(top_builddir)/

freedreno_drv_la_LTLIBRARIES = freedreno_drv.la
freedreno_drv_la_LDFLAGS = -module -avoid-version
freedreno_drv_la_LIBADD = @XORG_LIBS@ @XATRACKER_LIBS@
freedreno_drv_ladir = @moduledir@/drivers

# Additional includes needed for msm fbdev + kgsl backend:
MSM_INCLUDES = \
	../system-includes/linux/msm_kgsl.h \
	../system-includes/linux/msm_mdp.h

freedreno_drv_la_SOURCES = \
	$(MSM_INCLUDES) \
	compat-api.h \
	freedreno_z1xx.h \
	drmmode_display.c \
	fbmode_display.c \
	msm-driver.c \
	msm-accel.c \
	msm-accel.h \
	msm-accel-z1xx.c \
	msm-accel-z1xx.h \
	msm-exa.c \
	msm-dri2.c \
	msm-pixmap.c

if BUILD_XA
freedreno_drv_la_SOURCES += \
	msm-exa-xa.c
endif

EXTRA_DIST = \
	msm.h
