#include "json.hpp"
#include <cctype>
#include <cstdlib>

// ── parser state ─────────────────────────────────────────────────────────────

struct Parser {
    const std::string& src;
    std::size_t pos = 0;

    Parser(const std::string& src) : src(src) {}

    // ── helpers ───────────────────────────────────────────────────────────────

    char peek() const {
        if (pos >= src.size()) throw std::runtime_error("JSON: unexpected end of input");
        return src[pos];
    }

    char consume() {
        if (pos >= src.size()) throw std::runtime_error("JSON: unexpected end of input");
        return src[pos++];
    }

    void expect(char c) {
        char got = consume();
        if (got != c) throw std::runtime_error(
            std::string("JSON: expected '") + c + "', got '" + got + "' at pos " + std::to_string(pos-1));
    }

    void skipWhitespace() {
        while (pos < src.size() && std::isspace((unsigned char)src[pos]))
            ++pos;
    }

    // ── value parsers ─────────────────────────────────────────────────────────

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

    JsonValue parseLiteral(const std::string& literal, JsonValue result) {
        for (char c : literal) expect(c);
        return result;
    }

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
