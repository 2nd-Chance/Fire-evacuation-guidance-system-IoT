#ifndef RoomMap_h__
#define RoomMap_h__


#include <cinttypes>

#include "MyMap.h"
#include "Room.h"

namespace model
{
	class RoomMap \
		: public MyMap<std::uint32_t, std::shared_ptr<model::Room>>
	{
	public:

	};
}


#endif // RoomMap_h__