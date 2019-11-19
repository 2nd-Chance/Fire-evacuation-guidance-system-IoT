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

#include "redis-db.h"

RedisDb::RedisDb(const char *hostname, const int port, const char* password)
{
	bool is_valid = false;

	struct timeval timeout = { 1, 500000 }; // 1.5 seconds

	m_context = redisConnectWithTimeout(hostname, port, timeout);
	is_valid = !(m_context == NULL || m_context->err);

	if (!is_valid) {
		if (m_context) {
			std::cout << "Connection error: " << m_context->errstr 
				<< std::endl;
			redisFree(m_context);
		} else {
			std::cout << "Connection error: can't allocate redis context"
			       	<< std::endl;
		}
		exit(1);
	}

	if (password != NULL) {
		redisReply *reply = (redisReply *)redisCommand(m_context, "AUTH %s", password);
		if (reply == NULL) {
			printf("reply is NULL: %s\n", m_context->errstr);
			freeReplyObject(reply);
			throw std::runtime_error("cannot retrieve the value");
		}
		freeReplyObject(reply);
	}
}

std::map<std::string, std::string> RedisDb::getEntries(const char *key)
{
	std::map<std::string, std::string> entry;
	std::vector<std::string> values, keys;
	int i = 0;

	keys = getArray("HKEYS", key);
	values = getArray("HVALS", key);

	if (keys.size() != values.size())
		throw std::length_error("key and value doesn't sample length");

	for(const auto& key : keys) {
		entry[key] = values[i++];
	}

	return entry;
}

void RedisDb::appendToList(const char *key, const char *data)
{
	redisReply *reply;
	reply = (redisReply *)redisCommand(m_context, "RPUSH %s %s", key, data);
	if (reply == NULL) {
		printf("reply is NULL: %s\n", m_context->errstr);
		freeReplyObject(reply);
		throw std::runtime_error("cannot retrieve the value");
	}
	freeReplyObject(reply);
}

void RedisDb::removeFromList(const char *key, const int count, const char *value)
{
	redisReply *reply;
	reply = (redisReply *)redisCommand(m_context, "LREM %s %d %s", key, count, value);
	if (reply == NULL) {
		printf("reply is NULL: %s\n", m_context->errstr);
		freeReplyObject(reply);
		throw std::runtime_error("cannot retrieve the value");
	}
	freeReplyObject(reply);
}

std::vector<std::string> RedisDb::getList(const char *key, const int start, const int end)
{
	std::vector<std::string> resultList;
	redisReply *reply;
	int i;

	reply = (redisReply *)redisCommand(m_context, "LRANGE %s %d %d", key, start, end);
	if (reply == NULL) {
		printf("reply is NULL: %s\n", m_context->errstr);
		freeReplyObject(reply);
		throw std::runtime_error("cannot retrieve the value");
	}

	for(i = 0; i < (int)reply->elements; i++) {
		std::string element(reply->element[i]->str);
		resultList.push_back(element);
	}
	return resultList;
}

void RedisDb::setEntries(const char *key,
	       	std::map<std::string, std::string> &entriesInfo)
{
	std::stringstream command;
	redisReply *reply;

	command << "HMSET " << key;
	for(const auto& field : entriesInfo) {
		command << " " << field.first << " " << field.second;
	}

	reply = (redisReply *)redisCommand(m_context, command.str().c_str());
	if (reply == NULL) {
		printf("reply is NULL: %s\n", m_context->errstr);
		freeReplyObject(reply);
		throw std::runtime_error("cannot retrieve the value");
	}
	freeReplyObject(reply);
}

std::vector<std::string> RedisDb::getArray(const char *cmd, const char *key)
{
	std::vector<std::string> resultList;
	redisReply *reply;
	int i;

	reply = (redisReply *)redisCommand(m_context, "%s %s", cmd, key);
	if (reply == NULL) {
		printf("reply is NULL: %s\n", m_context->errstr);
		freeReplyObject(reply);
		throw std::runtime_error("cannot retrieve the value");
	}

	for(i = 0; i < (int)reply->elements; i++) {
		std::string element(reply->element[i]->str);
		resultList.push_back(element);
	}
	return resultList;
}

RedisDb::~RedisDb()
{
	redisFree(m_context);
}

#ifdef REDIS_DB_TEST_MODE_ON
int main(void)
{
	std::unique_ptr<RedisDb> redisDb(new RedisDb("127.0.0.1"));
	std::map<std::string, std::string> src;
	std::map<std::string, std::string> entries;
	std::vector<std::string> values;

	src["name"] = "gijun";
	src["age"] = "26";
	src["birth"] = "1995-11-21";
	src["gender"] = "Male";

	redisDb->setEntries("test", src);
	entries = redisDb->getEntries("test");
	std::cout << entries["name"] << std::endl
		<< entries["age"] << std::endl
		<< entries["gender"] << std::endl
		<< entries["birth"] << std::endl;

	redisDb->appendToList("sample", "hello");
	redisDb->appendToList("sample", "trash");
	redisDb->appendToList("sample", "world");

	values = redisDb->getList("sample", 0, -1);
	for (auto& value : values)
		std::cout << value << std::endl;
	redisDb->removeFromList("sample", 1, "trash");

	values = redisDb->getList("sample", 0, -1);
	for (auto& value : values)
		std::cout << value << std::endl;
	return 0;
}
#endif
