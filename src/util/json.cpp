/**
 * @file json.cpp
 * @brief Recursive-descent JSON parser implementation. Defines the
 *        internal Parser struct used to build a JsonValue tree, and
 *        implements parseJson() (see json.hpp for its documentation).
 */
#include "json.hpp"
#include <cctype>
#include <cstdlib>

// ── parser state ─────────────────────────────────────────────────────────────

/**
 * @struct Parser
 * @brief Internal recursive-descent parser state for turning a JSON source
 *        string into a JsonValue tree. Not exposed outside this file; used
 *        only by parseJson().
 */
struct Parser {
    /** @brief The JSON source text being parsed. */
    const std::string& src;
    /** @brief The current read offset into src. */
    std::size_t pos = 0;

    /**
     * @brief Construct a parser over the given source text, starting at position 0.
     * @param src The JSON source text to parse.
     */
    Parser(const std::string& src) : src(src) {}

    // ── helpers ───────────────────────────────────────────────────────────────

    /**
     * @brief Look at the character at the current position without consuming it.
     * @return The character at pos.
     * @throws std::runtime_error if pos is at or past the end of the input.
     */
    char peek() const {
        if (pos >= src.size()) throw std::runtime_error("JSON: unexpected end of input");
        return src[pos];
    }

    /**
     * @brief Consume and return the character at the current position, advancing pos.
     * @return The character that was at pos.
     * @throws std::runtime_error if pos is at or past the end of the input.
     */
    char consume() {
        if (pos >= src.size()) throw std::runtime_error("JSON: unexpected end of input");
        return src[pos++];
    }

    /**
     * @brief Consume the next character and verify it matches an expected character.
     * @param c The expected character.
     * @throws std::runtime_error if the consumed character does not equal c.
     */
    void expect(char c) {
        char got = consume();
        if (got != c) throw std::runtime_error(
            std::string("JSON: expected '") + c + "', got '" + got + "' at pos " + std::to_string(pos-1));
    }

    /**
     * @brief Advance pos past any run of whitespace characters.
     */
    void skipWhitespace() {
        while (pos < src.size() && std::isspace((unsigned char)src[pos]))
            ++pos;
    }

    // ── value parsers ─────────────────────────────────────────────────────────

    /**
     * @brief Parse a single JSON value of any type, dispatching on the next non-whitespace character.
     * @return The parsed JsonValue.
     * @throws std::runtime_error if the next character does not begin a valid JSON value.
     */
    JsonValue parseValue() {
        skipWhitespace();
        char c = peek();
        if (c == '"')  return parseString();
        if (c == '{')  return parseObject();
        if (c == '[')  return parseArray();
        if (c == 't')  return parseLiteral("true",  JsonValue(true));
        if (c == 'f')  return parseLiteral("false", JsonValue(false));
        if (c == 'n')  return parseLiteral("null",  JsonValue(nullptr));
        if (c == '-' || std::isdigit((unsigned char)c)) return parseNumber();
        throw std::runtime_error(std::string("JSON: unexpected character '") + c + "' at pos " + std::to_string(pos));
    }

    /**
     * @brief Consume a fixed keyword literal (e.g. "true", "false", "null") character by character.
     * @param literal The exact keyword expected at the current position.
     * @param result The JsonValue to return once the literal has been consumed.
     * @return result, once the literal has matched.
     * @throws std::runtime_error if any character does not match the expected literal.
     */
    JsonValue parseLiteral(const std::string& literal, JsonValue result) {
        for (char c : literal) expect(c);
        return result;
    }

    /**
     * @brief Parse a JSON number (integer, decimal, and/or exponent parts) into a numeric JsonValue.
     * @return The parsed JsonValue holding a double.
     * @throws std::runtime_error if the number's integer part is missing a digit.
     */
    JsonValue parseNumber() {
        std::size_t start = pos;
        if (peek() == '-') ++pos;
        if (!std::isdigit((unsigned char)peek()))
            throw std::runtime_error("JSON: invalid number at pos " + std::to_string(pos));
        while (pos < src.size() && std::isdigit((unsigned char)src[pos])) ++pos;
        if (pos < src.size() && src[pos] == '.') {
            ++pos;
            while (pos < src.size() && std::isdigit((unsigned char)src[pos])) ++pos;
        }
        if (pos < src.size() && (src[pos] == 'e' || src[pos] == 'E')) {
            ++pos;
            if (pos < src.size() && (src[pos] == '+' || src[pos] == '-')) ++pos;
            while (pos < src.size() && std::isdigit((unsigned char)src[pos])) ++pos;
        }
        std::string numStr = src.substr(start, pos - start);
        return JsonValue(std::stod(numStr));
    }

    /**
     * @brief Parse a double-quoted JSON string, resolving backslash escape sequences.
     *
     * Unicode ("\\uXXXX") escapes are recognized and skipped over but not
     * decoded; each is emitted as a literal '?' placeholder character.
     *
     * @return The parsed JsonValue holding the decoded string.
     * @throws std::runtime_error if the string or an escape sequence is unterminated, or an unknown escape character is used.
     */
    JsonValue parseString() {
        expect('"');
        std::string result;
        while (true) {
            if (pos >= src.size())
                throw std::runtime_error("JSON: unterminated string");
            char c = src[pos++];
            if (c == '"') break;
            if (c == '\\') {
                if (pos >= src.size())
                    throw std::runtime_error("JSON: unterminated escape sequence");
                char esc = src[pos++];
                switch (esc) {
                    case '"':  result += '"';  break;
                    case '\\': result += '\\'; break;
                    case '/':  result += '/';  break;
                    case 'b':  result += '\b'; break;
                    case 'f':  result += '\f'; break;
                    case 'n':  result += '\n'; break;
                    case 'r':  result += '\r'; break;
                    case 't':  result += '\t'; break;
                    case 'u':
                        // basic 4-hex-digit unicode escape → skip for now, emit '?'
                        if (pos + 4 > src.size())
                            throw std::runtime_error("JSON: invalid unicode escape");
                        pos += 4;
                        result += '?';
                        break;
                    default:
                        throw std::runtime_error(std::string("JSON: unknown escape '\\") + esc + "'");
                }
            } else {
                result += c;
            }
        }
        return JsonValue(std::move(result));
    }

    /**
     * @brief Parse a JSON array ("[...]") of comma-separated values.
     * @return The parsed JsonValue holding a JsonArray.
     * @throws std::runtime_error if elements are not separated by ',' or the array is not properly closed with ']'.
     */
    JsonValue parseArray() {
        expect('[');
        JsonArray arr;
        skipWhitespace();
        if (peek() == ']') { consume(); return JsonValue(std::move(arr)); }
        while (true) {
            arr.push_back(parseValue());
            skipWhitespace();
            char c = consume();
            if (c == ']') break;
            if (c != ',') throw std::runtime_error(
                std::string("JSON: expected ',' or ']', got '") + c + "' at pos " + std::to_string(pos-1));
        }
        return JsonValue(std::move(arr));
    }

    /**
     * @brief Parse a JSON object ("{...}") of comma-separated "key": value pairs.
     * @return The parsed JsonValue holding a JsonObject.
     * @throws std::runtime_error if a key/value pair is malformed, members are not separated by ',', or the object is not properly closed with '}'.
     */
    JsonValue parseObject() {
        expect('{');
        JsonObject obj;
        skipWhitespace();
        if (peek() == '}') { consume(); return JsonValue(std::move(obj)); }
        while (true) {
            skipWhitespace();
            std::string key = parseString().asString();
            skipWhitespace();
            expect(':');
            obj[std::move(key)] = parseValue();
            skipWhitespace();
            char c = consume();
            if (c == '}') break;
            if (c != ',') throw std::runtime_error(
                std::string("JSON: expected ',' or '}', got '") + c + "' at pos " + std::to_string(pos-1));
        }
        return JsonValue(std::move(obj));
    }
};

// ── public entry point ────────────────────────────────────────────────────────

JsonValue parseJson(const std::string& json) {
    Parser p(json);
    JsonValue result = p.parseValue();
    p.skipWhitespace();
    if (p.pos != json.size())
        throw std::runtime_error("JSON: trailing content at pos " + std::to_string(p.pos));
    return result;
}