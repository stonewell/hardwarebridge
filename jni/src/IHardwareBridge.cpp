#include <stdint.h>
#include <sys/types.h>

#include <binder/Parcel.h>
#include <binder/IServiceManager.h>

#include "IHardwareBridge.h"
#include "vibrator.h"

using namespace android;

namespace hardwarebridge {

enum {
	VIBRATOR_ON_TRANSACTION = IBinder::FIRST_CALL_TRANSACTION,
	VIBRATOR_OFF_TRANSACTION,
};

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

};

class BnHardwareBridge: public BnInterface<IHardwareBridge> {
public:
	BnHardwareBridge() :
		BnInterface<IHardwareBridge> () {
	}

	virtual status_t onTransact(uint32_t code, const Parcel& data,
			Parcel* reply, uint32_t flags = 0) {
		switch (code) {
		case VIBRATOR_ON_TRANSACTION: {
			CHECK_INTERFACE(IHardwareBridge, data, reply);
			int32_t timeout = data.readInt32();
			reply->writeInt32(vibrator_on(timeout));
			return NO_ERROR;
		}
			break;
		case VIBRATOR_OFF_TRANSACTION: {
			CHECK_INTERFACE(IHardwareBridge, data, reply);
			reply->writeInt32(vibrator_off());
			return NO_ERROR;
		}
			break;
		default:
			return BnInterface<IHardwareBridge>::onTransact(code, data, reply, flags);
		}
	}
};

class HardwareBridge : public BnHardwareBridge {
	virtual int vibrator_on(int timeout) {
		return vendor_vibrator_on(timeout);
	}

	virtual int vibrator_off() {
		return vendor_vibrator_off();
	}
};

void HardwareBridgeInitialize() {
	defaultServiceManager()->addService(String16(
			"hardwarebridge.IHardwareBridge"), new HardwareBridge());
}

IMPLEMENT_META_INTERFACE(HardwareBridge, "hardwarebridge.IHardwareBridge");
};
