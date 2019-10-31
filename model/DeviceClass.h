#ifndef DeviceClass_h__
#define DeviceClass_h__


#include <cinttypes>
#include <string>

#include "json.hpp"

namespace model
{
	class DeviceClass
	{
		friend class DeviceClassBuilder;

	public:
		using JsonType = nlohmann::json;

		enum class JsonEnum { CLASS_ID, ALERT_STATE, ALIVE_STATE };
		static std::string getJsonKey(JsonEnum jsonEnum);
		static std::shared_ptr<DeviceClass> parse(JsonType json);

		uint32_t getClassId(void);
		bool getAlertState(void);
		void setAlertState(bool state);
		bool getAliveState(void);
		void setAliveState(bool state);
		virtual JsonType toJson(void);

	protected:
		DeviceClass(void) {}

		void setClassId(uint32_t classId);

	private:
		uint32_t classId = 0;
		bool alertState = false;
		bool aliveState = true;
	};

	class DeviceClass3 : public DeviceClass
	{
	public:
		DeviceClass3(void);

		enum class JsonEnum { CLASS_ID, ALERT_STATE, ALIVE_STATE };
		static std::string getJsonKey(JsonEnum jsonEnum);
		static std::shared_ptr<DeviceClass3> parse(JsonType json);

		JsonType toJson(void) override;
	};

	class DeviceClass2 : public DeviceClass3
	{
	public:
		DeviceClass2(void);
		DeviceClass2(std::string sensorType, std::string sensorValue);

		enum class JsonEnum { CLASS_ID, ALERT_STATE, ALIVE_STATE, SENSOR_TYPE, SENSOR_VALUE };
		static std::string getJsonKey(JsonEnum jsonEnum);
		static std::shared_ptr<DeviceClass2> parse(JsonType json);

		std::string getSensorType(void);
		void setSensorType(std::string sensorType);
		std::string getSensorValue(void);
		void setSensorValue(std::string sensorValue);
		JsonType toJson(void) override;

	private:
		std::string sensorType;
		std::string sensorValue;
	};

	class DeviceClass1 : public DeviceClass2
	{
	public:
		DeviceClass1(void);
		DeviceClass1(std::string sensorType, std::string sensorValue);

		enum class JsonEnum { CLASS_ID, ALERT_STATE, ALIVE_STATE, SENSOR_TYPE, SENSOR_VALUE };
		static std::string getJsonKey(JsonEnum jsonEnum);
		static std::shared_ptr<DeviceClass1> parse(JsonType json);
		
		JsonType toJson(void) override;
	};
}


#endif // DeviceClass_h__