#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include "dlfcn.h"
#include "unistd.h"

#include "hardware_bridge.h"
#include "vendor.h"
#include "flashlight.h"

static int make_sure_functions_ok();

VENDOR_FUNC0_INT(get_flashlight_enabled);
VENDOR_FUNC1_INT_INT(set_flashlight_enabled);
VENDOR_FUNC1_INT_INT(enable_camera_flash);

static int make_sure_functions_ok() {

	if (!g_hardware_legacy_handler) {
		LOGE("invalid hardware legacy handler");
		return -1;
	}

	VENDOR_FUNC1_INT_INT_SYM_ENTRY(set_flashlight_enabled);
	VENDOR_FUNC1_INT_INT_SYM_ENTRY(enable_camera_flash);
	VENDOR_FUNC0_INT_SYM_ENTRY(get_flashlight_enabled);

	return 0;
}


