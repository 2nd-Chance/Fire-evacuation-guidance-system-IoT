#ifndef MySet_h__
#define MySet_h__


#include <iostream>
#include <memory>
#include <set>

#include "json.hpp"

namespace model
{
	template<typename T>
	class MySet
	{
	public:
		using JsonType = nlohmann::json;

		MySet(void);
		static std::shared_ptr<MySet<T>> parse(JsonType json);

		std::shared_ptr<std::set<T>> getSet(void);
		bool add(T value);
		bool remove(T value);
		bool has(T value);
		void clear(void);
		void print(void);
		void print(std::ostream &out);
		JsonType toJson(void);

	protected:
		void setSet(std::shared_ptr<std::set<T>> set);

	private:
		std::shared_ptr<std::set<T>> set;
	};

	template<typename T>
	inline MySet<T>::MySet(void)
	{
		this->setSet(std::make_shared<std::set<T>>());
	}

	template<typename T>
	inline std::shared_ptr<MySet<T>> MySet<T>::parse(JsonType json)
	{
		auto mySet = std::make_shared<MySet<T>>();
		for (const auto &element : json)
		{
			mySet->add(element);
		}
		return mySet;
	}

	template<typename T>
	inline std::shared_ptr<std::set<T>> MySet<T>::getSet(void)
	{
		return this->set;
	}

	template<typename T>
	inline void MySet<T>::setSet(std::shared_ptr<std::set<T>> set)
	{
		this->set = set;
	}

	template<typename T>
	inline bool MySet<T>::add(T value)
	{
		auto insertPair = this->getSet()->insert(value);
		return insertPair.second;
	}

	template<typename T>
	inline bool MySet<T>::remove(T value)
	{
		auto removeCount = this->getSet()->erase(value);
		return removeCount > 0;
	}

	template<typename T>
	inline bool MySet<T>::has(T value)
	{
		auto findPair = this->getSet()->find(value);
		return findPair != this->getSet()->end();
	}

	template<typename T>
	inline void MySet<T>::clear(void)
	{
		this->getSet()->clear();
	}

	template<typename T>
	inline void MySet<T>::print(void)
	{
		this->print(std::cout);
	}

	template<typename T>
	inline void MySet<T>::print(std::ostream &out)
	{
		out << "[";
		for (const T &value : *(this->getSet()))
		{
			out << value << ", ";
		}
		out << "]\n";
	}
	
	template<typename T>
	inline typename MySet<T>::JsonType MySet<T>::toJson(void)
	{
		JsonType json = *(this->getSet());
		return json;
	}
}


#endif // MySet_h__