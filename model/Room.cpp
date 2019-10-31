#include <algorithm>

#include "Room.h"

namespace model
{
	Room::Room(IdType id) : Room(id, std::make_shared<Location>()) {}

	Room::Room(IdType id, std::shared_ptr<Location> location) : Room(id, location, false, true) {}

	Room::Room(IdType id, std::shared_ptr<Location> location, bool alertState, bool aliveState)
	{
		this->setId(id);
		this->setLocation(location);
		this->setAlertState(alertState);
		this->setAliveState(aliveState);

		this->setLinks(std::make_shared<LinkListType>());
		this->setStaticDevices(std::make_shared<StaticDeviceListType>());
		this->setDynamicDevices(std::make_shared<DynamicDeviceListType>());
	}

	std::string Room::getJsonKey(JsonEnum jsonEnum)
	{
		switch (jsonEnum)
		{
		case model::Room::JsonEnum::ID:
			return "id";
		case model::Room::JsonEnum::LOCATION:
			return "lc";
		case model::Room::JsonEnum::ALERT_STATE:
			return "ar";
		case model::Room::JsonEnum::ALIVE_STATE:
			return "al";
		case model::Room::JsonEnum::LINKS:
			return "lk";
		case model::Room::JsonEnum::STATIC_DEVICES:
			return "sd";
		case model::Room::JsonEnum::DYNAMIC_DEVICES:
			return "dd";
		default:
			return "?";
		}
	}

	std::shared_ptr<Room> Room::parse(JsonType &json)
	{
		IdType id = json[Room::getJsonKey(JsonEnum::ID)];
		std::shared_ptr<Location> location =
			model::Location::parse(json[Room::getJsonKey(JsonEnum::LOCATION)]);
		bool alertState = json[Room::getJsonKey(JsonEnum::ALERT_STATE)] != 0;
		bool aliveState = json[Room::getJsonKey(JsonEnum::ALIVE_STATE)] != 0;
		std::shared_ptr<LinkListType> links =
			LinkListType::parse(json[Room::getJsonKey(JsonEnum::LINKS)]);
		std::shared_ptr<StaticDeviceListType> staticDevices =
			StaticDeviceListType::parse(json[Room::getJsonKey(JsonEnum::STATIC_DEVICES)]);
		std::shared_ptr<DynamicDeviceListType> dynamicDevices =
			DynamicDeviceListType::parse(json[Room::getJsonKey(JsonEnum::DYNAMIC_DEVICES)]);

		auto room = std::make_shared<Room>(id, location, alertState, aliveState);
		room->setLinks(links);
		room->setStaticDevices(staticDevices);
		room->setDynamicDevices(dynamicDevices);
		return room;
	}

	Room::IdType Room::getId(void)
	{
		return this->id;
	}
	
	void Room::setId(IdType id)
	{
		this->id = id;
	}

	std::shared_ptr<Location> Room::getLocation(void)
	{
		return this->location;
	}

	void Room::setLocation(std::shared_ptr<Location> location)
	{
		this->location = location;
	}

	bool Room::getAlertState(void)
	{
		return this->alertState;
	}

	void Room::setAlertState(bool state)
	{
		this->alertState = state;
	}

	bool Room::getAliveState(void)
	{
		return this->aliveState;
	}

	void Room::setAliveState(bool state)
	{
		this->aliveState = state;
	}

	std::shared_ptr<Room::LinkListType> Room::getLinks(void)
	{
		return this->links;
	}

	void Room::setLinks(std::shared_ptr<LinkListType> links)
	{
		this->links = links;
	}

	std::shared_ptr<Room::StaticDeviceListType> Room::getStaticDevices(void)
	{
		return this->staticDevices;
	}

	void Room::setStaticDevices(std::shared_ptr<StaticDeviceListType> devices)
	{
		this->staticDevices = devices;
	}

	std::shared_ptr<Room::DynamicDeviceListType> Room::getDynamicDevices(void)
	{
		return this->dynamicDevices;
	}

	void Room::setDynamicDevices(std::shared_ptr<DynamicDeviceListType> devices)
	{
		this->dynamicDevices = devices;
	}

	Room::JsonType Room::toJson(void)
	{
		JsonType json = JsonType::object();
		json[Room::getJsonKey(JsonEnum::ID)] = this->getId();
		json[Room::getJsonKey(JsonEnum::LOCATION)] = this->getLocation()->toJson();
		json[Room::getJsonKey(JsonEnum::ALERT_STATE)] = static_cast<int>(this->getAlertState());
		json[Room::getJsonKey(JsonEnum::ALIVE_STATE)] = static_cast<int>(this->getAliveState());
		json[Room::getJsonKey(JsonEnum::LINKS)] = this->getLinks()->toJson();
		json[Room::getJsonKey(JsonEnum::STATIC_DEVICES)] = this->getStaticDevices()->toJson();
		json[Room::getJsonKey(JsonEnum::DYNAMIC_DEVICES)] = this->getDynamicDevices()->toJson();
		return json;
	}
}