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

#include "Util.h"
#include "GenericModel.h"
#define SVR_DB_FILE_NAME    "./oic_svr_db_client.dat"
#define RESOURCE_URI        "/SpeechTTSResURI"
#define RESOURCE_KEY        "utterance"

#define DISCOVERY_TIMEOUT   (3)

typedef map<OCResourceIdentifier, shared_ptr<OCResource>> \
			DiscoveredResourceMap;
typedef map<string, shared_ptr<OCResource>> \
			RegisteredResourceMap;

static DiscoveredResourceMap _discovered_resources;
static RegisteredResourceMap _registered_resources;

static Util _util;
static GenericModel _generic_model;
static mutex _resource_lock;

const  char* convResCodeToString (int);
static void  foundResource       (shared_ptr<OCResource>);

static void onPost    (const HeaderOptions&, const OCRepresentation&, const int);
static void onObserve (const HeaderOptions&, const OCRepresentation&, const int, const int);
static void onGet     (const HeaderOptions&, const OCRepresentation&, const int);

static FILE* _client_fopen (const char*, const char*);
static void  _init         (PlatformConfig cfg);

int main(void)
{
	OCPersistentStorage persistentStorage {
		/* .open   = */ _client_fopen,
		/* .read   = */ fread,
		/* .write  = */ fwrite,
		/* .close  = */ fclose,
		/* .unlink = */ unlink,
	};
	
	PlatformConfig config {
		/* .serviceType = */ ServiceType::InProc,
		/* .mode        = */ ModeType::Both,
		/* .ps          = */ &persistentStorage,
	};

	_init(config);

	try {
		DeviceList_t       list;
		mutex		       blocker;
		condition_variable cond;
		unique_lock<mutex> lock(blocker);

		OC_VERIFY(OCPlatform::start() == OC_STACK_OK);

		cout << "START CLIENT" << endl;
		cout << "DISCOVER UNOWNED DEVICES..." << endl;

		list.clear();
		
		OCPlatform::findResource("",
				OC_RSRVD_WELL_KNOWN_URI,
				CT_ADAPTER_IP,
				&foundResource);

		cout << "FIND RESOURCES..." << endl;

		cond.wait(lock);

		list.clear();

		OC_VERIFY(OCPlatform::stop() == OC_STACK_OK);

		cout << "END CLIENT" << endl;
	} catch (OCException& e) {
		oclog() << "EXCEPTION IN main: " << e.what();
	}

	return 0;
}

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

void foundResource(shared_ptr<OCResource> resource)
{
	cout << "[DISCOVERY CALLBACK]" << endl;

	try {
		lock_guard<mutex> lock(_resource_lock);

		if (_discovered_resources.find(resource->uniqueIdentifier()) \
				== _discovered_resources.end()) {
			cout << "\tFOUND RESOURCE " 
			     << resource->uniqueIdentifier()
				 << " FOR THE FIRST TIME ON SERVER WITH ID: "
				 << resource->sid()
				 << endl;
		} else {
			cout << "\tFOUND RESOURCE " 
			     << resource->uniqueIdentifier()
				 << " AGAIN!"
				 << endl;
		} // end of if

		if (_registered_resources[RESOURCE_URI]) {
			cout << "\tREGISTER ANOTHER RESOURCE, IGNORING" << endl;
			return ;
		}

		if (resource) {
			OCConnectivityType transport_type_to_use = _util.CheckResourceInfo(resource);

			if (resource->uri() == RESOURCE_URI) {
				_util.CheckHeaderOptionsInfo("DISCOVERY", resource->getServerHeaderOptions());
				if (resource->connectivityType() & transport_type_to_use) {
					_registered_resources[RESOURCE_URI] = resource;
					cout << "\t<ADDRESS OF SELECTED RESOURCE: " << resource->host() << ">" << endl;
					
					_generic_model.GetRepresentation(_registered_resources[RESOURCE_URI], &onGet);
				}
			} // end of if
		} else {
			cout << "\tINVALID RESOURCE" << endl;
		} // end of if
	} catch (exception& e)  {
		 cerr << "\tEXCEPTION IN foundResource: " << e.what() << endl;
	}
}

void onPost(const HeaderOptions &/*headerOptions*/, const OCRepresentation &rep,
		const int eCode)
{
	cout << "[POST CALLBACK]" << endl;

	try {
		if (eCode == OC_STACK_OK || eCode == OC_STACK_RESOURCE_CHANGED) {
			cout << "\tPOST REQUEST WAS SUCCESSFUL\n" << endl;
			_generic_model.SetValue(rep, RESOURCE_KEY);
			cout << "\tVALUE: " << _generic_model.GetValue() << endl;
		} else {
			cout << "\tonPost RESPONSE ERROR: " << convResCodeToString(eCode) << endl;
		}
	} catch (exception& e) {
		cout << "\tEXCEPTION: " << e.what() << " IN onPost" << endl;
	}

	cout << endl;
}

void onObserve(const HeaderOptions& /*headerOptions*/, const OCRepresentation& rep,
		const int eCode, const int sequenceNumber)
{
	cout << "[OBSERVE CALLBAKC]" << endl;

	try {
		cout << "\tSEQUECE NUMBER: " << sequenceNumber << endl;

		if (eCode == OC_STACK_OK && sequenceNumber <= MAX_SEQUENCE_NUMBER) {
			const string uri = rep.getUri();

			if (sequenceNumber == OC_OBSERVE_REGISTER) {
				cout << "\tOBSERVE REGISTRATION ACTION IS SUCCESSFUL" << endl;
			}

			if (uri == RESOURCE_URI) {
				_generic_model.SetValue(rep, RESOURCE_KEY);
				cout << "\tVALUE: " << _generic_model.GetValue() << endl;
			}

			cout << endl;

			if (_registered_resources[RESOURCE_URI]) {
				string value = _generic_model.GetValue();
				cout << value << endl;

				/*@TODO: DO SOMETHING IN THIS PLACE */
			}
		} else {
			if (eCode == OC_STACK_OK) {
				cout << "\tNO OBSERVE OPTION HEADER IS RETURNED IN THE RESPONSE" << endl;
				cout << "\tFOR A REGISTRATION REQUEST, IT MEANS THE REGISTRATION FAILED" << endl;
				cout << "\tFOR A CANCELLATION REQUEST, \
					IT MEANS THE CANCELLATION WAS SUCCESSFUL" << endl;
			} else {
				cout << "\tonObserve RESPONSE ERROR: " << convResCodeToString(eCode) << endl;
				exit(-1);
			}
		} // end of if
	} catch (exception& e) {
		cout << "\tEXCEPTION: " << e.what() << " IN onObserver" << endl; 
	}

	cout << endl;
}

void onGet(const HeaderOptions &headerOptions, const OCRepresentation &rep,
		const int eCode)
{
	cout << "[GET CallBack]" << endl;

	try {
		if (eCode == OC_STACK_OK) {
			const string uri = rep.getUri();

			_util.CheckHeaderOptionsInfo("GET", headerOptions);

			cout << "\tGET REQUEST WAS SUCCESSFUL" << endl;
			cout << "\tRESOURCE URI: " << uri << endl;

			if (uri == RESOURCE_URI) {
				_generic_model.SetValue(rep, RESOURCE_KEY);
				cout << "\tvalue: " << _generic_model.GetValue() << endl;

				_generic_model.Observe(_registered_resources[RESOURCE_URI],
						ObserveType::Observe, &onObserve);
			} else {
				cout << "\tonGet RESPONSE ERROR: " << convResCodeToString(eCode) << endl;
			}
		} else {
			cout << "\tonGet RESPONSE ERROR: " << convResCodeToString(eCode) << endl;
		}
	} catch (exception& e) {
		cout << "\tException: " << e.what() << "IN onGet" << endl;
	}

	cout << endl;
}

FILE* _client_fopen(const char *path, const char *mode)
{
	char const *file_name = path;

	cout << "[IN] READING FILE=" << file_name << " mode=" << mode << endl;

	if (0 == strcmp(path, OC_SECURITY_DB_DAT_FILE_NAME))
		file_name = SVR_DB_FILE_NAME;

	cout << "[OUT] READING FILE=" << file_name << " mode=" << mode << endl; 

	return fopen(file_name, mode);
}

void _init(PlatformConfig cfg)
{
	OCPlatform::Configure(cfg);
}
