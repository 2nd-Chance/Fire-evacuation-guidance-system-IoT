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

#include <mutex>
#include <condition_variable>


using namespace OC;
using namespace std;


#include "LightControl.h"
#include "LightResource.h"


#define SVR_DB_FILE_NAME			"./oic_svr_db_server.dat"
#define INTROSPECTION_FILE_NAME		"light_introspection.dat"


static LightResource _light_resource;
static OCPlatformInfo _platform_info;


static void _duplicate_string(char **target_string, string source_string)
{
	*target_string = new char[source_string.length() + 1];
	strncpy(*target_string, source_string.c_str(), (source_string.length() + 1));
}

static void _delete_platform_info()
{
	delete[] _platform_info.platformID;
	delete[] _platform_info.manufacturerName;
	delete[] _platform_info.manufacturerUrl;
	delete[] _platform_info.modelNumber;
	delete[] _platform_info.dateOfManufacture;
	delete[] _platform_info.platformVersion;
	delete[] _platform_info.operatingSystemVersion;
	delete[] _platform_info.hardwareVersion;
	delete[] _platform_info.firmwareVersion;
	delete[] _platform_info.supportUrl;
	delete[] _platform_info.systemTime;
}

static OCStackResult _set_platform_info(void)
{
	_duplicate_string(&_platform_info.platformID, "0A3E0D6F-DBF5-404E-8719-D6880042463A");
	_duplicate_string(&_platform_info.manufacturerName, "OCF");
	_duplicate_string(&_platform_info.manufacturerUrl, "https://www.iotivity.org");
	_duplicate_string(&_platform_info.modelNumber, "Model Number");
	_duplicate_string(&_platform_info.dateOfManufacture, "2018-06-25");
	_duplicate_string(&_platform_info.platformVersion, "Platform Version");
	_duplicate_string(&_platform_info.operatingSystemVersion, "RASPBIAN JESSIE LITE");
	_duplicate_string(&_platform_info.hardwareVersion, "Hardware Version");
	_duplicate_string(&_platform_info.firmwareVersion, "0.0.0.1");
	_duplicate_string(&_platform_info.supportUrl, "https://www.iotivity.org");
	_duplicate_string(&_platform_info.systemTime, "2018-06-25T09.09");

	return OCPlatform::registerPlatformInfo(_platform_info);
}

static OCStackResult _set_device_info(void)
{
	OCStackResult result = OC_STACK_ERROR;
	string device_type = "oic.d.light";
	string device_name = "OCF IoTivity Light Server";
	string spec_version = "ocf.1.3.1";
	vector<string> data_model_versions = {"ocf.res.1.3.1"};
	string protocol_independent_id = "b0ed9259-ec95-4ac6-8f62-241d0da02683";

// [20191008 hyoungjoon.lee] modified to fix compile error by 1.3.1
	//OCResourceHandle handle = OCPlatform::getResourceHandleAtUri(OC_RSRVD_DEVICE_URI);
	OCResourceHandle handle = OCGetResourceHandleAtUri(OC_RSRVD_DEVICE_URI); 
// [20191008 hyoungjoon.lee] removed to fix compile error by 1.3.1

	if(handle == NULL)
	{
		cout << "Failed to find resource " << OC_RSRVD_DEVICE_URI << endl;
		return result;
	}

	result = OCPlatform::bindTypeToResource(handle, device_type);
	if(result != OC_STACK_OK)
	{
		cout << "Failed to add device type" << endl;
		return result;
	}

	result = OCPlatform::setPropertyValue(PAYLOAD_TYPE_DEVICE, OC_RSRVD_DEVICE_NAME,
			device_name);
	if(result != OC_STACK_OK)
	{
		cout << "Failed to set device name" << endl;
		return result;
	}

	result = OCPlatform::setPropertyValue(PAYLOAD_TYPE_DEVICE, OC_RSRVD_SPEC_VERSION,
			spec_version);
	if(result != OC_STACK_OK)
	{
		cout << "Failed to set spec version" << endl;
		return result;
	}

	result = OCPlatform::setPropertyValue(PAYLOAD_TYPE_DEVICE, OC_RSRVD_DATA_MODEL_VERSION,
			data_model_versions);
	if(result != OC_STACK_OK)
	{
		cout << "Failed to set data model versions" << endl;
		return result;
	}

	result = OCPlatform::setPropertyValue(PAYLOAD_TYPE_DEVICE, OC_RSRVD_PROTOCOL_INDEPENDENT_ID,
			protocol_independent_id);
	if(result != OC_STACK_OK)
	{
		cout << "Failed to set piid" << endl;
		return result;
	}

	return OC_STACK_OK;
}

static FILE* _server_fopen(const char *path, const char *mode)
{
	char const *file_name = path;

	cout << "[In] reading file=" << file_name << " mode=" << mode << endl;

	if(0 == strcmp(path, OC_SECURITY_DB_DAT_FILE_NAME))
		file_name = SVR_DB_FILE_NAME;
	else if(0 == strcmp(path, OC_INTROSPECTION_FILE_NAME))
		file_name = INTROSPECTION_FILE_NAME;

	cout << "[Out] reading file=" << file_name << " mode=" << mode << endl;

	return fopen(file_name, mode);
}

static void _init(PlatformConfig cfg)
{
	OCPlatform::Configure(cfg);
}

static void _setup(void)
{
	OCStackResult result = _set_platform_info();
	if(result != OC_STACK_OK)
	{
		cout << "Platform registration failed" << endl;
		return;
	}

	result = _set_device_info();
	if(result != OC_STACK_OK)
	{
		cout << "Device registration failed" << endl;
		return;
	}
}


int main(void)
{
	OCPersistentStorage ps { _server_fopen, fread, fwrite, fclose, unlink };

	PlatformConfig cfg { ServiceType::InProc, ModeType::Server, &ps };

	_init(cfg);

	OC_VERIFY(OCPlatform::start() == OC_STACK_OK);
	cout << "Start light server..." << endl;

	_setup();

	try
	{
		_light_resource.CreateResource();

		_delete_platform_info();

		mutex blocker;
		condition_variable cv;
		unique_lock<mutex> lock(blocker);
		cv.wait(lock);
	}
	catch(OCException& e)
	{
		oclog() << "Exception in main: "<< e.what();
	}

	OC_VERIFY(OCPlatform::stop() == OC_STACK_OK);
	cout << "End light server..." << endl;

	return 0;
}

