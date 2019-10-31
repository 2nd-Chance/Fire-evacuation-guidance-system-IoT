#ifndef DeviceClassBuilder_h__
#define DeviceClassBuilder_h__


#include <memory>

#include "DeviceClass.h"

namespace model
{
	class DeviceClassBuilder
	{
	public:
		DeviceClassBuilder &setAlertState(bool state);
		DeviceClassBuilder &setAliveState(bool state);
		virtual std::shared_ptr<DeviceClass> build(void) = 0;

	protected:
		DeviceClassBuilder() {}

		bool getAlertState(void);
		bool getAliveState(void);

	private:
		bool alertState = false;
		bool aliveState = true;
	};

	class DeviceClass3Builder : public DeviceClassBuilder
	{
	public:
		std::shared_ptr<DeviceClass> build(void) override;
	};

	class DeviceClass2Builder : public DeviceClass3Builder
	{
	public:
		DeviceClass2Builder &setSensorType(std::string sensorType);
		DeviceClass2Builder &setSensorValue(std::string sensorValue);
		std::shared_ptr<DeviceClass> build(void) override;

	protected:
		std::string getSensorType(void);
		std::string getSensorValue(void);

	private:
		std::string sensorType = "";
		std::string sensorValue = "";
	};

	class DeviceClass1Builder : public DeviceClass2Builder
	{
	public:
		std::shared_ptr<DeviceClass> build(void) override;
	};
}


#endif // DeviceClassBuilder_h__