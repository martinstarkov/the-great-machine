#pragma once
#include <string>
#include <iostream>
#include <cassert>
#include <memory>
#include <fstream>
#include <algorithm>
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

	enum class ParseFlag {
		COMMA = 0,
		CURLY,
		BRACKET,
		END
	};

	// Following two functions test if the passed string holds needed characters for object/array
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

	// Creates a substring from the first star_char to the last of end_char then removes new lines.
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
			std::string data((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
			LOG(data);
			assert(TestJsonObject(data));
			ParseObject(JsonSubstring(data, '{', '}'));
		}
		JsonObject(const std::string& key, const std::string& data) : key{ key } {
			assert(TestJsonObject(data));
			ParseObject(JsonSubstring(data, '{', '}'));
		}

		void SetKey(int index) {
			key = std::to_string(index);
		}
		friend std::ostream& operator<<(std::ostream& os, const JsonObject& obj);

	private:
		void AddPair(const std::string& key, JsonType value);

		void ParsePair(const std::string& data, std::size_t start = 0);

		void ParseObject(const std::string& data) {
			ParsePair(data);
		}

		std::string key = "json";
		std::unordered_map<std::string, JsonType> contents;

	};

	class JsonArray {

	public:
		JsonArray() = default;
		JsonArray(const std::string& key, const std::string& data) : key{ key } {
			assert(TestJsonArray(data));
			ParseArray(JsonSubstring(data, '[', ']'));
		}
		void SetKey(int index) {
			key = std::to_string(index);
		}
		friend std::ostream& operator<<(std::ostream& os, const JsonArray& obj);
	private:

		void AddElement(JsonType value);

		void ParseElement(const std::string& data, std::size_t start = 0);

		void ParseArray(const std::string& data);

		std::string key;
		std::vector<JsonType> contents;
	};

	class JsonNull {

	public:
		JsonNull() = default;
		friend std::ostream& operator<<(std::ostream& os, const JsonNull& obj);
	private:
		char null;
	};

	// array_index used for objects created inside arrays
	JsonType ParseType(const std::string& key, const std::string& value) {
		if (value == "true") return true;
		if (value == "false") return false;
		if (value == "null") return JsonNull{};
		if (TestJsonArray(value)) return JsonArray{ key, value };
		if (TestJsonObject(value)) return JsonObject{ key, value };
		auto new_value = internal::IsIntegral(value);
		if (std::is_floating_point_v<decltype(new_value)>) return internal::variant_cast(new_value);
		if (std::is_integral_v<decltype(new_value)>) return internal::variant_cast(new_value);
		return value;

	}

	// recursive function that checks for nested objects or arrays and returns the location of the outermost end 
	std::size_t JsonObject::NestedSearch(const std::string& data, ParseFlag& flag) {
		char character;
		char character_back;
		if (flag == ParseFlag::CURLY) {
			character = '{';
			character_back = '}';
		} else {
			character = '[';
			character_back = ']';
		}
		auto open_symbol = data.find(character);
		if (open_symbol == std::string::npos) open_symbol = data.length() - 1;
		auto close_symbol = data.find(character_back);
		if (close_symbol < open_symbol) { // there is no object inside this one
			return close_symbol;
		} else { // there is another object
			// we need to recurse without losing the location of the final }/] in relation to original data
			//NestedSearch(data, flag);
		}
	}

	// Finds whether the object is primitive or not by looking at { and ]
	void JsonObject::NextImportant(const std::string& data, std::size_t& result, ParseFlag& flag) {
		//look for comma, {, and [
		auto first_comma = data.find(',');
		auto first_curly = data.find('{');
		auto first_bracket = data.find('[');
		if (first_comma != std::string::npos) {
			if (first_comma < result) {
				flag = ParseFlag::COMMA;
				result = first_comma;
			}
		}
		if (first_curly != std::string::npos) {
			if (first_curly < result) {
				flag = ParseFlag::CURLY;
				result = first_curly;
			}
		}
		if (first_bracket != std::string::npos) {
			if (first_bracket < result) {
				flag = ParseFlag::BRACKET;
				result = first_bracket;
			}
		}
	}

	void JsonObject::ParsePairTwo(const std::string& data) {
		auto result = data.length() - 1;
		ParseFlag flag{ ParseFlag::END };
		// first we find the key and save it
		auto colon = data.find(':');
		assert(colon != std::string::npos);
		auto key = data.substr(0, colon);
		key = internal::Trim(key, ' ');
		// now we remove it and focus on the contents
		data.substr(colon + 1);
		NextImportant(data, result, flag);
		if (flag == ParseFlag::COMMA) {
			auto value = internal::Trim(data.substr(0, result - 1), ' ');
			ParsePairTwo(internal::Trim(data.substr(result + 1), ' '));
		}
		if (flag == ParseFlag::CURLY) {
			NestedSearch(data, flag);
		}
		if (flag == ParseFlag::BRACKET) {
			NestedSearch(data, flag);
		}
	}


	void JsonObject::ParsePair(const std::string& data, std::size_t start) {
		auto colon = data.find(':', start);
		assert(colon != std::string::npos);
		// Optimize this away using sub string algorithm
		std::size_t line_end;
		auto comma = data.find(',', colon);
		auto bracket = data.find('[', colon);
		if (bracket >= comma) {
			if (comma == std::string::npos) {
				line_end = data.length() - colon - 1;
				//LOG("Finished");
			} else {
				//consider whitespace
				line_end = comma - colon - 1;
				ParsePair(data, comma + 1);
			}
		} else { // array found
			auto closing_bracket = data.find(']', colon);
			line_end = closing_bracket - colon;
			auto next_comma = data.find(',', closing_bracket);
			if (next_comma != std::string::npos) {
				ParsePair(data, next_comma + 1);
			}
		}
		// Optimize this away using sub string algorithm
		auto key = data.substr(start, colon - start);
		key = internal::Trim(key, ' ');
		auto value = data.substr(colon + 1, line_end);
		value = internal::Trim(value, ' ');
		AddPair(key, ParseType(key, value));
	}

	// TODO: Nested arrays (sub arrays, for e.g. 2d arrays) abort with assert(line_end != std::string::npos); so check it out
	void JsonArray::ParseElement(const std::string& data, std::size_t start) {
		std::size_t line_end;
		auto comma = data.find(',', start);
		auto curly = data.find('{', start);
		auto bracket = data.find('[', start);
		// Optimize this away using sub string algorithm
		if (comma == std::string::npos) {
			comma = data.length() + 1;
		}
		if (curly == std::string::npos) {
			curly = data.length() + 1;
		}
		if (bracket == std::string::npos) {
			bracket = data.length() + 1;
		}
		if (bracket >= comma && curly >= comma) {
			if (comma == data.length() + 1) {
				line_end = data.length() - 1 - start - 1;
			} else {
				line_end = comma - start;
				ParseElement(data, comma + 1);
			}
		} else {
			// Optimize this away using sub string algorithm
			line_end = data.length() - 1;
			if (curly < bracket) {
				line_end = data.find('}', curly);
				assert(line_end != std::string::npos);
				ParseElement(data, line_end + 2);
				line_end += 1;
				line_end -= start;
			} else if (curly >= bracket) {
				line_end = data.find(']', bracket);
				assert(line_end != std::string::npos);
				line_end += 2;
				line_end -= start;
			}
		}
		auto element = data.substr(start, line_end);
		element = internal::Trim(element, ' ');
		AddElement(std::move(ParseType(key, element)));
	}

	void JsonArray::ParseArray(const std::string& data) {
		ParseElement(data);
		// Elements are added to contents in reverse order so it must be flipped after parsing
		std::reverse(contents.begin(), contents.end());
		// Set keys of JsonType structures (necessary if you wish to have non empty keys for them)
		// NOTE: must be done after flipping order
		for (auto i = 0; i < contents.size(); ++i) {
			auto& value = contents[i];
			if (std::holds_alternative<JsonArray>(value)) {
				std::get<JsonArray>(value).SetKey(i);
			} else if (std::holds_alternative<JsonObject>(value)) {
				std::get<JsonObject>(value).SetKey(i);
			}
		}
	}

	void JsonObject::AddPair(const std::string& key, JsonType value) {
		contents.emplace(key, value);
	}

	void JsonArray::AddElement(JsonType value) {
		static std::size_t size = 0;
		if (size >= contents.size()) {
			contents.resize(size + 1);
		}
		contents[size] = std::move(value);
		++size;
	}

	template<typename T, typename... Ts>
	std::ostream& operator<<(std::ostream& os, const std::variant<T, Ts...>& v) {
		std::visit([&os](auto&& arg) {
			os << arg;
		}, v);
		return os;
	}

	std::ostream& operator<<(std::ostream& os, const JsonObject& obj) {
		os << obj.key << " : { ";
		for (auto it = std::begin(obj.contents); it != std::end(obj.contents); ++it) {
			auto& pair = *it;
			// do not print keys for objects / arrays
			if (std::holds_alternative<JsonObject>(pair.second) || std::holds_alternative<JsonArray>(pair.second)) {
				os << pair.second;
			} else {
				os << pair.first << " : " << pair.second;
			}
			// No comma for last entry
			if (it != --std::end(obj.contents)) {
				os << ", ";
			}
		}
		os << " }";
		return os;
	}

	std::ostream& operator<<(std::ostream& os, const JsonArray& obj) {
		os << obj.key << " : [ ";
		for (auto it = std::begin(obj.contents); it != std::end(obj.contents); ++it) {
			os << *it;
			// No comma for last entry
			if (it != --std::end(obj.contents)) {
				os << ", ";
			}
		}
		os << " ]";
		return os;
	}

	std::ostream& operator<<(std::ostream& os, const JsonNull& obj) {
		os << "null";
		return os;
	}

} // namespace json