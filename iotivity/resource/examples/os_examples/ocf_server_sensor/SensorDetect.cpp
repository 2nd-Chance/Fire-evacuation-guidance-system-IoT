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

#include <iostream>
#include <errno.h>
#include <string.h>

#include <wiringPi.h>


using namespace std;


#include "SensorDetect.h"


#define LIGHT_SENSOR_IN 7


SensorDetect::SensorDetect(void)
{
	if(wiringPiSetup() < 0)
		cout << "Unable to setup wiringPi: " << strerror(errno) << endl;

	pinMode(LIGHT_SENSOR_IN, INPUT);
}

int SensorDetect::Detect(void)
{
	return digitalRead(LIGHT_SENSOR_IN);
}

