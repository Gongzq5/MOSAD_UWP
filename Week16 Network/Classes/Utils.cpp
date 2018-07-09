#include "Utils.h"
#include "json\writer.h"

std::string serializeRapidjsonDocument(const rapidjson::Document &doc) {
  rapidjson::StringBuffer buffer;
  buffer.Clear();
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  doc.Accept(writer);

  return std::string(buffer.GetString());
}

std::string vector2string(std::vector<char> *buffer) {
	std::string res;
	for (int i = 0; i < buffer->size(); ++i) {
		res += (*buffer)[i];
	}
	return res;
}