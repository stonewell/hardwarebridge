#define LOG_TAG "hardware_bridge"

#include <cutils/log.h>
#include <binder/IServiceManager.h>
#include <utils/threads.h>

#include "hardware_legacy/IHardwareBridge.h"

using namespace android;

static sp<hardwarebridge::IHardwareBridge> gHardwareBridge;
static Mutex gLock;

const sp<hardwarebridge::IHardwareBridge>& get_hardware_bridge() {
	Mutex::Autolock _l(gLock);
	//if (gHardwareBridge.get() == 0) {
	//sp<hardwarebridge::IHardwareBridge> gHardwareBridge;
		sp<IServiceManager> sm = defaultServiceManager();
		sp<IBinder> binder;
		do {
			binder = sm->getService(String16("hardwarebridge.IHardwareBridge"));
			if (binder != 0)
				break;
			LOGW("HardwareBridge not published, waiting...");
			usleep(500000); // 0.5 s
		} while (true);
		gHardwareBridge = interface_cast<hardwarebridge::IHardwareBridge> (binder);
	//}
	LOGE_IF(gHardwareBridge == 0, "no HardwareBridge!?");
	return gHardwareBridge;
}

extern "C" {

	int vibrator_on(int timeout) {
		return get_hardware_bridge()->vibrator_on(timeout);
	}

	int vibrator_off() {
		return get_hardware_bridge()->vibrator_off();
	}

	int wifi_load_driver() {
		return get_hardware_bridge()->wifi_load_driver();
	}

	int wifi_unload_driver() {
		return get_hardware_bridge()->wifi_unload_driver();
	}

	int wifi_start_supplicant() {
		return get_hardware_bridge()->wifi_start_supplicant();
	}

	int wifi_stop_supplicant() {
		return get_hardware_bridge()->wifi_stop_supplicant();
	}

	int wifi_connect_to_supplicant() {
		return get_hardware_bridge()->wifi_connect_to_supplicant();
	}

	int wifi_close_supplicant_connection() {
		return get_hardware_bridge()->wifi_close_supplicant_connection();
	}

	int wifi_command(const char * cmd, char * reply, size_t * reply_len) {
		return get_hardware_bridge()->wifi_command(cmd, reply, reply_len);
	}

	int wifi_wait_for_event(char *buf, size_t buflen) {
		return get_hardware_bridge()->wifi_wait_for_event(buf, buflen);
	}

	int get_flashlight_enabled() {
		return get_hardware_bridge()->get_flashlight_enabled();
	}

	int set_flashlight_enabled(int on) {
		return get_hardware_bridge()->set_flashlight_enabled(on);
	}

	int enable_camera_flash(int milliseconds) {
		return get_hardware_bridge()->enable_camera_flash(milliseconds);
	}
}
