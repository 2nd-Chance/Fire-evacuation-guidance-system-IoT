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

class LightModel
{
public:
	LightModel(void);

	void GetRepresentation(shared_ptr<OCResource> resource,
			GetCallback callback);

	void PostRepresentation(shared_ptr<OCResource> resource, string key, bool value,
			PostCallback callback);

	void Observe(shared_ptr<OCResource> resource, ObserveType type,
			ObserveCallback callback);

	bool GetValue(void) { return m_value; }

	void SetValue(OCRepresentation representation, string key);

private:
	/// false: off, true: on
	bool m_value;
};

