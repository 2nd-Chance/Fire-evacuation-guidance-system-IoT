#include "DeviceClass.h"

namespace model
{
	/*
		Base Class
	*/

	std::string DeviceClass::getJsonKey(JsonEnum jsonEnum)
	{
		switch (jsonEnum)
		{
		case JsonEnum::CLASS_ID:
			return "ci";
		case JsonEnum::ALERT_STATE:
			return "ar";
		case JsonEnum::ALIVE_STATE:
			return "al";
		default:
			return "?";
		}
	}

	std::shared_ptr<DeviceClass> DeviceClass::parse(JsonType json)
	{
		uint32_t classId =
			json[DeviceClass::getJsonKey(JsonEnum::CLASS_ID)];

		if (classId == 1)
		{
			return DeviceClass1::parse(json);
		}
		else if (classId == 2)
		{
			return DeviceClass2::parse(json);
		}
		else if (classId == 3)
		{
			return DeviceClass3::parse(json);
		}
		else
		{
			//@ToDo: throw exception
			return nullptr;
		}
	}
	
	uint32_t DeviceClass::getClassId(void)
	{
		return this->classId;
	}

	void DeviceClass::setClassId(uint32_t classId)
	{
		this->classId = classId;
	}
	
	bool DeviceClass::getAlertState(void)
	{
		return this->alertState;
	}
	
	void DeviceClass::setAlertState(bool state)
	{
		this->alertState = state;
	}
	
	bool DeviceClass::getAliveState(void)
	{
		return this->aliveState;
	}

	void DeviceClass::setAliveState(bool state)
	{
		this->aliveState = state;
	}

	DeviceClass::JsonType DeviceClass::toJson(void)
	{
		JsonType json = JsonType::object();
		json[DeviceClass::getJsonKey(JsonEnum::CLASS_ID)] \
			= this->getClassId();
		json[DeviceClass::getJsonKey(JsonEnum::ALERT_STATE)] \
			= static_cast<int>(this->getAlertState());
		json[DeviceClass::getJsonKey(JsonEnum::ALIVE_STATE)] \
			= static_cast<int>(this->getAliveState());
		return json;
	}

	/*
		Class 3
	*/
	
	DeviceClass3::DeviceClass3(void)
	{
		this->setClassId(3);
	}

	std::string DeviceClass3::getJsonKey(JsonEnum jsonEnum)
	{
		switch (jsonEnum)
		{
		case JsonEnum::CLASS_ID:
			return DeviceClass::getJsonKey(DeviceClass::JsonEnum:: \
				CLASS_ID);
		case JsonEnum::ALERT_STATE:
			return DeviceClass::getJsonKey(DeviceClass::JsonEnum:: \
				ALERT_STATE);
		case JsonEnum::ALIVE_STATE:
			return DeviceClass::getJsonKey(DeviceClass::JsonEnum:: \
				ALIVE_STATE);
		default:
			return "?";
		}
	}

	std::shared_ptr<DeviceClass3> DeviceClass3::parse(JsonType json)
	{
		bool alertState = json[DeviceClass3::getJsonKey(JsonEnum:: \
			ALERT_STATE)] != 0;
		bool aliveState = json[DeviceClass3::getJsonKey(JsonEnum:: \
			ALIVE_STATE)] != 0;

		auto class3 = std::make_shared<DeviceClass3>();
		class3->setAlertState(alertState);
		class3->setAliveState(aliveState);		
		return class3;
	}

	DeviceClass::JsonType DeviceClass3::toJson(void)
	{
		return DeviceClass::toJson();
	}

	/*
		Class 2
	*/
	
	DeviceClass2::DeviceClass2(void) : DeviceClass2("", "") {}

	DeviceClass2::DeviceClass2(const std::string &sensorType,
		const std::string &sensorValue) : DeviceClass3()
	{
		this->setClassId(2);
		this->setSensorType(sensorType);
		this->setSensorValue(sensorValue);
	}

	std::string DeviceClass2::getJsonKey(JsonEnum jsonEnum)
	{
		switch (jsonEnum)
		{
		case JsonEnum::CLASS_ID:
			return DeviceClass3::getJsonKey(DeviceClass3::JsonEnum:: \
				CLASS_ID);
		case JsonEnum::ALERT_STATE:
			return DeviceClass3::getJsonKey(DeviceClass3::JsonEnum:: \
				ALERT_STATE);
		case JsonEnum::ALIVE_STATE:
			return DeviceClass3::getJsonKey(DeviceClass3::JsonEnum:: \
				ALIVE_STATE);
		case JsonEnum::SENSOR_TYPE:
			return "st";
		case JsonEnum::SENSOR_VALUE:
			return "sv";
		default:
			return "?";
		}
	}

	std::shared_ptr<DeviceClass2> DeviceClass2::parse(JsonType json)
	{
		bool alertState = json[DeviceClass2::getJsonKey(JsonEnum:: \
			ALERT_STATE)] != 0;
		bool aliveState = json[DeviceClass2::getJsonKey(JsonEnum:: \
			ALIVE_STATE)] != 0;
		std::string sensorType = json[DeviceClass2::getJsonKey(JsonEnum:: \
			SENSOR_TYPE)];
		std::string sensorValue = json[DeviceClass2::getJsonKey(JsonEnum:: \
			SENSOR_VALUE)];

		auto class2 = std::make_shared<DeviceClass2>();
		class2->setAlertState(alertState);
		class2->setAliveState(aliveState);
		class2->setSensorType(sensorType);
		class2->setSensorValue(sensorValue);
		return class2;
	}

	std::string DeviceClass2::getSensorType(void)
	{
		return this->sensorType;
	}
	
	void DeviceClass2::setSensorType(const std::string &sensorType)
	{
		this->sensorType = sensorType;
	}
	
	std::string DeviceClass2::getSensorValue(void)
	{
		return this->sensorValue;
	}

	void DeviceClass2::setSensorValue(const std::string &sensorValue)
	{
		this->sensorValue = sensorValue;
	}

	DeviceClass::JsonType DeviceClass2::toJson(void)
	{
		JsonType json = DeviceClass3::toJson();
		json[DeviceClass2::getJsonKey(JsonEnum::SENSOR_TYPE)] \
			= this->getSensorType();
		json[DeviceClass2::getJsonKey(JsonEnum::SENSOR_VALUE)] \
			= this->getSensorValue();
		return json;
	}
	
	/*
		Class 1
	*/

	DeviceClass1::DeviceClass1(void) : DeviceClass1("", "") {}

	DeviceClass1::DeviceClass1(const std::string &sensorType, 
		const std::string &sensorValue) : \
		DeviceClass2(sensorType, sensorValue)
	{
		this->setClassId(1);
	}

	std::string DeviceClass1::getJsonKey(JsonEnum jsonEnum)
	{
		switch (jsonEnum)
		{
		case JsonEnum::CLASS_ID:
			return DeviceClass2::getJsonKey(DeviceClass2::JsonEnum:: \
				CLASS_ID);
		case JsonEnum::ALERT_STATE:
			return DeviceClass2::getJsonKey(DeviceClass2::JsonEnum:: \
				ALERT_STATE);
		case JsonEnum::ALIVE_STATE:
			return DeviceClass2::getJsonKey(DeviceClass2::JsonEnum:: \
				ALIVE_STATE);
		case JsonEnum::SENSOR_TYPE:
			return DeviceClass2::getJsonKey(DeviceClass2::JsonEnum:: \
				SENSOR_TYPE);
		case JsonEnum::SENSOR_VALUE:
			return DeviceClass2::getJsonKey(DeviceClass2::JsonEnum:: \
				SENSOR_VALUE);
		default:
			return "?";
		}
	}

	std::shared_ptr<DeviceClass1> DeviceClass1::parse(JsonType json)
	{
		bool alertState = json[DeviceClass1::getJsonKey(JsonEnum:: \
			ALERT_STATE)] != 0;
		bool aliveState = json[DeviceClass1::getJsonKey(JsonEnum:: \
			ALIVE_STATE)] != 0;
		std::string sensorType = json[DeviceClass1::getJsonKey(JsonEnum:: \
			SENSOR_TYPE)];
		std::string sensorValue = json[DeviceClass1::getJsonKey(JsonEnum:: \
			SENSOR_VALUE)];

		auto class1 = std::make_shared<DeviceClass1>();
		class1->setAlertState(alertState);
		class1->setAliveState(aliveState);
		class1->setSensorType(sensorType);
		class1->setSensorValue(sensorValue);
		return class1;
	}

	DeviceClass::JsonType DeviceClass1::toJson(void)
	{
		return DeviceClass2::toJson();
	}
}