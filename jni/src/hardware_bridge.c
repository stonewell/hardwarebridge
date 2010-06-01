#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>

#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/un.h>

#include <linux/netlink.h>

#include "hardware_bridge.h"
#include "cmd_dispatch.h"

#define CONTROL_SOCKET "hardware_bridge"

/*
 * Globals
 */

static int ver_major = 1;
static int ver_minor = 0;
int enable_debug = 0;

static int pipefd[2];

static void * handle_framework_sock_func(void * lpParam);

int process_framework_command(int fw_socket);

int main(int argc, char ** argv) {
	int door_sock = -1;

	LOGI("Hardware Bridge Daemon version %d.%d", ver_major, ver_minor);

	//do_test_cmd();

	/*
	 * Create all the various sockets we'll need
	 */
	// Socket to listen on for incomming framework connections
	if ((door_sock = android_get_control_socket(CONTROL_SOCKET)) < 0) {
		LOGE("Obtaining file descriptor socket '%s' failed: %s",
				CONTROL_SOCKET, strerror(errno));
		exit(1);
	}

	if (listen(door_sock, 4) < 0) {
		LOGE("Unable to listen on fd '%d' for socket '%s': %s", door_sock,
				CONTROL_SOCKET, strerror(errno));
		exit(1);
	}

	if (pipe(pipefd) < 0) {
		LOGE("Unable to create pipes: %s", strerror(errno));
		exit(1);
	}

	/*
	 * Main loop
	 */
	LOGD("Bootstrapping complete");

	while (1) {
		fd_set read_fds;
		struct timeval to;
		int max = 0;
		int rc = 0;

		to.tv_sec = (60 * 60);
		to.tv_usec = 0;

		FD_ZERO(&read_fds);

		FD_SET(door_sock, &read_fds);
		if (door_sock > max)
			max = door_sock;

		FD_SET(pipefd[0], &read_fds);
		if (pipefd[0] > max)
			max = pipefd[0];

		if ((rc = select(max + 1, &read_fds, NULL, NULL, &to)) < 0) {
			LOGE("select() failed (%s)", strerror(errno));
			sleep(1);
			continue;
		}

		if (!rc) {
			continue;
		}

		if (FD_ISSET(pipefd[0], &read_fds)) {
			pthread_t tmp;

			if (sizeof(pthread_t) == read(pipefd[0], &tmp, sizeof(pthread_t))) {
				pthread_join(tmp, NULL);
				LOGD("Join child thread,%ld", tmp);
			}
		}

		if (FD_ISSET(door_sock, &read_fds)) {
			struct sockaddr addr;
			socklen_t alen;
			int fw_sock;
			pthread_t thread_id;
			int t_error;

			alen = sizeof(addr);

			if ((fw_sock = accept(door_sock, &addr, &alen)) < 0) {
				LOGE("Unable to accept framework connection (%s)", strerror(
						errno));
			}

			t_error = pthread_create(&thread_id, NULL,
					handle_framework_sock_func, (void*)fw_sock);

			LOGD("Accepted connection from framework,%d,%d", (int)thread_id, t_error);

			if (t_error) {
				LOGE("create thread to process framework command fail,%d", t_error);
			}
		}

	} // while

}

static void * handle_framework_sock_func(void * lpParam) {
	int rc;
	int fw_sock;
	pthread_t self = pthread_self();

	fw_sock = (int) lpParam;
	while (1) {
		fd_set read_fds;
		struct timeval to;
		int max = 0;

		to.tv_sec = (60 * 60);
		to.tv_usec = 0;

		FD_ZERO(&read_fds);
		FD_SET(fw_sock, &read_fds);

		if (fw_sock > max)
			max = fw_sock;

		if ((rc = select(max + 1, &read_fds, NULL, NULL, &to)) < 0) {
			LOGE("select() failed (%s)", strerror(errno));
			sleep(1);
			continue;
		}

		if (!rc) {
			continue;
		}

		if ((rc = process_framework_command(fw_sock)) < 0) {
			if (rc == -ECONNRESET) {
				LOGD("Framework disconnected");
				close(fw_sock);
				break;
			} else {
				LOGE("Error processing framework command (%s)", strerror(errno));
			}
		}//process frame work command
	}//while

	write(pipefd[1], &self, sizeof(pthread_t) );

	return (void*)0;
}
