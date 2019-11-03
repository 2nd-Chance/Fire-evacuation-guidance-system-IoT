#include "OCPlatform.h"


using namespace OC;
using namespace std;
namespace PH = std::placeholders;


#include "GenericControl.h"
#include "GenericResource.h"


#define RESOURCE_URI		"/SpeechTTSResURI"
#define RESOURCE_TYPE_NAME	"oic.r.speech.tts"
#define RESOURCE_INTERFACE	"oic.if.a"
#define RESOURCE_PROPERTY	(OC_DISCOVERABLE | OC_OBSERVABLE | OC_SECURE)
#define RESOURCE_KEY		"value"


GenericResource::GenericResource(void)
{
	m_representation.setUri(RESOURCE_URI);
	m_representation.setValue(RESOURCE_KEY, m_genericControl.GetValue());

	m_isStartedThread = false;
}

void GenericResource::CreateResource(void)
{
	string resourceURI = RESOURCE_URI;
	string resourceTypeName = RESOURCE_TYPE_NAME;
	string resourceInterface = RESOURCE_INTERFACE;
	uint8_t resourceProperty = RESOURCE_PROPERTY;

	EntityHandler entityHd = bind(&GenericResource::EntityHandlerCB, this, PH::_1);

	OCStackResult result = OCPlatform::registerResource(m_resourceHandle,
			resourceURI, resourceTypeName, resourceInterface,
			entityHd, resourceProperty);
	if(OC_STACK_OK != result) {
		cout << "Resource creation was unsuccessful" << endl;
	}
}

OCRepresentation GenericResource::Get(void)
{
	string value = m_genericControl.GetValue();

	cout << "\tvalue: " << value << endl;

	m_representation.setValue(RESOURCE_KEY, value);

	return m_representation;
}

void GenericResource::Post(OCRepresentation rep)
{
	try {
		string value = "{}";

		if(rep.getValue(RESOURCE_KEY, value)) {
			cout << "\tvalue: " << value << endl;

			m_genericControl.Control(value);
		} else {
			cout << "\tvalue not found in the representation" << endl;
		} // end of if
	} catch (exception& e) {
		cout << "\t" << e.what() << endl;
	}
}

void GenericResource::Observe(ObservationInfo observationInfo)
{
	const ObserveAction& action = observationInfo.action;

	switch (action) {
		case ObserveAction::ObserveRegister:
			m_interestedObservers.push_back(observationInfo.obsId);
			break;
		case ObserveAction::ObserveUnregister:
			m_interestedObservers.erase(
				std::remove(
					m_interestedObservers.begin(),
					m_interestedObservers.end(),
					observationInfo.obsId
				),
				m_interestedObservers.end()
			);
			break;
	} // end of switch

	if(!m_isStartedThread) {
		pthread_t threadId;

		m_isStartedThread = true;
		pthread_create(&threadId, NULL,
				&GenericResource::ChangeGenericRepresentation, (void *)this);
	}
}

OCEntityHandlerResult GenericResource::EntityHandlerCB(shared_ptr<OCResourceRequest> request)
{
	cout << "[Entity Handler Callback]" << endl;

	OCEntityHandlerResult ehResult = OC_EH_ERROR;

	if (request) {
		string requestType = request->getRequestType();
		int requestFlag = request->getRequestHandlerFlag();

		if(requestFlag & RequestHandlerFlag::RequestFlag) {
			cout << "\trequestFlag: Request" << endl;
			cout << "\trequestType: " << requestType << endl;

			auto response = make_shared<OCResourceResponse>();

			response->setRequestHandle(request->getRequestHandle());
			response->setResourceHandle(request->getResourceHandle());

			if(requestType == "GET") {
				response->setResponseResult(OC_EH_OK);
				response->setResourceRepresentation(this->Get());
			} else if(requestType == "POST") {
				OCRepresentation rep = request->getResourceRepresentation();

				this->Post(rep);

				response->setResponseResult(OC_EH_OK);
				response->setResourceRepresentation(this->Get());
			} else {
				response->setResponseResult(OC_EH_ERROR);
			} // end of if

			if(OC_STACK_OK == OCPlatform::sendResponse(response)) {
				ehResult = OC_EH_OK;
			}
		} // end of if

		if(requestFlag & RequestHandlerFlag::ObserverFlag) {
			cout << "\trequestFlag : Observer" << endl;

			this->Observe(request->getObservationInfo());
			ehResult = OC_EH_OK;
		}
	} else {
		cout << "\tRequest invalid" << endl;
	} // end of if

	cout << endl;

	return ehResult;
}

void* GenericResource::ChangeGenericRepresentation(void *param)
{
	GenericResource *genericResource = (GenericResource *)param;

	while(1) {
		sleep(1);

		if(genericResource->m_isStartedThread) {
			OCStackResult result = OC_STACK_OK;

			cout << "[NotifyListOfObservers]" << endl;

			shared_ptr<OCResourceResponse> resourceResponse = make_shared<OCResourceResponse>();

			resourceResponse->setResourceRepresentation(genericResource->Get(), DEFAULT_INTERFACE);

			result = OCPlatform::notifyListOfObservers(
					genericResource->GetHandle(),
					genericResource->m_interestedObservers,
					resourceResponse);

			if(result == OC_STACK_NO_OBSERVERS) {
                cout << "\tNo More observers, stopping notifications" << endl;
				genericResource->m_isStartedThread = false;
			}

			cout << endl;
		} else {
			cout << "Finish notify thread..." << endl;
			break;
		} // end of if
	}

	return NULL;
}

