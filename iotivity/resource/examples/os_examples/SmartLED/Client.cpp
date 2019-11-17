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
#include <thread>
#include <sstream>
#include <vector>
#include <regex>

using namespace OC;
using namespace std;

#include "experimental/ocrandom.h"
#include "OCProvisioningManager.hpp"
#include "devMgmt/Device.h"
#include "devMgmt/DeviceBuilder.h"
#include "devMgmt/DeviceClassBuilder.h"

#include "Util.h"
#include "GenericModel.h"
#include "GenericControl.h"
#include "Bluetooth.h"
#include "audio.h"
#include "hiredis.h"

#define SVR_DB_FILE_NAME    "./oic_svr_db_client.dat"

#define RESOURCE_TYPE		"LED" /* MUST CHANGE */

#define RESOURCE_STATE      RESOURCE_STATIC /* MUST CHANGE */
#define RESOURCE_STATIC     1
#define RESOURCE_DYNAMIC    0

#define RESOURCE_URI        "/SpeechTTSResURI"
#define RESOURCE_KEY        "utterance"

#define DISCOVERY_TIMEOUT   (3)

#define BLUETOOTH_COUNT (10)
#define BLUETOOTH_DELAY (10)
#define BLUETOOTH_TIMEOUT (100)

#define ROOM_ID_FIELD "id"
#define LINKS_FIELD "lk"
#define PING_PONG_START false

#define NSEC_TO_MSEC 1000000L

typedef map<OCResourceIdentifier, shared_ptr<OCResource>> \
			DiscoveredResourceMap;
typedef map<string, shared_ptr<OCResource>> \
			RegisteredResourceMap;

static DiscoveredResourceMap _discovered_resources;
static RegisteredResourceMap _registered_resources;

static Util _util;
static GenericModel _generic_model;
static mutex _resource_lock;
static mutex _led_lock;
static Bluetooth& bluetooth = Bluetooth::getInstance();

static std::shared_ptr<GenericControl<int>> led;
static std::string uuid;

static redisContext *m_context;

const  char* convResCodeToString (int);
static void  foundResource       (shared_ptr<OCResource>);

static void onPost    (const HeaderOptions&, const OCRepresentation&, const int);
static void onObserve (const HeaderOptions&, const OCRepresentation&, const int, const int);
static void onGet     (const HeaderOptions&, const OCRepresentation&, const int);

static bool getAlertStatus(shared_ptr<OCResource> resource);
// static string getDeviceValue();
static string getPostValue(shared_ptr<OCResource> resource);
static void bluetooth_init();
static void bluetooth_state_run();
static void normal_state_run();

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

	bluetooth_init();
	led = std::make_shared<GenericControl<int>>();
	std::thread runner1(normal_state_run);
	std::thread runner2(bluetooth_state_run);

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
		redisFree(m_context);
		oclog() << "EXCEPTION IN main: " << e.what();
	}

	runner1.join();
	runner2.join();
	redisFree(m_context);
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

bool getAlertStatus(shared_ptr<OCResource> resource)
{
	std::string alert_value;
	redisReply *reply;

	bool ret = false;

	if (m_context == NULL) {
		std::regex reg("\\d{1,3}(\\.\\d{1,3}){3}");
		std::smatch result;
		std::string hostname = resource->host();
		int port = 6379;
		struct timeval timeout = {1, 500000}; // 1.5 seconds

		if (!std::regex_search(hostname, result, reg)) {
			std::cerr << "Cannot find the host" << resource->host() << std::endl;
			return ret;
		}

		hostname = result.str(0);
		std::cout << "Connection location ==> " << hostname << std::endl;
		m_context = redisConnectWithTimeout(hostname.c_str(), port ,timeout);

		if (m_context == NULL || m_context->err) {
			if (m_context) {
				std::cerr << "Connection fail to redis" << std::endl;
				redisFree(m_context);
				return false;
			} // end of if
		} else {
			std::cerr << "Cannot allocated m_context by server redis" << std::endl;
			return false;
		}
	} // end of if

	_led_lock.lock();
	reply = (redisReply *)redisCommand(m_context, "GET alert");
	if (reply == NULL) {
		printf("reply is NULL: %s\n", m_context->errstr);
		freeReplyObject(reply);
		throw std::runtime_error("cannot retrieve the value");
	}
	alert_value = reply->str;
	_led_lock.unlock();

	if (alert_value == "1") {
		led->SetValue(2);
		ret = true;
	} else {
		led->SetValue(3);
		ret = false;
	}
	return ret;
}

#if 0
string getDeviceValue()
{
	stringstream stream;
	_led_lock.lock();
	stream << led->GetValue();
	_led_lock.unlock();
	return (stream.str());
}
#endif

string getPostValue(shared_ptr<OCResource> resource)
{
	static int room_id = 0;
	stringstream stream;
	string post_value;
	string value;

	if (uuid == "") {
		stream << resource->uniqueIdentifier();
		uuid = stream.str();
		uuid = uuid.substr(0, uuid.find("/"));
	}
	
	stream.str(string()); /* reset the value stream */
	stream.clear();

	auto device = model::DeviceBuilder()
		.setUuid(uuid)
		.setRoomId(room_id) /* Same as NULL*/
		.setDeviceClass(model::DeviceClass3Builder()
			.setAlertState(getAlertStatus(resource))
			.setAliveState(true)
			.build()
		)
		.setBluetoothMac(bluetooth.getLocalMAC())
#if (RESOURCE_STATE == RESOURCE_DYNAMIC)
		.buildDynamic();
#else
		.buildStatic();
#endif
	stream << device->toJson().dump();
	return (stream.str());
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
				string post_value = getPostValue(_registered_resources[RESOURCE_URI]);
				_generic_model.PostRepresentation(_registered_resources[RESOURCE_URI],
						RESOURCE_KEY, post_value, &onPost);
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

void bluetooth_init() 
{
	bluetooth.setCount(BLUETOOTH_COUNT);
	bluetooth.setDelay(BLUETOOTH_DELAY);
	bluetooth.setTimeout(BLUETOOTH_TIMEOUT);
}

void bluetooth_state_run()
{
	struct timespec ts = {
		.tv_sec = 0,
		.tv_nsec = 250*NSEC_TO_MSEC,
	};

	std::string room_id, MACs_str;
	redisReply *reply;

	while (m_context == NULL || !PING_PONG_START) {
		nanosleep(&ts, NULL);
	}

	_led_lock.lock();
	reply = (redisReply *)redisCommand(m_context, "HMGET %s %s", uuid, ROOM_ID_FIELD);
	if (reply == NULL) {
		printf("reply is NULL: %s\n", m_context->errstr);
		freeReplyObject(reply);
		throw std::runtime_error("cannot retrieve the value");
	}
	MACs_str = reply->str;
	_led_lock.unlock();

	while (1) {
		std::regex re("([0-9A-Fa-f]{2}[:-]){5}([0-9A-Fa-f]{2})");
		_led_lock.lock();
		reply = (redisReply *)redisCommand(m_context, "HMGET %s %s", room_id, LINKS_FIELD);
		if (reply == NULL) {
			printf("reply is NULL: %s\n", m_context->errstr);
			freeReplyObject(reply);
			throw std::runtime_error("cannot retrieve the value");
		}
		MACs_str = reply->str;
		_led_lock.unlock();

		for (std::sregex_iterator it = std::sregex_iterator(MACs_str.begin(), MACs_str.end(), re);
				it != std::sregex_iterator();
				it++) {
			smatch match;
			match = *it;
			bluetooth.ping(match.str(0).c_str());
		}

		nanosleep(&ts,NULL);
	}
}

void normal_state_run() 
{	
	struct timespec ts = {
		.tv_sec = 0,
		.tv_nsec = 500*NSEC_TO_MSEC,
	};

	
	while (1) {
		led->Control();
		nanosleep(&ts,NULL);
	}
}

void _init(PlatformConfig cfg)
{
	OCPlatform::Configure(cfg);
}
