/**
 * @file json.hpp
 * @brief Minimal JSON value representation (JsonValue) and parser entry
 *        point (parseJson), used to load precinct, dataset, and geometry
 *        definitions from GeoJSON/JSON source files.
 */
#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <variant>
#include <stdexcept>

class JsonValue;
/** @brief A JSON object: an unordered map from string keys to JsonValue. */
using JsonObject = std::unordered_map<std::string, JsonValue>;
/** @brief A JSON array: an ordered vector of JsonValue. */
using JsonArray  = std::vector<JsonValue>;

/**
 * @class JsonValue
 * @brief A single JSON value -- null, boolean, number, string, array, or
 *        object -- stored as a std::variant and accessed via type-checked
 *        getters and safe key/index lookup operators.
 */
class JsonValue {
    public:
        /** @brief The underlying value: one of null, bool, number (double), string, array, or object. */
        std::variant<
            std::nullptr_t,
            bool,
            double,
            std::string,
            JsonArray,
            JsonObject
        > value;

        /** @brief Construct a null JsonValue. */
        JsonValue() : value(nullptr) {}
        /**
         * @brief Construct a null JsonValue.
         * @param  An unused nullptr tag selecting this overload.
         */
        JsonValue(std::nullptr_t)        : value(nullptr) {}
        /**
         * @brief Construct a boolean JsonValue.
         * @param b The boolean value.
         */
        JsonValue(bool b)                : value(b) {}
        /**
         * @brief Construct a numeric JsonValue.
         * @param d The numeric value.
         */
        JsonValue(double d)              : value(d) {}
        /**
         * @brief Construct a string JsonValue.
         * @param s The string value (moved in).
         */
        JsonValue(std::string s)         : value(std::move(s)) {}
        /**
         * @brief Construct an array JsonValue.
         * @param a The array value (moved in).
         */
        JsonValue(JsonArray a)           : value(std::move(a)) {}
        /**
         * @brief Construct an object JsonValue.
         * @param o The object value (moved in).
         */
        JsonValue(JsonObject o)          : value(std::move(o)) {}

        /**
         * @brief Check whether this value is null.
         * @return True if null.
         */
        bool isNull()   const { return std::holds_alternative<std::nullptr_t>(value); }
        /**
         * @brief Check whether this value is a boolean.
         * @return True if a boolean.
         */
        bool isBool()   const { return std::holds_alternative<bool>(value); }
        /**
         * @brief Check whether this value is a number.
         * @return True if a number.
         */
        bool isNumber() const { return std::holds_alternative<double>(value); }
        /**
         * @brief Check whether this value is a string.
         * @return True if a string.
         */
        bool isString() const { return std::holds_alternative<std::string>(value); }
        /**
         * @brief Check whether this value is an array.
         * @return True if an array.
         */
        bool isArray()  const { return std::holds_alternative<JsonArray>(value); }
        /**
         * @brief Check whether this value is an object.
         * @return True if an object.
         */
        bool isObject() const { return std::holds_alternative<JsonObject>(value); }

        /**
         * @brief Get this value as a boolean.
         * @return The boolean value.
         * @throws std::bad_variant_access if this value does not hold a boolean.
         */
        bool               asBool()   const { return std::get<bool>(value); }
        /**
         * @brief Get this value as a number.
         * @return The numeric value.
         * @throws std::bad_variant_access if this value does not hold a number.
         */
        double             asNumber() const { return std::get<double>(value); }
        /**
         * @brief Get this value as an integer, truncating from its underlying double representation.
         * @return The value cast to int.
         * @throws std::bad_variant_access if this value does not hold a number.
         */
        int                asInt()    const { return static_cast<int>(std::get<double>(value)); }
        /**
         * @brief Get this value as a string.
         * @return A const reference to the string value.
         * @throws std::bad_variant_access if this value does not hold a string.
         */
        const std::string& asString() const { return std::get<std::string>(value); }
        /**
         * @brief Get this value as an array.
         * @return A const reference to the array value.
         * @throws std::bad_variant_access if this value does not hold an array.
         */
        const JsonArray&   asArray()  const { return std::get<JsonArray>(value); }
        /**
         * @brief Get this value as an object.
         * @return A const reference to the object value.
         * @throws std::bad_variant_access if this value does not hold an object.
         */
        const JsonObject&  asObject() const { return std::get<JsonObject>(value); }

        // safe key lookup — returns null JsonValue if key absent or not an object
        /**
         * @brief Safely look up a key in this value, if it is an object.
         * @param key The key to look up.
         * @return A const reference to the corresponding value, or a shared null JsonValue if this is not an object or the key is absent.
         */
        const JsonValue& operator[](const std::string& key) const {
            static const JsonValue null{nullptr};
            if (!isObject()) return null;
            const auto& obj = std::get<JsonObject>(value);
            auto it = obj.find(key);
            return it != obj.end() ? it->second : null;
        }

        // safe index lookup — returns null JsonValue if out of bounds or not an array
        /**
         * @brief Safely look up an index in this value, if it is an array.
         * @param index The index to look up.
         * @return A const reference to the corresponding value, or a shared null JsonValue if this is not an array or the index is out of bounds.
         */
        const JsonValue& operator[](std::size_t index) const {
            static const JsonValue null{nullptr};
            if (!isArray()) return null;
            const auto& arr = std::get<JsonArray>(value);
            return index < arr.size() ? arr[index] : null;
        }
};

// parse a JSON string into a JsonValue tree
// throws std::runtime_error on malformed input
/**
 * @brief Parse a JSON string into a JsonValue tree.
 * @param json The JSON source text.
 * @return The parsed JsonValue tree.
 * @throws std::runtime_error on malformed input (unexpected characters, unterminated strings/escapes, unbalanced brackets, or trailing content after the top-level value).
 */
JsonValue parseJson(const std::string& json);