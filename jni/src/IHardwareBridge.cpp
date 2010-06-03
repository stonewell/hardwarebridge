#include <stdint.h>
#include <sys/types.h>

#ifdef BUILD_HARDWARE_LEGACY
#include <binder/Parcel.h>
#include <binder/IServiceManager.h>
#else
#include <utils/Parcel.h>
#include <utils/IServiceManager.h>
#endif
#include <utils/String8.h>

using namespace android;

#include "hardware_legacy/IHardwareBridge.h"

namespace hardwarebridge {

class BpHardwareBridge: public BpInterface<IHardwareBridge> {
public:
	BpHardwareBridge(const sp<IBinder>& impl) :
		BpInterface<IHardwareBridge> (impl) {
	}

	virtual int vibrator_on(int timeout) {
		Parcel data, reply;
		data.writeInterfaceToken(IHardwareBridge::getInterfaceDescriptor());
		data.writeInt32(timeout);
		remote()->transact(VIBRATOR_ON_TRANSACTION, data, &reply);
		return reply.readInt32();
	}

	virtual int vibrator_off() {
		Parcel data, reply;
		data.writeInterfaceToken(IHardwareBridge::getInterfaceDescriptor());
		remote()->transact(VIBRATOR_OFF_TRANSACTION, data, &reply);
		return reply.readInt32();
	}

	virtual int wifi_load_driver() {
		Parcel data, reply;
		data.writeInterfaceToken(IHardwareBridge::getInterfaceDescriptor());
		remote()->transact(WIFI_LOAD_DRIVER_TRANSACTION, data, &reply);
		return reply.readInt32();
	}
	virtual int wifi_unload_driver() {
		Parcel data, reply;
		data.writeInterfaceToken(IHardwareBridge::getInterfaceDescriptor());
		remote()->transact(WIFI_UNLOAD_DRIVER_TRANSACTION, data, &reply);
		return reply.readInt32();
	}
	virtual int wifi_start_supplicant() {
		Parcel data, reply;
		data.writeInterfaceToken(IHardwareBridge::getInterfaceDescriptor());
		remote()->transact(WIFI_START_SUPPLICANT_TRANSACTION, data, &reply);
		return reply.readInt32();
	}
	virtual int wifi_stop_supplicant() {
		Parcel data, reply;
		data.writeInterfaceToken(IHardwareBridge::getInterfaceDescriptor());
		remote()->transact(WIFI_STOP_SUPPLICANT_TRANSACTION, data, &reply);
		return reply.readInt32();
	}
	virtual int wifi_connect_to_supplicant() {
		Parcel data, reply;
		data.writeInterfaceToken(IHardwareBridge::getInterfaceDescriptor());
		remote()->transact(WIFI_CONNECT_TO_SUPPLICANT_TRANSACTION, data, &reply);
		return reply.readInt32();
	}
	virtual int wifi_close_supplicant_connection() {
		Parcel data, reply;
		data.writeInterfaceToken(IHardwareBridge::getInterfaceDescriptor());
		remote()->transact(WIFI_CLOSE_SUPPLICANT_CONNECTION_TRANSACTION, data,
				&reply);
		return reply.readInt32();
	}
	virtual int wifi_command(const char * cmd, char * reply_buf,
			size_t * reply_len) {
		Parcel data, reply;
		data.writeInterfaceToken(IHardwareBridge::getInterfaceDescriptor());
		data.writeString8(String8(cmd));
		data.writeInt32(*reply_len);
		data.write(reply_buf, *reply_len);

		remote()->transact(WIFI_COMMAND_TRANSACTION, data, &reply);

		int ret = reply.readInt32();

		if (!ret) {
			*reply_len = reply.readInt32();

			if (reply_len > 0)
				reply.read(reply_buf, *reply_len);
		}
		return ret;
	}
	virtual int wifi_wait_for_event(char *buf, size_t buflen) {
		Parcel data, reply;
		data.writeInterfaceToken(IHardwareBridge::getInterfaceDescriptor());
		data.writeInt32(buflen);
		data.write(buf, buflen);
		remote()->transact(WIFI_WAIT_FOR_EVENT_TRANSACTION, data, &reply);

		int ret = reply.readInt32();

		if (!ret) {
			reply.read(buf, buflen);
		}

		return ret;
	}
};

IMPLEMENT_META_INTERFACE(HardwareBridge, "hardwarebridge.IHardwareBridge");
};
