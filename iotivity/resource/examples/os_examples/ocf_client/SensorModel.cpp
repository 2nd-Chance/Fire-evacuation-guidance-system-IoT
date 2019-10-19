//******************************************************************
//
// Copyright 2018 OHSUNG ELECTRONICS All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#include "OCPlatform.h"


using namespace OC;
using namespace std;


#include "SensorModel.h"


SensorModel::SensorModel(void)
{
	m_illuminance = 0;
}

void SensorModel::GetRepresentation(shared_ptr<OCResource> resource,
		GetCallback callback)
{
	if(resource)
	{
		QueryParamsMap test;
		resource->get(test, callback);
	}
}

void SensorModel::Observe(shared_ptr<OCResource> resource, ObserveType type,
		ObserveCallback callback)
{
	if(resource)
		resource->observe(type, QueryParamsMap(), callback);
}

void SensorModel::SetIlluminance(OCRepresentation representation, string key)
{
	representation.getValue(key, m_illuminance);
}

