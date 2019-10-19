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

class LightControl;

class LightResource
{
public:
    LightResource(void);

    void CreateResource(void);

    OCRepresentation Get(void);

    void Post(OCRepresentation rep);

	void Observe(ObservationInfo observationInfo);

    OCResourceHandle GetHandle(void) { return m_resourceHandle; }

private:
	OCEntityHandlerResult EntityHandlerCB(shared_ptr<OCResourceRequest> request);

	static void* ChangeLightRepresentation(void *param);

public:
	OCResourceHandle m_resourceHandle;
	OCRepresentation m_representation;
    ObservationIds m_interestedObservers;

	LightControl m_lightControl;

	bool m_isStartedThread;
};

