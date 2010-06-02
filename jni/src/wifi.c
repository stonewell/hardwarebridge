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
#include "wifi.h"

typedef int (*pfn_vendor_func_wifi_wait_for_event)(char *, size_t);
typedef int (*pfn_vendor_func_wifi_command)(const char *, char *, size_t *);

static pfn_vendor_func_wifi_wait_for_event gpfn_wifi_wait_for_event = 0;
static pfn_vendor_func_wifi_command gpfn_wifi_command = 0;

static int make_sure_functions_ok();

VENDOR_FUNC0_INT(wifi_load_driver);
VENDOR_FUNC0_INT(wifi_unload_driver);
VENDOR_FUNC0_INT(wifi_start_supplicant);
VENDOR_FUNC0_INT(wifi_stop_supplicant);
VENDOR_FUNC0_INT(wifi_connect_to_supplicant);
VENDOR_FUNC0_VOID(wifi_close_supplicant_connection);

static int make_sure_functions_ok() {

	if (!g_hardware_legacy_handler) {
		return -1;
	}

	VENDOR_FUNC0_INT_SYM_ENTRY(wifi_load_driver);
	VENDOR_FUNC0_INT_SYM_ENTRY(wifi_unload_driver);
	VENDOR_FUNC0_INT_SYM_ENTRY(wifi_start_supplicant);
	VENDOR_FUNC0_INT_SYM_ENTRY(wifi_stop_supplicant);
	VENDOR_FUNC0_INT_SYM_ENTRY(wifi_connect_to_supplicant);
	VENDOR_FUNC0_VOID_SYM_ENTRY(wifi_close_supplicant_connection);
	VENDOR_FUNC_SYM_ENTRY(wifi_wait_for_event, pfn_vendor_func_wifi_wait_for_event);
	VENDOR_FUNC_SYM_ENTRY(wifi_command, pfn_vendor_func_wifi_command);

	return 0;
}

int vendor_wifi_command(const char * cmd, char * reply, size_t * reply_len)
{
	if (make_sure_library_ok())
		return -1;
	if (make_sure_functions_ok())
		return -1;
	return gpfn_wifi_command(cmd,reply,reply_len);
}

int vendor_wifi_wait_for_event(char *buf, size_t buflen)
{
	if (make_sure_library_ok())
		return -1;
	if (make_sure_functions_ok())
		return -1;
	return gpfn_wifi_wait_for_event(buf, buflen);
}

