#include "logic_expression_impl.h"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

enum class LETokenType { Identifier, Number, Boolean, EnumConstant, ParenOpen, ParenClose, Comma, Operator, End };

struct Token {
    LETokenType Type;
    std::string Text;
    size_t StartIndex;
    size_t EndIndex;
};

static std::string GetCharacterErrorContext(const std::string& input, size_t pos, size_t contextLen = 10) {
    size_t start = (pos >= contextLen) ? pos - contextLen : 0;
    size_t end = std::min(input.size(), pos + contextLen + 1);
    std::string contextLine = input.substr(start, end - start);
    std::string pointerLine;
    pointerLine.append(pos - start, ' ');
    pointerLine.push_back('^');
    return "\n" + contextLine + "\n" + pointerLine;
}

bool IsEnumConstant(const std::string& s) {
    if (LogicExpression::Impl::enumMap.empty()) {
        LogicExpression::Impl::PopulateEnumMap();
    }
    return LogicExpression::Impl::enumMap.find(s) != LogicExpression::Impl::enumMap.end();
}

static bool IsIdentifierChar(char c) {
    return std::isalnum(static_cast<unsigned char>(c)) || c == '_' || c == '.' || c == ':';
}

static std::vector<Token> Tokenize(const std::string& input) {
    std::vector<Token> tokens;
    size_t i = 0;
    const size_t len = input.length();

    while (i < len) {
        if (std::isspace(static_cast<unsigned char>(input[i]))) {
            ++i;
            continue;
        }

        size_t start = i;
        if (std::isalpha(static_cast<unsigned char>(input[i]))) {
            while (i < len && IsIdentifierChar(input[i]))
                ++i;
            std::string id = input.substr(start, i - start);
            if (id == "true" || id == "false")
                tokens.emplace_back(Token{ LETokenType::Boolean, id, start, i });
            else if (IsEnumConstant(id))
                tokens.emplace_back(Token{ LETokenType::EnumConstant, id, start, i });
            else
                tokens.emplace_back(Token{ LETokenType::Identifier, id, start, i });
        } else if (std::isdigit(static_cast<unsigned char>(input[i]))) {
            while (i < len && std::isdigit(static_cast<unsigned char>(input[i])))
                ++i;
            tokens.emplace_back(Token{ LETokenType::Number, input.substr(start, i - start), start, i });
        } else if (input[i] == '(') {
            tokens.emplace_back(Token{ LETokenType::ParenOpen, "(", start, start + 1 });
            ++i;
        } else if (input[i] == ')') {
            tokens.emplace_back(Token{ LETokenType::ParenClose, ")", start, start + 1 });
            ++i;
        } else if (input[i] == ',') {
            tokens.emplace_back(Token{ LETokenType::Comma, ",", start, start + 1 });
            ++i;
        } else if (input[i] == '?') {
            tokens.emplace_back(Token{ LETokenType::Operator, "?", start, start + 1 });
            ++i;
        } else if (input[i] == ':') {
            tokens.emplace_back(Token{ LETokenType::Operator, ":", start, start + 1 });
            ++i;
        } else if (i + 1 < len &&
                   ((input[i] == '!' && input[i + 1] == '=') || (input[i] == '&' && input[i + 1] == '&') ||
                    (input[i] == '|' && input[i + 1] == '|') || (input[i] == '=' && input[i + 1] == '=') ||
                    (input[i] == '>' && input[i + 1] == '=') || (input[i] == '<' && input[i + 1] == '='))) {
            tokens.emplace_back(Token{ LETokenType::Operator, input.substr(i, 2), start, start + 2 });
            i += 2;
        } else if (std::strchr("!<>=+-*/", input[i])) {
            tokens.emplace_back(Token{ LETokenType::Operator, std::string(1, input[i]), start, start + 1 });
            ++i;
        } else {
            throw std::runtime_error("Unknown character: '" + std::string(1, input[i]) + "' at position " +
                                     std::to_string(i) + GetCharacterErrorContext(input, i));
        }
    }

    tokens.emplace_back(Token{ LETokenType::End, "", i, i });
    return tokens;
}

static std::string GetTokenErrorContext(const std::string& input, const Token& token, size_t contextLen = 10) {
    size_t start = (token.StartIndex >= contextLen) ? token.StartIndex - contextLen : 0;
    size_t end = std::min(input.size(), token.EndIndex + contextLen);
    std::string contextLine = input.substr(start, end - start);
    size_t pointerOffset = token.StartIndex - start;
    size_t pointerLength = (token.EndIndex > token.StartIndex) ? token.EndIndex - token.StartIndex : 1;
    std::string pointerLine(pointerOffset, ' ');
    pointerLine.append(std::string(pointerLength, '^'));
    return "\n" + contextLine + "\n" + pointerLine;
}

class Parser {
    const std::vector<Token> tokens;
    const std::string& input;
    size_t pos = 0;

    const Token& Peek() const {
        return tokens[pos];
    }

    const Token& Next() {
        return tokens[pos++];
    }

    bool Match(const std::string& op) {
        if (Peek().Type == LETokenType::Operator && Peek().Text == op) {
            ++pos;
            return true;
        }
        return false;
    }

    std::shared_ptr<LogicExpression::Impl>
    BuildNAryNode(LogicExpression::Type type, const std::string& op, size_t initial_pos,
                  const std::vector<std::shared_ptr<LogicExpression::Impl>>& terms) {
        auto expr = std::make_shared<LogicExpression::Impl>();
        expr->type = type;
        expr->operation = op;
        for (auto& t : terms) {
            expr->children.emplace_back(t);
            expr->children.back()->parent = expr.get();
        }
        expr->startIndex = tokens[initial_pos].StartIndex;
        expr->endIndex = tokens[pos - 1].EndIndex;
        return expr;
    }

    std::shared_ptr<LogicExpression::Impl> ParsePrimary() {
        size_t initial_pos = pos;
        std::shared_ptr<LogicExpression::Impl> expr;

        if (Match("!")) {
            expr = std::make_shared<LogicExpression::Impl>();
            expr->type = LogicExpression::Type::Not;
            expr->children.emplace_back(ParsePrimary());
            expr->children.back()->parent = expr.get();
        } else if (Peek().Type == LETokenType::ParenOpen) {
            Next();
            expr = ParseExpression();
            if (Peek().Type != LETokenType::ParenClose) {
                throw std::runtime_error("Expected ')' at position " + std::to_string(Peek().StartIndex) +
                                         GetTokenErrorContext(input, Peek()));
            }
            Next();
        } else if (Peek().Type == LETokenType::Identifier || Peek().Type == LETokenType::Boolean ||
                   Peek().Type == LETokenType::Number || Peek().Type == LETokenType::EnumConstant) {
            Token token = Next();
            expr = std::make_shared<LogicExpression::Impl>();

            if (Peek().Type == LETokenType::ParenOpen && token.Type == LETokenType::Identifier) {
                --pos;
                std::string id = Next().Text;
                Next();
                expr->type = LogicExpression::Type::FunctionCall;
                expr->functionName = id;
                while (Peek().Type != LETokenType::ParenClose) {
                    if (!expr->children.empty()) {
                        if (Peek().Type != LETokenType::Comma) {
                            throw std::runtime_error("Expected ',' at position " + std::to_string(Peek().StartIndex) +
                                                     GetTokenErrorContext(input, Peek()));
                        }
                        Next();
                    }
                    expr->children.emplace_back(ParseExpression());
                    expr->children.back()->parent = expr.get();
                }
                Next();
            } else {
                expr->type = LogicExpression::Type::Value;
                expr->value = token.Text;
                if (token.Type == LETokenType::Boolean)
                    expr->valueType = LogicExpression::ValueType::Boolean;
                else if (token.Type == LETokenType::Number)
                    expr->valueType = LogicExpression::ValueType::Number;
                else if (token.Type == LETokenType::EnumConstant)
                    expr->valueType = LogicExpression::ValueType::Enum;
                else
                    expr->valueType = LogicExpression::ValueType::Identifier;
            }
        }

        if (expr == nullptr) {
            throw std::runtime_error("Unexpected token: " + Peek().Text + GetTokenErrorContext(input, Peek()));
        }

        expr->startIndex = tokens[initial_pos].StartIndex;
        expr->endIndex = tokens[pos - 1].EndIndex;
        return expr;
    }

    template <typename LowerFunc>
    std::shared_ptr<LogicExpression::Impl>
    ParseBinaryOp(size_t& posRef, const std::vector<Token>& tokensRef, LowerFunc lowerFunc,
                  const std::vector<std::pair<std::string, LogicExpression::Type>>& operators) {
        size_t initial_pos = posRef;
        auto left = (this->*lowerFunc)();

        if (posRef < tokensRef.size() && tokensRef[posRef].Type == LETokenType::Operator) {
            for (const auto& [op, exprType] : operators) {
                if (tokensRef[posRef].Text == op) {
                    ++posRef;
                    auto right = ParseBinaryOp(posRef, tokensRef, lowerFunc, operators);
                    auto expr = std::make_shared<LogicExpression::Impl>();
                    expr->type = exprType;
                    expr->operation = op;
                    expr->children.emplace_back(left);
                    expr->children.back()->parent = expr.get();
                    expr->children.emplace_back(right);
                    expr->children.back()->parent = expr.get();
                    expr->startIndex = tokensRef[initial_pos].StartIndex;
                    expr->endIndex = tokensRef[posRef - 1].EndIndex;
                    return expr;
                }
            }
        }

        return left;
    }

    std::shared_ptr<LogicExpression::Impl> ParseMulDiv() {
        size_t initial_pos = pos;
        auto first = ParsePrimary();

        if (Peek().Type == LETokenType::Operator && (Peek().Text == "*" || Peek().Text == "/")) {
            if (Peek().Text == "*") {
                std::vector<std::shared_ptr<LogicExpression::Impl>> terms;
                terms.emplace_back(first);
                while (Peek().Type == LETokenType::Operator && Peek().Text == "*") {
                    Next();
                    auto nextTerm = ParsePrimary();
                    terms.emplace_back(nextTerm);
                }
                if (terms.size() == 1) {
                    return first;
                }
                return BuildNAryNode(LogicExpression::Type::Multiply, "*", initial_pos, terms);
            }

            std::vector<std::shared_ptr<LogicExpression::Impl>> terms;
            terms.emplace_back(first);
            while (Peek().Type == LETokenType::Operator && Peek().Text == "/") {
                Next();
                auto nextTerm = ParsePrimary();
                terms.emplace_back(nextTerm);
            }
            if (terms.size() == 1) {
                return first;
            }
            return BuildNAryNode(LogicExpression::Type::Divide, "/", initial_pos, terms);
        }

        return first;
    }

    std::shared_ptr<LogicExpression::Impl> ParseAddSub() {
        size_t initial_pos = pos;
        auto first = ParseMulDiv();

        if (Peek().Type == LETokenType::Operator && (Peek().Text == "+" || Peek().Text == "-")) {
            if (Peek().Text == "+") {
                std::vector<std::shared_ptr<LogicExpression::Impl>> terms;
                terms.emplace_back(first);
                while (Peek().Type == LETokenType::Operator && Peek().Text == "+") {
                    Next();
                    auto nextTerm = ParseMulDiv();
                    terms.emplace_back(nextTerm);
                }
                if (terms.size() == 1) {
                    return first;
                }
                return BuildNAryNode(LogicExpression::Type::Add, "+", initial_pos, terms);
            }

            std::vector<std::shared_ptr<LogicExpression::Impl>> terms;
            terms.emplace_back(first);
            while (Peek().Type == LETokenType::Operator && Peek().Text == "-") {
                Next();
                auto nextTerm = ParseMulDiv();
                terms.emplace_back(nextTerm);
            }
            if (terms.size() == 1) {
                return first;
            }
            return BuildNAryNode(LogicExpression::Type::Subtract, "-", initial_pos, terms);
        }

        return first;
    }

    std::shared_ptr<LogicExpression::Impl> ParseComparison() {
        return ParseBinaryOp(pos, tokens, &Parser::ParseAddSub,
                             { { "==", LogicExpression::Type::Comparison },
                               { "!=", LogicExpression::Type::Comparison },
                               { ">=", LogicExpression::Type::Comparison },
                               { "<=", LogicExpression::Type::Comparison },
                               { ">", LogicExpression::Type::Comparison },
                               { "<", LogicExpression::Type::Comparison } });
    }

    std::shared_ptr<LogicExpression::Impl> ParseAnd() {
        size_t initial_pos = pos;
        auto first = ParseComparison();
        std::vector<std::shared_ptr<LogicExpression::Impl>> terms;
        terms.emplace_back(first);

        while (Peek().Type == LETokenType::Operator && Peek().Text == "&&") {
            Next();
            auto nextTerm = ParseComparison();
            terms.emplace_back(nextTerm);
        }

        if (terms.size() == 1) {
            return first;
        }

        return BuildNAryNode(LogicExpression::Type::And, "&&", initial_pos, terms);
    }

    std::shared_ptr<LogicExpression::Impl> ParseOr() {
        size_t initial_pos = pos;
        auto first = ParseAnd();
        std::vector<std::shared_ptr<LogicExpression::Impl>> terms;
        terms.emplace_back(first);

        while (Peek().Type == LETokenType::Operator && Peek().Text == "||") {
            Next();
            auto nextTerm = ParseAnd();
            terms.emplace_back(nextTerm);
        }

        if (terms.size() == 1) {
            return first;
        }

        return BuildNAryNode(LogicExpression::Type::Or, "||", initial_pos, terms);
    }

    std::shared_ptr<LogicExpression::Impl> ParseTernary() {
        size_t initial_pos = pos;
        auto cond = ParseOr();

        if (Peek().Type == LETokenType::Operator && Peek().Text == "?") {
            Next();
            auto trueExpr = ParseTernary();

            if (!(Peek().Type == LETokenType::Operator && Peek().Text == ":")) {
                throw std::runtime_error("Expected ':' in ternary expression at position " +
                                         std::to_string(Peek().StartIndex) + GetTokenErrorContext(input, Peek()));
            }

            Next();
            auto falseExpr = ParseTernary();

            auto expr = std::make_shared<LogicExpression::Impl>();
            expr->type = LogicExpression::Type::Ternary;

            expr->children.emplace_back(cond);
            expr->children.back()->parent = expr.get();
            expr->children.emplace_back(trueExpr);
            expr->children.back()->parent = expr.get();
            expr->children.emplace_back(falseExpr);
            expr->children.back()->parent = expr.get();

            expr->startIndex = tokens[initial_pos].StartIndex;
            expr->endIndex = tokens[pos - 1].EndIndex;
            return expr;
        }

        return cond;
    }

    std::shared_ptr<LogicExpression::Impl> ParseExpression() {
        return ParseTernary();
    }

  public:
    Parser(const std::string& in) : tokens(Tokenize(in)), input(in) {
    }

    std::shared_ptr<LogicExpression::Impl> Parse() {
        if (tokens.empty() || tokens[0].Type == LETokenType::End) {
            throw std::runtime_error("Empty input");
        }
        auto expr = ParseExpression();
        if (Peek().Type != LETokenType::End) {
            throw std::runtime_error("Unexpected token: " + Peek().Text + " at position " +
                                     std::to_string(Peek().StartIndex) + GetTokenErrorContext(input, Peek()));
        }
        expr->expressionString = std::make_unique<std::string>(input);
        return expr;
    }
};

std::shared_ptr<LogicExpression> LogicExpression::Parse(const std::string& exprStr) {
    Parser parser(exprStr);
    std::shared_ptr<LogicExpression::Impl> impl = parser.Parse();

    std::function<std::shared_ptr<LogicExpression>(const std::shared_ptr<LogicExpression::Impl>&)> populateChildren;
    populateChildren = [&](const std::shared_ptr<LogicExpression::Impl>& node) {
        auto expr = std::make_shared<LogicExpression>();
        expr->impl = node;
        node->expression = expr;
        for (const auto& child : node->children) {
            expr->children.emplace_back(populateChildren(child));
        }
        return expr;
    };

    return populateChildren(impl);
}
