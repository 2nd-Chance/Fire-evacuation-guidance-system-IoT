#ifndef LocationBuilder_h__
#define LocationBuilder_h__


#include <memory>

#include "Location.h"

namespace model
{
	class LocationBuilder
	{
	public:
		LocationBuilder &setLevel(int level);
		LocationBuilder &setX(int x);
		LocationBuilder &setY(int y);
		std::shared_ptr<model::Location> build(void);

	protected:
		int getLevel(void);
		int getX(void);
		int getY(void);

	private:
		int level = 1;
		int x = 0;
		int y = 0;
	};
}


#endif // LocationBuilder_h__