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


#include "Util.h"


#define CA_OPTION_CONTENT_VERSION	2053
#define COAP_OPTION_CONTENT_FORMAT	12


OCConnectivityType Util::CheckResourceInfo(shared_ptr<OCResource> resource)
{
	OCConnectivityType transport_type_to_use = CT_ADAPTER_IP;

	cout << "[DISCOVERED Resource]" << endl;

	// Get the resource URI
	cout << "\tURI of the resource: " << resource->uri() << endl;

	// Get the resource host address
	cout << "\tHost address of the resource: " << resource->host() << endl;

	// Get the resource types
	cout << "\tList of resource types: " << endl;
	for(auto &resourceTypes : resource->getResourceTypes())
	{
		cout << "\t\t" << resourceTypes << endl;
	}

	// Get the resource interfaces
	cout << "\tList of resource interfaces: " << endl;
	for(auto &resourceInterfaces : resource->getResourceInterfaces())
	{
		cout << "\t\t" << resourceInterfaces << endl;
	}

	// Get Resource current host
	cout << "\tHost of resource: " << endl;
	cout << "\t\t" << resource->host() << endl;

	// Get Resource Endpoint Infomation
	cout << "\tList of resource endpoints: " << endl;
	for(auto &resourceEndpoints : resource->getAllHosts())
	{
		cout << "\t\t" << resourceEndpoints << endl;
	}

	// If resource is found from ip based adapter.
	if (string::npos != resource->host().find("coap://")
			|| string::npos != resource->host().find("coaps://")
			|| string::npos != resource->host().find("coap+tcp://")
			|| string::npos != resource->host().find("coaps+tcp://"))
	{
		for(auto &resourceEndpoints : resource->getAllHosts())
		{
			if(resourceEndpoints.compare(resource->host()) != 0 &&
					string::npos == resourceEndpoints.find("coap+rfcomm"))
			{
				string newHost = resourceEndpoints;

				if(string::npos != newHost.find("tcp"))
				{
					transport_type_to_use = CT_ADAPTER_TCP;
				}
				else
				{
					transport_type_to_use = CT_ADAPTER_IP;
				}

				// Change Resource host if another host exists
				cout << "\tChange host of resource endpoints" << endl;
				cout << "\t\t" << "Current host is " << resource->setHost(newHost) << endl;
				break;
			}
		}
	}

	cout << endl;

	return transport_type_to_use;
}

void Util::CheckHeaderOptionsInfo(string type, const HeaderOptions headerOptions)
{
	cout << "[" << type << " Header Options]" << endl;

	if(headerOptions.size() == 0)
	{
		cout << "\tNo header option exists" << endl;
	}
	else
	{
		for(auto it = headerOptions.begin(); it != headerOptions.end(); ++it)
		{
			uint16_t optionID = it->getOptionID();
			size_t dataLength = it->getOptionData().length();
			char *optionData = new char[dataLength];

			strncpy(optionData, it->getOptionData().c_str(), dataLength);

			if(optionID == COAP_OPTION_CONTENT_FORMAT)
			{
				int format = optionData[0] * 256 + optionData[1];

				cout << "\tServer format in " << type << " response:" << format << endl;
			}
			else if(optionID == CA_OPTION_CONTENT_VERSION)
			{
				int version = optionData[0] * 256;

				cout << "\tServer version in " << type << " response:" << version << endl;
			}

			delete[] optionData;
		}
	}

	cout << endl;
}

