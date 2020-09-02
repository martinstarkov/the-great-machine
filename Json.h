#pragma once
#include <string>
#include <iostream>
#include <cassert>
#include <memory>
#include <fstream>
#include <sstream>
#include <variant>
#include "TGM.h"


namespace internal {

	std::string Trim(const std::string& string, char character = ' ') {
		auto start = string.find_first_not_of(character);
		auto end = string.find_last_not_of(character);
		return string.substr(start, end);
	}

	std::variant<int, double, std::string> IsIntegral(const std::string& string) {
		bool period = false;
		for (auto i = 0; i < string.length(); ++i) {
			if (!std::isdigit(i)) {
				if (!period && string[i] == '.') {
					period = true;
					continue;
				}
				return string;
			}
		}
		if (period) {
			try { 
				return std::stod(string);
			} catch (const std::invalid_argument& e) {
				LOG(e.what());
				return string;
			} catch (const std::out_of_range& e) {
				LOG(e.what());
				return string;
			}
		} else {
			try {
				return std::stoi(string);
			} catch (const std::invalid_argument& e) {
				LOG(e.what());
				return string;
			} catch (const std::out_of_range& e) {
				LOG(e.what());
				return string;
			}
		}
	}

	// Thanks to bolov for Variant Cast https://stackoverflow.com/a/47204507

	template <class... Args>
	struct variant_cast_proxy {
		std::variant<Args...> v;

		template <class... ToArgs>
		operator std::variant<ToArgs...>() const {
			return std::visit([](auto&& arg) -> std::variant<ToArgs...> { return arg; },
				v);
		}
	};

	template <class... Args>
	auto variant_cast(const std::variant<Args...>& v) -> variant_cast_proxy<Args...> {
		return { v };
	}

} // namespace internal

namespace json {

	//class will need to be able to have functions which grab strings info from jsons, identify the info,
	//convert the info into what is needed.

	//types: string, number, bool, array, null, and objects

	class JsonObject;
	class JsonArray;
	class JsonNull;

	using JsonType = std::variant<JsonObject, JsonArray, JsonNull, std::string, int, double, bool>;

	static bool TestJsonObject(const std::string& data) {
		if (data.length() == 0) return false;
		if (data.find('{') == std::string::npos) return false;
		if (data.find('}') == std::string::npos) return false;
		return true;
	}

	static bool TestJsonArray(const std::string& data) {
		if (data.length() == 0) return false;
		if (data.find('[') == std::string::npos) return false;
		if (data.find(']') == std::string::npos) return false;
		return true;
	}

	std::string JsonSubstring(const std::string& data, char start_char, char end_char) {
		auto start = data.find_first_of(start_char);
		auto end = data.find_last_of(end_char);
		assert(start != std::string::npos);
		assert(end != std::string::npos);
		auto object = data.substr(start + 1, end - start - 1);
		object.erase(std::remove(object.begin(), object.end(), '\n'), object.end());
		return object;
	}

	class JsonObject {

	public:
		JsonObject() = default;
		JsonObject(const char* file_path) {
			std::ifstream file{ file_path };
			//std::stringstream temp = 
			std::string raw_data((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
			//raw_data = std::get(file); //file.get(*std::cout.rdbuf());
			LOG(raw_data);
			if (TestJsonObject(raw_data)) {
				LOG("Yay it's good");
				ParseObject(JsonSubstring(raw_data, '{', '}'));
			} else {
				LOG("REJECTED");
				assert(false);
			}
		}
		JsonObject(const std::string& data) {
			assert(TestJsonObject(data));
			ParseObject(JsonSubstring(data, '{', '}'));
		}

		void AddPair(const std::string& key, JsonType value);

	private:

		void ParsePair(const std::string& data, int start);

		void ParseObject(const std::string& data) {
			//LOG("Data");
			//LOG(data);
			ParsePair(data, 0);
		}

		//std::string raw_data;
		std::unordered_map<std::string, JsonType> contents;

	};

	class JsonArray {

	public:
		JsonArray() = default;
		JsonArray(const std::string& data) {
			assert(TestJsonArray(data));
			ParseArray(JsonSubstring(data, '[', ']'));
		}

	private:

		void AddElement(JsonType value);

		void ParseElement(const std::string& data, int start);

		void ParseArray(const std::string& data) {
			ParseElement(data, 0);
		}

		std::vector<JsonType> contents;
	};

	class JsonNull {

	public:
		JsonNull() = default;

	private:
		bool other_temp = true;
	};

	JsonType ParseType(const std::string& value) {
		if (value == "true") return true;
		if (value == "false") return false;
		if (value == "null") return JsonNull{};
		if (TestJsonObject(value)) return JsonObject{ value };
		if (TestJsonArray(value)) return JsonArray{ value };
		auto new_value = internal::IsIntegral(value);
		if (std::is_floating_point_v<decltype(new_value)>) return internal::variant_cast(new_value);
		if (std::is_integral_v<decltype(new_value)>) return internal::variant_cast(new_value);
		return value;

	}

	void JsonObject::ParsePair(const std::string& data, int start) {
		auto colon = data.find(':', start);
		assert(colon != std::string::npos);
		std::size_t line_end;
		auto comma = data.find(',', colon);
		if (comma == std::string::npos) {
			line_end = data.length() - 1 - colon;
			//LOG("Finished");
		} else {
			//consider whitespace
			line_end = comma - colon - 1;
			ParsePair(data, comma + 1);
		}
		auto key = data.substr(start, colon - start);
		key = internal::Trim(key, ' ');
		auto value = data.substr(colon + 1, line_end);
		value = internal::Trim(value, ' ');
		AddPair(key, ParseType(value));
	}

	void JsonArray::ParseElement(const std::string& data, int start) {
		std::size_t line_end;
		auto comma = data.find(',', start);
		if (comma == std::string::npos) {
			line_end = data.length() - 1;
			//LOG("Finished");
		} else {
			//consider whitespace
			line_end = comma - start - 1;
			ParseElement(data, comma + 1);
		}
		auto element = data.substr(start, comma - start);
		element = internal::Trim(element, ' ');
		AddElement(ParseType(element));
	}

	void JsonObject::AddPair(const std::string& key, JsonType value) {
		contents.emplace(key, value);
		LOG("Key: " << key);
	}

	void JsonArray::AddElement(JsonType value) {
		contents.emplace_back(value);
	}

} // namespace json
/*
auto test = json::Json{ "test.json" };

Json({
	"chacteristic": "blue",
	"dog" : 13,
	"hair" : {
		"color": "red"
	},
	"eyes" : "green"
	}) {
	Read() {adjkjfnsfljksjfn
		test.Add("characteristic", std::string("blue")); 
		test.Add("dog", int("13")); 
		test.Add("hair", Json("{"color":"red"}")); 
		test.Add("hair", JsonArray("["bird","grey"]")); 
		test.Add("eyes",std::string("green"));
	}
} */