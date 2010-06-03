#ifndef __IHARDWARE_BRIDGE_H__
#define __IHARDWARE_BRIDGE_H__

#ifdef BUILD_HARDWARE_LEGACY
#include <binder/IInterface.h>
#else
#include <utils/IInterface.h>
#endif

#include <utils/String16.h>

namespace hardwarebridge {

class IHardwareBridge: public android::IInterface {
public:
	DECLARE_META_INTERFACE(HardwareBridge);

	virtual int vibrator_on(int timeout) = 0;
	virtual int vibrator_off() = 0;
	virtual int wifi_load_driver() = 0;
	virtual int wifi_unload_driver() = 0;
	virtual int wifi_start_supplicant() = 0;
	virtual int wifi_stop_supplicant() = 0;
	virtual int wifi_connect_to_supplicant() = 0;
	virtual int wifi_close_supplicant_connection() = 0;
	virtual int wifi_command(const char * cmd, char * reply, size_t * reply_len) = 0;
	virtual int wifi_wait_for_event(char *buf, size_t buflen) = 0;

	enum {
		VIBRATOR_ON_TRANSACTION = android::IBinder::FIRST_CALL_TRANSACTION,
		VIBRATOR_OFF_TRANSACTION,
		WIFI_LOAD_DRIVER_TRANSACTION,
		WIFI_UNLOAD_DRIVER_TRANSACTION,
		WIFI_START_SUPPLICANT_TRANSACTION,
		WIFI_STOP_SUPPLICANT_TRANSACTION,
		WIFI_CONNECT_TO_SUPPLICANT_TRANSACTION,
		WIFI_CLOSE_SUPPLICANT_CONNECTION_TRANSACTION,
		WIFI_COMMAND_TRANSACTION,
		WIFI_WAIT_FOR_EVENT_TRANSACTION,
	};
};

}
;

#endif
