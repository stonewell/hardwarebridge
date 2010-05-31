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
//static pthread_mutex_t write_mutex = PTHREAD_MUTEX_INITIALIZER;
int enable_debug = 0;

int bootstrap = 0;

//struct st_framework_sock {
//	struct st_framework_sock * next;
//	int fw_sock;
//};

static void * handle_framework_sock_func(void * lpParam);

int process_framework_command(int fw_socket);

int main(int argc, char ** argv) {
	int door_sock = -1;
	//struct st_framework_sock * header = NULL;

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

	/*
	 * Bootstrap
	 */

	bootstrap = 1;

	bootstrap = 0;
	/*
	 * Main loop
	 */
	LOGD("Bootstrapping complete");

	while (1) {
		fd_set read_fds;
		struct timeval to;
		int max = 0;
		int rc = 0;
		//		struct st_framework_sock * tmp_sock;
		//		struct st_framework_sock * prev_sock;

		to.tv_sec = (60 * 60);
		to.tv_usec = 0;

		FD_ZERO(&read_fds);
		FD_SET(door_sock, &read_fds);
		if (door_sock > max)
			max = door_sock;

		//		tmp_sock = header;
		//		while (tmp_sock != NULL) {
		//			FD_SET(tmp_sock->fw_sock, &read_fds);
		//			if (tmp_sock->fw_sock > max)
		//				max = tmp_sock->fw_sock;
		//			tmp_sock = tmp_sock->next;
		//		}

		if ((rc = select(max + 1, &read_fds, NULL, NULL, &to)) < 0) {
			LOGE("select() failed (%s)", strerror(errno));
			sleep(1);
			continue;
		}

		if (!rc) {
			continue;
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

			//			tmp_sock = (struct st_framework_sock *)malloc(
			//					sizeof(struct st_framework_sock));
			//			tmp_sock->fw_sock = fw_sock;
			//			tmp_sock->next = header;
			//			header = tmp_sock;

			LOGD("Accepted connection from framework,%d,%d", (int)thread_id, t_error);

			if (!t_error) {
				LOGD("Accepted connection from framework fail,%d", t_error);
			}
		}

		//		tmp_sock = header;
		//		prev_sock = NULL;
		//		while (tmp_sock != NULL) {
		//			if (FD_ISSET(tmp_sock->fw_sock, &read_fds)) {
		//				if ((rc = process_framework_command(tmp_sock->fw_sock)) < 0) {
		//					if (rc == -ECONNRESET) {
		//						LOGE("Framework disconnected");
		//						close(tmp_sock->fw_sock);
		//
		//						if (prev_sock == NULL)
		//							header = tmp_sock->next;
		//						else
		//							prev_sock->next = tmp_sock->next;
		//
		//						free(tmp_sock);
		//
		//						if (prev_sock == NULL)
		//							tmp_sock = header;
		//						else
		//							tmp_sock = prev_sock->next;
		//					} else {
		//						LOGE("Error processing framework command (%s)",
		//								strerror(errno));
		//					}
		//				}
		//			} else {
		//				prev_sock = tmp_sock;
		//				tmp_sock = tmp_sock->next;
		//			}
		//		}

	} // while

}

static void * handle_framework_sock_func(void * lpParam) {
	int rc;
	int fw_sock;

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

	return (void*)0;
}
