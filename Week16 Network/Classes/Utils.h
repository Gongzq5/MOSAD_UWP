#pragma once
#include <string>
#include "json\document.h"
#include "cocos2d.h"

std::string serializeRapidjsonDocument(const rapidjson::Document &doc);
// In C++11, it's ok to use std::to_string
std::string vector2string(std::vector<char>* buffer);