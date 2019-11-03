#ifndef UTIL_H_
#define UTIL_H_

#include <memory>

using namespace std;
using namespace OC;

class Util
{
public:
	OCConnectivityType CheckResourceInfo \
		(shared_ptr<OCResource> resource);

	void CheckHeaderOptionsInfo \
		(string type, const HeaderOptions headerOptions);
};

#endif
