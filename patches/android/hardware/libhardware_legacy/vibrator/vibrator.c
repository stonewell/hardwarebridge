/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <hardware_legacy/vibrator.h>
#include "qemu.h"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <stdlib.h>
#include <string.h>

#define LOG_TAG "VibratorHW"

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

static int make_sure_socket_valid ();

static int call_cmd(const char * cmd);
static int call_cmd1(const char * cmd, int val);

int vibrator_on(int timeout_ms)
{
	return call_cmd1("vibrator_on", timeout_ms);
}

int vibrator_off()
{
	return call_cmd("vibrator_off");
}

static pthread_mutex_t write_mutex = PTHREAD_MUTEX_INITIALIZER;
#define CONTROL_SOCKET "hardware_bridge"

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
	}while (0);

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
	}while (0);

	close(fw_sock);

	return ret;
}

static int call_cmd1(const char * cmd, int val) {
	char tmp_buf[101];
	int tmp_len;
	int fw_sock = 0;
	int ret = -1;

	if (make_sure_socket_valid(&fw_sock))
	return -1;

	do {
		sprintf(tmp_buf, "%s%d", cmd, val);

		tmp_len = strlen(tmp_buf);

		CHECK_RET(write(fw_sock, &tmp_len, sizeof(int)));
		CHECK_RET(write(fw_sock, tmp_buf, tmp_len));
		CHECK_RET(read(fw_sock, &ret, sizeof(int)));
	}while (0);

	close(fw_sock);

	return ret;
}

