#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include "hardware_legacy/wifi.h"

#define LOG_TAG "WifiHW"
#define CONTROL_SOCKET "hardware_bridge"

#include "cutils/log.h"
#include "cutils/memory.h"
#include "cutils/misc.h"
#include "cutils/properties.h"
#include <cutils/sockets.h>
#include <netinet/tcp.h>
#include <cutils/properties.h>
#include <cutils/adb_networking.h>

#include "private/android_filesystem_config.h"
#ifdef HAVE_LIBC_SYSTEM_PROPERTIES
#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>
#endif

extern int do_dhcp();
extern int ifc_init();
extern void ifc_close();
extern char *dhcp_lasterror();
extern void get_dhcp_info();
extern int init_module(void *, unsigned long, const char *);
extern int delete_module(const char *, unsigned int);

static char iface[PROPERTY_VALUE_MAX];
// TODO: use new ANDROID_SOCKET mechanism, once support for multiple
// sockets is in

#ifndef WIFI_DRIVER_MODULE_PATH
#define WIFI_DRIVER_MODULE_PATH         "/system/lib/modules/wlan.ko"
#endif
#ifndef WIFI_DRIVER_MODULE_NAME
#define WIFI_DRIVER_MODULE_NAME         "wlan"
#endif
#ifndef WIFI_DRIVER_MODULE_ARG
#define WIFI_DRIVER_MODULE_ARG          ""
#endif
#ifndef WIFI_FIRMWARE_LOADER
#define WIFI_FIRMWARE_LOADER		""
#endif
#define WIFI_TEST_INTERFACE		"sta"

static const char IFACE_DIR[] = "/data/system/wpa_supplicant";
static const char DRIVER_MODULE_NAME[] = WIFI_DRIVER_MODULE_NAME;
static const char DRIVER_MODULE_TAG[] = WIFI_DRIVER_MODULE_NAME " ";
static const char DRIVER_MODULE_PATH[] = WIFI_DRIVER_MODULE_PATH;
static const char DRIVER_MODULE_ARG[] = WIFI_DRIVER_MODULE_ARG;
static const char FIRMWARE_LOADER[] = WIFI_FIRMWARE_LOADER;
static const char DRIVER_PROP_NAME[] = "wlan.driver.status";
static const char SUPPLICANT_NAME[] = "wpa_supplicant";
static const char SUPP_PROP_NAME[] = "init.svc.wpa_supplicant";
static const char SUPP_CONFIG_TEMPLATE[] =
		"/system/etc/wifi/wpa_supplicant.conf";
static const char SUPP_CONFIG_FILE[] = "/data/misc/wifi/wpa_supplicant.conf";
static const char MODULE_FILE[] = "/proc/modules";

static int make_sure_socket_valid();

static int call_cmd(const char * cmd);
static int call_cmd2(const char * cmd, char * buf, size_t len);
static int call_cmd3(const char * cmd, const char * wifi_cmd, char * reply,
		size_t * reply_len);

int wifi_load_driver() {
	return call_cmd("wifi_load_driver");
}

int wifi_unload_driver() {
	return call_cmd("wifi_unload_driver");
}

int wifi_start_supplicant() {
	return call_cmd("wifi_start_supplicant");
}

int wifi_stop_supplicant() {
	return call_cmd("wifi_stop_supplicant");
}

int wifi_connect_to_supplicant() {
	return call_cmd("wifi_connect_to_supplicant");
}

int wifi_wait_for_event(char *buf, size_t buflen) {
	return call_cmd2("wifi_wait_for_event", buf, buflen);
}

void wifi_close_supplicant_connection() {
	call_cmd("wifi_close_supplicant_connection");
}

int wifi_command(const char *command, char *reply, size_t *reply_len) {
	return call_cmd3("wifi_command", command, reply, reply_len);
}

static pthread_mutex_t write_mutex = PTHREAD_MUTEX_INITIALIZER;

static int make_sure_socket_valid(int * fw_sock) {
	int ret = -1;

	pthread_mutex_lock(&write_mutex);

	do {
		if ((*fw_sock = socket_local_client(CONTROL_SOCKET, 1, SOCK_STREAM))
				< 0) {
			LOGE("Obtaining file descriptor socket '%s' failed: %s",
					CONTROL_SOCKET, strerror(errno));

			break;
		}

		ret = 0;
	} while (0);

	pthread_mutex_unlock(&write_mutex);

	return ret;
}

#define CHECK_RET(x) \
	{ if ((x) <= 0) { close(fw_sock); fw_sock=0; break; } }

static int call_cmd(const char * cmd) {
	int ret = -1;
	int len = 0;
	int fw_sock = 0;

	if (make_sure_socket_valid(&fw_sock))
		return -1;

	do {
		len = strlen(cmd);

		CHECK_RET(write(fw_sock, &len, sizeof(int)));
		CHECK_RET(write(fw_sock, cmd, strlen(cmd)));
		CHECK_RET(read(fw_sock, &ret, sizeof(int)));
	} while (0);

	close(fw_sock);

	return ret;
}

static int call_cmd2(const char * cmd, char * buf, size_t len) {
	char tmp_buf[101];
	int tmp_len;
	int fw_sock = 0;

	if (make_sure_socket_valid(&fw_sock))
		return -1;

	do {
		sprintf(tmp_buf, "%s%d", cmd, len);

		tmp_len = strlen(tmp_buf);

		CHECK_RET(write(fw_sock, &tmp_len, sizeof(int)));
		CHECK_RET(write(fw_sock, tmp_buf, strlen(tmp_buf)));
		CHECK_RET(read(fw_sock, &len, sizeof(size_t)));

		if (len > 0)
			CHECK_RET(read(fw_sock, buf, len));
	} while (0);

	close(fw_sock);

	return len;
}

static int call_cmd3(const char * cmd, const char * wifi_cmd, char * reply,
		size_t * reply_len) {
	char tmp_buf[101];
	int ret;
	int len;
	int fw_sock = 0;

	if (make_sure_socket_valid(&fw_sock))
		return -1;

	do {
		sprintf(tmp_buf, "%s%s|%d", cmd, wifi_cmd, *reply_len);

		len = strlen(tmp_buf);

		CHECK_RET(write(fw_sock, &len, sizeof(int)));
		CHECK_RET(write(fw_sock, tmp_buf, strlen(tmp_buf)));
		CHECK_RET(read(fw_sock, &ret, sizeof(int)));

		if (!ret) {
			CHECK_RET(read(fw_sock, reply_len, sizeof(size_t)));
			CHECK_RET(read(fw_sock, reply, *reply_len));
		}
	} while (0);
	close(fw_sock);

	return ret;
}

int do_dhcp_request(int *ipaddr, int *gateway, int *mask, int *dns1, int *dns2,
		int *server, int *lease) {
	/* For test driver, always report success */
	if (strcmp(iface, WIFI_TEST_INTERFACE) == 0)
		return 0;

	if (ifc_init() < 0) {
		LOGE("ifc_init error");
		return -1;
	}

	if (do_dhcp(iface) < 0) {
		LOGE("do_dhcp error");
		ifc_close();
		return -1;
	}
	ifc_close();
	get_dhcp_info(ipaddr, gateway, mask, dns1, dns2, server, lease);
	LOGE("get dhcp info: %d %d %d %d %d %d %d", *ipaddr, *gateway, *mask, *dns1, *dns2, *server, *lease);
	return 0;
}

const char *get_dhcp_error_string() {
	const char * err = dhcp_lasterror();
	LOGE("get dhcp error: %s", err);
	return err;
}

int ensure_config_file_exists() {
	char buf[2048];
	int srcfd, destfd;
	int nread;

	if (access(SUPP_CONFIG_FILE, R_OK | W_OK) == 0) {
		return 0;
	} else if (errno != ENOENT) {
		LOGE("Cannot access \"%s\": %s", SUPP_CONFIG_FILE, strerror(errno));
		return -1;
	}

	srcfd = open(SUPP_CONFIG_TEMPLATE, O_RDONLY);
	if (srcfd < 0) {
		LOGE("Cannot open \"%s\": %s", SUPP_CONFIG_TEMPLATE, strerror(errno));
		return -1;
	}

	destfd = open(SUPP_CONFIG_FILE, O_CREAT | O_WRONLY, 0660);
	if (destfd < 0) {
		close(srcfd);
		LOGE("Cannot create \"%s\": %s", SUPP_CONFIG_FILE, strerror(errno));
		return -1;
	}

	while ((nread = read(srcfd, buf, sizeof(buf))) != 0) {
		if (nread < 0) {
			LOGE("Error reading \"%s\": %s", SUPP_CONFIG_TEMPLATE, strerror(
					errno));
			close(srcfd);
			close(destfd);
			unlink(SUPP_CONFIG_FILE);
			return -1;
		}
		write(destfd, buf, nread);
	}

	close(destfd);
	close(srcfd);

	if (chown(SUPP_CONFIG_FILE, AID_SYSTEM, AID_WIFI) < 0) {
		LOGE("Error changing group ownership of %s to %d: %s",
				SUPP_CONFIG_FILE, AID_WIFI, strerror(errno));
		unlink(SUPP_CONFIG_FILE);
		return -1;
	}
	return 0;
}
