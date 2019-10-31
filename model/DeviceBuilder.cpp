#include "DeviceBuilder.h"

namespace model
{
	DeviceBuilder::UuidType DeviceBuilder::getUuid(void)
	{
		return this->uuid;
	}
	
	DeviceBuilder &model::DeviceBuilder::setUuid(UuidType uuid)
	{
		this->uuid = uuid;
		return *this;
	}
	
	std::shared_ptr<DeviceClass> DeviceBuilder::getDeviceClass(void)
	{
		return this->deviceClass;
	}
	
	DeviceBuilder &DeviceBuilder::setDeviceClass(std::shared_ptr<DeviceClass> deviceClass)
	{
		this->deviceClass = deviceClass;
		return *this;
	}
	
	DeviceBuilder::BtMacType DeviceBuilder::getBluetoothMac(void)
	{
		return this->bluetoothMac;
	}

	DeviceBuilder &DeviceBuilder::setBluetoothMac(BtMacType bluetoothMac)
	{
		this->bluetoothMac = bluetoothMac;
		return *this;
	}
	
	DeviceBuilder::RoomIdType DeviceBuilder::getRoomId(void)
	{
		return this->roomId;
	}
	
	DeviceBuilder &DeviceBuilder::setRoomId(RoomIdType roomId)
	{
		this->roomId = roomId;
		return *this;
	}
	
	std::shared_ptr<StaticDevice> DeviceBuilder::buildStatic(void)
	{
		return std::make_shared<StaticDevice>(
			this->getUuid(),
			this->getDeviceClass(),
			this->getBluetoothMac(),
			this->getRoomId()
		);
	}
	
	std::shared_ptr<DynamicDevice> DeviceBuilder::buildDynamic(void)
	{
		return std::make_shared<DynamicDevice>(
			this->getUuid(),
			this->getDeviceClass(),
			this->getBluetoothMac(),
			this->getRoomId()
		);
	}
}