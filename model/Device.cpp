#include <stdexcept>

#include "Device.h"

namespace model
{
	Device::Device(UuidType uuid, std::shared_ptr<DeviceClass> deviceClass) \
		: Device(uuid, deviceClass, "") {}

	Device::Device(UuidType uuid, std::shared_ptr<DeviceClass> deviceClass, \
		BtMacType bluetoothMac) \
		: Device(uuid, deviceClass, bluetoothMac, 0) {}

	Device::Device(UuidType uuid, std::shared_ptr<DeviceClass> deviceClass, \
		BtMacType bluetoothMac, RoomIdType roomId)
	{
		this->setUuid(uuid);
		this->setDeviceClass(deviceClass);
		this->setBluetoothMac(bluetoothMac);
		this->setRoomId(roomId);
	}

	std::string Device::getJsonKey(JsonEnum jsonEnum)
	{
		switch (jsonEnum)
		{
		case model::Device::JsonEnum::UUID:
			return "id";
		case model::Device::JsonEnum::DEVICE_CLASS:
			return "cl";
		case model::Device::JsonEnum::BLUETOOTH_MAC:
			return "bm";
		case model::Device::JsonEnum::ROOM_ID:
			return "rm";
		default:
			return "?";
		}
	}

	Device::UuidType Device::getUuid(void)
	{
		return this->uuid;
	}
	
	void Device::setUuid(UuidType uuid)
	{
		this->uuid = uuid;
	}

	std::shared_ptr<DeviceClass> Device::getDeviceClass(void)
	{
		return this->deviceClass;
	}

	void Device::setDeviceClass(std::shared_ptr<DeviceClass> deviceClass)
	{
		if (deviceClass == nullptr)
		{
			throw std::invalid_argument("Device class cannot be a nullptr.");
		}

		this->deviceClass = deviceClass;
	}

	Device::BtMacType Device::getBluetoothMac(void)
	{
		return this->bluetoothMac;
	}
	
	void Device::setBluetoothMac(BtMacType bluetoothMac)
	{
		this->bluetoothMac = bluetoothMac;
	}
	
	Device::RoomIdType Device::getRoomId(void)
	{
		return this->roomId;
	}
	
	void Device::setRoomId(RoomIdType roomId)
	{
		this->roomId = roomId;
	}
	
	Device::JsonType Device::toJson(void)
	{
		JsonType json = JsonType::object();
		json["id"] = this->getUuid();
		json["cl"] = this->getDeviceClass()->toJson();
		json["bm"] = this->getBluetoothMac();
		json["rm"] = this->getRoomId();
		return json;
	}
	
	std::shared_ptr<DynamicDevice> DynamicDevice::parse(JsonType json)
	{
		UuidType uuid = json[Device::getJsonKey(JsonEnum::UUID)];
		std::shared_ptr<DeviceClass> deviceClass \
			= DeviceClass::parse(
				json[Device::getJsonKey(JsonEnum::DEVICE_CLASS)]);
		BtMacType bluetoothMac \
			= json[Device::getJsonKey(JsonEnum::BLUETOOTH_MAC)];
		RoomIdType roomId \
			= json[Device::getJsonKey(JsonEnum::ROOM_ID)];

		return std::make_shared<DynamicDevice>(
			uuid, deviceClass, bluetoothMac, roomId);
	}
	
	std::shared_ptr<StaticDevice> StaticDevice::parse(JsonType json)
	{
		UuidType uuid = json[Device::getJsonKey(JsonEnum::UUID)];
		std::shared_ptr<DeviceClass> deviceClass \
			= DeviceClass::parse(
				json[Device::getJsonKey(JsonEnum::DEVICE_CLASS)]);
		BtMacType bluetoothMac \
			= json[Device::getJsonKey(JsonEnum::BLUETOOTH_MAC)];
		RoomIdType roomId \
			= json[Device::getJsonKey(JsonEnum::ROOM_ID)];

		return std::make_shared<StaticDevice>(
			uuid, deviceClass, bluetoothMac, roomId);
	}
}