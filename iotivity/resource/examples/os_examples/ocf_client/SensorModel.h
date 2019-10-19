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

class SensorModel
{
public:
	SensorModel(void);

	void GetRepresentation(shared_ptr<OCResource> resource,
			GetCallback callback);

	void Observe(shared_ptr<OCResource> resource, ObserveType type,
			ObserveCallback callback);

	int GetIlluminance(void) { return m_illuminance; }

	void SetIlluminance(OCRepresentation representation, string key);

private:
	/// 0: dark, 1: full
	int m_illuminance;
};

