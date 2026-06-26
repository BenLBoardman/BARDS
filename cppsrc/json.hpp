#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <variant>
#include <stdexcept>

class JsonValue;
using JsonObject = std::unordered_map<std::string, JsonValue>;
using JsonArray  = std::vector<JsonValue>;

class JsonValue {
    public:
        std::variant<
            std::nullptr_t,
            bool,
            double,
            std::string,
            JsonArray,
            JsonObject
        > value;

        JsonValue() : value(nullptr) {}
        JsonValue(std::nullptr_t)        : value(nullptr) {}
        JsonValue(bool b)                : value(b) {}
        JsonValue(double d)              : value(d) {}
        JsonValue(std::string s)         : value(std::move(s)) {}
        JsonValue(JsonArray a)           : value(std::move(a)) {}
        JsonValue(JsonObject o)          : value(std::move(o)) {}

        bool isNull()   const { return std::holds_alternative<std::nullptr_t>(value); }
        bool isBool()   const { return std::holds_alternative<bool>(value); }
        bool isNumber() const { return std::holds_alternative<double>(value); }
        bool isString() const { return std::holds_alternative<std::string>(value); }
        bool isArray()  const { return std::holds_alternative<JsonArray>(value); }
        bool isObject() const { return std::holds_alternative<JsonObject>(value); }

        bool               asBool()   const { return std::get<bool>(value); }
        double             asNumber() const { return std::get<double>(value); }
        int                asInt()    const { return static_cast<int>(std::get<double>(value)); }
        const std::string& asString() const { return std::get<std::string>(value); }
        const JsonArray&   asArray()  const { return std::get<JsonArray>(value); }
        const JsonObject&  asObject() const { return std::get<JsonObject>(value); }

        // safe key lookup — returns null JsonValue if key absent or not an object
        const JsonValue& operator[](const std::string& key) const {
            static const JsonValue null{nullptr};
            if (!isObject()) return null;
            const auto& obj = std::get<JsonObject>(value);
            auto it = obj.find(key);
            return it != obj.end() ? it->second : null;
        }

        // safe index lookup — returns null JsonValue if out of bounds or not an array
        const JsonValue& operator[](std::size_t index) const {
            static const JsonValue null{nullptr};
            if (!isArray()) return null;
            const auto& arr = std::get<JsonArray>(value);
            return index < arr.size() ? arr[index] : null;
        }
};

// parse a JSON string into a JsonValue tree
// throws std::runtime_error on malformed input
JsonValue parseJson(const std::string& json);
