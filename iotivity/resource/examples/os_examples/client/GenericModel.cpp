
#include "OCPlatform.h"


using namespace OC;
using namespace std;


#include "GenericModel.h"


GenericModel::GenericModel(void)
	:m_value("{}")
{}

void GenericModel::GetRepresentation(shared_ptr<OCResource> resource,
		GetCallback callback)
{
	if (resource) {
		QueryParamsMap test;
		resource->get(test, callback);
	}
}

void GenericModel::PostRepresentation(shared_ptr<OCResource> resource, 
		string key, string value, PostCallback callback)
{
	if (resource) {
		OCRepresentation representation;

		representation.setValue(key, value);

		resource->post(representation, QueryParamsMap(), callback);
	}
}

void GenericModel::Observe(shared_ptr<OCResource> resource, 
		ObserveType type, ObserveCallback callback)
{
	if(resource)
		resource->observe(type, QueryParamsMap(), callback);
}

void GenericModel::SetValue(OCRepresentation representation, string key)
{
	representation.getValue(key, m_value);
}

