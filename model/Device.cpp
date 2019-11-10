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
		case JsonEnum::UUID:
			return "id";
		case JsonEnum::DEVICE_TYPE:
			return "dt";
		case JsonEnum::DEVICE_CLASS:
			return "cl";
		case JsonEnum::BLUETOOTH_MAC:
			return "bm";
		case JsonEnum::ROOM_ID:
			return "rm";
		default:
			return "?";
		}
	}

	std::shared_ptr<Device> Device::parse(JsonType json)
	{
		DeviceTypeType deviceType = json[Device::getJsonKey(JsonEnum::DEVICE_TYPE)];

		if (deviceType == "s")
		{
			return StaticDevice::parse(json);
		}
		else if (deviceType == "d")
		{
			return DynamicDevice::parse(json);
		}
		else
		{
			return nullptr;
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

	Device::DeviceTypeType Device::getDeviceType(void)
	{
		return this->deviceType;
	}

	void Device::setDeviceType(DeviceTypeType deviceType)
	{
		this->deviceType = deviceType;
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
		json[Device::getJsonKey(JsonEnum::UUID)] \
			= this->getUuid();
		json[Device::getJsonKey(JsonEnum::DEVICE_TYPE)] \
			= this->getDeviceType();
		json[Device::getJsonKey(JsonEnum::DEVICE_CLASS)] \
			= this->getDeviceClass()->toJson();
		json[Device::getJsonKey(JsonEnum::BLUETOOTH_MAC)] \
			= this->getBluetoothMac();
		json[Device::getJsonKey(JsonEnum::ROOM_ID)] \
			= this->getRoomId();
		return json;
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
}