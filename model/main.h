#pragma once

#include "Room.h"

int main(void);

std::string testCreationWithBuilder(void);

std::string testCreationWithoutBuilder(void);

std::shared_ptr<model::Room> testParsing(std::string &jsonString);