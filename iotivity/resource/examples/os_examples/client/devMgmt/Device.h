#ifndef Device_h__
#define Device_h__


#include <memory>
#include <string>

#include "DeviceClass.h"

namespace model
{
	class Device
	{
		friend class DeviceBuilder;

	public:
		using UuidType = std::string;
		using DeviceTypeType = std::string;
		using BtMacType = std::string;
		using RoomIdType = std::uint32_t;
		using JsonType = nlohmann::json;

		enum class JsonEnum
		{
			UUID, DEVICE_TYPE, DEVICE_CLASS, BLUETOOTH_MAC, ROOM_ID
		};
		static std::string getJsonKey(JsonEnum jsonEnum);
		static std::shared_ptr<Device> parse(JsonType json);

		UuidType getUuid(void);
		void setUuid(UuidType uuid);
		DeviceTypeType getDeviceType(void);
		std::shared_ptr<DeviceClass> getDeviceClass(void);
		void setDeviceClass(std::shared_ptr<DeviceClass> deviceClass);
		BtMacType getBluetoothMac(void);
		void setBluetoothMac(BtMacType bluetoothMac);
		RoomIdType getRoomId(void);
		void setRoomId(RoomIdType roomId);
		JsonType toJson(void);

	protected:
		Device(UuidType uuid, std::shared_ptr<DeviceClass> deviceClass);
		Device(UuidType uuid, std::shared_ptr<DeviceClass> deviceClass,
			BtMacType bluetoothMac);
		Device(UuidType uuid, std::shared_ptr<DeviceClass> deviceClass, 
			BtMacType bluetoothMac, RoomIdType roomId);

		void setDeviceType(DeviceTypeType deviceType);

	private:
		UuidType uuid;
		DeviceTypeType deviceType;
		std::shared_ptr<DeviceClass> deviceClass;
		BtMacType bluetoothMac;
		RoomIdType roomId;
	};
	
	class StaticDevice : public Device
	{
	public:
		StaticDevice(UuidType uuid, std::shared_ptr<DeviceClass> \
			deviceClass) : StaticDevice(uuid, deviceClass, "") {}
		StaticDevice(UuidType uuid, std::shared_ptr<DeviceClass> \
			deviceClass, BtMacType bluetoothMac) \
			: StaticDevice(uuid, deviceClass, bluetoothMac, 0) {}
		StaticDevice(UuidType uuid, std::shared_ptr<DeviceClass> \
			deviceClass, BtMacType bluetoothMac, RoomIdType roomId) \
			: Device(uuid, deviceClass, bluetoothMac, roomId)
		{
			this->setDeviceType("s");
		}
	
		static std::shared_ptr<StaticDevice> parse(JsonType json);
	};

	class DynamicDevice : public Device
	{
	public:
		DynamicDevice(UuidType uuid, std::shared_ptr<DeviceClass> \
			deviceClass) : DynamicDevice(uuid, deviceClass, "") {}
		DynamicDevice(UuidType uuid, std::shared_ptr<DeviceClass> \
			deviceClass, BtMacType bluetoothMac) \
			: DynamicDevice(uuid, deviceClass, bluetoothMac, 0) {}
		DynamicDevice(UuidType uuid, std::shared_ptr<DeviceClass> \
			deviceClass, BtMacType bluetoothMac, RoomIdType roomId) \
			: Device(uuid, deviceClass, bluetoothMac, roomId)
		{
			this->setDeviceType("d");
		}

		static std::shared_ptr<DynamicDevice> parse(JsonType json);
	};
}


#endif // Device_h__