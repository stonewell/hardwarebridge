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
#include "IHardwareBridge.h"

/*
 * Globals
 */

static int ver_major = 1;
static int ver_minor = 0;
int enable_debug = 0;

int main() {
	LOGI("Hardware Bridge Daemon version %d.%d", ver_major, ver_minor);

	hardwarebridge::HardwareBridgeInitialize();

	LOGD("Bootstrapping complete");
}


