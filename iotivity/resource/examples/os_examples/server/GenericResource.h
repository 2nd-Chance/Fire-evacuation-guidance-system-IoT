#ifndef GENERIC_RESOURCE_H__
#define GENERIC_RESOURCE_H__

#include "GenericControl.h"

class GenericResource
{
public:
    GenericResource(void);

    void CreateResource(void);

    OCRepresentation Get(void);

    void Post(OCRepresentation rep);

	void Observe(ObservationInfo observationInfo);

    OCResourceHandle GetHandle(void) { return m_resourceHandle; }

private:
	OCEntityHandlerResult EntityHandlerCB(shared_ptr<OCResourceRequest> request);

	static void* ChangeGenericRepresentation(void *param);

public:
	OCResourceHandle m_resourceHandle;
	OCRepresentation m_representation;
    ObservationIds m_interestedObservers;

	GenericControl m_genericControl;

	bool m_isStartedThread;
};

#endif
