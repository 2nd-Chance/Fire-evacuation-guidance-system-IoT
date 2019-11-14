#ifndef RoomBuilder_h__
#define RoomBuilder_h__


#include <memory>

#include "Room.h"

namespace model
{
	class RoomBuilder
	{
	public:
		using IdType = Room::IdType;
		using LinkListType = Room::LinkListType;
		using StaticDeviceListType = Room::StaticDeviceListType;
		using DynamicDeviceListType = Room::DynamicDeviceListType;

		RoomBuilder(void);

		RoomBuilder &setId(IdType id);
		RoomBuilder &setLocation(std::shared_ptr<Location> location);
		RoomBuilder &setAlertState(bool state);
		RoomBuilder &setAliveState(bool state);
		RoomBuilder &setLinks(std::shared_ptr<LinkListType> links);
		RoomBuilder &addLink(IdType linkId);
		RoomBuilder &setStaticDevices(
			std::shared_ptr<StaticDeviceListType> devices);
		RoomBuilder &addStaticDevice(
			std::shared_ptr<StaticDevice> device);
		RoomBuilder &setDynamicDevices(
			std::shared_ptr<DynamicDeviceListType> devices);
		RoomBuilder &addDynamicDevice(
			std::shared_ptr<DynamicDevice> device);
		std::shared_ptr<Room> build(void);

	protected:
		IdType getId(void);
		std::shared_ptr<Location> getLocation(void);
		bool getAlertState(void);
		bool getAliveState(void);
		std::shared_ptr<LinkListType> getLinks(void);
		std::shared_ptr<StaticDeviceListType> getStaticDevices(void);
		std::shared_ptr<DynamicDeviceListType> getDynamicDevices(void);

	private:
		IdType id;
		std::shared_ptr<Location> location;
		bool alertState;
		bool aliveState;
		std::shared_ptr<LinkListType> links;
		std::shared_ptr<StaticDeviceListType> staticDevices;
		std::shared_ptr<DynamicDeviceListType> dynamicDevices;
	};
}


#endif // RoomBuilder_h__