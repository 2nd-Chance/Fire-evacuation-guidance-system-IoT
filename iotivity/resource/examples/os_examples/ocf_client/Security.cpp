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


#include "securevirtualresourcetypes.h"
#include "doxmresource.h"
#include "OCProvisioningManager.hpp"

#include "Security.h"


Security::Security(void)
{

}

void Security::ProvisionInit()
{
	OCSecure::provisionInit("");
}

void Security::ProvisionClose()
{
	OCSecure::provisionClose();
}

void Security::DoOwnershipTransfer(shared_ptr<OCSecureResource> secureResource, ResultCallBack callback)
{
	if(!secureResource || !callback)
	{
		cout << "Invalid Parameter" << endl;
		return;
	}

	if(secureResource->doOwnershipTransfer(callback) != OC_STACK_OK)
	{
		cout << "Ownership Transfer is failed" << endl;
		return;
	}
}

