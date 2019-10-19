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

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#endif

#include <string>
#include <map>
#include <cstdlib>
#include <mutex>
#include <condition_variable>


using namespace OC;
using namespace std;

#include "experimental/ocrandom.h"
#include "OCProvisioningManager.hpp"

#ifdef SECURED // [20191008 hyoungjoon.lee] added build option - SECURED
#include "Security.h"
#endif // [20191008 hyoungjoon.lee] added build option - SECURED

#include "Util.h"
#include "LightModel.h"
#include "SensorModel.h"


#define SVR_DB_FILE_NAME		"./oic_svr_db_client.dat"

#define LIGHT_RESOURCE_URI		"/BinarySwitchResURI"
#define SENSOR_RESOURCE_URI		"/IlluminanceSensorResURI"

#define LIGHT_RESOURCE_KEY		"value"
#define SENSOR_RESOURCE_KEY		"illuminance"

#define DISCOVERY_TIMEOUT		(3)


typedef map<OCResourceIdentifier, shared_ptr<OCResource>> DiscoveredResourceMap;
typedef map<string, shared_ptr<OCResource>> RegisteredResourceMap;
typedef map<int, shared_ptr<OCSecureResource>> SecureResourceMap;


static DiscoveredResourceMap _discovered_resources;
static RegisteredResourceMap _registered_resources;
static SecureResourceMap _secure_resources;

#ifdef SECURED // [20191008 hyoungjoon.lee] added build option - SECURED
static Security _security;
#endif // [20191008 hyoungjoon.lee] added build option - SECURED

static Util _util;
static LightModel _light_model;
static SensorModel _sensor_model;

static mutex _cur_resource_lock;


const char* convResCodeToString(int resCode)
{
    switch (resCode)
    {
        case OC_STACK_OK:
            return "OC_STACK_OK";
        case OC_STACK_INVALID_URI:
            return "OC_STACK_INVALID_URI";
        case OC_STACK_INVALID_QUERY:
            return "OC_STACK_INVALID_QUERY";
        case OC_STACK_INVALID_IP:
            return "OC_STACK_INVALID_IP";
        case OC_STACK_INVALID_PORT:
            return "OC_STACK_INVALID_PORT";
        case OC_STACK_INVALID_CALLBACK:
            return "OC_STACK_INVALID_CALLBACK";
        case OC_STACK_INVALID_METHOD:
            return "OC_STACK_INVALID_METHOD";
        case OC_STACK_NO_MEMORY:
            return "OC_STACK_NO_MEMORY";
        case OC_STACK_COMM_ERROR:
            return "OC_STACK_COMM_ERROR";
        case OC_STACK_INVALID_PARAM:
            return "OC_STACK_INVALID_PARAM";
        case OC_STACK_NOTIMPL:
            return "OC_STACK_NOTIMPL";
        case OC_STACK_NO_RESOURCE:
            return "OC_STACK_NO_RESOURCE";
        case OC_STACK_RESOURCE_ERROR:
            return "OC_STACK_RESOURCE_ERROR";
        case OC_STACK_SLOW_RESOURCE:
            return "OC_STACK_SLOW_RESOURCE";
        case OC_STACK_NO_OBSERVERS:
            return "OC_STACK_NO_OBSERVERS";
        case OC_STACK_ERROR:
            return "OC_STACK_ERROR";
        default:
            return "UNKNOWN";
    }
}

static void onPost(const HeaderOptions &/*headerOptions*/, const OCRepresentation &rep, const int eCode)
{
	cout << "[POST Callback]" << endl;

	try
	{
		if(eCode == OC_STACK_OK || eCode == OC_STACK_RESOURCE_CHANGED)
		{
			cout << "\tPOST request was successful" << endl;

			_light_model.SetValue(rep, LIGHT_RESOURCE_KEY);
			cout << "\tvalue: " << _light_model.GetValue() << endl;
		}
		else
		{
			cout << "\tonPost Response error: " << convResCodeToString(eCode) << endl;
		}
	}
	catch(exception& e)
	{
		cout << "\tException: " << e.what() << " in onPost" << endl;
	}

	cout << endl;
}

static void onObserve(const HeaderOptions /*headerOptions*/, const OCRepresentation& rep, const int &eCode,
		const int& sequenceNumber)
{
	cout << "[OBSERVE Callback]" << endl;

    try
    {
		cout << "\tSequenceNumber: " << sequenceNumber << endl;

        if(eCode == OC_STACK_OK && sequenceNumber <= MAX_SEQUENCE_NUMBER)
        {
			const string uri = rep.getUri();

            if(sequenceNumber == OC_OBSERVE_REGISTER)
                cout << "\tObserve registration action is successful" << endl;

			if(uri == LIGHT_RESOURCE_URI)
			{
				_light_model.SetValue(rep, LIGHT_RESOURCE_KEY);
				cout << "\tvalue: " << _light_model.GetValue() << endl;
			}
			else if(uri == SENSOR_RESOURCE_URI)
			{
				_sensor_model.SetIlluminance(rep, SENSOR_RESOURCE_KEY);
				cout << "\tilluminance: " << _sensor_model.GetIlluminance() << endl;
			}

			cout << endl;

			if(_registered_resources[LIGHT_RESOURCE_URI])
			{
				bool value = _light_model.GetValue();

#ifndef TEST
				if(value == true)
				{
					value = false;
				}
				else
				{
					value = true;
				}
#else
				if(_sensor_model.GetIlluminance() == 0 && _light_model.GetValue() == false)
					value = true;

				if(_sensor_model.GetIlluminance() == 1 && _light_model.GetValue() == true)
					value = false;
#endif

				if(value != _light_model.GetValue())
				{
					// Invoke resource's post API with rep, query map and the callback parameter
					_light_model.PostRepresentation(_registered_resources[LIGHT_RESOURCE_URI],
							LIGHT_RESOURCE_KEY, value, &onPost);
					cout << "\t<POST Light Representation>" << endl;
					cout << "\t\tlight: " << _light_model.GetValue()
						 << " | sensor: " << _sensor_model.GetIlluminance()
						 << " | value: " << value << endl;
				}
			}
        }
        else
        {
            if(eCode == OC_STACK_OK)
            {
                cout << "\tNo observe option header is returned in the response." << endl;
                cout << "\tFor a registration request, it means the registration failed" << endl;
                cout << "\tFor a cancelation request, it means the cancelation was successful" << endl;
            }
            else
            {
                cout << "\tonObserve Response error: " << convResCodeToString(eCode) << endl;
                exit(-1);
            }
        }
    }
    catch(exception& e)
    {
        cout << "\tException: " << e.what() << " in onObserve" << endl;
    }

	cout << endl;
}

static void onGet(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
	cout << "[GET Callback]" << endl;

	try
	{
		if(eCode == OC_STACK_OK)
		{
			const string uri = rep.getUri();

			_util.CheckHeaderOptionsInfo("GET", headerOptions);

			cout << "\tGET request was successful" << endl;
			cout << "\tResource URI: " << uri << endl;

			if(uri == LIGHT_RESOURCE_URI)
			{
				_light_model.SetValue(rep, LIGHT_RESOURCE_KEY);
				cout << "\tvalue: " << _light_model.GetValue() << endl;

				_light_model.Observe(_registered_resources[LIGHT_RESOURCE_URI],
						ObserveType::Observe, &onObserve);
			}
			else if(uri == SENSOR_RESOURCE_URI)
			{
				_sensor_model.SetIlluminance(rep, SENSOR_RESOURCE_KEY);
				cout << "\tilluminance: " << _sensor_model.GetIlluminance() << endl;

				_sensor_model.Observe(_registered_resources[SENSOR_RESOURCE_URI],
						ObserveType::Observe, &onObserve);
			}
		}
		else
		{
			cout << "\tonGet Response error: " << convResCodeToString(eCode) << endl;
		}
	}
	catch(exception& e)
	{
		cout << "\tException: " << e.what() << " in onGet" << endl;
	}

	cout << endl;
}

static void foundResource(shared_ptr<OCResource> resource)
{
	cout << "[Discovery Callback]" << endl;

	try
	{
		lock_guard<mutex> lock(_cur_resource_lock);

		if(_discovered_resources.find(resource->uniqueIdentifier())
				== _discovered_resources.end())
		{
			cout << "\tFound resource " << resource->uniqueIdentifier()
				 << " for the first time on server with ID: " << resource->sid() << endl;

			_discovered_resources[resource->uniqueIdentifier()] = resource;
		}
		else
		{
			cout << "\tFound resource "<< resource->uniqueIdentifier() << " again!" << endl;
		}

		if(_registered_resources[LIGHT_RESOURCE_URI] && _registered_resources[SENSOR_RESOURCE_URI])
		{
			cout << "\tRegister another resource, ignoring" << endl;
			return;
		}

		if(resource)
		{
			OCConnectivityType transport_type_to_use = _util.CheckResourceInfo(resource);

			if(resource->uri() == LIGHT_RESOURCE_URI)
			{
				_util.CheckHeaderOptionsInfo("Discovery", resource->getServerHeaderOptions());

				if(resource->connectivityType() & transport_type_to_use)
				{
					_registered_resources[LIGHT_RESOURCE_URI] = resource;
					cout << "\t<Address of selected resource: " << resource->host() << ">" << endl;

					_light_model.GetRepresentation(_registered_resources[LIGHT_RESOURCE_URI], &onGet);
				}
			}
			else if(resource->uri() == SENSOR_RESOURCE_URI)
			{
				_util.CheckHeaderOptionsInfo("Discovery", resource->getServerHeaderOptions());

				if(resource->connectivityType() & transport_type_to_use)
				{
					_registered_resources[SENSOR_RESOURCE_URI] = resource;
					cout << "\t<Address of selected resource: " << resource->host() << ">" << endl;

					_sensor_model.GetRepresentation(_registered_resources[SENSOR_RESOURCE_URI], &onGet);
				}
			}
		}
		else
		{
			cout << "\tResource is invalid" << endl;
		}
	}
	catch(exception& e)
	{
		cerr << "\tException in foundResource: " << e.what() << endl;
	}
}

static void ownershipTransferCB(PMResultList_t *result, int hasError)
{
#ifdef SECURED // [20191008 hyoungjoon.lee] added build option - SECURED
    if(hasError)
    {
        cout << "Error!!! in OwnershipTransfer" << endl;
    }
    else
    {
        cout << "Transferred Ownership successfuly for device" << endl;
        delete result;

		if(_secure_resources.size() == 2 && !_secure_resources[1]->getDevPtr()->doxm->owned)
		{
			_security.DoOwnershipTransfer(_secure_resources[1], ownershipTransferCB);
		}
		else
		{
			OCPlatform::findResource("",
					OC_RSRVD_WELL_KNOWN_URI,
					CT_ADAPTER_IP,
					&foundResource);
			cout << "Finding resources... " << endl;
		}
    }
#endif	// [20191008 hyoungjoon.lee] added build option - SECURED
}

static FILE* _client_fopen(const char* path, const char* mode)
{
	char const *file_name = path;

	cout << "[In] reading file=" << file_name << " mode=" << mode << endl;

	if(0 == strcmp(path, OC_SECURITY_DB_DAT_FILE_NAME))
		file_name = SVR_DB_FILE_NAME;

	cout << "[Out] reading file=" << file_name << " mode=" << mode << endl;

	return fopen(file_name, mode);
}

static void _init(PlatformConfig cfg)
{
	OCPlatform::Configure(cfg);
}


int main(void)
{
	OCPersistentStorage ps { _client_fopen, fread, fwrite, fclose, unlink };

	PlatformConfig cfg { ServiceType::InProc, ModeType::Both, &ps };

	_init(cfg);

	try
	{
		OC_VERIFY(OCPlatform::start() == OC_STACK_OK);
		cout << "Start client\n";

#ifdef SECURED // [20191008 hyoungjoon.lee] added build option - SECURED
		_security.ProvisionInit();
#endif // [20191008 hyoungjoon.lee] added build option - SECURED

		cout << "Discover unowned devices..." << endl;
		DeviceList_t list;
		list.clear();

#ifdef SECURED	 // [20191008 hyoungjoon.lee] added build option - SECURED	
		OCStackResult result = OCSecure::discoverUnownedDevices(DISCOVERY_TIMEOUT, list);
		if(result != OC_STACK_OK)
		{
			cout << "UnOwned Discovery failed."<< endl;
		}
		else if(list.size())
		{
			cout << "Found unowned secure devices, count = " << list.size() << endl;

			for(unsigned int i = 0; i < list.size(); i++ )
			{
				cout << "Device ID: "<< list[i]->getDeviceID() << endl;
				_secure_resources[i] = list[i];
			}

			_security.DoOwnershipTransfer(_secure_resources[0], ownershipTransferCB);
		}
		else
		{
			OCPlatform::findResource("",
					OC_RSRVD_WELL_KNOWN_URI,
					CT_ADAPTER_IP,
					&foundResource);
			cout << "Finding resources... " << endl;
		}
#else // [20191008 hyoungjoon.lee] added build option - SECURED
		OCPlatform::findResource("",
		OC_RSRVD_WELL_KNOWN_URI,
		CT_ADAPTER_IP,
		&foundResource);
		cout << "Finding resources... " << endl;
#endif // [20191008 hyoungjoon.lee] added build option - SECURED

		mutex blocker;
		condition_variable cv;
		unique_lock<mutex> lock(blocker);
		cv.wait(lock);

		list.clear();

#ifdef SECURED // [20191008 hyoungjoon.lee] added build option - SECURED
		_security.ProvisionClose();
#endif // [20191008 hyoungjoon.lee] added build option - SECURED

		OC_VERIFY(OCPlatform::stop() == OC_STACK_OK);
		cout << "End client\n";
	}
	catch(OCException& e)
	{
		oclog() << "Exception in main: "<<e.what();
	}

	return 0;
}

