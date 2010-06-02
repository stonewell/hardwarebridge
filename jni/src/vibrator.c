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
#include "vibrator.h"

static int make_sure_functions_ok();

VENDOR_FUNC1_INT_INT(vibrator_on);
VENDOR_FUNC0_INT(vibrator_off);

static int make_sure_functions_ok() {

	if (!g_hardware_legacy_handler) {
		return -1;
	}

	VENDOR_FUNC1_INT_INT_SYM_ENTRY(vibrator_on);
	VENDOR_FUNC0_INT_SYM_ENTRY(vibrator_off);

	return 0;
}


