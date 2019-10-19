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
namespace PH = std::placeholders;


#include "SensorDetect.h"
#include "SensorResource.h"


#define RESOURCE_URI		"/IlluminanceSensorResURI"
#define RESOURCE_TYPE_NAME	"oic.r.sensor.illuminance"
#define RESOURCE_INTERFACE	"oic.if.s"
#define RESOURCE_PROPERTY	(OC_DISCOVERABLE | OC_OBSERVABLE | OC_SECURE)
#define RESOURCE_KEY		"illuminance"


SensorResource::SensorResource(void)
{
	m_representation.setUri(RESOURCE_URI);
	m_representation.setValue(RESOURCE_KEY, m_sensorDetect.Detect());

	m_isStartedThread = false;
}

void SensorResource::CreateResource(void)
{
	string resourceURI = RESOURCE_URI;
	string resourceTypeName = RESOURCE_TYPE_NAME;
	string resourceInterface = RESOURCE_INTERFACE;
	uint8_t resourceProperty = RESOURCE_PROPERTY;

	EntityHandler entityHd = bind(&SensorResource::EntityHandlerCB, this, PH::_1);

	OCStackResult result = OCPlatform::registerResource(m_resourceHandle,
			resourceURI, resourceTypeName, resourceInterface,
			entityHd, resourceProperty);
	if(OC_STACK_OK != result)
	{
		cout << "Resource creation was unsuccessful" << endl;;
	}
}

OCRepresentation SensorResource::Get(void)
{
	int illuminance = m_sensorDetect.Detect();

	cout << "\tilluminance: " << illuminance << endl;

	m_representation.setValue(RESOURCE_KEY, illuminance);

	return m_representation;
}

void SensorResource::Observe(ObservationInfo observationInfo)
{
	if(observationInfo.action == ObserveAction::ObserveRegister)
	{
		m_interestedObservers.push_back(observationInfo.obsId);
	}
	else if(observationInfo.action == ObserveAction::ObserveUnregister)
	{
		m_interestedObservers.erase(
				std::remove(
					m_interestedObservers.begin(),
					m_interestedObservers.end(),
					observationInfo.obsId),
				m_interestedObservers.end());
	}

	if(!m_isStartedThread)
	{
		pthread_t threadId;

		m_isStartedThread = true;
		pthread_create(&threadId, NULL, &SensorResource::ChangeSensorRepresentation, (void *)this);
	}
}

OCEntityHandlerResult SensorResource::EntityHandlerCB(shared_ptr<OCResourceRequest> request)
{
	cout << "[Entity Handler Callback]" << endl;

	OCEntityHandlerResult ehResult = OC_EH_ERROR;

	if(request)
	{
		string requestType = request->getRequestType();
		int requestFlag = request->getRequestHandlerFlag();

		if(requestFlag & RequestHandlerFlag::RequestFlag)
		{
			cout << "\trequestFlag: Request" << endl;
			cout << "\trequestType: " + requestType << endl;

			auto response = make_shared<OCResourceResponse>();

			response->setRequestHandle(request->getRequestHandle());
			response->setResourceHandle(request->getResourceHandle());
			if(requestType == "GET")
			{
				response->setResponseResult(OC_EH_OK);
				response->setResourceRepresentation(Get());
			}
			else
			{
				response->setResponseResult(OC_EH_ERROR);
			}

			if(OC_STACK_OK == OCPlatform::sendResponse(response))
			{
				ehResult = OC_EH_OK;
			}
		}

		if(requestFlag & RequestHandlerFlag::ObserverFlag)
		{
			cout << "\trequestFlag: Observer" << endl;

			Observe(request->getObservationInfo());
			ehResult = OC_EH_OK;
		}
	}
	else
	{
		cout << "Request invalid" << endl;
	}

	cout << endl;

	return ehResult;
}

void* SensorResource::ChangeSensorRepresentation(void *param)
{
	SensorResource *sensorResource = (SensorResource *)param;

	while(1)
	{
		sleep(1);

		if(sensorResource->m_isStartedThread)
		{
			OCStackResult result = OC_STACK_OK;

			cout << "[NotifyListOfObservers]" << endl;

			shared_ptr<OCResourceResponse> resourceResponse = make_shared<OCResourceResponse>();

			resourceResponse->setResourceRepresentation(sensorResource->Get(), DEFAULT_INTERFACE);

			result = OCPlatform::notifyListOfObservers(
					sensorResource->GetHandle(),
					sensorResource->m_interestedObservers,
					resourceResponse);

			if(result == OC_STACK_NO_OBSERVERS)
			{
                cout << "No More observers, stopping notifications" << endl;
				sensorResource->m_isStartedThread = false;
			}

			cout << endl;
		}
		else
		{
			cout << "Finish notify thread..." << endl;
			break;
		}
	}

	return NULL;
}

