#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "hardware_bridge.h"
#include "cmd_dispatch.h"

#include "wifi.h"

struct cmd_dispatch {
	char *cmd;
	int (* dispatch)(int sock, char *);
};

#define FRAMEWORK_FUNC0(x) static int do_##x(int sock, char *cmd) \
{ \
	int ret = -1; \
    ret = vendor_##x(); \
	write(sock, &ret, sizeof(int));\
    return ret;\
}

#define FRAMEWORK_CMD_DISPATCH_ENTRY(x) \
	    { #x, do_##x }

static void dispatch_cmd(int sock, char *cmd);

FRAMEWORK_FUNC0(wifi_load_driver)
FRAMEWORK_FUNC0(wifi_connect_to_supplicant)
FRAMEWORK_FUNC0(wifi_unload_driver)
FRAMEWORK_FUNC0(wifi_start_supplicant)
FRAMEWORK_FUNC0(wifi_stop_supplicant)
FRAMEWORK_FUNC0(wifi_close_supplicant_connection)

static int do_wifi_wait_for_event(int sock, char *cmd);
static int do_wifi_command(int sock, char *cmd);

static struct cmd_dispatch dispatch_table[] = {
		FRAMEWORK_CMD_DISPATCH_ENTRY(wifi_load_driver),
		FRAMEWORK_CMD_DISPATCH_ENTRY(wifi_unload_driver),
		FRAMEWORK_CMD_DISPATCH_ENTRY(wifi_start_supplicant),
		FRAMEWORK_CMD_DISPATCH_ENTRY(wifi_stop_supplicant),
		FRAMEWORK_CMD_DISPATCH_ENTRY(wifi_connect_to_supplicant),
		FRAMEWORK_CMD_DISPATCH_ENTRY(wifi_close_supplicant_connection), {
		"wifi_wait_for_event", do_wifi_wait_for_event }, { "wifi_command",
		do_wifi_command }, { NULL, NULL } };

void do_test_cmd() {
	struct cmd_dispatch *c;

	for (c = dispatch_table; c->cmd != NULL; c++) {
		LOGD("test dispatch_cmd(%s):", c->cmd);

		c->dispatch(0, c->cmd);
	}
}

int process_framework_command(int socket) {
	int rc;
	char buffer[101];
	int start = 0;
	int i;
	int len;

	LOGD("start to read framework");

	if ((rc = read(socket, &len, sizeof(int))) < 0) {
		LOGE("Unable to read framework command length (%s)", strerror(errno));
		return -errno;
	} else if (!rc)
		return -ECONNRESET;

	LOGD("frame command length:%d", len);

	if ((rc = read(socket, buffer, len)) < 0) {
		LOGE("Unable to read framework command (%s)", strerror(errno));
		return -errno;
	} else if (!rc)
		return -ECONNRESET;

	buffer[rc] = 0;

	LOGD("frame command read:%d,%d,%s", len, rc, buffer);

	for (i = 0; i < rc; i++) {
		if (buffer[i] == 0) {
			dispatch_cmd(socket, buffer + start);
			start = i + 1;
		}
	}
	return 0;
}

static void dispatch_cmd(int sock, char *cmd) {
	struct cmd_dispatch *c;

	LOGD("dispatch_cmd(%s):", cmd);

	for (c = dispatch_table; c->cmd != NULL; c++) {
		if (!strncmp(c->cmd, cmd, strlen(c->cmd))) {
			c->dispatch(sock, cmd);
			return;
		}
	}

	LOGE("No cmd handlers defined for '%s'", cmd);
}

static int do_wifi_wait_for_event(int sock, char *cmd) {
	char * buf = NULL;
	size_t len = atol(&cmd[strlen("wifi_wait_for_event")]);

	buf = (char *) malloc(sizeof(char) * len);

	LOGD("do_wifi_wait_for_event:before %d", len);

	len = vendor_wifi_wait_for_event(buf, len);

	LOGD("do_wifi_wait_for_event:after %d,%s", len, buf);

	write(sock, &len, sizeof(size_t));

	if (len > 0) {
		write(sock, buf, len);
	}

	free(buf);

	return len < 0 ? -1 : 0;
}

static int do_wifi_command(int sock, char *cmd) {
	char * buf = NULL;
	int pos = 0;
	size_t len = 0;
	char * wifi_cmd = NULL;

	for (pos = 0; pos < strlen(cmd); pos++) {
		if (cmd[pos] == ' ') {
			cmd[pos] = 0;
			break;
		}
	}

	if (pos == 0 || pos == strlen(cmd)) {
		len = -1;

		write(sock, &len, sizeof(size_t));

		return -1;
	}

	wifi_cmd = &cmd[strlen("wifi_command")];
	len = atol(&cmd[pos + 1]);

	buf = (char *) malloc(sizeof(char) * len);

	LOGD("do_wifi_command:before %d,%s", len, wifi_cmd);

	pos = vendor_wifi_command(wifi_cmd, buf, &len);

	LOGD("do_wifi_command:after %d,%s,%s", len, wifi_cmd, buf);

	write(sock, &pos, sizeof(int));

	if (!pos) {
		write(sock, &len, sizeof(size_t));

		if (len > 0) {
			write(sock, buf, len);
		}
	}

	free(buf);

	return pos;
}

