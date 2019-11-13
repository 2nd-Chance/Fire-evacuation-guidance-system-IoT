#ifdef MODEL_TEST_MODE_ON


#include <iostream>
#include <cinttypes>
#include <memory>
#include <string>

#include "main.h"
#include "Room.h"
#include "RoomBuilder.h"
#include "LocationBuilder.h"
#include "DeviceBuilder.h"
#include "DeviceClassBuilder.h"

int main(void)
{
	std::cout << "<Test Creation With Builder>" << std::endl;
	std::string jsonWithBuilder = testCreationWithBuilder();
	std::cout << jsonWithBuilder << std::endl;
	std::cout << "=============" << std::endl;

	std::cout << "<Test Creation Without Builder>" << std::endl;
	std::string jsonWithoutBuilder = testCreationWithoutBuilder();
	std::cout << jsonWithoutBuilder << std::endl;
	std::cout << "=============" << std::endl;

	std::cout << "<Test Parsing>" << std::endl;
	std::shared_ptr<model::Room> room = testParsing(jsonWithoutBuilder);
	std::cout << "=============" << std::endl;
}

std::string testCreationWithBuilder(void) {
	std::shared_ptr<model::StaticDevice> class3Device1 \
		= model::DeviceBuilder()
		.setUuid("10")
		.setDeviceClass(model::DeviceClass3Builder()
			.build()
		)
		.setBluetoothMac("aa:bb:cc:dd:ee:ff")
		.setRoomId(11)
		.buildStatic();

	std::shared_ptr<model::StaticDevice> class2Device2 \
		= model::DeviceBuilder()
		.setUuid("20")
		.setDeviceClass(model::DeviceClass2Builder()
			.setSensorType("type")
			.setSensorValue("value")
			.build()
		)
		.setBluetoothMac("ff:ee:dd:cc:bb:aa")
		.setRoomId(22)
		.buildStatic();

	std::shared_ptr<model::DynamicDevice> class1Device3 \
		= model::DeviceBuilder()
		.setUuid("30")
		.setDeviceClass(model::DeviceClass1Builder()
			.setSensorType("type2")
			.setSensorValue("value2")
			.build()
		)
		.setBluetoothMac("cc:cc:cc:cc:cc:cc")
		.setRoomId(33)
		.buildDynamic();

	std::shared_ptr<model::Room> room = model::RoomBuilder()
		.setId(1)
		.setLocation(model::LocationBuilder()
			.setLevel(7)
			.setX(5)
			.setY(-5)
			.build()
		)
		.setAlertState(true)
		.setAliveState(false)
		.addLink(2)
		.addLink(3)
		.addStaticDevice(class3Device1)
		.addStaticDevice(class2Device2)
		.addDynamicDevice(class1Device3)
		.build();

	return room->toJson().dump();
}

std::string testCreationWithoutBuilder(void) {
	auto deviceClass3 = std::make_shared<model::DeviceClass3>();
	auto device1 = std::make_shared<model::StaticDevice>(
		"10", deviceClass3);
	device1->setBluetoothMac("aa:bb:cc:dd:ee:ff");
	device1->setRoomId(11);

	auto deviceClass2 = std::make_shared<model::DeviceClass2>(
		"type", "value");
	auto device2 = std::make_shared<model::StaticDevice>(
		"20", deviceClass2);
	device2->setBluetoothMac("ff:ee:dd:cc:bb:aa");
	device2->setRoomId(22);

	auto deviceClass1 = std::make_shared<model::DeviceClass1>(
		"type2", "value2");
	auto device3 = std::make_shared<model::DynamicDevice>(
		"30", deviceClass1);
	device3->setBluetoothMac("cc:cc:cc:cc:cc:cc");
	device3->setRoomId(33);

	auto location = std::make_shared<model::Location>(7, 5, -5);
	auto room = std::make_shared<model::Room>(1, location, true, false);
	room->getLinks()->add(2);
	room->getLinks()->add(3);
	room->getStaticDevices()->add(device1->getUuid());
	room->getStaticDevices()->add(device2->getUuid());
	room->getDynamicDevices()->add(device3->getUuid());

	return room->toJson().dump();
}

std::shared_ptr<model::Room> testParsing(std::string &jsonString) {
	if (nlohmann::json::accept(jsonString))
	{
		nlohmann::json json = nlohmann::json::parse(jsonString);
		std::cout << json.dump(2) << std::endl;

		return model::Room::parse(json);
	}
	else
	{
		std::cout << ">>> CANNOT PARSE! <<<" << std::endl;
		std::cout << jsonString << std::endl;
	}
}


#endif