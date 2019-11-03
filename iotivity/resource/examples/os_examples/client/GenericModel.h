#ifndef GENERIC_MODEL_H__
#define GENERIC_MODEL_H__

#include "OCPlatform.h"

#include <memory>
#include <string>

using namespace OC;
using namespace std;

class GenericModel
{
public:
	GenericModel(void);

	void GetRepresentation(shared_ptr<OCResource> resource,
			GetCallback callback);
	void PostRepresentation(shared_ptr<OCResource> resource, 
			string key, string value, PostCallback callback);
	void Observe(shared_ptr<OCResource> resource,
			ObserveType type, ObserveCallback callback);

	string GetValue(void) { return m_value; }
	void SetValue(OCRepresentation representation, string key);

private:
	string m_value;
};

#endif
