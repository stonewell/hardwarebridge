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

void * g_hardware_legacy_handler = 0;

int make_sure_library_ok()
{
	if (!g_hardware_legacy_handler) {
		g_hardware_legacy_handler = dlopen("libhardware_legacy.so", RTLD_LAZY
				| RTLD_GLOBAL);

		if (!g_hardware_legacy_handler) {
			LOGE("unable to load libhardware_legacy.so,%s\n", dlerror());
			return -1;
		}
	}

	return 0;
}
