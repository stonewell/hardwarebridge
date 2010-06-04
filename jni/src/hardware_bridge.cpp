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

#include <utils/IPCThreadState.h>
#include <utils/ProcessState.h>
#include <utils/IServiceManager.h>

using namespace android;

#include "IHardwareBridge.h"

#include "vibrator.h"
#include "wifi.h"
#include "flashlight.h"

namespace hardwarebridge {
	void HardwareBridgeInitialize();
}

#define CHECK_INTERFACE(interface, data, reply) \
        do { if (!data.enforceInterface(interface::getInterfaceDescriptor())) { \
            LOGW("Call incorrectly routed to " #interface); \
            return PERMISSION_DENIED; \
        } } while (0)

/*
 * Globals
 */

static int ver_major = 1;
static int ver_minor = 0;
int enable_debug = 0;

int main() {
	LOGI("Hardware Bridge Daemon version %d.%d", ver_major, ver_minor);

	sp<ProcessState> proc(ProcessState::self());
	sp<IServiceManager> sm = defaultServiceManager();
	LOGI("ServiceManager: %p", sm.get());

	hardwarebridge::HardwareBridgeInitialize();

	LOGD("Bootstrapping complete");

	ProcessState::self()->startThreadPool();
	IPCThreadState::self()->joinThreadPool();
}

namespace hardwarebridge {

	class BnHardwareBridge: public BnInterface<IHardwareBridge> {
	public:
		BnHardwareBridge() :
			BnInterface<IHardwareBridge> () {
		}

		virtual status_t onTransact(uint32_t code, const Parcel& data,
				Parcel* reply, uint32_t flags = 0) {
			LOGD("OnTransact: %d", code);
			switch (code) {
				case VIBRATOR_ON_TRANSACTION: {
					CHECK_INTERFACE(IHardwareBridge, data, reply);
					int32_t timeout = data.readInt32();
					LOGD("Vibrator Timeout: %d", timeout);
					reply->writeInt32(vibrator_on(timeout));
					return NO_ERROR;
				}
					break;
				case VIBRATOR_OFF_TRANSACTION: {
					CHECK_INTERFACE(IHardwareBridge, data, reply);
					LOGD("Vibrator off");
					reply->writeInt32(vibrator_off());
					return NO_ERROR;
				}
					break;
				case WIFI_LOAD_DRIVER_TRANSACTION: {
					CHECK_INTERFACE(IHardwareBridge, data, reply);
					LOGD("Load wifi driver");
					reply->writeInt32(wifi_load_driver());
					return NO_ERROR;
				}
					break;
				case WIFI_UNLOAD_DRIVER_TRANSACTION: {
					CHECK_INTERFACE(IHardwareBridge, data, reply);
					reply->writeInt32(wifi_unload_driver());
					return NO_ERROR;
				}
					break;
				case WIFI_START_SUPPLICANT_TRANSACTION: {
					CHECK_INTERFACE(IHardwareBridge, data, reply);
					reply->writeInt32(wifi_start_supplicant());
					return NO_ERROR;
				}
					break;
				case WIFI_STOP_SUPPLICANT_TRANSACTION: {
					CHECK_INTERFACE(IHardwareBridge, data, reply);
					reply->writeInt32(wifi_stop_supplicant());
					return NO_ERROR;
				}
					break;
				case WIFI_CONNECT_TO_SUPPLICANT_TRANSACTION: {
					CHECK_INTERFACE(IHardwareBridge, data, reply);
					reply->writeInt32(wifi_connect_to_supplicant());
					return NO_ERROR;
				}
					break;
				case WIFI_CLOSE_SUPPLICANT_CONNECTION_TRANSACTION: {
					CHECK_INTERFACE(IHardwareBridge, data, reply);
					reply->writeInt32(wifi_close_supplicant_connection());
					return NO_ERROR;
				}
					break;
				case WIFI_COMMAND_TRANSACTION: {
					CHECK_INTERFACE(IHardwareBridge, data, reply);
					const char * cmd = data.readString8();
					size_t reply_len = data.readInt32();
					char * reply_buf = new char[reply_len];
					data.read(reply_buf, reply_len);
					int ret = wifi_command(cmd, reply_buf, &reply_len);
					reply->writeInt32(ret);
					if (!ret) {
						reply->writeInt32(reply_len);

						if (reply_len > 0) {
							reply->write(reply_buf, reply_len);
						}
					}

					LOGD("Wifi Command:%s,%d,%s", cmd, ret, reply_buf);
					delete reply_buf;
					return NO_ERROR;
				}
					break;
				case WIFI_WAIT_FOR_EVENT_TRANSACTION: {
					CHECK_INTERFACE(IHardwareBridge, data, reply);
					size_t buflen = data.readInt32();
					char * buf = new char[buflen];
					data.read(buf, buflen);
					int ret = wifi_wait_for_event(buf, buflen);
					reply->writeInt32(ret);
					if (ret >= 0) {
						reply->write(buf, buflen);
					}
					LOGD("Wifi WaitForEvent:%d,%s", ret, buf);
					delete buf;
					return NO_ERROR;
				}
					break;
				case GET_FLASHLIGHT_ENABLED_TRANSACTION: {
					CHECK_INTERFACE(IHardwareBridge, data, reply);
					reply->writeInt32(get_flashlight_enabled());
					return NO_ERROR;
				}
					break;
				case SET_FLASHLIGHT_ENABLED_TRANSACTION: {
					CHECK_INTERFACE(IHardwareBridge, data, reply);
					reply->writeInt32(set_flashlight_enabled(data.readInt32()));
					return NO_ERROR;
				}
				case ENABLE_CAMERA_FLASH_TRANSACTION: {
					CHECK_INTERFACE(IHardwareBridge, data, reply);
					reply->writeInt32(enable_camera_flash(data.readInt32()));
					return NO_ERROR;
				}
					break;
				default:
					return BnInterface<IHardwareBridge>::onTransact(code, data, reply,
							flags);
			}
		}
	};

	class HardwareBridge: public BnHardwareBridge {
		virtual int vibrator_on(int timeout) {
			return vendor_vibrator_on(timeout);
		}

		virtual int vibrator_off() {
			return vendor_vibrator_off();
		}

		virtual int wifi_load_driver() {
			return vendor_wifi_load_driver();
		}
		virtual int wifi_unload_driver() {
			return vendor_wifi_unload_driver();
		}
		virtual int wifi_start_supplicant() {
			return vendor_wifi_start_supplicant();
		}
		virtual int wifi_stop_supplicant() {
			return vendor_wifi_stop_supplicant();
		}
		virtual int wifi_connect_to_supplicant() {
			return vendor_wifi_connect_to_supplicant();
		}
		virtual int wifi_close_supplicant_connection() {
			return vendor_wifi_close_supplicant_connection();
		}
		virtual int wifi_command(const char * cmd, char * reply, size_t * reply_len) {
			return vendor_wifi_command(cmd, reply, reply_len);
		}
		virtual int wifi_wait_for_event(char *buf, size_t buflen) {
			return vendor_wifi_wait_for_event(buf, buflen);
		}
		virtual int get_flashlight_enabled() {
			return vendor_get_flashlight_enabled();
		}
		virtual int set_flashlight_enabled(int on) {
			return vendor_set_flashlight_enabled(on);
		}
		virtual int enable_camera_flash(int milliseconds) {
			return vendor_enable_camera_flash(milliseconds);
		}
	};

	void HardwareBridgeInitialize() {
		defaultServiceManager()->addService(String16(
				"hardwarebridge.IHardwareBridge"), new HardwareBridge());
	}

}
