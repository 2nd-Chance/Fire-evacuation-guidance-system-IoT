#include "OCPlatform.h"

using namespace OC;
using namespace std;

#include "Util.h"

#define CA_OPTION_CONTENT_VERSION   2053
#define COAP_OPTION_CONTENT_FORMAT  12

OCConnectivityType Util::CheckResourceInfo(shared_ptr<OCResource> resource)
{
	OCConnectivityType transportTypeToUse = CT_ADAPTER_IP;

	cout << "[DISCOVERED RESOURCE]" << endl;

	cout << "\tURI OF THE RESOURCE: "
		 << resource->uri() << endl;

	cout << "\tHOST ADDRESS OF THE RESOURCE: "
		 << resource->host() << endl;

	cout << "\tLIST OF RESOURCE TYPES: " << endl;
	for (auto &resTypes : resource->getResourceTypes()) {
		cout << "\t\t" << resTypes << endl;
	}

	cout << "\tLIST OF RESOURCE INTERFACES: " << endl;
	for (auto &resInterfaces : resource->getResourceInterfaces()) {
		cout << "\t\t" << resInterfaces << endl;
	}

	cout << "\tHOST OF RESOURCE: "     << endl;
	cout << "\t\t" << resource->host() << endl;

	cout << "\tLIST OF RESOURCE ENDPOINTS: " << endl;
	for (auto &resEndPoints : resource->getAllHosts()) {
		cout << "\t\t" << resEndPoints << endl;
	}

	bool isCoap     = (string::npos == resource->host().find("coap://"));
	bool isCoaps    = (string::npos == resource->host().find("coaps://"));
	bool isTcpCoap  = (string::npos == resource->host().find("coap+tcp://"));
	bool isTcpCoaps = (string::npos == resource->host().find("coaps+tcp://"));

	bool isIpBased  = !isCoap || !isCoaps || !isTcpCoap || !isTcpCoaps;

	if (isIpBased) {
		for (auto &resEndPoints : resource->getAllHosts()) {
			bool isHost       = (resEndPoints.compare(resource->host()) == 0);
			bool isCoapRfcomm = (string::npos \
					== resEndPoints.find("coap+rfcomm"));
			if (!isHost && isCoapRfcomm) {
				string newHost = resEndPoints;
				bool   isTcp   = (string::npos == newHost.find("tcp"));
				if (!isTcp) {
					transportTypeToUse = CT_ADAPTER_TCP;
				} else {
					transportTypeToUse = CT_ADAPTER_IP;
				} // end of if (TCP CHECK)

				cout << "\tCHANGE HOST OF RESOURCE ENDPOINTS" << endl;
				cout << "\t\t" << "CURRENT HOST IS" 
					 << resource->setHost(newHost) << endl;

				break;
			} // end of if (HOST AND COAP CHECK)
		} // end of for
	} // end of if (IS ADAPTER CHECK)

	cout << endl;

	return transportTypeToUse;
}

void Util::CheckHeaderOptionsInfo (const string type,
		const HeaderOptions headerOptions)
{
	cout << "[" << type << " HEADER OPTION] "<< endl;

	if (headerOptions.size() == 0) {
		cout << "\tNo header option exists" << endl;
	} else {
		const auto &optionsBegin = headerOptions.begin();
		const auto &optionsEnd   = headerOptions.end();
		for (auto it = optionsBegin; it != optionsEnd; ++it) {
			uint16_t optionID = it->getOptionID();
			size_t dataLength = it->getOptionData().length();
			char *optionData;
			int format, version;

			try {
				optionData = new char[dataLength];
			} catch (bad_alloc exception) {
				cerr << "Bad allocation error" << exception.what() << endl;
				return ;
			}
			strncpy(optionData, it->getOptionData().c_str(), dataLength);

			format = optionData[0] * 256 + optionData[1];
			version = optionData[0] * 256;

			if(optionID == COAP_OPTION_CONTENT_FORMAT) {
				cout << "\tServer format in " << type << " response:" << format << endl;
			} else if(optionID == CA_OPTION_CONTENT_VERSION) {
				cout << "\tServer version in " << type << " response:" << version << endl;
			} // end of if

			delete[] optionData;
		}
	} // end of if
}
