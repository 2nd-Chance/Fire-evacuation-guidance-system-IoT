#include "DeviceClassBuilder.h"

namespace model
{
	/*
		Base class
	*/

	bool DeviceClassBuilder::getAlertState(void)
	{
		return this->alertState;
	}

	DeviceClassBuilder &DeviceClassBuilder::setAlertState(bool state)
	{
		this->alertState = state;
		return *this;
	}
	
	bool DeviceClassBuilder::getAliveState(void)
	{
		return this->aliveState;
	}
	
	DeviceClassBuilder &DeviceClassBuilder::setAliveState(bool state)
	{
		this->aliveState = state;
		return *this;
	}

	/*
		Class 3
	*/

	std::shared_ptr<DeviceClass> DeviceClass3Builder::build(void)
	{
		auto class3 = std::make_shared<DeviceClass3>();
		class3->setAlertState(this->getAlertState());
		class3->setAliveState(this->getAliveState());
		return class3;
	}

	/*
		Class 2
	*/

	std::string DeviceClass2Builder::getSensorType(void)
	{
		return this->sensorType;
	}
	
	model::DeviceClass2Builder & DeviceClass2Builder::setSensorType(
		const std::string &sensorType)
	{
		this->sensorType = sensorType;
		return *this;
	}

	std::string DeviceClass2Builder::getSensorValue(void)
	{
		return this->sensorValue;
	}
	
	model::DeviceClass2Builder & DeviceClass2Builder::setSensorValue(
		const std::string &sensorValue)
	{
		this->sensorValue = sensorValue;
		return *this;
	}

	std::shared_ptr<DeviceClass> DeviceClass2Builder::build(void)
	{
		auto class2 = std::make_shared<DeviceClass2>();
		class2->setAlertState(this->getAlertState());
		class2->setAliveState(this->getAliveState());
		class2->setSensorType(this->getSensorType());
		class2->setSensorValue(this->getSensorValue());
		return class2;
	}

	/*
		Class 1
	*/
	
	std::shared_ptr<DeviceClass> DeviceClass1Builder::build(void)
	{
		auto class1 = std::make_shared<DeviceClass1>();
		class1->setAlertState(this->getAlertState());
		class1->setAliveState(this->getAliveState());
		class1->setSensorType(this->getSensorType());
		class1->setSensorValue(this->getSensorValue());
		return class1;
	}

}