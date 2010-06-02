#ifndef __IHARDWARE_BRIDGE_H__
#define __IHARDWARE_BRIDGE_H__

#include <binder/IInterface.h>
#include <utils/String16.h>

namespace hardwarebridge {

	class IHardwareBridge : public android::IInterface
	{
		public:
			DECLARE_META_INTERFACE(HardwareBridge);

			virtual int vibrator_on(int timeout) = 0;
			virtual int vibrator_off() = 0;
	};

	void HardwareBridgeInitialize();
};

#endif
