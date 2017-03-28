#define FORMAT(fmt) "%s: %d: " fmt, __func__, __LINE__
#define pr_fmt(fmt) KBUILD_MODNAME ": " FORMAT(fmt)
#include "adriver.h"
#undef FORMAT
#undef pr_fmt
#include "../alsa-kernel/core/compress_offload.c"
