#include "RoomBuilder.h"

namespace model
{
	RoomBuilder::IdType RoomBuilder::getId(void)
	{
		return this->id;
	}
	
	RoomBuilder::RoomBuilder(void)
	{
		this->setLinks(std::make_shared<LinkListType>());
		this->setStaticDevices(std::make_shared<StaticDeviceListType>());
		this->setDynamicDevices(std::make_shared<DynamicDeviceListType>());
	}

	RoomBuilder &RoomBuilder::setId(IdType id)
	{
		this->id = id;
		return *this;
	}
	
	std::shared_ptr<Location> RoomBuilder::getLocation(void)
	{
		return this->location;
	}
	
	RoomBuilder &RoomBuilder::setLocation(std::shared_ptr<Location> location)
	{
		this->location = location;
		return *this;
	}
	
	bool RoomBuilder::getAlertState(void)
	{
		return this->alertState;
	}
	
	RoomBuilder &RoomBuilder::setAlertState(bool state)
	{
		this->alertState = state;
		return *this;
	}
	
	bool RoomBuilder::getAliveState(void)
	{
		return this->aliveState;
	}
	
	RoomBuilder &RoomBuilder::setAliveState(bool state)
	{
		this->aliveState = state;
		return *this;
	}
	
	std::shared_ptr<RoomBuilder::LinkListType> RoomBuilder::getLinks(void)
	{
		return this->links;
	}
	
	RoomBuilder &RoomBuilder::setLinks(std::shared_ptr<LinkListType> links)
	{
		this->links = links;
		return *this;
	}

	RoomBuilder &RoomBuilder::addLink(IdType linkId)
	{
		this->getLinks()->add(linkId);
		return *this;
	}
	
	std::shared_ptr<RoomBuilder::StaticDeviceListType> RoomBuilder::getStaticDevices(void)
	{
		return this->staticDevices;
	}
	
	RoomBuilder &RoomBuilder::setStaticDevices(std::shared_ptr<StaticDeviceListType> devices)
	{
		this->staticDevices = devices;
		return *this;
	}

	RoomBuilder &RoomBuilder::addStaticDevice(std::shared_ptr<StaticDevice> device)
	{
		this->getStaticDevices()->add(device->getUuid());
		return *this;
	}
	
	std::shared_ptr<RoomBuilder::DynamicDeviceListType> RoomBuilder::getDynamicDevices(void)
	{
		return this->dynamicDevices;
	}

	RoomBuilder &RoomBuilder::setDynamicDevices(std::shared_ptr<DynamicDeviceListType> devices)
	{
		this->dynamicDevices = devices;
		return *this;
	}
	
	RoomBuilder &RoomBuilder::addDynamicDevice(std::shared_ptr<DynamicDevice> device)
	{
		this->getDynamicDevices()->add(device->getUuid());
		return *this;
	}

	std::shared_ptr<Room> RoomBuilder::build(void)
	{
		auto room = std::make_shared<Room>(this->getId());
		room->setLocation(this->getLocation());
		room->setAlertState(this->getAlertState());
		room->setAliveState(this->getAliveState());
		room->setLinks(this->getLinks());
		room->setStaticDevices(this->getStaticDevices());
		room->setDynamicDevices(this->getDynamicDevices());
		return room;
	}	
}