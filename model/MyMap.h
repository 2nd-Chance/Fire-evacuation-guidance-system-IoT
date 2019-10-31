#ifndef MyMap_h__
#define MyMap_h__


#include <type_traits>
#include <iostream>
#include <memory>
#include <map>

#include "json.hpp"

namespace model
{
	template <typename K, typename V>
	class MyMap
	{
	public:
		using JsonType = nlohmann::json;

		MyMap(void);

		std::shared_ptr<std::unordered_map<K, V>> getMap(void);
		bool add(K key, V value);
		V get(K key);
		bool remove(K key);
		void clear(void);
		void print(void);
		void print(std::ostream &out);
		JsonType toJson(void);

	protected:
		void setMap(std::shared_ptr<std::unordered_map<K, V>> map);

	private:
		std::shared_ptr<std::unordered_map<K, V>> map;
	};
	
	template<typename K, typename V>
	inline MyMap<K, V>::MyMap(void)
	{
		this->setMap(std::make_shared<std::unordered_map<K, V>>());
	}

	template<typename K, typename V>
	inline std::shared_ptr<std::unordered_map<K, V>> MyMap<K, V>::getMap(void)
	{
		return this->map;
	}
	
	template<typename K, typename V>
	inline void MyMap<K, V>::setMap(std::shared_ptr<std::unordered_map<K, V>> map)
	{
		this->map = map;
	}

	template<typename K, typename V>
	inline bool MyMap<K, V>::add(K key, V value)
	{
		auto insertPair = this->getMap()->insert(std::make_pair(key, value));
		return insertPair.second;
	}

	template<typename K, typename V>
	inline V MyMap<K, V>::get(K key)
	{
		auto foundPair = this->getMap()->find(key);
		return foundPair->second;
	}

	template<typename K, typename V>
	inline bool MyMap<K, V>::remove(K key)
	{
		auto removeCount = this->getMap()->erase(key);
		return removeCount > 0;
	}

	template<typename K, typename V>
	inline void MyMap<K, V>::clear(void)
	{
		this->getMap()->clear();
	}

	template<typename K, typename V>
	inline void MyMap<K, V>::print(void)
	{
		this->print(std::cout);
	}

	template<typename K, typename V>
	inline void MyMap<K, V>::print(std::ostream &out)
	{
		out << "{";
		for (auto item : *(this->getMap()))
		{
			out << item.first << ": " << item.second << ", ";
		}
		out << "}\n";
	}
	
	template<typename K, typename V>
	inline typename MyMap<K, V>::JsonType MyMap<K, V>::toJson(void)
	{
		JsonType json = JsonType::object();
		for (const auto &element : *(this->getMap()))
		{
			if (std::is_same<K, std::string>::value)
			{
				json[element.first] = element.second;
			}
			else
			{
				json[std::to_string(element.first)] = element.second;
			}
		}
	}
}


#endif // MyMap_h__