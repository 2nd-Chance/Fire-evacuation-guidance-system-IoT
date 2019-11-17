//******************************************************************
//
// Copyright 2019 BlaCkinkGJ All Rights Reserved.
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

#ifndef REDIS_DB_H_
#define REDIS_DB_H_

#include <iostream>
#include <memory>
#include <string>
#include <map>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <hiredis.h>
#include <sstream>
#include <stdexcept>
#include <vector>

class RedisDb
{
public:
	RedisDb(const char *hostname="127.0.0.1", const int port=6379, const char *password=NULL);
	std::map<std::string, std::string> getEntries(const char *key);
	void appendToList(const char *key, const char *data);
	void removeFromList(const char *key, const int count, const char *value);
	std::vector<std::string> getList(const char *key, const int start, const int end);
	void setEntries(const char *key, 
			std::map<std::string, std::string> &entriesInfo);
	std::vector<std::string> getArray(const char *cmd, const char *key);
	~RedisDb(void);
private:
	redisContext *m_context;
};

#endif
