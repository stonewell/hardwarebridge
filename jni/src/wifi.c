#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include "dlfcn.h"
#include "unistd.h"
#include "dirent.h"

#include "hardware_bridge.h"
#include "wifi.h"

typedef int (*pfn_vendor_func0)();
typedef void (*pfn_vendor_func0_void)();
typedef int (*pfn_vendor_func_wifi_wait_for_event)(char *, size_t);
typedef int (*pfn_vendor_func_wifi_command)(const char *, char *, size_t *);

static int make_sure_library_ok();

static void * g_hardware_legacy_handler = 0;
static pfn_vendor_func_wifi_wait_for_event gpfn_wifi_wait_for_event = 0;
static pfn_vendor_func_wifi_command gpfn_wifi_command = 0;

#define VENDOR_FUNC0(x) \
		static pfn_vendor_func0 gpfn_##x = 0; \
		int vendor_##x() { \
			if (make_sure_library_ok()) \
				return -1; \
			return gpfn_##x(); \
		}
#define VENDOR_FUNC0_VOID(x) \
		static pfn_vendor_func0_void gpfn_##x = 0; \
		int vendor_##x() { \
			if (make_sure_library_ok()) \
				return -1; \
			gpfn_##x(); \
			return 0;\
		}
#define VENDOR_FUNC_SYM_ENTRY(x,y) \
		if (!gpfn_##x) {\
			dlerror();\
			gpfn_##x = (y) dlsym(\
					(void *) g_hardware_legacy_handler, #x);\
			if (!gpfn_##x) {\
				LOGE("unable to find function:%s,%s\n", #x, dlerror());\
				return -2;\
			}\
		}
#define VENDOR_FUNC0_SYM_ENTRY(x) VENDOR_FUNC_SYM_ENTRY(x,pfn_vendor_func0)
#define VENDOR_FUNC0_VOID_SYM_ENTRY(x) VENDOR_FUNC_SYM_ENTRY(x,pfn_vendor_func0_void)
VENDOR_FUNC0(wifi_load_driver)
VENDOR_FUNC0(wifi_unload_driver)
VENDOR_FUNC0(wifi_start_supplicant)
VENDOR_FUNC0(wifi_stop_supplicant)
VENDOR_FUNC0(wifi_connect_to_supplicant)
VENDOR_FUNC0_VOID(wifi_close_supplicant_connection)

static int make_sure_library_ok() {

	if (!g_hardware_legacy_handler) {
		g_hardware_legacy_handler = dlopen("libhardware_legacy.so", RTLD_LAZY
				| RTLD_GLOBAL);

		if (!g_hardware_legacy_handler) {
			LOGE("unable to load libhardware_legacy.so,%s\n", dlerror());
			return -1;
		}
	}

	VENDOR_FUNC0_SYM_ENTRY(wifi_load_driver);
	VENDOR_FUNC0_SYM_ENTRY(wifi_unload_driver);
	VENDOR_FUNC0_SYM_ENTRY(wifi_start_supplicant);
	VENDOR_FUNC0_SYM_ENTRY(wifi_stop_supplicant);
	VENDOR_FUNC0_SYM_ENTRY(wifi_connect_to_supplicant);
	VENDOR_FUNC0_VOID_SYM_ENTRY(wifi_close_supplicant_connection);
	VENDOR_FUNC_SYM_ENTRY(wifi_wait_for_event, pfn_vendor_func_wifi_wait_for_event);
	VENDOR_FUNC_SYM_ENTRY(wifi_command, pfn_vendor_func_wifi_command);

	return 0;
}

int vendor_wifi_command(const char * cmd, char * reply, size_t * reply_len)
{
	if (make_sure_library_ok())
		return -1;
	return gpfn_wifi_command(cmd,reply,reply_len);
}

int vendor_wifi_wait_for_event(char *buf, size_t buflen)
{
	if (make_sure_library_ok())
		return -1;
	return gpfn_wifi_wait_for_event(buf, buflen);
}

