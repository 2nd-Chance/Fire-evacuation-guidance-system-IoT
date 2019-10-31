#include "Location.h"

namespace model
{
	Location::Location(void) : Location(0, 0) {}
	
	Location::Location(int x, int y) : Location(1, x, y) {}
	
	Location::Location(int level, int x, int y)
	{
		this->setLevel(level);
		this->setX(x);
		this->setY(y);
	}
	
	std::string Location::getJsonKey(JsonEnum jsonEnum)
	{
		switch (jsonEnum)
		{
		case JsonEnum::LEVEL:
			return "lv";
		case JsonEnum::X:
			return "x";
		case JsonEnum::Y:
			return "y";
		default:
			return "?";
		}
	}

	std::shared_ptr<Location> Location::parse(JsonType &json)
	{
		int level = json[Location::getJsonKey(JsonEnum::LEVEL)];
		int x = json[Location::getJsonKey(JsonEnum::X)];
		int y = json[Location::getJsonKey(JsonEnum::Y)];

		return std::make_shared<Location>(level, x, y);
	}
	
	int Location::getLevel(void)
	{
		return this->level;
	}

	void Location::setLevel(int level)
	{
		if (level == 0) {
			throw level;
		}
		this->level = level;
	}
	
	int Location::getX(void)
	{
		return this->x;
	}

	void Location::setX(int x)
	{
		this->x = x;
	}

	int Location::getY(void)
	{
		return this->y;
	}

	void Location::setY(int y)
	{
		this->y = y;
	}

	Location::JsonType Location::toJson(void)
	{
		nlohmann::json json;
		json[Location::getJsonKey(JsonEnum::LEVEL)] = this->getLevel();
		json[Location::getJsonKey(JsonEnum::X)] = this->getX();
		json[Location::getJsonKey(JsonEnum::Y)] = this->getY();
		return json;
	}
}
