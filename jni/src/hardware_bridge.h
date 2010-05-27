#ifndef __HARDWARE_BRIDGE_H__
#define __HARDWARE_BRIDGE_H__

#include <jni.h>
#include <android/log.h>

#define  LOG_TAG    "hardware_bridge"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define  LOGD(...)  { if (enable_debug) __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__); }

#define ANDROID_SOCKET_ENV_PREFIX   "ANDROID_SOCKET_"
#define ANDROID_SOCKET_DIR      "/dev/socket"

/*
 *  android_get_control_socket - simple helper function to get the file
 *  descriptor of our init-managed Unix domain socket. `name' is the name of the
 *  socket, as given in init.rc. Returns -1 on error.
 *  
 *  This is inline and not in libcutils proper because we want to use this in
 *  third-party daemons with minimal modification.
 */
static inline int android_get_control_socket(const char *name)
{
		char key[64] = ANDROID_SOCKET_ENV_PREFIX;
		const char *val;
		int fd;

		/* build our environment variable, counting cycles like a wolf ... */
#if HAVE_STRLCPY
		strlcpy(key + sizeof(ANDROID_SOCKET_ENV_PREFIX) - 1,
						name,
						sizeof(key) - sizeof(ANDROID_SOCKET_ENV_PREFIX));
#else   /* for the host, which may lack the almightly strncpy ... */
		strncpy(key + sizeof(ANDROID_SOCKET_ENV_PREFIX) - 1,
						name,
						sizeof(key) - sizeof(ANDROID_SOCKET_ENV_PREFIX));
		key[sizeof(key)-1] = '\0';
#endif
		val = getenv(key);
		if (!val)
				return -1;

		errno = 0;
		fd = strtol(val, NULL, 10);
		if (errno)
				return -1;

		return fd;
}

#endif

