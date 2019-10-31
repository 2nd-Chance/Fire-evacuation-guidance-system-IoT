#ifndef DeviceMap_h__
#define DeviceMap_h__


#include <cinttypes>
#include <memory>

#include "Device.h"
#include "MyMap.h"
#include "json.hpp"

namespace model
{
	template<typename T>
	class DeviceMap : public MyMap<std::string, std::shared_ptr<T>>
	{
	public:
		using JsonType = nlohmann::json;
		
		static std::shared_ptr<DeviceMap<T>> parse(JsonType json);

		bool add(T device);
		bool add(std::shared_ptr<T> device);

		JsonType toJson(void);

	protected:
		bool add(std::string key, std::shared_ptr<T> value);
	};
	
	template<typename T>
	inline std::shared_ptr<DeviceMap<T>> DeviceMap<T>::parse(JsonType json)
	{
		auto deviceMap = std::make_shared<DeviceMap<T>>();
		for (const auto &element : json)
		{
			std::shared_ptr<T> device = T::parse(element);
			deviceMap->add(device);
		}
		return deviceMap;
	}

	template<typename T>
	inline bool DeviceMap<T>::add(T device)
	{
		return this->add(std::make_shared<T>(device));
	}

	template<typename T>
	inline bool DeviceMap<T>::add(std::shared_ptr<T> device)
	{
		return this->add(device->getUuid(), device);
	}

	template<typename T>
	inline bool DeviceMap<T>::add(std::string key, std::shared_ptr<T> value)
	{
		return MyMap<std::string, std::shared_ptr<T>>::add(key, value);
	}

	template<typename T>
	inline typename DeviceMap<T>::JsonType DeviceMap<T>::toJson(void)
	{
		JsonType json = JsonType::object();
		for (const auto element : *(this->getMap()))
		{
			json[element.first] = element.second->toJson();
		}
		return json;
	}
}


#endif // DeviceMap_h__