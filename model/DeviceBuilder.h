#ifndef DeviceBuilder_h__
#define DeviceBuilder_h__


#include "Device.h"

namespace model
{
	class DeviceBuilder
	{
	public:
		using UuidType = Device::UuidType;
		using BtMacType = Device::BtMacType;
		using RoomIdType = Device::RoomIdType;

		DeviceBuilder &setUuid(UuidType uuid);
		DeviceBuilder &setDeviceClass(std::shared_ptr<DeviceClass> \
			deviceClass);
		DeviceBuilder &setBluetoothMac(BtMacType bluetoothMac);
		DeviceBuilder &setRoomId(RoomIdType roomId);
		std::shared_ptr<StaticDevice> buildStatic(void);
		std::shared_ptr<DynamicDevice> buildDynamic(void);

	protected:
		UuidType getUuid(void);
		std::shared_ptr<DeviceClass> getDeviceClass(void);
		BtMacType getBluetoothMac(void);
		RoomIdType getRoomId(void);

	private:
		UuidType uuid;
		std::shared_ptr<DeviceClass> deviceClass;
		BtMacType bluetoothMac;
		RoomIdType roomId = 0;
	};
}


#endif // DeviceBuilder_h__